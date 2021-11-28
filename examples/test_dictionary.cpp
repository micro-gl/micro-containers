#include <iostream>
#include <sstream>

#include <micro-containers/dictionary.h>

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

template<class U1, class U2>
std::string to_string(const pair<U1, U2>& value, bool compact=false) {
    std::ostringstream os;
    if(!compact)
        os << "\n{\n" << "  key: \n    " << to_string(value.first) << ", \n  value: \n    "
           << to_string(value.second) << "\n},";
    else
        os << "{" << "key: " << to_string(value.first) << ", value: "
            << to_string(value.second) << "}";
    return os.str();
}

template<class Container>
void print_dictionary(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << to_string(item, true) << ", ";
    }
    std::cout << ")" << std::endl;
}

//#include <map>
void test_insert() {
    std::cout << "test_insert" << std::endl;

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));

    std::cout << "- printing dictionary" << std::endl;
    print_dictionary(d);
}

void test_erase_with_iterator() {
    std::cout << "test_erase_with_iterator" << std::endl;

    using dict = dictionary<int, int>;
    dict d;

    auto pos1 = d.insert(pair<int, int>(50, 50)).first;
    auto pos2 = d.insert(pair<int, int>(150, 150)).first;
    auto pos3 = d.insert(pair<int, int>(250, 250)).first;
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));

    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);
    //

    const auto iter_after_erase2 = d.erase(pos2);
    // pos3 should be invalid
    const auto iter_after_erase3 = d.erase(pos3);
    bool invalid = iter_after_erase3==d.end();
    std::cout << "- after erase" << std::endl;
    print_dictionary(d);
}

void test_erase_with_key() {
    std::cout << "test_erase_with_key" << std::endl;

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));

    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);
    //

    d.erase(250);
    d.erase(450);

    std::cout << "- after erase of 250 and 450 keys" << std::endl;
    print_dictionary(d);
}

void test_erase_with_range_iterator() {
    std::cout << "test_erase_with_range_iterator" << std::endl;

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
//    d.insert(pair<int, int>(450, 450));

    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);
    //
    d.erase(d.begin(), d.begin());
//    d.erase(d.begin(), ++d.begin());
    std::cout << "- after erase" << std::endl;
    print_dictionary(d);
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

/*
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
*/

int main() {
//    test_insert();
//    test_erase_with_iterator();
//    test_erase_with_key();
    test_erase_with_range_iterator();
//    test_clear();
//    test_find();
//    test_max_min();
//    test_successor();
//    test_predecessor();
//    test_contains();
//    test_iterator();
//    test_copy_and_move_ctor();
//    test_copy_and_move_assignment();
}

