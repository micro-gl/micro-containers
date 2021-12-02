#include "src/test_utils.h"
#include <micro-containers/hash_set.h>


template<class Container>
void print_hash_set(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << "{ k: " << to_string(item) << "}" << ", ";
    }
    std::cout << ")" << std::endl;
}

template<class Key, class Hash, class Allocator>
void print_hash_set_bucket(const hash_set<Key, Hash, Allocator> & c,
                           const unsigned bucket_index) {
    using bucket_t = const typename hash_set<Key, Hash, Allocator>::bucket_type &;
    const auto & bucket = c.bucket_node(bucket_index);
    std::cout << " == bucket #" << bucket_index << " has "
              << c.bucket_size(bucket_index) << " items ==" << std::endl;
    const auto * node_head = bucket.list;
    std::cout << "(";
    while(node_head) {
        std::cout << "{ k: " << to_string(node_head->key) << "}" << ", ";
        node_head = node_head->next;
    }
    std::cout << ")";
}

template<class Key, class Hash, class Allocator>
void print_hash_set_info(const hash_set<Key, Hash, Allocator> & c) {
    std::cout << "== HASH-MAP INFO ==" << std::endl;
    std::cout << " -- " << c.size() << " items in " << c.bucket_count() << " buckets";
    std::cout << " -- load factor="  << c.load_factor();
    std::cout << " -- max load factor="  << c.max_load_factor() << std::endl;
    std::cout << "** Buckets Info" << std::endl;

    for (unsigned ix = 0; ix < c.bucket_count(); ix++) {
        print_hash_set_bucket(c, ix);
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void test_emplace() {
    print_test_header("test_emplace");

    using set = hash_set<int>;
    set d;

    d.emplace(50);
    d.emplace(150);
    d.emplace(250);

    std::cout << "- printing set" << std::endl;
    print_hash_set(d);
}

void test_insert() {
    print_test_header("test_insert");

    using set = hash_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);

    std::cout << "- printing set" << std::endl;
    print_hash_set(d);
}

void test_insert_with_range() {
    print_test_header("test_insert_with_range");

    using set = hash_set<int>;
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

    std::cout << "- printing set d1" << std::endl;
    print_hash_set(d_1);
    std::cout << "- printing set d2" << std::endl;
    print_hash_set(d_2);

    d_1.insert(d_2.begin(), d_2.end());

    std::cout << "- printing set d1 after range insert d2" << std::endl;
    print_hash_set(d_1);
}

void test_clear() {
    print_test_header("test_clear");

    using set = hash_set<int>;
    set d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);

    std::cout << "- printing set" << std::endl;
    print_hash_set(d);

    std::cout << "- printing set after clear" << std::endl;
    d.clear();
    print_hash_set(d);
}

void test_erase_with_key() {
    print_test_header("test_erase_with_key");

    using dict = hash_set<int>;
    dict d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);

    std::cout << "- set" << std::endl;
    print_hash_set(d);
    //

    d.erase(250);
    d.erase(450);

    std::cout << "- after erase of 250 and 450 keys" << std::endl;
    print_hash_set(d);
}

void test_erase_with_range_iterator() {
    print_test_header("test_erase_with_range_iterator");

    using dict = hash_set<int>;
    dict d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
//    d.insert(450, 450);

    std::cout << "- set" << std::endl;
    print_hash_set(d);
    //
    d.erase(d.begin(), (d.begin()+2));
    std::cout << "- after erase" << std::endl;
    print_hash_set(d);
}

void test_erase_with_iterator() {
    print_test_header("test_erase_with_iterator");

    using dict = hash_set<int>;
    dict d;

    auto pos1 = d.insert(50).first;
    auto pos2 = d.insert(150).first;
    auto pos3 = d.insert(250).first;
    d.insert(350);
    d.insert(450);

    std::cout << "- set" << std::endl;
    print_hash_set(d);
    //

    const auto iter_after_erase2 = d.erase(pos2);
    // pos3 should be invalid
    const auto iter_after_erase3 = d.erase(pos3);
    bool invalid = iter_after_erase3==d.end();
    std::cout << "- after erase" << std::endl;
    print_hash_set(d);
}

