#include <vector>

namespace NTT {
const int P1 = 469762049, P2 = 167772161, G = 3;
int ksm(int a, int b, int P) {
    int s = 1;
    for (; b; b >>= 1, a = 1LL * a * a % P)
        if (b & 1)
            s = 1LL * s * a % P;
    return s;
}
int inv(int a, int P) { return ksm(a, P - 2, P); }
const int P1_inv = inv(P1, P2);

struct CRT {
    int A = 0, B = 0;
    CRT() {}
    CRT(int n) : A(n + (n >> 31 & P1)), B(n + (n >> 31 & P2)) {}
    CRT(int A, int B) : A(A + (A >> 31 & P1)), B(B + (B >> 31 & P2)) {}
    friend CRT operator+(const CRT &a, const CRT &b) { return CRT(a.A + b.A - P1, a.B + b.B - P2); }
    friend CRT operator-(const CRT &a, const CRT &b) { return CRT(a.A - b.A, a.B - b.B); }
    friend CRT operator*(const CRT &a, const CRT &b) {
        return CRT(1LL * a.A * b.A % P1, 1LL * a.B * b.B % P2);
    }
    long long get() {
        long long x1 = A;
        long long x2 = x1 + 1LL * (B - x1 % P2 + P2) % P2 * P1_inv % P2 * P1;
        return x2;
    }
};
void NTT(CRT a[], size_t n, int inverse) {
    std::vector<size_t> rev(n);
    for (size_t i = 0; i < n; i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << std::__lg(n >> 1));
        if (i < rev[i])
            std::swap(a[i], a[rev[i]]);
    }
    for (size_t i = 2; i <= n; i <<= 1) {
        CRT gn = CRT(ksm(inverse == 1 ? G : inv(G, P1), (P1 - 1) / i, P1),
                     ksm(inverse == 1 ? G : inv(G, P2), (P2 - 1) / i, P2));
        for (size_t j = 0; j < n; j += i) {
            CRT g0 = 1;
            for (size_t k = j; k < j + i / 2; ++k, g0 = g0 * gn) {
                CRT x = a[k], y = g0 * a[k + i / 2];
                a[k] = x + y;
                a[k + i / 2] = x - y;
            }
        }
    }
    if (inverse == -1) {
        const CRT n_inv(inv(n, P1), inv(n, P2));
        for (size_t i = 0; i < n; i++)
            a[i] = a[i] * n_inv;
    }
}
} // namespace NTT