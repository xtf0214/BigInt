#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>
class big_int {
  public:
    using value_type = int;

    // 构造函数
    big_int();
    big_int(const big_int &t);
    big_int(big_int &&t);
    big_int(long long num);
    big_int(const std::string &num);

    // 析构函数
    ~big_int();

    // 赋值操作符
    big_int &operator=(const big_int &t);
    big_int &operator=(big_int &&t);
    big_int &operator=(long long num);
    big_int &operator=(const std::string &num);

    // 输入输出
    friend std::ostream &operator<<(std::ostream &os, const big_int &t);

    // 算术运算
    friend big_int operator+(const big_int &a, const big_int &b);
    big_int &operator+=(const big_int &t);
    friend big_int operator-(const big_int &a, const big_int &b);
    big_int &operator-=(const big_int &t);
    friend big_int operator*(const big_int &a, const big_int &b);
    big_int &operator*=(const big_int &t);

    // 比较运算
    friend bool operator==(const big_int &a, const big_int &b);
    friend bool operator!=(const big_int &a, const big_int &b);
    friend bool operator<(const big_int &a, const big_int &b);
    friend bool operator<=(const big_int &a, const big_int &b);

  private:
    static const size_t MAX_SIZE, BIT_SIZE, BIT_LIMIT, pow10[19];
    static const int P;

    // 内部辅助函数
    void copyByString(const std::string &num);
    static int sgn(long long x);
    static int compareAbs(const big_int &a, const big_int &b);
    static void plus(value_type a[], value_type b[], value_type c[], size_t l);
    static void minus(value_type a[], value_type b[], value_type c[], size_t l);
    static void multiply(value_type a[], value_type b[], value_type c[], size_t al, size_t bl,
                         bool ntt);
    static void NTT(value_type a[], size_t n, int inv);
    static int ksm(int a, int b);