void test_find() {
    print_test_header("test_find");

    using dict = hash_set<int>;
    dict d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);
    //
    std::cout << "- set" << std::endl;
    print_hash_set(d);

    auto iter = d.find(350);
    std::cout << "- found 350: " << to_string(*iter) << std::endl;
    iter = d.find(-5);
    std::cout << "- found -5 ? iter==d.end(): " << to_string(iter==d.end()) << std::endl;
}

void test_contains() {
    print_test_header("test_contains");

    using dict = hash_set<int>;
    dict d;

    d.insert(50);
    d.insert(150);
    d.insert(250);
    d.insert(350);
    d.insert(450);
    //
    std::cout << "- set" << std::endl;
    print_hash_set(d);

    for (const auto & item : d) {
        std::cout << "- does set contains " << to_string(item)
                  << " ? " << d.contains(item) << std::endl;
    }

    std::cout << "- does set contains " << 5
              << " ? " << d.contains(5) << std::endl;

}

// assign and ctors
// move/copy
void test_copy_and_move_ctor() {
    print_test_header("test_copy_and_move_ctor");

    using dict = hash_set<int>;
    dict d1;

    d1.insert(50);
    d1.insert(150);
    d1.insert(250);
    d1.insert(350);
    d1.insert(450);

    std::cout << "- printing set d1" << std::endl;
    print_hash_set(d1);

    //

    dict d2 = d1;

    std::cout << "- printing set d2 after copy constructing with d1" << std::endl;
    print_hash_set(d2);

    dict d3 = std::move(d1);
    std::cout << "- printing set d3 after move constructing with d1" << std::endl;
    print_hash_set(d3);
    std::cout << "- printing set d1" << std::endl;
    print_hash_set(d1);
}

void test_copy_and_move_assign() {
    print_test_header("test_copy_and_move_assign");

    using set = hash_set<int>;
    set d1, d2, d3;

    d1.insert(50);
    d1.insert(150);
    d1.insert(250);
    d1.insert(350);
    d1.insert(450);

    std::cout << "- printing set d1" << std::endl;
    print_hash_set(d1);

    d2 = d1;

    std::cout << "- printing set d2 after copy assign with d1" << std::endl;
    print_hash_set(d2);

    d3 = std::move(d1);
    std::cout << "- printing set d3 after move assign with d1" << std::endl;
    print_hash_set(d3);
    std::cout << "- printing set d1" << std::endl;
    print_hash_set(d1);
}

void test_rehash() {
    print_test_header("test_rehash");

    using set = hash_set<int>;
    set d1(1);
    d1.insert(1);
    d1.insert(2);
    d1.insert(3);
    d1.insert(4);
    d1.insert(5);
    d1.insert(6);

    print_hash_set_info(d1);

    d1.max_load_factor(3.0f);
    d1.rehash(2);
    print_hash_set_info(d1);
}

void test_rehash_2() {
    print_test_header("test_rehash_2");

    using set = hash_set<int>;
    set d1(1);
    d1.insert(1);
    print_hash_set_info(d1);
    d1.insert(2);
    print_hash_set_info(d1);
    d1.insert(3);
    print_hash_set_info(d1);
    d1.insert(4);
    print_hash_set_info(d1);
    d1.insert(5);
    print_hash_set_info(d1);
    d1.insert(6);
    print_hash_set_info(d1);

//    d1.max_load_factor(3.0f);
//    d1.rehash(2);
//    print_hash_set_info(d1);
}

int main() {
    // modifiers
    test_insert();
    test_insert_with_range();

    test_emplace();
//
    test_erase_with_key();
    test_erase_with_range_iterator();
    test_erase_with_iterator();

    test_clear();

    // lookup
    test_find();
    test_contains();

    // assign and ctors
    test_copy_and_move_ctor();
    test_copy_and_move_assign();

    test_rehash();
//    test_rehash_2();
}

