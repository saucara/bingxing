#include <iostream>
using namespace std;
const int N = 8192;
const int REPEAT = 20; // 重复执行以延长采样时间
double B[N][N], a[N], sum[N];

int main() {
    for(int i = 0; i < N; i++) {//初始化
        a[i] = 1.1;
        for(int j = 0; j < N; j++) {
            B[i][j] = 2.2;
        }
    }

    for(int t = 0; t < REPEAT; t++) {
        for(int i = 0; i < N; i++) {
            sum[i] = 0.0;
            for(int j = 0; j < N; j++) {
                sum[i] += B[j][i] * a[j];
            }
        }
    }
    cout << "Result sample: " << sum[0] << endl; 
    return 0;
}