#include <iostream>
using namespace std;
const int N = 1000000;
const int REPEAT = 2000;
double a[N];

int main() {
    for(int i = 0; i < N; i++) a[i] = 1.5;

    double res = 0;
    for(int t = 0; t < REPEAT; t++) {
        res = 0;
        for(int i = 0; i < N; i++) {
            res += a[i];//等上一步加完再加下一步
        }
    }

    cout << "Result: " << res << endl;
    return 0;
}