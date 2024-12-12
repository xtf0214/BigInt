#include "NTT.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>

const size_t pow10[19] = {1, 10, 100, 1000, 10000};
const size_t BIT_SIZE = 4;
const size_t BIT_LIMIT = pow10[BIT_SIZE];
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
    static void plus(value_type a[], value_type b[], size_t al, size_t bl);
    static void minus(value_type a[], value_type b[], size_t al, size_t bl);
    static void multiply(value_type a[], value_type b[], size_t al, size_t bl, bool ntt);

    void resize(size_t new_capacity);

    // 成员变量
    size_t len = 0;
    size_t capacity = 4; // 默认容量为4
    value_type *data = nullptr;
};
// 构造函数
big_uint::big_uint() : data(new value_type[capacity]{0}) {}

big_uint::big_uint(const big_uint &t)
    : len(t.len), capacity(t.capacity), data(new value_type[t.capacity]{0}) {
    std::copy(t.data, t.data + t.len, data);
}

big_uint::big_uint(big_uint &&t) : len(t.len), capacity(t.capacity), data(t.data) {
    t.len = 0;
    t.capacity = 0;
    t.data = nullptr;
}

big_uint::big_uint(value_type num) : data(new value_type[capacity]{0}) {
    while (num) {
        data[len++] = num % BIT_LIMIT;
        num /= BIT_LIMIT;
        if (len == capacity) {
            resize(capacity * 2);
        }
    }
}

big_uint::big_uint(const std::string &num) : data(new value_type[capacity]{0}) {
    copyByString(num);
}

// 析构函数
big_uint::~big_uint() { delete[] data; }

void big_uint::resize(size_t new_capacity) {
    if (new_capacity <= capacity)
        return;
    value_type *new_data = new value_type[new_capacity]{0};
    std::copy(data, data + len, new_data);
    delete[] data;
    data = new_data;
    capacity = new_capacity;
}
// 赋值运算符
big_uint &big_uint::operator=(const big_uint &t) {
    if (this == &t)
        return *this;
    delete[] data;
    len = t.len;
    capacity = t.capacity;
    data = new value_type[capacity]{0};
    std::copy(t.data, t.data + t.len, data);
    return *this;
}

big_uint &big_uint::operator=(big_uint &&t) {
    if (this == &t)
        return *this;
    delete[] data;
    len = t.len;
    capacity = t.capacity;
    data = t.data;
    t.len = 0;
    t.capacity = 0;
    t.data = nullptr;
    return *this;
}

big_uint &big_uint::operator=(value_type num) {
    delete[] data;
    capacity = 4;
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

big_uint &big_uint::operator=(const std::string &num) {
    delete[] data;
    capacity = 4;
    data = new value_type[capacity]{0};
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
    resize(len * 2);
    plus(data, t.data, len, t.len);
    while (len && !data[len - 1])
        len--;
    return *this;
}

big_uint &big_uint::operator-=(const big_uint &t) {
    assert(t <= *this);
    resize(len * 2);
    minus(data, t.data, len, t.len);
    while (len && !data[len - 1])
        len--;
    return *this;
}

big_uint &big_uint::operator*=(const big_uint &t) {
    resize((len + t.len) * 4);
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
        resize(len);
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
void big_uint::plus(value_type a[], value_type b[], size_t al, size_t bl) {
    for (int i = 0; i < al; i++) {
        if (i < bl)
            a[i] += b[i];
        if (a[i] >= BIT_LIMIT) {
            a[i + 1]++;
            a[i] -= BIT_LIMIT;
        }
    }
}
void big_uint::minus(value_type a[], value_type b[], size_t al, size_t bl) {
    for (int i = 0; i < al; i++) {
        if (i < bl)
            a[i] -= b[i];
        if (a[i] < 0) {
            a[i + 1]--;
            a[i] += BIT_LIMIT;
        }
    }
}
void big_uint::multiply(value_type a[], value_type b[], size_t al, size_t bl, bool ntt) {
    if (ntt) {
        size_t len = 2 << std::__lg(al + bl - 1);
        NTT::CRT *A = new NTT::CRT[len], *B = new NTT::CRT[len];
        for (int i = 0; i < al; i++)
            A[i] = NTT::CRT(a[i]);
        for (int i = 0; i < bl; i++)
            B[i] = NTT::CRT(b[i]);
        NTT::NTT(A, len, 1);
        NTT::NTT(B, len, 1);
        for (int i = 0; i < len; i++)
            A[i] = A[i] * B[i];
        NTT::NTT(A, len, -1);
        std::fill(a, a + al, 0);
        const int margin = 20;
        for (int i = 0; i < len - margin; ++i) {
            long long val = A[i].get();
            for (int j = i; val > 0 && j < len; ++j) {
                a[j] += val % BIT_LIMIT;
                if (a[j] >= BIT_LIMIT) {
                    a[j] -= BIT_LIMIT;
                    a[j + 1] += 1;
                }
                val /= BIT_LIMIT;
            }
        }
        delete[] A;
        delete[] B;
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
        for (int i = 0; i < al + bl; i++) {
            a[i + 1] += a[i] / BIT_LIMIT;
            a[i] %= BIT_LIMIT;
        }
    }
}
