#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

const size_t pow10[19] = {1, 10, 100, 1000, 10000};
const size_t MAX_SIZE = 1024;
const size_t BIT_SIZE = 4;
const size_t BIT_LIMIT = pow10[BIT_SIZE];
const int P = 998244353;

class big_int {
  public:
    using value_type = int;

    // 构造函数
    big_int();
    big_int(const big_int &t);
    big_int(big_int &&t);
    big_int(value_type num);
    big_int(const std::string &num);

    // 析构函数
    ~big_int();

    // 赋值操作符
    big_int &operator=(const big_int &t);
    big_int &operator=(big_int &&t);
    big_int &operator=(value_type num);
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
    // 内部辅助函数
    void copyByString(const std::string &num);
    static int sgn(value_type x);
    static int compareAbs(const big_int &a, const big_int &b);
    static void plus(value_type a[], value_type b[], size_t al, size_t bl);
    static void minus(value_type a[], value_type b[], size_t al, size_t bl);
    static void multiply(value_type a[], value_type b[], size_t al, size_t bl, bool ntt);
    static void NTT(value_type a[], size_t n, int inv);
    static int ksm(int a, int b, int P);
    static int inv(int a, int P);

    void resize(size_t new_capacity);

    // 成员变量
    size_t len = 0;
    value_type signum = 0;
    size_t capacity = 4; // 默认容量为4
    value_type *data = nullptr;
};
// 构造函数
big_int::big_int() : data(new value_type[capacity]{0}) {}
big_int::big_int(const big_int &t)
    : signum(t.signum), len(t.len), capacity(t.capacity), data(new value_type[capacity]{0}) {
    std::copy(t.data, t.data + t.len, data);
}
big_int::big_int(big_int &&t) : signum(t.signum), len(t.len), capacity(t.capacity), data(t.data) {
    t.signum = 0;
    t.len = 0;
    t.capacity = 0;
    t.data = nullptr;
}
big_int::big_int(value_type num) : signum(sgn(num)), data(new value_type[capacity]{0}) {
    num *= signum;
    while (num) {
        data[len++] = num % BIT_LIMIT;
        num /= BIT_LIMIT;
        if (len == capacity) {
            resize(capacity * 2);
        }
    }
}
big_int::big_int(const std::string &num) : data(new value_type[capacity]{0}) { copyByString(num); }
// 析构函数
big_int::~big_int() { delete[] data; }

