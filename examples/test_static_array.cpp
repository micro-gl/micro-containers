#include <iostream>
#include <sstream>

#include "src/test_utils.h"
#include <micro-containers/traits.h>
#include <micro-containers/static_array.h>

void test_insert() {
    print_test_header("test_insert");

    using c = static_array<int, 5>;
    c d;
    d.push_back(0);
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);
    d.push_back(4);

    std::cout << "- printing array" << std::endl;
    print_simple_container(d);

    d[2] = 22;
    std::cout << "- printing array after d[2] = 22" << std::endl;
    print_simple_container(d);
}

void test_emplace() {
    print_test_header("test_emplace");

    using c = static_array<dummy_t, 3>;
    c d;
    d.push_back(dummy_t(0,1));
    d.emplace_back(1,2);
    d.emplace_back(2,3);

    std::cout << "- printing array" << std::endl;
    print_simple_container(d);
}


int main() {
    // modifiers
    test_insert();
    test_emplace();

    auto aaa= micro_containers::traits::is_allocator_aware<static_array<int, 5>>::value;
    auto aaa2= micro_containers::traits::is_allocator_aware<dummy_t>::value;
}

