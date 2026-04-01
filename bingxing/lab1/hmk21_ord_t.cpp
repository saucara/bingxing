#include <iostream>
#include <windows.h>
using namespace std;
const int REPEAT = 2000;
const int N = 100000000;
double a[N];

int main() {
    for(int i = 0; i < N; i++) a[i] = 1.5;

    LARGE_INTEGER frequency, start_time, end_time;//记录开始时间
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);
    double res = 0;
    for(int t = 0; t < REPEAT; t++) {
        res = 0;
        for(int i = 0; i < N; i++) {
            res += a[i];//等上一步加完再加下一步
        }
    }
    QueryPerformanceCounter(&end_time);    //记录结束时间
    
    //计算公式，并防止结果误差过大
    double elapsed_ms = (double)(end_time.QuadPart - start_time.QuadPart) * 1000.0 / frequency.QuadPart;

    cout << "time：" << elapsed_ms<< endl;
    cout << "Result: " << res << endl;
    return 0;
}