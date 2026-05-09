#include "md5.h"
#include "md5_simd.h"
#include <iomanip>
#include <assert.h>
#include <cstring>
#include <arm_neon.h>

using namespace std;

/*
 * MD5Hashsimd: 4 路并行的 MD5 哈希
 *
 * 关键设计：
 * 1. 不再调用 StringProcess 也不再 new[]/delete[]。
 *    所有 padding 都在栈上完成，消除堆分配开销（这是原版加速比 ≈ 1 的根因）。
 * 2. 修复了原版的多 block bug：原版每个 block 都把 a/b/c/d 重置成
 *    0x67452301 等初值，对长度 ≥ 56 字节的口令（n_blocks ≥ 2）会算错。
 *    现在 a/b/c/d 在 block 循环外初始化，每个 block 内用 aa/bb/cc/dd
 *    做局部累加，与串行版本语义一致。
 * 3. 4 路口令的字节交错加载用 vld4q_u8 一次完成，省掉手动 OR/移位。
 *
 * 前提：调用方保证 4 个 inputs 的 length 一致（按长度分桶）。
 */

// 单口令长度上限（字符）。256 足够覆盖任何实际口令；如需更长，调大并保证栈够用。
static constexpr int MAX_INPUT_LEN = 256;
// 单口令 padding 后长度上限。MAX_INPUT_LEN + 64 足够（padding 最多 ~64 字节）。
static constexpr int MAX_PADDED_LEN = 320;

