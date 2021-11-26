#include <iostream>
#include <sstream>

#include <micro-containers/avl_tree.h>

void test_insert() {
    std::cout << "test_insert" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
}

void test_remove() {
    std::cout << "test_remove" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //
    avl.remove(100);
    avl.remove(50);
    avl.remove(150);
    avl.remove(250);
    avl.remove(350);
}

void test_clear() {
    std::cout << "test_clear" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    avl.clear();
}

void test_find() {
    std::cout << "test_find" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    const auto * node = avl.find(450);
}

void test_max_min() {
    std::cout << "test_max_min" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    const auto * node_max = avl.maximum();
    const auto * node_min = avl.minimum();
}

void test_successor() {
    std::cout << "test_successor" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    const auto * node_max = avl.maximum();
    const auto * current = avl.minimum();
    std::cout << "(";
    while (current!=node_max) {
        std::cout << current->key << ", ";
        current = avl.successor(current);
    }
    std::cout << current->key << ", ";
    std::cout << ")" << std::endl;
    current = avl.successor(current);

}

void test_predecessor() {
    std::cout << "test_predecessor" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    const auto * node_min = avl.minimum();
    const auto * current = avl.maximum();
    std::cout << "(";
    while (current!=node_min) {
        std::cout << current->key << ", ";
        current = avl.predecessor(current);
    }
    std::cout << current->key << ", ";
    std::cout << ")" << std::endl;
}

void test_contains() {
    std::cout << "test_contains" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    std::cout << avl.contains(50) << std::endl;
    std::cout << avl.contains(450) << std::endl;
    std::cout << avl.contains(54) << std::endl;
    std::cout << avl.contains(449) << std::endl;
}

void test_iterator() {
    std::cout << "test_iterator" << std::endl;

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    std::cout << "(";
    for (const auto & node : avl) {
        std::cout << node.key << ", ";
    }
    std::cout << ")" << std::endl;

    //
    auto current = avl.begin();
    const auto end = avl.end();
    std::cout << "(";
    do {
        std::cout << (*current).key << ", ";
    } while(++current!=end);
    std::cout << ")";
}

int main() {
//    test_insert();
//    test_remove();
//    test_clear();
//    test_find();
//    test_max_min();
//    test_successor();
//    test_predecessor();
//    test_contains();
    test_iterator();

    int b = 5;
    int * a = &b;
    int & c = *a;
}

