#include <iostream>
using namespace std;
const int N = 1000000;
const int REPEAT = 2000;
double a[N];

int main() {
    for(int i = 0; i < N; i++) a[i] = 1.5;

    double res = 0;
    
    for(int t = 0; t < REPEAT; t++) {
        double sum1 = 0, sum2 = 0;
        for(int i = 0; i < N; i += 2) {
            sum1 += a[i];
            sum2 += a[i+1];
        }
        res = sum1 + sum2;
    }

    cout << "Result: " << res << endl;
    return 0;
}