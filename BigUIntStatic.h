#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

const size_t pow10[19] = {1, 10, 100, 1000, 10000};
const size_t MAX_SIZE = 1024 * 1024 * 1024;
const size_t BIT_SIZE = 4;
const size_t BIT_LIMIT = pow10[BIT_SIZE]; // MAX_SIZE * BIT_SIZE <= 2^64
const int P = 998244353;

class big_uint {
  public:
    using value_type = int;

    // 构造函数
    big_uint();
    big_uint(const big_uint &t);
    big_uint(big_uint &&t);
    big_uint(value_type num);
    big_uint(const std::string &num);

    // 析构函数
    ~big_uint();

    // 赋值操作符
    big_uint &operator=(const big_uint &t);
    big_uint &operator=(big_uint &&t);
    big_uint &operator=(value_type num);
    big_uint &operator=(const std::string &num);

    // 输入输出
    friend std::ostream &operator<<(std::ostream &os, const big_uint &t);

    // 算术运算
    big_uint &operator+=(const big_uint &t);
    friend big_uint operator+(const big_uint &a, const big_uint &b);
    big_uint &operator-=(const big_uint &t);
    friend big_uint operator-(const big_uint &a, const big_uint &b);
    big_uint &operator*=(const big_uint &t);
    friend big_uint operator*(const big_uint &a, const big_uint &b);
    std::string estimation(size_t sz);

    // 比较运算
    friend bool operator==(const big_uint &a, const big_uint &b);
    friend bool operator!=(const big_uint &a, const big_uint &b);
    friend bool operator<(const big_uint &a, const big_uint &b);
    friend bool operator<=(const big_uint &a, const big_uint &b);

  private:
    // 内部辅助函数
    void copyByString(const std::string &num);
    static int sgn(value_type x);
    static int compareAbs(const big_uint &a, const big_uint &b);
    static void plus(value_type a[], value_type b[], size_t l);
    static void minus(value_type a[], value_type b[], size_t l);
    static void multiply(value_type a[], value_type b[], size_t al, size_t bl, bool ntt);
    static void NTT(value_type a[], size_t n, int inv);
    static int ksm(int a, int b, int P);
    static int inv(int a, int P);

    void resize(size_t new_capacity);

