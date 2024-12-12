#include "BigInt.h"
#include "test.h"

#include <Timer.h>
#include <iostream>

int main() {
    test<big_int>();
    randtest<big_int>;
    return 0;
}