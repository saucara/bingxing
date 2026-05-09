// 独立的 SIMD 正确性验证程序。
// 用法（产物名仍叫 main 以走 qsub.sh）：
//   g++ correctness_simd.cpp md5.cpp md5_simd.cpp train.cpp guessing.cpp -march=armv8-a -o main
// 然后 qsub。如果输出 "ALL PASS"，说明 SIMD 实现正确。

#include "PCFG.h"
#include "md5.h"
#include "md5_simd.h"
#include <iomanip>
#include <iostream>
#include <sstream>
using namespace std;

static string hash_to_str(bit32 *state) {
    stringstream ss;
    for (int i = 0; i < 4; i++) {
        ss << std::setw(8) << std::setfill('0') << hex << state[i];
    }
    return ss.str();
}

static bool run_batch_test(const string &name, string batch[4]) {
    cout << "[" << name << "] length=" << batch[0].size() << endl;
    bit32 ref[4][4];
    for (int i = 0; i < 4; i++) MD5Hash(batch[i], ref[i]);

    bit32 simd_state[4][4];
    MD5Hashsimd(batch, simd_state);

    bool ok = true;
    for (int i = 0; i < 4; i++) {
        string s_ref  = hash_to_str(ref[i]);
        string s_simd = hash_to_str(simd_state[i]);
        cout << "  in=" << batch[i] << "  ref=" << s_ref << "  simd=" << s_simd;
        if (s_ref != s_simd) { cout << "  <-- MISMATCH"; ok = false; }
        cout << endl;
    }
    return ok;
}

int main() {
    bool all_ok = true;

    // 1) 短口令，1 个 block
    {
        string b[4] = {"123456", "qwerty", "111111", "abcdef"};
        all_ok &= run_batch_test("short", b);
    }
    // 2) 长度 = 55，仍是 1 个 block 的边界
    {
        string b[4] = {string(55,'a'), string(55,'b'), string(55,'c'), string(55,'d')};
        all_ok &= run_batch_test("len55", b);
    }
    // 3) 长度 = 56，跨入 2 个 block
    {
        string b[4] = {string(56,'a'), string(56,'b'), string(56,'c'), string(56,'d')};
        all_ok &= run_batch_test("len56", b);
    }
    // 4) 长度 = 64，整 2 个 block
    {
        string b[4] = {string(64,'a'), string(64,'b'), string(64,'c'), string(64,'d')};
        all_ok &= run_batch_test("len64", b);
    }
    // 5) 长度 = 128，3 个 block
    {
        string b[4] = {string(128,'a'), string(128,'b'), string(128,'c'), string(128,'d')};
        all_ok &= run_batch_test("len128", b);
    }

    cout << (all_ok ? "ALL PASS" : "FAILED") << endl;
    return all_ok ? 0 : 1;
}
