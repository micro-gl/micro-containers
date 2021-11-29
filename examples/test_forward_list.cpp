#include <iostream>
#include <sstream>

#include <micro-containers/forward_list.h>

using std::to_string;

struct dummy_t {
    int a, b;
    explicit dummy_t(int $a=0, int $b=1) : a($a), b($b) {}
};

std::string to_string(const dummy_t& value) {
    std::ostringstream os;
    os << "{ a: " << value.a << ", b: " << value.b << "}";
    return os.str();
}

template<class Container>
void print_simple_container(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << to_string(item) << ", ";
    }
    std::cout << ")" << std::endl;
}

/*
void test_clear() {
    std::cout << "test_clear" << std::endl;
    linked_list<int> list{};
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);
    std::cout << "- list1 before clear" << std::endl;
    print_simple_container(list);
    list.clear();
    std::cout << "- list1 after clear" << std::endl;
    print_simple_container(list);
}

void test_erase() {
    std::cout << "test_erase" << std::endl;
    linked_list<int> list{};

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    print_simple_container(list);

    auto iter = list.begin(); // 1
    ++iter; // 2
    ++iter; // 3
    list.erase(iter);

    std::cout << "list1 after erase" << std::endl;
    print_simple_container(list);
}

void test_erase_range() {
    std::cout << "test_erase_range" << std::endl;
    linked_list<int> list{};

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    print_simple_container(list);

    auto iter_start = list.begin(); // 1
    auto iter_end = list.end(); // 1
    ++iter_start; // 2
    --iter_end;
    list.erase(iter_start, iter_end);

    std::cout << "list1 after move" << std::endl;
    print_simple_container(list);
}

void test_assign_copy() {
    std::cout << "test_assign_copy" << std::endl;
    linked_list<int> list{};
    linked_list<int> list2{};

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);
    list2 = list;
    print_simple_container(list2);
}

void test_assign_move() {
    std::cout << "test_assign_move" << std::endl;
    linked_list<int> list1{};
    linked_list<int> list2{};

    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    list1.push_back(4);
    list1.push_back(5);

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
    std::cout << "test_construct_copy" << std::endl;
    linked_list<int> list{};

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);
    linked_list<int> list2 = list;
    print_simple_container(list2);
}

void test_construct_move() {
    std::cout << "test_construct_move" << std::endl;
    linked_list<int> list1{};

    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    list1.push_back(4);
    list1.push_back(5);

    std::cout << "list1 before move" << std::endl;
    print_simple_container(list1);
    // move
    linked_list<int> list2 = std::move(list1);
    //
    std::cout << "list1 after move" << std::endl;
    print_simple_container(list1);
    //
    std::cout << "list2 after move" << std::endl;
    print_simple_container(list2);
}



void test_insert_multi() {
    std::cout << "test_insert_multi" << std::endl;
    linked_list<float> list1{};

    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    list1.push_back(4);
    list1.push_back(5);

    std::cout << "list1 before insert_node" << std::endl;
    print_simple_container(list1);

    // iterator
    auto iter = list1.begin(); // 1
    ++iter; // 2
    ++iter; // 3
    auto iter2 = list1.insert(iter, 5, 2.5f);

    std::cout << "list1 after insert_node" << std::endl;
    print_simple_container(list1);
}

void test_insert_with_iterator() {
    std::cout << "test_insert_with_iterator" << std::endl;
    linked_list<int> list1{};

    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    list1.push_back(4);
    list1.push_back(5);

    linked_list<int> list2{list1};

    std::cout << "list2 before insert_node" << std::endl;
    print_simple_container(list1);

    //
    auto iter_2 = list2.begin();
    ++iter_2; ++iter_2; // 3

    list2.insert(iter_2, list1.begin(), list1.end());

    std::cout << "list2 after insert_node" << std::endl;
    print_simple_container(list2);
}

void test_push_front() {
    std::cout << "test_push_front" << std::endl;
    linked_list<float> list1{};

    list1.push_front(1);
    list1.push_front(2);
    list1.push_front(3);
    list1.push_front(4);
    list1.push_front(5);

    print_simple_container(list1);
}

void test_back_and_front() {
    std::cout << "test_back_and_front" << std::endl;
    linked_list<float> list1{};

    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    list1.push_back(4);
    list1.push_back(5);

    std::cout << "front:" << list1.front() << std::endl;
    std::cout << "back:" << list1.back() << std::endl;
}


void test_emplace() {
    std::cout << "test_emplace" << std::endl;

    linked_list<dummy_t> list1{};

    list1.push_back(dummy_t());
    list1.push_back(dummy_t());
    list1.push_back(dummy_t());

    std::cout << "- before emplace" << std::endl;
    print_simple_container(list1);

    // emplace
    auto iter = list1.begin();
    iter++;
    list1.emplace(iter, 5, 6);
    list1.emplace_back(5, 6);
    list1.emplace_front(5, 6);

    std::cout << "- after emplace" << std::endl;
    print_simple_container(list1);
}

void test_pop_back_and_front() {
    std::cout << "test_pop_back_and_front" << std::endl;

    linked_list<int> list1{};

    list1.push_back(1);
    list1.push_back(2);
    list1.push_back(3);
    list1.push_back(4);
    list1.push_back(5);

    print_simple_container(list1);

    // pop back
    list1.pop_back();
    std::cout << "- after pop_back" << std::endl;
    print_simple_container(list1);

    // pop front
    list1.pop_front();
    std::cout << "- after pop_front" << std::endl;
    print_simple_container(list1);

    // pop all
    while (!list1.empty())
        list1.pop_back();

    std::cout << "- after loop pop_back" << std::endl;
    print_simple_container(list1);
}
 */

void test_push_front() {
    std::cout << "test_push_front" << std::endl;
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
    std::cout << "test_insert_after" << std::endl;
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
    std::cout << "test_insert_after_many_uniform_values" << std::endl;
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
    std::cout << "test_pop_front" << std::endl;
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
    std::cout << "test_emplace_front" << std::endl;
    forward_list<int> list1{};

    list1.emplace_front(5);
    list1.emplace_front(4);
    list1.emplace_front(3);
    std::cout << "- list1 " << std::endl;
    print_simple_container(list1);
}

void test_emplace_after() {
    std::cout << "test_emplace_after" << std::endl;
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
    std::cout << "test_erase_after" << std::endl;
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

int main() {
    // Modifiers
//    test_push_front();
//    test_pop_front();
//    test_insert_after();
//    test_insert_after_many_uniform_values();
//    test_emplace_front();
//    test_emplace_after();
    test_erase_after();

//    test_clear();
//    test_erase();
//    test_pop_back_and_front();
//    test_erase_range();
//    test_assign_copy();
//    test_assign_move();
//    test_insert_multi();
//    test_insert_with_iterator();
//    test_construct_copy();
//    test_construct_move();
//    test_push_front();
//    test_back_and_front();
//    test_emplace();
}
