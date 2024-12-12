## 项目说明

### 项目名称
BigInt

### 项目描述
本项目实现了一个大整数类 `big_uint` 和带符号的大整数类 `big_int`，支持基本的算术运算和比较运算。这些类使用动态数组存储大整数的每一位，并提供了多种构造函数、赋值操作符以及输入输出功能。此外，项目还包含了一个快速数论变换（NTT）的实现，用于优化大整数的乘法运算。

### 文件结构
- `BigUIntStd.h`: 包含 `big_uint` 类的定义和实现。
- `BigInt.h`: 包含 `big_int` 类的定义和实现。
- `ntt.h`: 包含快速数论变换（NTT）的实现。

### 主要功能
1. **构造函数**
   - `big_uint()` 和 `big_int()`: 默认构造函数。
   - `big_uint(const big_uint &t)` 和 `big_int(const big_int &t)`: 拷贝构造函数。
   - `big_uint(big_uint &&t)` 和 `big_int(big_int &&t)`: 移动构造函数。
   - `big_uint(value_type num)` 和 `big_int(value_type num)`: 从整数构造。
   - `big_uint(const std::string &num)` 和 `big_int(const std::string &num)`: 从字符串构造。

2. **析构函数**
   - `~big_uint()` 和 `~big_int()`: 释放动态分配的内存。

3. **赋值操作符**
   - `operator=(const big_uint &t)` 和 `operator=(const big_int &t)`: 拷贝赋值。
   - `operator=(big_uint &&t)` 和 `operator=(big_int &&t)`: 移动赋值。
   - `operator=(value_type num)`: 从整数赋值。
   - `operator=(const std::string &num)`: 从字符串赋值。

4. **输入输出**
   - `friend std::ostream &operator<<(std::ostream &os, const big_uint &t)`: 输出 `big_uint`。
   - `friend std::ostream &operator<<(std::ostream &os, const big_int &t)`: 输出 `big_int`。

5. **算术运算**
   - `operator+=(const big_uint &t)` 和 `operator+=(const big_int &t)`: 加法赋值。
   - `operator-=(const big_uint &t)` 和 `operator-=(const big_int &t)`: 减法赋值。
   - `operator*=(const big_uint &t)` 和 `operator*=(const big_int &t)`: 乘法赋值。
   - `operator+(const big_uint &a, const big_uint &b)` 和 `operator+(const big_int &a, const big_int &b)`: 加法。
   - `operator-(const big_uint &a, const big_uint &b)` 和 `operator-(const big_int &a, const big_int &b)`: 减法。
   - `operator*(const big_uint &a, const big_uint &b)` 和 `operator*(const big_int &a, const big_int &b)`: 乘法。

6. **比较运算**
   - `operator==(const big_uint &a, const big_uint &b)` 和 `operator==(const big_int &a, const big_int &b)`: 相等比较。
   - `operator!=(const big_uint &a, const big_uint &b)` 和 `operator!=(const big_int &a, const big_int &b)`: 不等比较。
   - `operator<(const big_uint &a, const big_uint &b)` 和 `operator<(const big_int &a, const big_int &b)`: 小于比较。
   - `operator<=(const big_uint &a, const big_uint &b)` 和 `operator<=(const big_int &a, const big_int &b)`: 小于等于比较。

7. **辅助函数**
   - `copyByString(const std::string &num)`: 从字符串复制数据。
   - `sgn(value_type x)`: 返回符号。
   - `compareAbs(const big_uint &a, const big_uint &b)` 和 `compareAbs(const big_int &a, const big_int &b)`: 比较绝对值。
   - `plus(value_type a[], value_type b[], size_t al, size_t bl)`: 加法。
   - `minus(value_type a[], value_type b[], size_t al, size_t bl)`: 减法。
   - `multiply(value_type a[], value_type b[], size_t al, size_t bl, bool ntt)`: 乘法。
   - `NTT(value_type a[], size_t n, int inv)`: 快速数论变换。
   - `ksm(int a, int b, int P)`: 快速幂。
   - `inv(int a, int P)`: 模逆元。

