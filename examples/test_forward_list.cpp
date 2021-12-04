#include "src/test_utils.h"
#include <micro-containers/forward_list.h>

using namespace microc;

void test_push_front() {
    print_test_header("test_push_front");
    forward_list<int> list1{};

    list1.push_front(1);
    list1.push_front(2);
    list1.push_front(3);
    list1.push_front(4);
    list1.push_front(5);

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);
}

void test_insert_after() {
    print_test_header("test_insert_after");
    forward_list<int> list1{};

    list1.push_front(1);
    list1.push_front(2);
    list1.push_front(3);
    list1.push_front(4);
    list1.push_front(6);

    auto iter_pos = list1.begin();

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    //
    list1.insert_after(iter_pos, 5);
    std::cout << "- list1 after inserting 5 after 6" << std::endl;
    print_simple_container(list1);
}

void test_insert_after_many_uniform_values() {
    print_test_header("test_insert_after_many_uniform_values");
    forward_list<int> list1{};

    list1.push_front(10);
    list1.push_front(20);
    list1.push_front(30);
    list1.push_front(40);
    list1.push_front(60);

    auto iter_pos = ++list1.begin();

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    //
    list1.insert_after(iter_pos, 5, 35);
    std::cout << "- list1 after inserting " << std::endl;
    print_simple_container(list1);
}

void test_pop_front() {
    print_test_header("test_pop_front");
    forward_list<int> list1{};

    list1.push_front(10);
    list1.push_front(20);
    list1.push_front(30);
    list1.push_front(40);
    list1.push_front(60);

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    //
    list1.pop_front();
    list1.pop_front();
    list1.pop_front();

    std::cout << "- list1 after pop fron 3 times " << std::endl;
    print_simple_container(list1);
}

void test_emplace_front() {
    print_test_header("test_emplace_front");
    forward_list<int> list1{};

    list1.emplace_front(5);
    list1.emplace_front(4);
    list1.emplace_front(3);
    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);
}

void test_emplace_after() {
    print_test_header("test_emplace_after");
    forward_list<int> list1{};

    list1.emplace_front(15);
    list1.emplace_front(4);
    list1.emplace_front(3);
    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    auto iter_pos = ++list1.begin();
    iter_pos = list1.emplace_after(iter_pos, 10);
    list1.emplace_after(iter_pos, 12);

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);
}

void test_erase_after() {
    print_test_header("test_erase_after");
    forward_list<int> list1{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    auto iter_pos = ++list1.begin();
    iter_pos = list1.erase_after(iter_pos);

    std::cout << "- list1 after single erase after 3" << std::endl;
    print_simple_container(list1);

    list1.erase_after(list1.begin(), (list1.begin()+3));

    std::cout << "- list1 after range erase" << std::endl;
    print_simple_container(list1);
}

void test_clear() {
    print_test_header("test_clear");
    forward_list<int> list1{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    list1.clear();

    std::cout << "- list1 after clear" << std::endl;
    print_simple_container(list1);
}

void test_empty() {
    print_test_header("test_empty");
    forward_list<int> list1{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    std::cout << "- is list empty ? " << list1.empty() << std::endl;

    list1.clear();
    std::cout << "- list1 after clear" << std::endl;
    print_simple_container(list1);

    std::cout << "- is list empty ? " << list1.empty() << std::endl;
}

void test_front() {
    print_test_header("test_front");
    forward_list<int> list1{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);

    std::cout << "- front:  " << list1.front() << std::endl;
}

void test_assign_copy() {
    print_test_header("test_assign_copy");
    forward_list<int> list1{};
    forward_list<int> list2{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "- list 1: " << std::endl;
    print_simple_container(list1);

    list2 = list1;
    std::cout << "- list2 after list2=list1" << std::endl;
    print_simple_container(list2);
}

void test_assign_move() {
    print_test_header("test_assign_move");
    forward_list<int> list1{};
    forward_list<int> list2{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "list1 before move" << std::endl;
    print_simple_container(list1);
    // move
    list2 = std::move(list1);
    //
    std::cout << "list1 after move" << std::endl;
    print_simple_container(list1);
    //
    std::cout << "list2 after move" << std::endl;
    print_simple_container(list2);
}

void test_construct_copy() {
    print_test_header("test_construct_copy");
    forward_list<int> list1{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "- list1" << std::endl;
    print_simple_container(list1);

    std::cout << "- list2 after copy-construction" << std::endl;
    forward_list<int> list2 = list1;
    print_simple_container(list2);
}

void test_construct_move() {
    print_test_header("test_construct_move");
    forward_list<int> list1{};

    list1.emplace_front(9);
    list1.emplace_front(7);
    list1.emplace_front(5);
    list1.emplace_front(3);
    list1.emplace_front(1);

    std::cout << "list1 before move" << std::endl;
    print_simple_container(list1);
    // move
    forward_list<int> list2 = std::move(list1);
    //
    std::cout << "list1 after move" << std::endl;
    print_simple_container(list1);
    //
    std::cout << "list2 after move" << std::endl;
    print_simple_container(list2);
}

int main() {
    // Modifiers
    test_push_front();
    test_pop_front();
    test_insert_after();
    test_insert_after_many_uniform_values();
    test_emplace_front();
    test_emplace_after();
    test_erase_after();
    test_clear();

    // Capacity
    test_empty();

    // Element access
    test_front();

    // ctors and assignment
    test_assign_copy();
    test_assign_move();
    test_construct_copy();
    test_construct_move();
}

