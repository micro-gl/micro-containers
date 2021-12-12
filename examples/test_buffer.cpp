#include "src/test_utils.h"
#include <micro-containers/buffer.h>

using namespace microc;

void test_1() {
    print_test_header("test_insert");
    print_simple_container(vec);
}

int main() {
    // modifiers
//    test_insert();
//    test_erase();
    test_1();
}

