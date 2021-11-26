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

int main() {
//    test_insert();
//    test_remove();
//    test_clear();
//    test_find();
    test_max_min();

}

