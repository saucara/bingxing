#include <iostream>
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

    for(int t = 0; t < REPEAT; t++) {
        for(int i = 0; i < N; i++) sum[i] = 0.0; //清零
        
        for(int j = 0; j < N; j++) {
            for(int i = 0; i < N; i++) {
                sum[i] += B[j][i] * a[j]; //连续访问内存
            }
        }
    }

    cout << "Result sample: " << sum[0] << std::endl;
    return 0;
}