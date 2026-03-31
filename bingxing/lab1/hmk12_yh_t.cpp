#include <iostream>
#include <windows.h>
using namespace std;
const int N = 8192;
const int REPEAT = 20;
double B[N][N], a[N], sum[N];

int main() {
    for(int i = 0; i < N; i++) {//初始化
        a[i] = 1.1;
        for(int j = 0; j < N; j++) {
            B[i][j] = 2.2;
        }
    }

    LARGE_INTEGER frequency, start_time, end_time;//记录开始时间
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);

    for(int t = 0; t < REPEAT; t++) {
        for(int i = 0; i < N; i++) sum[i] = 0.0; //清零
        
        for(int j = 0; j < N; j++) {
            for(int i = 0; i < N; i++) {
                sum[i] += B[j][i] * a[j]; //连续访问内存
            }
        }
    }
    QueryPerformanceCounter(&end_time);    //记录结束时间
    
    //计算公式，并防止结果误差过大
    double elapsed_ms = (double)(end_time.QuadPart - start_time.QuadPart) * 1000.0 / frequency.QuadPart;

    cout << "time：" << elapsed_ms<< endl;
    cout << "Result sample: " << sum[0] << endl;
    return 0;
}