    // 成员变量
    size_t len = 0;
    value_type *data = nullptr;
};
// 构造函数
big_uint::big_uint() : data(new value_type[MAX_SIZE]{0}) {}
big_uint::big_uint(const big_uint &t) : len(t.len), data(new value_type[MAX_SIZE]{0}) {
    std::copy(t.data, t.data + t.len, data);
}
big_uint::big_uint(big_uint &&t) : len(t.len), data(t.data) {
    t.len = 0;
    t.data = nullptr;
}
big_uint::big_uint(value_type num) : len(0), data(new value_type[MAX_SIZE]{0}) {
    while (num) {
        data[len++] = num % BIT_LIMIT;
        num /= BIT_LIMIT;
    }
}
big_uint::big_uint(const std::string &num) : data(new value_type[MAX_SIZE]{0}) {
    copyByString(num);
}
// 析构函数
big_uint::~big_uint() { delete[] data; }
// 赋值运算符
big_uint &big_uint::operator=(const big_uint &t) {
    if (this == &t)
        return *this;
    delete[] data;
    len = t.len;
    data = new value_type[MAX_SIZE]{0};
    std::copy(t.data, t.data + t.len, data);
    return *this;
}
big_uint &big_uint::operator=(big_uint &&t) {
    if (this == &t)
        return *this;
    delete[] data;
    len = t.len;
    data = t.data;
    t.len = 0;
    t.data = nullptr;
    return *this;
}
big_uint &big_uint::operator=(value_type num) {
    delete[] data;
    data = new value_type[MAX_SIZE]{0};
    len = 0;
    while (num) {
        data[len++] = num % BIT_LIMIT;
        num /= BIT_LIMIT;
    }
    return *this;
}
big_uint &big_uint::operator=(const std::string &num) {
    delete[] data;
    data = new value_type[MAX_SIZE]{0};
    copyByString(num);
    return *this;
}
// 输出
std::ostream &operator<<(std::ostream &os, const big_uint &t) {
    if (t.len == 0)
        return os << 0;
    for (int i = t.len - 1; i >= 0; i--)
        os << t.data[i];
    return os;
}
// 算术运算
big_uint &big_uint::operator+=(const big_uint &t) {
    len = std::max(len, t.len) + 1;
    plus(data, t.data, len);
    while (len && !data[len - 1])
        len--;
    return *this;
}
big_uint &big_uint::operator-=(const big_uint &t) {
    assert(t <= *this);
    minus(data, t.data, len);
    while (len && !data[len - 1])
        len--;
    return *this;
}
big_uint &big_uint::operator*=(const big_uint &t) {
    assert((len + t.len) * 2 <= MAX_SIZE);
    big_uint::multiply(data, t.data, len, t.len, len + t.len > 128);
    len += t.len;
    while (len && !data[len - 1])
        len--;
    return *this;
}
big_uint operator+(const big_uint &a, const big_uint &b) { return big_uint(a) += b; }
big_uint operator-(const big_uint &a, const big_uint &b) { return big_uint(a) -= b; }
big_uint operator*(const big_uint &a, const big_uint &b) { return big_uint(a) *= b; }
std::string big_uint::estimation(size_t sz = 6) {
    if (len == 0)
        return "0";
    std::string s = std::to_string(data[len - 1]);
    size_t exponent = s.size() + (len - 1) * BIT_SIZE;
    for (int i = len - 2; s.size() < sz && i >= 0; i--) {
        std::string t = std::to_string(data[i]);
        s += std::string(BIT_SIZE - t.size(), '0') + std::to_string(data[i]);
    }
    s.resize(std::min(sz, s.size()));
    s.insert(1, ".");
    return s + "e" + std::to_string(exponent - 1);
}
big_uint pow(big_uint a, long long b) {
    big_uint res(1);
    for (; b; a *= a, b >>= 1) {
        if (b & 1)
            res *= a;
    }
    return res;
}
// 比较运算
bool operator==(const big_uint &a, const big_uint &b) { return big_uint::compareAbs(a, b) == 0; }
bool operator!=(const big_uint &a, const big_uint &b) { return !(a == b); }
bool operator<(const big_uint &a, const big_uint &b) { return big_uint::compareAbs(a, b) < 0; }
bool operator<=(const big_uint &a, const big_uint &b) { return big_uint::compareAbs(a, b) <= 0; }
// 辅助函数
void big_uint::copyByString(const std::string &num) {
    if (num.front() == '0' && num.size() == 1) {
        len = 0;
    } else {
        assert(num[0] != '-');
        size_t n = num.size();
        len = (n - 1) / BIT_SIZE + 1;
        for (size_t i = 0, j = num.size() - 1; i < n; i++, j--) {
            assert(num[j] >= '0' && num[j] <= '9');
            data[i / BIT_SIZE] += (num[j] - '0') * pow10[i % BIT_SIZE];
        }
    }
}
int big_uint::sgn(value_type x) { return (x > 0) - (x < 0); }
int big_uint::compareAbs(const big_uint &a, const big_uint &b) {
    if (a.len == b.len) {
        for (int i = a.len - 1; i >= 0; i--)
            if (a.data[i] != b.data[i])
                return sgn(a.data[i] - b.data[i]);
        return 0;
    } else {
        return sgn(a.len - b.len);
    }
}
void big_uint::plus(value_type a[], value_type b[], size_t l) {
    for (int i = 0; i < l; i++) {
        a[i] += b[i];
        if (a[i] >= BIT_LIMIT) {
            a[i + 1]++;
            a[i] -= BIT_LIMIT;
        }
    }
}
void big_uint::minus(value_type a[], value_type b[], size_t l) {
    for (int i = 0; i < l; i++) {
        a[i] -= b[i];
        if (a[i] < 0) {
            a[i + 1]--;
            a[i] += BIT_LIMIT;
        }
    }
}
void big_uint::multiply(value_type a[], value_type b[], size_t al, size_t bl, bool ntt) {
    if (ntt) {
        size_t n = al + bl, len = 2 << std::__lg(n - 1);
        NTT(a, len, 1);
        NTT(b, len, 1);
        for (int i = 0; i < len; i++)
            a[i] = 1LL * a[i] * b[i] % P;
        NTT(a, len, -1);
        NTT(b, len, -1);
    } else {
        for (int k = al + bl - 2; k >= 0; k--) {
            value_type tmp = 0;
            for (int i = std::min(k, (int)al - 1); i >= 0 && k - i < bl; i--)
                tmp += a[i] * b[k - i];
            a[k] = tmp;
        }
    }
    for (int i = 0; i < al + bl; i++) {
        a[i + 1] += a[i] / BIT_LIMIT;
        a[i] %= BIT_LIMIT;
    }
}
int big_uint::ksm(int a, int b, int P) {
    int s = 1;
    for (; b; b >>= 1, a = 1LL * a * a % P)
        if (b & 1)
            s = 1LL * s * a % P;
    return s;
}
int big_uint::inv(int a, int P) { return big_uint::ksm(a, P - 2, P); }
void big_uint::NTT(value_type a[], size_t n, int inv) {
    std::vector<size_t> rev(n);
    for (size_t i = 0; i < n; i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << std::__lg(n >> 1));
        if (i < rev[i])
            std::swap(a[i], a[rev[i]]);
    }
    for (size_t i = 2; i <= n; i <<= 1) {
        int gn = ksm(inv == 1 ? 3 : big_uint::inv(3, P), (P - 1) / i, P);
        for (size_t j = 0; j < n; j += i) {
            int g0 = 1;
            for (size_t k = j; k < j + i / 2; ++k, g0 = 1LL * g0 * gn % P) {
                int x = a[k], y = 1LL * g0 * a[k + i / 2] % P;
                a[k] = (x + y) % P;
                a[k + i / 2] = (x - y + P) % P;
            }
        }
    }
    if (inv == -1) {
        int n_inv = big_uint::inv(n, P);
        for (int i = 0; i < n; i++)
            a[i] = 1LL * a[i] * n_inv % P;
    }
}
