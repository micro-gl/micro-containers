#include "src/test_utils.h"
#include <micro-containers/queue.h>
#include <micro-containers/linked_list.h>
#include <micro-containers/circular_array.h>

void test_1() {
    print_test_header("test_1");

//    using stack_t = queue<int, linked_list<int>>;
    using stack_t = queue<int, circular_array<int>>;

    stack_t s;

    s.push(1);
    print_simple_container(s.get_container());
    s.push(2);
    print_simple_container(s.get_container());
    s.push(3);
    print_simple_container(s.get_container());
    s.push(4);
    print_simple_container(s.get_container());
    s.push(5);
    print_simple_container(s.get_container());

    s.pop();
    print_simple_container(s.get_container());
    s.pop();
    print_simple_container(s.get_container());
    s.pop();
    print_simple_container(s.get_container());
    s.pop();
    print_simple_container(s.get_container());
    s.pop();

    print_simple_container(s.get_container());
}

int main() {
    test_1();
}

