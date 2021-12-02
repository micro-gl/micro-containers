#include <iostream>
#include <sstream>

#include "src/test_utils.h"
#include <micro-containers/hash_map.h>

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
void print_hash_map(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << "\n one";
        std::cout << to_string(item, true) << ", ";
    }
    std::cout << ")" << std::endl;
}

void test_insert() {
    print_test_header("test_insert");

    using map = hash_map<int, int>;
    map d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));

    std::cout << "- printing map" << std::endl;
    print_hash_map(d);
}

void test_insert_with_perfect_forward() {
    std::cout << "test_insert_with_perfect_forward" << std::endl;

    using map = hash_map<int, int>;
    map d;

    d.insert(50, 50);
    d.insert(150, 150);
    d.insert(250, 250);
    d.insert(350, 350);
    d.insert(450, 450);

    std::cout << "- printing dictionary" << std::endl;
    print_hash_map(d);
}

void test_insert_with_range() {
    std::cout << "test_insert_with_range" << std::endl;

    using map = hash_map<int, int>;
    map d_1, d_2;

    d_1.insert(50, 50);
    d_1.insert(150, 150);
    d_1.insert(250, 250);
    d_1.insert(350, 350);
    d_1.insert(450, 450);

    d_2.insert(0, 0);
    d_2.insert(1, 1);
    d_2.insert(2, 2);
    d_2.insert(350, 350);
    d_2.insert(351, 351);

    std::cout << "- printing map d1" << std::endl;
    print_hash_map(d_1);
    std::cout << "- printing map d2" << std::endl;
    print_hash_map(d_2);

    d_1.insert(d_2.begin(), d_2.end());

    std::cout << "- printing map d1 after range insert d2" << std::endl;
    print_hash_map(d_1);
}

void test_clear() {
    print_test_header("test_clear");

    using map = hash_map<int, int>;
    map d;

    d.insert(50, 50);
//    d.insert(150, 150);
//    d.insert(250, 250);
//    d.insert(350, 350);
//    d.insert(450, 450);

    std::cout << "- printing map" << std::endl;
    print_hash_map(d);

//    std::cout << "- printing map after clear" << std::endl;
//    d.clear();
//    print_hash_map(d);
}

void test_erase_with_key() {
    print_test_header("test_erase_with_key");

    using dict = hash_map<int, int>;
    dict d;

    d.insert(50, 50);
    d.insert(150, 150);
    d.insert(250, 250);
    d.insert(350, 350);
    d.insert(450, 450);

    std::cout << "- dictionary" << std::endl;
    print_hash_map(d);
    //

    d.erase(250);
    d.erase(450);

    std::cout << "- after erase of 250 and 450 keys" << std::endl;
    print_hash_map(d);
}

void test_erase_with_range_iterator() {
    print_test_header("test_erase_with_range_iterator");

    using dict = hash_map<int, int>;
    dict d;

    d.insert(50, 50);
    d.insert(150, 150);
    d.insert(250, 250);
    d.insert(350, 350);
//    d.insert(450, 450);

    std::cout << "- dictionary" << std::endl;
    print_hash_map(d);
    //
    d.erase(d.begin(), (d.begin()+2));
    std::cout << "- after erase" << std::endl;
    print_hash_map(d);
}

void test_erase_with_iterator() {
    print_test_header("test_erase_with_iterator");

    using dict = hash_map<int, int>;
    dict d;

    auto pos1 = d.insert(50, 50).first;
    auto pos2 = d.insert(150, 150).first;
    auto pos3 = d.insert(250, 250).first;
    d.insert(350, 350);
    d.insert(450, 450);

    std::cout << "- dictionary" << std::endl;
    print_hash_map(d);
    //

    const auto iter_after_erase2 = d.erase(pos2);
    // pos3 should be invalid
    const auto iter_after_erase3 = d.erase(pos3);
    bool invalid = iter_after_erase3==d.end();
    std::cout << "- after erase" << std::endl;
    print_hash_map(d);
}

void test_find() {
    print_test_header("test_find");

    using dict = hash_map<int, int>;
    dict d;

    d.insert(50, 50);
    d.insert(150, 150);
    d.insert(250, 250);
    d.insert(350, 350);
    d.insert(450, 450);
    //
    std::cout << "- dictionary" << std::endl;
    print_hash_map(d);

    auto iter = d.find(350);
    std::cout << "- found 350: " << to_string(*iter) << std::endl;
    iter = d.find(-5);
    std::cout << "- found -5 ? iter==d.end(): " << to_string(iter==d.end()) << std::endl;
}

int main() {
    // modifiers
//    test_insert();
//    test_insert_with_perfect_forward();
//    test_insert_with_range();

//    test_erase_with_key();
//    test_erase_with_range_iterator();
//    test_erase_with_iterator();

    test_clear();
//    test_clear();

    // lookup
//    test_find();
//    test_contains();

}

