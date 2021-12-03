#include "src/test_utils.h"
#include <micro-containers/array.h>


void test_1() {
    print_test_header("test_1");

    array<int, 2> a = {0,1};
    array<int, 2> b = {2,3};

    a=b;

    print_simple_container(a);
}

int main() {
    test_1();
}