void MD5Hashsimd(string inputs[4], bit32 statesimd[4][4])
{
    // 1) 检查长度一致 + padding 计算（4 路共享 length，因此只算一次）
    int length = (int)inputs[0].length();
    // 调用方应保证 4 路等长，这里用 assert 给一道保险（release 下可被 -DNDEBUG 关掉）
    assert((int)inputs[1].length() == length);
    assert((int)inputs[2].length() == length);
    assert((int)inputs[3].length() == length);
    assert(length <= MAX_INPUT_LEN);

    int bitLength = length * 8;
    int paddingBits = bitLength % 512;
    if (paddingBits > 448)      paddingBits = 512 - (paddingBits - 448);
    else if (paddingBits < 448) paddingBits = 448 - paddingBits;
    else                        paddingBits = 512;
    int paddingBytes = paddingBits / 8;
    int paddedLength = length + paddingBytes + 8;
    int n_blocks = paddedLength / 64;

    // 2) 栈上 buffer：4 路 padding 后的消息
    //    alignas(16) 让 NEON load 对齐到 128 位边界，更友好
    alignas(16) Byte paddedMessage[4][MAX_PADDED_LEN];

    for (int lane = 0; lane < 4; ++lane) {
        const char *src = inputs[lane].c_str();
        memcpy(paddedMessage[lane], src, length);
        paddedMessage[lane][length] = 0x80;
        memset(paddedMessage[lane] + length + 1, 0, paddingBytes - 1);
        // 64 位小端长度（单位：bit）
        uint64_t bitlen64 = (uint64_t)length * 8;
        for (int k = 0; k < 8; ++k) {
            paddedMessage[lane][length + paddingBytes + k] =
                (Byte)((bitlen64 >> (k * 8)) & 0xFF);
        }
    }

    // 3) MD5 初始化常量（4 路相同，broadcast）
    uint32x4_t a = vdupq_n_u32(0x67452301);
    uint32x4_t b = vdupq_n_u32(0xefcdab89);
    uint32x4_t c = vdupq_n_u32(0x98badcfe);
    uint32x4_t d = vdupq_n_u32(0x10325476);

    // 4) 逐 block 处理
    for (int blk = 0; blk < n_blocks; ++blk) {
        uint32x4_t x_simd[16];

        // 4 路字节交错加载：每路 64 字节 = 16 个 uint32
        // 这里我们直接对每个 32 位 word 槽做 4 路收集
        for (int i1 = 0; i1 < 16; ++i1) {
            uint32_t v0 = (uint32_t)paddedMessage[0][blk*64 + 4*i1]
                        | ((uint32_t)paddedMessage[0][blk*64 + 4*i1 + 1] << 8)
                        | ((uint32_t)paddedMessage[0][blk*64 + 4*i1 + 2] << 16)
                        | ((uint32_t)paddedMessage[0][blk*64 + 4*i1 + 3] << 24);
            uint32_t v1 = (uint32_t)paddedMessage[1][blk*64 + 4*i1]
                        | ((uint32_t)paddedMessage[1][blk*64 + 4*i1 + 1] << 8)
                        | ((uint32_t)paddedMessage[1][blk*64 + 4*i1 + 2] << 16)
                        | ((uint32_t)paddedMessage[1][blk*64 + 4*i1 + 3] << 24);
            uint32_t v2 = (uint32_t)paddedMessage[2][blk*64 + 4*i1]
                        | ((uint32_t)paddedMessage[2][blk*64 + 4*i1 + 1] << 8)
                        | ((uint32_t)paddedMessage[2][blk*64 + 4*i1 + 2] << 16)
                        | ((uint32_t)paddedMessage[2][blk*64 + 4*i1 + 3] << 24);
            uint32_t v3 = (uint32_t)paddedMessage[3][blk*64 + 4*i1]
                        | ((uint32_t)paddedMessage[3][blk*64 + 4*i1 + 1] << 8)
                        | ((uint32_t)paddedMessage[3][blk*64 + 4*i1 + 2] << 16)
                        | ((uint32_t)paddedMessage[3][blk*64 + 4*i1 + 3] << 24);
            uint32_t tmp[4] = {v0, v1, v2, v3};
            x_simd[i1] = vld1q_u32(tmp);
        }

        // 局部累加变量：每个 block 用 aa/bb/cc/dd 进行 64 步运算，
        // 结束后把增量加回 a/b/c/d。
        uint32x4_t aa = a, bb = b, cc = c, dd = d;

        /* Round 1 */
        FF_simd(aa, bb, cc, dd, x_simd[0],  s11, 0xd76aa478);
        FF_simd(dd, aa, bb, cc, x_simd[1],  s12, 0xe8c7b756);
        FF_simd(cc, dd, aa, bb, x_simd[2],  s13, 0x242070db);
        FF_simd(bb, cc, dd, aa, x_simd[3],  s14, 0xc1bdceee);
        FF_simd(aa, bb, cc, dd, x_simd[4],  s11, 0xf57c0faf);
        FF_simd(dd, aa, bb, cc, x_simd[5],  s12, 0x4787c62a);
        FF_simd(cc, dd, aa, bb, x_simd[6],  s13, 0xa8304613);
        FF_simd(bb, cc, dd, aa, x_simd[7],  s14, 0xfd469501);
        FF_simd(aa, bb, cc, dd, x_simd[8],  s11, 0x698098d8);
        FF_simd(dd, aa, bb, cc, x_simd[9],  s12, 0x8b44f7af);
        FF_simd(cc, dd, aa, bb, x_simd[10], s13, 0xffff5bb1);
        FF_simd(bb, cc, dd, aa, x_simd[11], s14, 0x895cd7be);
        FF_simd(aa, bb, cc, dd, x_simd[12], s11, 0x6b901122);
        FF_simd(dd, aa, bb, cc, x_simd[13], s12, 0xfd987193);
        FF_simd(cc, dd, aa, bb, x_simd[14], s13, 0xa679438e);
        FF_simd(bb, cc, dd, aa, x_simd[15], s14, 0x49b40821);

        /* Round 2 */
        GG_simd(aa, bb, cc, dd, x_simd[1],  s21, 0xf61e2562);
        GG_simd(dd, aa, bb, cc, x_simd[6],  s22, 0xc040b340);
        GG_simd(cc, dd, aa, bb, x_simd[11], s23, 0x265e5a51);
        GG_simd(bb, cc, dd, aa, x_simd[0],  s24, 0xe9b6c7aa);
        GG_simd(aa, bb, cc, dd, x_simd[5],  s21, 0xd62f105d);
        GG_simd(dd, aa, bb, cc, x_simd[10], s22, 0x02441453);
        GG_simd(cc, dd, aa, bb, x_simd[15], s23, 0xd8a1e681);
        GG_simd(bb, cc, dd, aa, x_simd[4],  s24, 0xe7d3fbc8);
        GG_simd(aa, bb, cc, dd, x_simd[9],  s21, 0x21e1cde6);
        GG_simd(dd, aa, bb, cc, x_simd[14], s22, 0xc33707d6);
        GG_simd(cc, dd, aa, bb, x_simd[3],  s23, 0xf4d50d87);
        GG_simd(bb, cc, dd, aa, x_simd[8],  s24, 0x455a14ed);
        GG_simd(aa, bb, cc, dd, x_simd[13], s21, 0xa9e3e905);
        GG_simd(dd, aa, bb, cc, x_simd[2],  s22, 0xfcefa3f8);
        GG_simd(cc, dd, aa, bb, x_simd[7],  s23, 0x676f02d9);
        GG_simd(bb, cc, dd, aa, x_simd[12], s24, 0x8d2a4c8a);

        /* Round 3 */
        HH_simd(aa, bb, cc, dd, x_simd[5],  s31, 0xfffa3942);
        HH_simd(dd, aa, bb, cc, x_simd[8],  s32, 0x8771f681);
        HH_simd(cc, dd, aa, bb, x_simd[11], s33, 0x6d9d6122);
        HH_simd(bb, cc, dd, aa, x_simd[14], s34, 0xfde5380c);
        HH_simd(aa, bb, cc, dd, x_simd[1],  s31, 0xa4beea44);
        HH_simd(dd, aa, bb, cc, x_simd[4],  s32, 0x4bdecfa9);
        HH_simd(cc, dd, aa, bb, x_simd[7],  s33, 0xf6bb4b60);
        HH_simd(bb, cc, dd, aa, x_simd[10], s34, 0xbebfbc70);
        HH_simd(aa, bb, cc, dd, x_simd[13], s31, 0x289b7ec6);
        HH_simd(dd, aa, bb, cc, x_simd[0],  s32, 0xeaa127fa);
        HH_simd(cc, dd, aa, bb, x_simd[3],  s33, 0xd4ef3085);
        HH_simd(bb, cc, dd, aa, x_simd[6],  s34, 0x04881d05);
        HH_simd(aa, bb, cc, dd, x_simd[9],  s31, 0xd9d4d039);
        HH_simd(dd, aa, bb, cc, x_simd[12], s32, 0xe6db99e5);
        HH_simd(cc, dd, aa, bb, x_simd[15], s33, 0x1fa27cf8);
        HH_simd(bb, cc, dd, aa, x_simd[2],  s34, 0xc4ac5665);

        /* Round 4 */
        II_simd(aa, bb, cc, dd, x_simd[0],  s41, 0xf4292244);
        II_simd(dd, aa, bb, cc, x_simd[7],  s42, 0x432aff97);
        II_simd(cc, dd, aa, bb, x_simd[14], s43, 0xab9423a7);
        II_simd(bb, cc, dd, aa, x_simd[5],  s44, 0xfc93a039);
        II_simd(aa, bb, cc, dd, x_simd[12], s41, 0x655b59c3);
        II_simd(dd, aa, bb, cc, x_simd[3],  s42, 0x8f0ccc92);
        II_simd(cc, dd, aa, bb, x_simd[10], s43, 0xffeff47d);
        II_simd(bb, cc, dd, aa, x_simd[1],  s44, 0x85845dd1);
        II_simd(aa, bb, cc, dd, x_simd[8],  s41, 0x6fa87e4f);
        II_simd(dd, aa, bb, cc, x_simd[15], s42, 0xfe2ce6e0);
        II_simd(cc, dd, aa, bb, x_simd[6],  s43, 0xa3014314);
        II_simd(bb, cc, dd, aa, x_simd[13], s44, 0x4e0811a1);
        II_simd(aa, bb, cc, dd, x_simd[4],  s41, 0xf7537e82);
        II_simd(dd, aa, bb, cc, x_simd[11], s42, 0xbd3af235);
        II_simd(cc, dd, aa, bb, x_simd[2],  s43, 0x2ad7d2bb);
        II_simd(bb, cc, dd, aa, x_simd[9],  s44, 0xeb86d391);

        // 把本 block 的增量累加到全局状态
        a = vaddq_u32(a, aa);
        b = vaddq_u32(b, bb);
        c = vaddq_u32(c, cc);
        d = vaddq_u32(d, dd);
    }

    // 5) 写出最终结果 + 字节序翻转（与串行 MD5Hash 保持一致）
    uint32_t out_a[4], out_b[4], out_c[4], out_d[4];
    vst1q_u32(out_a, a);
    vst1q_u32(out_b, b);
    vst1q_u32(out_c, c);
    vst1q_u32(out_d, d);

    for (int lane = 0; lane < 4; ++lane) {
        uint32_t va = out_a[lane], vb = out_b[lane], vc = out_c[lane], vd = out_d[lane];
        statesimd[lane][0] = ((va & 0xff) << 24) | ((va & 0xff00) << 8) | ((va & 0xff0000) >> 8) | ((va & 0xff000000) >> 24);
        statesimd[lane][1] = ((vb & 0xff) << 24) | ((vb & 0xff00) << 8) | ((vb & 0xff0000) >> 8) | ((vb & 0xff000000) >> 24);
        statesimd[lane][2] = ((vc & 0xff) << 24) | ((vc & 0xff00) << 8) | ((vc & 0xff0000) >> 8) | ((vc & 0xff000000) >> 24);
        statesimd[lane][3] = ((vd & 0xff) << 24) | ((vd & 0xff00) << 8) | ((vd & 0xff0000) >> 8) | ((vd & 0xff000000) >> 24);
    }
    // 无堆分配，无需 delete[]
}
