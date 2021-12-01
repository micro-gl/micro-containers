#include <iostream>
#include <sstream>

#include <micro-containers/avl_tree.h>

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
void print_tree(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << to_string(item) << ", ";
    }
    std::cout << ")" << std::endl;
}

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

    std::cout << "- tree after insertion: " << std::endl;
    print_tree(avl);
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

    std::cout << "- tree after insertion: " << std::endl;
    print_tree(avl);
    //
    avl.remove(100);
    avl.remove(50);
    avl.remove(150);
    avl.remove(250);
    avl.remove(350);

    std::cout << "- tree after removal: " << std::endl;
    print_tree(avl);
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
    std::cout << "- tree after insertion: " << std::endl;
    print_tree(avl);

    avl.clear();

    std::cout << "- tree after clear: " << std::endl;
    print_tree(avl);

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

    std::cout << "- tree after insertion: " << std::endl;
    print_tree(avl);

    auto iterator = avl.find(450);
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
    std::cout << "- tree after insertion: " << std::endl;
    print_tree(avl);

    const auto iter_max = avl.maximum();
    const auto iter_min = avl.minimum();
    std::cout << "- Maximum: " << *iter_max << std::endl;
    std::cout << "- Minimum: " << *iter_min << std::endl;
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
    std::cout << "- tree after insertion: " << std::endl;
    print_tree(avl);

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
    std::cout << "- tree after insertion: " << std::endl;
    print_tree(avl);

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
    std::cout << "test_copy_and_move_ctor" << std::endl;

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
    print_tree(avl1);

    // copy ctor of avl2
    avl_t avl2 = avl1;
    std::cout << "- printing copy-constructed avl_2" << std::endl;
    print_tree(avl2);

    // move ctor of avl3
    avl_t avl3 = std::move(avl1);
    std::cout << "- printing move-constructed avl_3 from avl_1" << std::endl;
    print_tree(avl3);
    std::cout << "- printing moved avl_1" << std::endl;
    print_tree(avl1);
}

void test_copy_and_move_assignment() {
    std::cout << "test_copy_and_move_assignment" << std::endl;

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
    print_tree(avl1);

    // copy-assign of avl2
    avl2 = avl1;
    std::cout << "- printing copy-assigned avl_2" << std::endl;
    print_tree(avl2);

    // move-assigned of avl3
    avl3 = std::move(avl1);
    std::cout << "- printing move-assigned avl_3 from avl_1" << std::endl;
    print_tree(avl3);
    std::cout << "- printing moved avl_1" << std::endl;
    print_tree(avl1);
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
//    test_copy_and_move_ctor();
//    test_copy_and_move_assignment();

}

