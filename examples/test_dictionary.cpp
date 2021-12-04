#include "src/test_utils.h"
#include <micro-containers/dictionary.h>

template<class Container>
void print_dictionary(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << to_string(item, true) << ", ";
    }
    std::cout << ")" << std::endl;
}

void test_insert_with_perfect_forward() {
    print_test_header("test_insert_with_perfect_forward");

    using dict = dictionary<int, int>;
    dict d;

    d.insert(50, 50);
    d.insert(150, 150);
    d.insert(250, 250);
    d.insert(350, 350);
    d.insert(450, 450);

    std::cout << "- printing dictionary" << std::endl;
    print_dictionary(d);
}

void test_insert() {
    print_test_header("test_insert");

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

void test_insert_with_range() {
    print_test_header("test_insert_with_range");

    using dict = dictionary<int, int>;
    dict d_1, d_2;

    d_1.insert(pair<int, int>(50, 50));
    d_1.insert(pair<int, int>(150, 150));
    d_1.insert(pair<int, int>(250, 250));
    d_1.insert(pair<int, int>(350, 350));
    d_1.insert(pair<int, int>(450, 450));

    d_2.insert(pair<int, int>(0, 0));
    d_2.insert(pair<int, int>(1, 1));
    d_2.insert(pair<int, int>(2, 2));
    d_2.insert(pair<int, int>(350, 350));
    d_2.insert(pair<int, int>(351, 351));

    std::cout << "- printing dictionary d1" << std::endl;
    print_dictionary(d_1);
    std::cout << "- printing dictionary d2" << std::endl;
    print_dictionary(d_2);

    d_1.insert(d_2.begin(), d_2.end());

    std::cout << "- printing dictionary d1 after insert" << std::endl;
    print_dictionary(d_1);
}

void test_erase_with_iterator() {
    print_test_header("test_erase_with_iterator");

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
    print_test_header("test_erase_with_key");

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

//    d.erase(250);
//    d.erase(450);
    d.erase(450);

    std::cout << "- after erase of 250 and 450 keys" << std::endl;
    print_dictionary(d);
}

void test_erase_with_range_iterator() {
    print_test_header("test_erase_with_range_iterator");

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
    print_test_header("test_clear");

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));
    //
    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);

    d.clear();
    std::cout << "- after clear" << std::endl;
    print_dictionary(d);
}

void test_find() {
    print_test_header("test_find");

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));
    //
    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);

    auto iter = d.find(350);
    std::cout << "- found 350: " << to_string(*iter) << std::endl;
    iter = d.find(-5);
    std::cout << "- found -5 ? iter==d.end(): " << to_string(iter==d.end()) << std::endl;
}

void test_contains() {
    print_test_header("test_contains");

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));
    //
    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);

    std::cout << "- found 350: " << to_string(d.contains(350)) << std::endl;
    std::cout << "- found -5: " << to_string(d.contains(-5)) << std::endl;
}

// Element Access

void test_at() {
    print_test_header("test_at");

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));
    //
    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);

    const auto & d_const = d;
    auto & value_1 = d_const.at(150);

    std::cout << "- at(150) is" << value_1 << std::endl;
}

void test_access_operator() {
    print_test_header("test_access_operator");

    using dict = dictionary<int, int>;
    dict d;

    d.insert(pair<int, int>(50, 50));
    d.insert(pair<int, int>(150, 150));
    d.insert(pair<int, int>(250, 250));
    d.insert(pair<int, int>(350, 350));
    d.insert(pair<int, int>(450, 450));
    //
    std::cout << "- dictionary" << std::endl;
    print_dictionary(d);

    d[150] = 5555;
    auto & value_1 = d[150];

    std::cout << "- dict[150] is " << d[150] << std::endl;
}

// move/copy
void test_copy_and_move_ctor() {
    print_test_header("test_copy_and_move_ctor");

    using dict = dictionary<int, int>;
    dict d_1;

    d_1.insert(pair<int, int>(50, 50));
    d_1.insert(pair<int, int>(150, 150));
    d_1.insert(pair<int, int>(250, 250));
    d_1.insert(pair<int, int>(350, 350));
    d_1.insert(pair<int, int>(450, 450));

    std::cout << "- printing dictionary d1" << std::endl;
    print_dictionary(d_1);

    //

    dict d2 = d_1;

    std::cout << "- printing dictionary d2 after copy constructing with d1" << std::endl;
    print_dictionary(d2);

    dict d3 = std::move(d_1);
    std::cout << "- printing dictionary d3 after move constructing with d1" << std::endl;
    print_dictionary(d3);
    std::cout << "- printing dictionary d1" << std::endl;
    print_dictionary(d_1);
}

void test_copy_and_move_assign() {
    print_test_header("test_copy_and_move_assign");

    using dict = dictionary<int, int>;
    dict d1, d2, d3;

    d1.insert(pair<int, int>(50, 50));
    d1.insert(pair<int, int>(150, 150));
    d1.insert(pair<int, int>(250, 250));
    d1.insert(pair<int, int>(350, 350));
    d1.insert(pair<int, int>(450, 450));

    std::cout << "- printing dictionary d1" << std::endl;
    print_dictionary(d1);

    d2 = d1;

    std::cout << "- printing dictionary d2 after copy assign with d1" << std::endl;
    print_dictionary(d2);

    d3 = std::move(d1);
    std::cout << "- printing dictionary d3 after move assign with d1" << std::endl;
    print_dictionary(d3);
    std::cout << "- printing dictionary d1" << std::endl;
    print_dictionary(d1);
}

int main() {
    // modifiers
    test_insert();
    test_insert_with_perfect_forward();
    test_insert_with_range();

    test_erase_with_iterator();
    test_erase_with_key();
    test_erase_with_range_iterator();

    test_clear();

    // lookup
    test_find();
    test_contains();

    // Element Access
    test_at();
    test_access_operator();

    // move/copy
    test_copy_and_move_ctor();
    test_copy_and_move_assign();
}