8. **快速数论变换（NTT）**
   - `ntt.h` 中实现了快速数论变换（NTT），用于优化大整数的乘法运算。
   - `CRT` 结构体用于中国剩余定理（CRT）的实现，将结果映射回原始模数。
   - `NTT(CRT a[], size_t n, int inverse)`: 执行 NTT 变换。



### 高精度运算空间

`using value_type = int;`

`sizeof(value_type) = 4 byte`

| 内存空间 | 数组长度（十进制位数） | len           | 运算范围 | 计算结果空间 |
| -------- | ---------------------- | ------------- | -------- | ------------ |
| 4KB      | num[1024]              | 3,401         | 2↑2↑11   | 1KB          |
| 4MB      | num[1024\*1024]        | 3,483,294     | 2↑2↑21   | 1MB          |
| 4GB      | num[1024\*1024\*1024]  | 3,566,893,131 | 2↑2↑31   | 1GB          |

### 部分计算结果

| n            | head(10)   | len       |
| ------------ | ---------- | --------- |
| $2^{2^{0}}$  | 2          | 1         |
| $2^{2^{1}}$  | 4          | 1         |
| $2^{2^{2}}$  | 16         | 2         |
| $2^{2^{3}}$  | 256        | 3         |
| $2^{2^{4}}$  | 65536      | 5         |
| $2^{2^{5}}$  | 4294967296 | 10        |
| $2^{2^{6}}$  | 1844674407 | 20        |
| $2^{2^{7}}$  | 3402823669 | 39        |
| $2^{2^{8}}$  | 1157920892 | 78        |
| $2^{2^{9}}$  | 1340780792 | 155       |
| $2^{2^{10}}$ | 1797693134 | 309       |
| $2^{2^{11}}$ | 3231700607 | 617       |
| $2^{2^{12}}$ | 1044388881 | 1234      |
| $2^{2^{13}}$ | 1090748135 | 2467      |
| $2^{2^{14}}$ | 1189731495 | 4933      |
| $2^{2^{15}}$ | 1415461031 | 9865      |
| $2^{2^{16}}$ | 2003529930 | 19729     |
| $2^{2^{17}}$ | 4014132182 | 39457     |
| $2^{2^{18}}$ | 1611325717 | 78914     |
| $2^{2^{19}}$ | 2596370567 | 157827    |
| $2^{2^{20}}$ | 6741140125 | 315653    |
| $2^{2^{21}}$ | 4544297019 | 631306    |
| $2^{2^{22}}$ | 2065063539 | 1262612   |
| $2^{2^{23}}$ | 4264487423 | 2525223   |
| $2^{2^{24}}$ | 1818585298 | 5050446   |
| $2^{2^{25}}$ | 3307252488 | 10100891  |
| $2^{2^{26}}$ | 1093791902 | 20201782  |
| $2^{2^{27}}$ | 1196380724 | 40403563  |
| $2^{2^{28}}$ | 1431326839 | 80807125  |
| $2^{2^{29}}$ | 2048696520 | 161614249 |


### 使用示例
```cpp
#include "BigUIntStd.h"
#include "BigInt.h"
#include "ntt.h"
#include <iostream>

int main() {
    big_uint a("12345678901234567890");
    big_uint b("98765432109876543210");

    std::cout << "a: " << a << std::endl;
    std::cout << "b: " << b << std::endl;

    big_uint sum = a + b;
    std::cout << "a + b: " << sum << std::endl;

    big_int c("-12345678901234567890");
    big_int d("98765432109876543210");

    std::cout << "c: " << c << std::endl;
    std::cout << "d: " << d << std::endl;

    big_int diff = c - d;
    std::cout << "c - d: " << diff << std::endl;

    return 0;
}
```

### 编译与运行
1. 使用 C++11 或更高版本的编译器。
2. 编译命令示例：
   ```sh
   g++ -std=c++11 -o bigint_example main.cpp BigUIntStd.h BigInt.h ntt.h
   ```
3. 运行程序：
   ```sh
   ./bigint_example
   ```

### 注意事项
- 动态数组的管理需要注意内存泄漏问题。
- 大整数的乘法使用了快速数论变换（NTT）以提高效率。
- 字符串输入的格式需要符合大整数的表示规则。

### 贡献者
- Tanphoon

### 许可证
- 本项目采用 MIT 许可证，详情见 LICENSE 文件。