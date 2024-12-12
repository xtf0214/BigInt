#include <vector>
#include <random>

template <typename BigInt> void check(long long a, long long b) {
    BigInt x(a), y(b), c;
    if ((x == y) != (BigInt(a) == BigInt(b)))
        std::cout << "ERROR ==: " << a << " " << b << std::endl;
    if ((x < y) != (BigInt(a) < BigInt(b)))
        std::cout << "ERROR < : " << a << " " << b << std::endl;
    if ((x <= y) != (BigInt(a) <= BigInt(b)))
        std::cout << "ERROR <= : " << a << " " << b << std::endl;

    if ((x + y) != BigInt(a + b))
        std::cout << "ERROR + : " << a << " " << b << " " << x + y << ' ' << BigInt(a + b) << std::endl;
    c = x;
    c += y;
    if (c != BigInt(a + b))
        std::cout << "ERROR += : " << a << " " << b << " " << c << ' ' << std::endl;

    if ((x - y) != BigInt(a - b))
        std::cout << "ERROR - : " << a << " " << b << " " << x - y << ' ' << BigInt(a - b) << std::endl;
    c = x;
    c -= y;
    if (c != BigInt(a - b))
        std::cout << "ERROR -= : " << a << " " << b << " " << c << ' ' << std::endl;
    if ((x * y) != BigInt(a * b))
        std::cout << "ERROR * : " << a << " " << b << " " << x * y << ' ' << BigInt(a * b) << std::endl;
    c = x;
    c *= y;
    if (c != BigInt(a * b))
        std::cout << "ERROR *= : " << a << " " << b << " " << c << ' ' << std::endl;
}
template<typename BigInt>
void test() {
    std::mt19937 gen(time(0));
    int n = 20;
    long long a[20]{
        4, 30, 21, 9, 15, 31, 5, 20, 11, 5, 298, 113, 116, 291, 364, 71, 193, 60, 225, 491,
    };
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (a[i] < a[j])
                check<BigInt>(a[j], a[i]);
            else
                check<BigInt>(a[i], a[j]);
}
#include <ProgressBar.h>
template<typename BigInt>
void randtest() {
    int n = 1e6, m = 1;
    std::vector<long long> a(n), b(n);
    std::mt19937 rng(114514);
    for (int i = 0; i < n; i++)
        a[i] = rng(), b[i] = rng();
    ProgressBar tqdm(0, n - 1);
    for (int i = 0; i < n; tqdm(i++)) {
        a[i] = std::abs(a[i]) % 10000;
        b[i] = std::abs(b[i]) % 10000;
        if (a[i] < b[i])
            std::swap(a[i], b[i]);
        check<BigInt>(a[i], b[i]);

    }
}