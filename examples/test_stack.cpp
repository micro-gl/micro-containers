#include "src/test_utils.h"
#include <micro-containers/stack.h>
#include <micro-containers/static_array.h>

void test_1() {
    print_test_header("test_1");

    using stack_t = stack<int, static_array<int, 100>>;

    stack_t s;

    s.push(1);
    s.push(2);
    s.push(3);
    s.push(4);
    s.push(5);
    print_simple_container(s.get_container());

    s.pop();
    s.pop();
    s.pop();
    s.pop();
    s.pop();

    print_simple_container(s.get_container());
}

int main() {
    test_1();
}

