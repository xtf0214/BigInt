#include "BigUInt.h"
#include <ProgressBar.h>
#include "test.h"
#include <Timer.h>
#include <fstream>
#include <iostream>

void task() {
    std::ofstream out("biguint.txt");
    big_uint a(2);
    out << "2^(2^0) " << a.estimation(10) << std::endl;
    ProgressBar tqdm(1, 32);
    for (int i = 1; i <= 28; tqdm(i++)) {
        a *= a;
        out << "2^(2^" << i << ") " << a.estimation(10) << std::endl;
    }
    out.close();
}

signed main() {
    task();
    randtest<big_uint>();
    return 0;
}
