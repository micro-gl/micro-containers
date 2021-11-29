#include <iostream>
#include <sstream>

#include <micro-containers/ordered_set.h>

using std::to_string;

struct dummy_t {
    int a, b;
    explicit dummy_t(int $a=0, int $b=1) : a($a), b($b) {}
    dummy_t(dummy_t && other) : a(other.a), b(other.b) {
        std::cout << "move ctor dummy ! \n";
    }
    dummy_t(const dummy_t & other) : a(other.a), b(other.b) {
        std::cout << "copy ctor dummy ! \n";
    }
    bool operator<(const dummy_t& rhs) const {
        return (a < rhs.a) || (a==rhs.a && b<rhs.b);
    }
};

std::string to_string(const dummy_t& value) {
    std::ostringstream os;
    os << "{ a: " << value.a << ", b: " << value.b << "}";
    return os.str();
}

template<class Container>
void print_ordered_set(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << to_string(item) << ", ";
    }
    std::cout << ")" << std::endl;
}

void test_insert() {
    std::cout << "test_insert" << std::endl;

    using set = ordered_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);

    std::cout << "- printing ordered set" << std::endl;
    print_ordered_set(d);
}

void test_insert_with_range() {
    std::cout << "test_insert_with_range" << std::endl;

    using set = ordered_set<int>;
    set d_1, d_2;

    d_1.insert(50);
    d_1.insert(150);
    d_1.insert(250);
    d_1.insert(350);
    d_1.insert(450);

    d_2.insert(0);
    d_2.insert(1);
    d_2.insert(2);
    d_2.insert(350);
    d_2.insert(351);

    std::cout << "- printing ordered set d1" << std::endl;
    print_ordered_set(d_1);
    std::cout << "- printing ordered set d2" << std::endl;
    print_ordered_set(d_2);

    d_1.insert(d_2.begin(), d_2.end());

    std::cout << "- printing ordered set d1 after insert" << std::endl;
    print_ordered_set(d_1);
}

void test_erase_with_iterator() {
    std::cout << "test_erase_with_iterator" << std::endl;

    using set = ordered_set<int>;
    set d;

    auto pos1 = d.insert(50).first;
    auto pos2 = d.insert(150).first;
    auto pos3 = d.insert(250).first;
    d.insert(350);
    d.insert(450);

    std::cout << "- ordered set" << std::endl;
    print_ordered_set(d);
    //

    const auto iter_after_erase2 = d.erase(pos2);
    // pos3 should be invalid
    const auto iter_after_erase3 = d.erase(pos3);
    bool invalid = iter_after_erase3==d.end();
    std::cout << "- after erase 150, 250" << std::endl;
    print_ordered_set(d);
}

void test_erase_with_key() {
    std::cout << "test_erase_with_key" << std::endl;

    using set = ordered_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);

    std::cout << "- ordered set" << std::endl;
    print_ordered_set(d);
    //

//    d.erase(250);
//    d.erase(450);
    d.erase(450);

    std::cout << "- after erase of 250 and 450 keys" << std::endl;
    print_ordered_set(d);
}

void test_erase_with_range_iterator() {
    std::cout << "test_erase_with_range_iterator" << std::endl;

    using set = ordered_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
//    d.insert(450);

    std::cout << "- ordered set" << std::endl;
    print_ordered_set(d);
    //
    d.erase(d.begin(), d.end());
//    d.erase(d.begin(), ++d.begin());
    std::cout << "- after erase" << std::endl;
    print_ordered_set(d);
}

void test_clear() {
    std::cout << "test_clear" << std::endl;

    using set = ordered_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);
    //
    std::cout << "- ordered set" << std::endl;
    print_ordered_set(d);

    d.clear();
    std::cout << "- after clear" << std::endl;
    print_ordered_set(d);
}

void test_find() {
    std::cout << "test_find" << std::endl;

    using set = ordered_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);
    //
    std::cout << "- ordered set" << std::endl;
    print_ordered_set(d);

    auto iter = d.find(350);
    std::cout << "- found 350: " << to_string(*iter) << std::endl;
    iter = d.find(-5);
    std::cout << "- found -5 ? iter==d.end(): " << to_string(iter==d.end()) << std::endl;
}

void test_contains() {
    std::cout << "test_contains" << std::endl;

    using set = ordered_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);
    //
    std::cout << "- ordered set" << std::endl;
    print_ordered_set(d);

    std::cout << "- found 350: " << to_string(d.contains(350)) << std::endl;
    std::cout << "- found -5: " << to_string(d.contains(-5)) << std::endl;
}

// move/copy
void test_copy_and_move_ctor() {
    std::cout << "test_copy_and_move_ctor" << std::endl;

    using set = ordered_set<int>;
    set d_1;

    d_1.insert(50);
    d_1.insert(150);
    d_1.insert(250);
    d_1.insert(350);
    d_1.insert(450);

    std::cout << "- printing ordered set d1" << std::endl;
    print_ordered_set(d_1);

    //

    set d2 = d_1;

    std::cout << "- printing ordered set d2 after copy constructing with d1" << std::endl;
    print_ordered_set(d2);

    set d3 = std::move(d_1);
    std::cout << "- printing ordered set d3 after move constructing with d1" << std::endl;
    print_ordered_set(d3);
    std::cout << "- printing ordered set d1" << std::endl;
    print_ordered_set(d_1);
}

void test_copy_and_move_assign() {
    std::cout << "test_copy_and_move_assign" << std::endl;

    using set = ordered_set<int>;
    set d1, d2, d3;

    d1.insert(50);
    d1.insert(150);
    d1.insert(250);
    d1.insert(350);
    d1.insert(450);

    std::cout << "- printing set d1" << std::endl;
    print_ordered_set(d1);

    d2 = d1;

    std::cout << "- printing set d2 after copy assign with d1" << std::endl;
    print_ordered_set(d2);

    d3 = std::move(d1);
    std::cout << "- printing set d3 after move assign with d1" << std::endl;
    print_ordered_set(d3);
    std::cout << "- printing set d1" << std::endl;
    print_ordered_set(d1);
}

void test_dummy() {
    std::cout << "test_dummy" << std::endl;

    using set = ordered_set<dummy_t>;
    set d1, d2, d3;

    d1.insert(dummy_t(0, 1));
    d1.insert(dummy_t(1, 1));
    d1.insert(dummy_t(2, 1));
    d1.insert(dummy_t(3, 1));
    d1.insert(dummy_t(1, 2));
    d1.insert(dummy_t(1, 3));

    std::cout << "- printing set d1" << std::endl;
    print_ordered_set(d1);
}

int main() {
    // modifiers
//    test_insert();
//    test_insert_with_range();

//    test_erase_with_iterator();
//    test_erase_with_key();
//    test_erase_with_range_iterator();

//    test_clear();

    // lookup
//    test_find();
//    test_contains();

    // move/copy
//    test_copy_and_move_ctor();
//    test_copy_and_move_assign();
    test_dummy();
}

