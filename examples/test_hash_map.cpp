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

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));

    std::cout << "- printing map" << std::endl;
    print_hash_map(d);

    std::cout << "- printing map after clear" << std::endl;
    d.clear();
    print_hash_map(d);
}

int main() {
    // modifiers
//    test_insert();
//    test_insert_with_perfect_forward();
    test_insert_with_range();

//    test_clear();
}

