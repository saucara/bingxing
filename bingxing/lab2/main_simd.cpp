#include "PCFG.h"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <vector>
#include "md5.h"
#include "md5_simd.h"
using namespace std;
using namespace chrono;

// 编译指令（注意：产物仍命名为 main，因为 qsub.sh 跑的是 main）：
//   g++ main_simd.cpp train.cpp guessing.cpp md5.cpp md5_simd.cpp -march=armv8-a -o main
//   g++ main_simd.cpp train.cpp guessing.cpp md5.cpp md5_simd.cpp -march=armv8-a -O1 -o main
//   g++ main_simd.cpp train.cpp guessing.cpp md5.cpp md5_simd.cpp -march=armv8-a -O2 -o main

int main()
{
    // ============== 1) 串行 MD5 正确性测试 ==============
    cout << "Testing MD5Hash correctness..." << endl;
    string test_pws[8] = {"123456", "password", "12345678", "qwerty", "123456789", "12345", "1234", "111111"};
    string test_hashes[8] = {
        "e10adc3949ba59abbe56e057f20f883e",
        "5f4dcc3b5aa765d61d8327deb882cf99",
        "25d55ad283aa400af464c76d713c07ad",
        "d8578edf8458ce06fbc5bb76a58c5ca4",
        "25f9e794323b453885f5181f1b624d0b",
        "827ccb0eea8a706c4c34a16891f84e7b",
        "81dc9bdb52d04dc20036dbd8313ed055",
        "96e79218965eb72c92a549dd5a330112"
    };
    for (int i = 0; i < 8; i++) {
        bit32 state[4];
        MD5Hash(test_pws[i], state);
        stringstream ss;
        for (int i1 = 0; i1 < 4; i1 += 1) {
            ss << std::setw(8) << std::setfill('0') << hex << state[i1];
        }
        if (ss.str() != test_hashes[i]) {
            cout << "MD5Hash test failed for " << test_pws[i] << "!" << endl;
            cout << "Expected: " << test_hashes[i] << "\nGot:      " << ss.str() << endl;
            return 1;
        }
    }
    cout << "MD5Hash test passed!" << endl; // 请不要修改这一行

    // ============== 2) SIMD MD5 正确性自检 ==============
    // 用两组测试：
    //   (a) 4 路同长度短口令（length=6，1 个 block）
    //   (b) 4 路同长度长口令（length=64，2 个 block，验证多 block 累积是否正确）
    {
        // (a)
        string batch_a[4] = {"123456", "qwerty", "111111", "abcdef"};
        bit32 simd_state[4][4];
        MD5Hashsimd(batch_a, simd_state);
        for (int i = 0; i < 4; i++) {
            bit32 ref[4];
            MD5Hash(batch_a[i], ref);
            for (int j = 0; j < 4; j++) {
                if (ref[j] != simd_state[i][j]) {
                    cout << "SIMD MISMATCH (short batch) at input " << i
                         << " word " << j
                         << " ref=" << hex << ref[j]
                         << " simd=" << simd_state[i][j] << endl;
                    return 1;
                }
            }
        }
        // (b) 多 block 测试：长度 64 的口令 padding 后是 2 个 block
        string long_pw1(64, 'a');
        string long_pw2(64, 'b');
        string long_pw3(64, 'c');
        string long_pw4(64, 'd');
        string batch_b[4] = {long_pw1, long_pw2, long_pw3, long_pw4};
        MD5Hashsimd(batch_b, simd_state);
        for (int i = 0; i < 4; i++) {
            bit32 ref[4];
            MD5Hash(batch_b[i], ref);
            for (int j = 0; j < 4; j++) {
                if (ref[j] != simd_state[i][j]) {
                    cout << "SIMD MISMATCH (long batch) at input " << i
                         << " word " << j
                         << " ref=" << hex << ref[j]
                         << " simd=" << simd_state[i][j] << endl;
                    return 1;
                }
            }
        }
    }
    cout << "MD5Hashsimd test passed!" << endl;

    // ============== 3) 主流程：训练 + 猜测 + 哈希 ==============
    double time_hash = 0;
    double time_guess = 0;
    double time_train = 0;
    PriorityQueue q;
    auto start_train = system_clock::now();
    q.m.train("/guessdata/Rockyou-singleLined-full.txt");
    q.m.order();
    auto end_train = system_clock::now();
    auto duration_train = duration_cast<microseconds>(end_train - start_train);
    time_train = double(duration_train.count()) * microseconds::period::num / microseconds::period::den;

    q.init();
    cout << "here" << endl;
    int curr_num = 0;
    auto start = system_clock::now();
    int history = 0;

    while (!q.priority.empty())
    {
        q.PopNext();
        q.total_guesses = q.guesses.size();
        if (q.total_guesses - curr_num >= 100000)
        {
            cout << "Guesses generated: " << history + q.total_guesses << endl;
            curr_num = q.total_guesses;

            int generate_n = 10000000;
            if (history + q.total_guesses > 10000000)
            {
                auto end = system_clock::now();
                auto duration = duration_cast<microseconds>(end - start);
                time_guess = double(duration.count()) * microseconds::period::num / microseconds::period::den;
                cout << "Guess time:" << time_guess - time_hash << "seconds" << endl; // 请不要修改这一行
                cout << "Hash time:"  << time_hash << "seconds" << endl;               // 请不要修改这一行
                cout << "Train time:" << time_train << "seconds" << endl;              // 请不要修改这一行
                break;
            }
        }

        if (curr_num > 1000000)
        {
            auto start_hash = system_clock::now();

            // ---------- 关键改动：按长度分桶后做 4 路 SIMD ----------
            // 同一长度的口令一起送进 SIMD（保证 batch 内 4 路 padding 一致）
            // 不足 4 个的尾巴走串行 MD5。
            unordered_map<size_t, vector<int>> buckets;
            buckets.reserve(64);
            for (int k = 0; k < (int)q.guesses.size(); ++k) {
                buckets[q.guesses[k].size()].push_back(k);
            }

            bit32 state[4];
            bit32 statesimd[4][4];
            for (auto &kv : buckets) {
                vector<int> &idxs = kv.second;
                int j = 0;
                int n = (int)idxs.size();
                for (; j + 4 <= n; j += 4) {
                    string batch[4] = {
                        q.guesses[idxs[j]],
                        q.guesses[idxs[j+1]],
                        q.guesses[idxs[j+2]],
                        q.guesses[idxs[j+3]]
                    };
                    MD5Hashsimd(batch, statesimd);
                }
                for (; j < n; ++j) {
                    MD5Hash(q.guesses[idxs[j]], state);
                }
            }
            // -------------------------------------------------------

            auto end_hash = system_clock::now();
            auto duration = duration_cast<microseconds>(end_hash - start_hash);
            time_hash += double(duration.count()) * microseconds::period::num / microseconds::period::den;

            history += curr_num;
            curr_num = 0;
            q.guesses.clear();
        }
    }
    return 0;
}
