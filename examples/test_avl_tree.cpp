#include "src/test_utils.h"
#include <micro-containers/avl_tree.h>

void test_insert() {
    print_test_header("test_insert");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);

    std::cout << "- tree after insertion: " << std::endl;
    print_simple_container(avl);
}

void test_remove() {
    print_test_header("test_remove");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);

    std::cout << "- tree after insertion: " << std::endl;
    print_simple_container(avl);
    //
    avl.remove(100);
    avl.remove(50);
    avl.remove(150);
    avl.remove(250);
    avl.remove(350);

    std::cout << "- tree after removal: " << std::endl;
    print_simple_container(avl);
}

void test_clear() {
    print_test_header("test_clear");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //
    std::cout << "- tree after insertion: " << std::endl;
    print_simple_container(avl);

    avl.clear();

    std::cout << "- tree after clear: " << std::endl;
    print_simple_container(avl);

}

void test_find() {
    print_test_header("test_find");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //

    std::cout << "- tree after insertion: " << std::endl;
    print_simple_container(avl);

    auto iterator = avl.find(450);
}

void test_max_min() {
    print_test_header("test_max_min");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //
    std::cout << "- tree after insertion: " << std::endl;
    print_simple_container(avl);

    const auto iter_max = avl.maximum();
    const auto iter_min = avl.minimum();
    std::cout << "- Maximum: " << *iter_max << std::endl;
    std::cout << "- Minimum: " << *iter_min << std::endl;
}

void test_successor() {
    print_test_header("test_successor");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //
    std::cout << "- tree after insertion: " << std::endl;
    print_simple_container(avl);

    auto iter_max = avl.maximum();
    auto iter_current = avl.minimum();

    std::cout << "- tree with insertion: " << std::endl;

    std::cout << "(";
    while (iter_current != iter_max) {
        std::cout << *(iter_current) << ", ";
        ++iter_current;
    }
    std::cout << *(iter_current) << ", ";
    std::cout << ")" << std::endl;
    ++iter_current;
}

void test_predecessor() {
    print_test_header("test_predecessor");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl;

    avl.insert(100);
    avl.insert(50);
    avl.insert(150);
    avl.insert(250);
    avl.insert(350);
    avl.insert(450);
    //
    std::cout << "- tree after insertion: " << std::endl;
    print_simple_container(avl);

    auto iter_min = avl.minimum();
    auto iter_current = avl.maximum();

    std::cout << "- tree with insertion: " << std::endl;

    std::cout << "(";
    while (iter_current != iter_min) {
        std::cout << *(iter_current) << ", ";
        --iter_current;
    }
    std::cout << *(iter_current) << ", ";
    std::cout << ")" << std::endl;
    ++iter_current;
}

void test_contains() {
    print_test_header("test_contains");

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
    print_test_header("test_iterator");

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
        std::cout << node << ", ";
    }
    std::cout << ")" << std::endl;

    //
    auto current = avl.begin();
    const auto end = avl.end();
    std::cout << "(";
    do {
        std::cout << (*current) << ", ";
    } while(++current!=end);
    std::cout << ")";
}

void test_copy_and_move_ctor() {
    print_test_header("test_copy_and_move_ctor");

    using avl_t = avl_tree<int, std::less<int>>;
    avl_t avl1;

    avl1.insert(100);
    avl1.insert(50);
    avl1.insert(150);
    avl1.insert(250);
    avl1.insert(350);
    avl1.insert(450);
    //

    std::cout << "- printing avl_1" << std::endl;
    print_simple_container(avl1);

    // copy ctor of avl2
    avl_t avl2 = avl1;
    std::cout << "- printing copy-constructed avl_2" << std::endl;
    print_simple_container(avl2);

    // move ctor of avl3
    avl_t avl3 = std::move(avl1);
    std::cout << "- printing move-constructed avl_3 from avl_1" << std::endl;
    print_simple_container(avl3);
    std::cout << "- printing moved avl_1" << std::endl;
    print_simple_container(avl1);
}

void test_copy_and_move_assignment() {
    print_test_header("test_copy_and_move_assignment");

    using avl_t = avl_tree<int>;
    avl_t avl1, avl2, avl3;

    avl1.insert(100);
    avl1.insert(50);
    avl1.insert(150);
    avl1.insert(250);
    avl1.insert(350);
    avl1.insert(450);
    //

    std::cout << "- printing avl_1" << std::endl;
    print_simple_container(avl1);

    // copy-assign of avl2
    avl2 = avl1;
    std::cout << "- printing copy-assigned avl_2" << std::endl;
    print_simple_container(avl2);

    // move-assigned of avl3
    avl3 = std::move(avl1);
    std::cout << "- printing move-assigned avl_3 from avl_1" << std::endl;
    print_simple_container(avl3);
    std::cout << "- printing moved avl_1" << std::endl;
    print_simple_container(avl1);
}

int main() {
    test_insert();
    test_remove();
    test_clear();
    test_find();
    test_max_min();
    test_successor();
    test_predecessor();
    test_contains();
    test_iterator();
    test_copy_and_move_ctor();
    test_copy_and_move_assignment();

}