    // 成员变量
    size_t len = 0;
    value_type signum = 0;
    value_type *data = nullptr;
};
// 构造函数
big_int::big_int() : data(new value_type[MAX_SIZE]{0}) {}
big_int::big_int(const big_int &t)
    : signum(t.signum), len(t.len), data(new value_type[MAX_SIZE]{0}) {
    std::copy(t.data, t.data + t.len, data);
}
big_int::big_int(big_int &&t) : signum(t.signum), len(t.len), data(t.data) {
    t.signum = 0;
    t.len = 0;
    t.data = nullptr;
}
big_int::big_int(long long num) : signum(sgn(num)), data(new value_type[MAX_SIZE]{0}) {
    num *= signum;
    while (num) {
        data[len++] = num % BIT_LIMIT;
        num /= BIT_LIMIT;
    }
}
big_int::big_int(const std::string &num) : data(new value_type[MAX_SIZE]{0}) { copyByString(num); }
// 析构函数
big_int::~big_int() { delete[] data; }
// 赋值运算符
big_int &big_int::operator=(const big_int &t) {
    if (this == &t)
        return *this;
    delete[] data;
    signum = t.signum;
    len = t.len;
    data = new value_type[MAX_SIZE]{0};
    std::copy(t.data, t.data + t.len, data);
    return *this;
}
big_int &big_int::operator=(big_int &&t) {
    if (this == &t)
        return *this;
    delete[] data;
    signum = t.signum;
    len = t.len;
    data = t.data;
    t.signum = 0;
    t.len = 0;
    t.data = nullptr;
    return *this;
}
big_int &big_int::operator=(long long num) {
    delete[] data;
    signum = sgn(num);
    num *= signum;
    data = new value_type[MAX_SIZE]{0};
    len = 0;
    while (num) {
        data[len++] = num % BIT_LIMIT;
        num /= BIT_LIMIT;
    }
    return *this;
}
big_int &big_int::operator=(const std::string &num) {
    delete[] data;
    data = new value_type[MAX_SIZE]{0};
    copyByString(num);
    return *this;
}
// 输出
std::ostream &operator<<(std::ostream &os, const big_int &t) {
    if (t.signum == -1)
        os << '-';
    for (int i = t.len - 1; i >= 0; i--)
        os << t.data[i];
    return os;
}
// 算术运算
big_int operator+(const big_int &a, const big_int &b) {
    big_int c;
    if (a.signum == b.signum) {
        c.signum = a.signum;
        c.len = std::max(a.len, b.len) + 1;
        big_int::plus(a.data, b.data, c.data, c.len);
    } else {
        int cmp = big_int::compareAbs(a, b);
        if (cmp == 1) {
            c.signum = a.signum;
            c.len = a.len;
            big_int::minus(a.data, b.data, c.data, c.len);
        } else if (cmp == -1) {
            c.signum = b.signum;
            c.len = b.len;
            big_int::minus(b.data, a.data, c.data, c.len);
        }
    }
    while (c.len && !c.data[c.len - 1])
        c.len--;
    return c;
}
big_int &big_int::operator+=(const big_int &t) {
    big_int c;
    c = *this + t;
    *this = std::move(c);
    return *this;
}
big_int operator-(const big_int &a, const big_int &b) {
    big_int c;
    if (a.signum != b.signum) {
        c.signum = a.signum;
        c.len = std::max(a.len, b.len) + 1;
        big_int::plus(a.data, b.data, c.data, c.len);
    } else {
        int cmp = big_int::compareAbs(a, b);
        if (cmp == 1) {
            c.signum = a.signum;
            c.len = a.len;
            big_int::minus(a.data, b.data, c.data, c.len);
        } else if (cmp == -1) {
            c.signum = -1 * b.signum;
            c.len = b.len;
            big_int::minus(b.data, a.data, c.data, c.len);
        }
    }
    while (c.len && !c.data[c.len - 1])
        c.len--;
    return c;
}
big_int &big_int::operator-=(const big_int &t) {
    big_int c;
    c = *this - t;
    *this = std::move(c);
    return *this;
}
big_int operator*(const big_int &a, const big_int &b) {
    big_int c;
    if (a.signum * b.signum != 0) {
        c.signum = a.signum * b.signum;
        c.len = a.len + b.len;
        if (a.len + b.len > 128) {
            big_int::multiply(a.data, b.data, c.data, a.len, b.len, true);
        } else {
            big_int::multiply(a.data, b.data, c.data, a.len, b.len, false);
        }
    }
    while (c.len && !c.data[c.len - 1])
        c.len--;
    return c;
}
big_int &big_int::operator*=(const big_int &t) {
    big_int c;
    c = *this * t;
    *this = std::move(c);
    return *this;
}
// 比较运算
bool operator==(const big_int &a, const big_int &b) {
    return (a.signum == b.signum) && !big_int::compareAbs(a, b);
}
bool operator!=(const big_int &a, const big_int &b) { return !(a == b); }
bool operator<(const big_int &a, const big_int &b) {
    if (a.signum != b.signum)
        return a.signum < b.signum;
    else
        return big_int::compareAbs(a, b) * a.signum < 0;
}
bool operator<=(const big_int &a, const big_int &b) {
    if (a.signum != b.signum)
        return a.signum < b.signum;
    else
        return big_int::compareAbs(a, b) * a.signum <= 0;
}
// 辅助函数
void big_int::copyByString(const std::string &num) {
    if (num.front() == '0' && num.size() == 1) {
        signum = len = 0;
    } else {
        signum = num[0] == '-' ? -1 : 1;
        size_t n = num.size() - (signum == -1);
        len = (n - 1) / BIT_SIZE + 1;
        for (size_t i = 0, j = num.size() - 1; i < n; i++, j--)
            data[i / BIT_SIZE] += (num[j] - '0') * pow10[i % BIT_SIZE];
    }
}
int big_int::sgn(long long x) { 
    int res =  (x > 0) - (x < 0); 
    return res;
}
int big_int::compareAbs(const big_int &a, const big_int &b) {
    if (a.len == b.len) {
        for (int i = a.len - 1; i >= 0; i--)
            if (a.data[i] != b.data[i])
                return sgn(a.data[i] - b.data[i]);
        return 0;
    } else {
        return sgn(a.len - b.len);
    }
}
void big_int::plus(value_type a[], value_type b[], value_type c[], size_t l) {
    for (int i = 0; i < l; i++) {
        c[i] += a[i] + b[i];
        if (c[i] >= BIT_LIMIT) {
            c[i + 1]++;
            c[i] -= BIT_LIMIT;
        }
    }
}
void big_int::minus(value_type a[], value_type b[], value_type c[], size_t l) {
    for (int i = 0; i < l; i++) {
        c[i] += a[i] - b[i];
        if (c[i] < 0) {
            c[i + 1]--;
            c[i] += BIT_LIMIT;
        }
    }
}
void big_int::multiply(value_type a[], value_type b[], value_type c[], size_t al, size_t bl,
                       bool ntt) {
    if (ntt) {
        size_t n = al + bl, len = 2 << std::__lg(n - 1);
        NTT(a, len, 1);
        NTT(b, len, 1);
        for (int i = 0; i < len; i++)
            c[i] = 1LL * a[i] * b[i] % P;
        NTT(c, len, -1);
        NTT(a, len, -1);
        NTT(b, len, -1);
    } else {
        for (int i = 0; i < al; i++)
            for (int j = 0; j < bl; j++)
                c[i + j] += a[i] * b[j];
    }
    for (int i = 0; i < al + bl; i++) {
        c[i + 1] += c[i] / BIT_LIMIT;
        c[i] %= BIT_LIMIT;
    }
}
int big_int::ksm(int a, int b) {
    int s = 1;
    for (; b; b >>= 1, a = 1LL * a * a % P)
        if (b & 1)
            s = 1LL * s * a % P;
    return s;
}
void big_int::NTT(value_type a[], size_t n, int inv) {
    std::vector<size_t> rev(n);
    for (size_t i = 0; i < n; i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << std::__lg(n >> 1));
        if (i < rev[i])
            std::swap(a[i], a[rev[i]]);
    }
    for (size_t i = 2; i <= n; i <<= 1) {
        int gn = ksm(inv == 1 ? 3 : ksm(3, P - 2), (P - 1) / i);
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
        int n_inv = ksm(n, P - 2);
        for (int i = 0; i < n; i++)
            a[i] = 1LL * a[i] * n_inv % P;
    }
}

const size_t big_int::pow10[19] = {1, 10, 100, 1000, 10000};
const size_t big_int::MAX_SIZE = 1024;
const size_t big_int::BIT_SIZE = 4;
const size_t big_int::BIT_LIMIT = big_int::pow10[BIT_SIZE]; // MAX_SIZE * BIT_SIZE <= 2^64
const int big_int::P = 998244353;