void big_int::resize(size_t new_capacity) {
    if (new_capacity <= capacity)
        return;
    value_type *new_data = new value_type[new_capacity]{0};
    std::copy(data, data + len, new_data);
    delete[] data;
    data = new_data;
    capacity = new_capacity;
}
// 赋值运算符
big_int &big_int::operator=(const big_int &t) {
    if (this == &t)
        return *this;
    delete[] data;
    signum = t.signum;
    len = t.len;
    capacity = t.capacity;
    data = new value_type[capacity]{0};
    std::copy(t.data, t.data + t.len, data);
    return *this;
}
big_int &big_int::operator=(big_int &&t) {
    if (this == &t)
        return *this;
    delete[] data;
    signum = t.signum;
    len = t.len;
    capacity = t.capacity;
    data = t.data;
    t.signum = 0;
    t.len = 0;
    t.capacity = 0;
    t.data = nullptr;
    return *this;
}
big_int &big_int::operator=(value_type num) {
    delete[] data;
    signum = sgn(num);
    num *= signum;
    data = new value_type[capacity]{0};
    len = 0;
    while (num) {
        data[len++] = num % BIT_LIMIT;
        num /= BIT_LIMIT;
        if (len == capacity) {
            resize(capacity * 2);
        }
    }
    return *this;
}
big_int &big_int::operator=(const std::string &num) {
    delete[] data;
    capacity = 4;
    data = new value_type[capacity]{0};
    copyByString(num);
    return *this;
}
// 输出
std::ostream &operator<<(std::ostream &os, const big_int &t) {
    if (t.signum == -1)
        os << '-';
    if (t.len == 0)
        return os << 0;
    for (int i = t.len - 1; i >= 0; i--)
        os << t.data[i];
    return os;
}
// 算术运算
big_int operator+(const big_int &a, const big_int &b) {
    big_int c;
    if (a.signum == b.signum) {
        c = a;
        c.len = std::max(c.len, b.len);
        c.resize(c.len + 1);
        big_int::plus(c.data, b.data, c.len, b.len);
        c.len++;
    } else {
        int cmp = big_int::compareAbs(a, b);
        if (cmp == 1) {
            c = a;
            c.resize(c.len);
            big_int::minus(c.data, b.data, c.len, b.len);
        } else if (cmp == -1) {
            c = b;
            c.resize(c.len);
            big_int::minus(c.data, a.data, c.len, a.len);
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
        c = a;
        c.len = std::max(a.len, b.len);
        c.resize(c.len + 1);
        big_int::plus(c.data, b.data, c.len, b.len);
        c.len++;
    } else {
        int cmp = big_int::compareAbs(a, b);
        if (cmp == 1) {
            c = a;
            c.signum = 1;
            c.resize(c.len);
            big_int::minus(c.data, b.data, c.len, b.len);
        } else if (cmp == -1) {
            c = b;
            c.signum = -1;
            c.resize(c.len);
            big_int::minus(c.data, a.data, c.len, a.len);
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
        c = a;
        c.signum = a.signum * b.signum;
        c.len = a.len + b.len;
        c.resize(c.len * 4);
        big_int::multiply(c.data, b.data, c.len, b.len, a.len + b.len > 128);
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
big_int pow(big_int a, long long b) {
    big_int res(1);
    for (; b; a *= a, b >>= 1) {
        if (b & 1)
            res *= a;
    }
    return res;
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
        resize(len);
        for (size_t i = 0, j = num.size() - 1; i < n; i++, j--) {
            assert(num[j] >= '0' && num[j] <= '9');
            data[i / BIT_SIZE] += (num[j] - '0') * pow10[i % BIT_SIZE];
        }
    }
}
int big_int::sgn(value_type x) { return (x > 0) - (x < 0); }
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
void big_int::plus(value_type a[], value_type b[], size_t al, size_t bl) {
    for (int i = 0; i < al; i++) {
        if (i < bl)
            a[i] += b[i];
        if (a[i] >= BIT_LIMIT) {
            a[i + 1]++;
            a[i] -= BIT_LIMIT;
        }
    }
}
void big_int::minus(value_type a[], value_type b[], size_t al, size_t bl) {
    for (int i = 0; i < al; i++) {
        if (i < bl)
            a[i] -= b[i];
        if (a[i] < 0) {
            a[i + 1]--;
            a[i] += BIT_LIMIT;
        }
    }
}
void big_int::multiply(value_type a[], value_type b[], size_t al, size_t bl, bool ntt) {
    if (ntt) {
        size_t len = 2 << std::__lg(al + bl - 1);
        value_type *t = new value_type[len];
        std::copy(b, b + bl, t);
        NTT(a, len, 1);
        NTT(t, len, 1);
        for (int i = 0; i < len; i++)
            a[i] = 1LL * a[i] * b[i] % P;
        NTT(a, len, -1);
        delete[] t;
    } else {
        if (al == 0 || bl == 0) {
            std::fill(a, a + al, 0);
            return;
        }
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
int big_int::ksm(int a, int b, int P) {
    int s = 1;
    for (; b; b >>= 1, a = 1LL * a * a % P)
        if (b & 1)
            s = 1LL * s * a % P;
    return s;
}
int big_int::inv(int a, int P) { return big_int::ksm(a, P - 2, P); }
void big_int::NTT(value_type a[], size_t n, int inv) {
    std::vector<size_t> rev(n);
    for (size_t i = 0; i < n; i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << std::__lg(n >> 1));
        if (i < rev[i])
            std::swap(a[i], a[rev[i]]);
    }
    for (size_t i = 2; i <= n; i <<= 1) {
        int gn = ksm(inv == 1 ? 3 : big_int::inv(3, P), (P - 1) / i, P);
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
        int n_inv = big_int::inv(n, P);
        for (int i = 0; i < n; i++)
            a[i] = 1LL * a[i] * n_inv % P;
    }
}
