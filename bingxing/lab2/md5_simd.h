#include <iostream>
#include <string>
#include <cstring>
#include "md5.h"
#include <arm_neon.h>
using namespace std;


/**
 * @Basic MD5 functions.
 *
 * @param there bit32.
 *
 * @return one bit32.
 */
// 定义了一系列MD5中的具体函数
// 这四个计算函数是需要你进行SIMD并行化的
// 可以看到，FGHI四个函数都涉及一系列位运算，在数据上是对齐的，非常容易实现SIMD的并行化


//SIMD
#define F_simd(x,y,z) vorrq_u32(vandq_u32(x,y), vandq_u32(vmvnq_u32(x),z))
#define G_simd(x,y,z) vorrq_u32(vandq_u32(x,z), vandq_u32(y, vmvnq_u32(z)))
#define H_simd(x,y,z) veorq_u32(veorq_u32(x,y),z)
#define I_simd(x,y,z) veorq_u32(y, vorrq_u32(x, vmvnq_u32(z)))

/**
 * @Rotate Left.
 *
 * @param {num} the raw number.
 *
 * @param {n} rotate left n.
 *
 * @return the number after rotated left.
 */
// 定义了一系列MD5中的具体函数
// 这五个计算函数（ROTATELEFT/FF/GG/HH/II）和之前的FGHI一样，都是需要你进行SIMD并行化的
// 但是你需要注意的是#define的功能及其效果，可以发现这里的FGHI是没有返回值的，为什么呢？你可以查询#define的含义和用法

#define FF_simd(a, b, c, d, x, s, ac) { \
    (a) = vaddq_u32(vaddq_u32(vaddq_u32(a, F_simd(b,c,d)), x), vdupq_n_u32(ac)); \
    (a) = vorrq_u32(vshlq_n_u32(a,s), vshrq_n_u32(a,32-s)); \
    (a) += (b); \
}
#define GG_simd(a, b, c, d, x, s, ac) { \
    (a) = vaddq_u32(vaddq_u32(vaddq_u32(a, G_simd(b,c,d)), x), vdupq_n_u32(ac)); \
    (a) = vorrq_u32(vshlq_n_u32(a,s), vshrq_n_u32(a,32-s)); \
    (a) += (b); \
}
#define HH_simd(a, b, c, d, x, s, ac) { \
    (a) = vaddq_u32(vaddq_u32(vaddq_u32(a, H_simd(b,c,d)), x), vdupq_n_u32(ac)); \
    (a) = vorrq_u32(vshlq_n_u32(a,s), vshrq_n_u32(a,32-s)); \
    (a) += (b); \
}
#define II_simd(a, b, c, d, x, s, ac) { \
    (a) = vaddq_u32(vaddq_u32(vaddq_u32(a, I_simd(b,c,d)), x), vdupq_n_u32(ac)); \
    (a) = vorrq_u32(vshlq_n_u32(a,s), vshrq_n_u32(a,32-s)); \
    (a) += (b); \
}


void MD5Hashsimd(string inputs[4], bit32 statesimd[4][4]);