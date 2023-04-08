#include "common.h"

void test_fun1() {
    ASSERT(0);
}

void test_fun() {
    test_fun1();
}

int main() {
    test_fun();
}