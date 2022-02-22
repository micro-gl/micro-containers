#define MICRO_CONTAINERS_ENABLE_THROW

#include "src/test_utils.h"
#include <micro-containers/circular_array.h>


using namespace microc;

void test_insert() {
    print_test_header("test_insert");

    using c = circular_array<int>;
    c vec(3, 100);
    std::cout << "\n- vec(3, 100) \n";
    print_simple_container(vec);

    auto it = vec.begin();
    it = vec.insert(it, 200);
    std::cout << "\n- vec.insert(it, 200) \n";
    print_simple_container(vec);

    vec.insert(it, 2, 300);
    std::cout << "\n- vec.insert(it, 2, 300) \n";
    print_simple_container(vec);

    // "it" no longer valid, get_or_put a new one:
    it = vec.begin();

    c vec2(2, 400);
    std::cout << "\n- vec2 is \n";
    print_simple_container(vec2);
    vec.insert(it+2, vec2.begin(), vec2.end());
    std::cout << "\n- vec.insert(it+2, vec2.begin(), vec2.end()) \n";
    print_simple_container(vec);

    int arr[] = { 501,502,503 };
    vec.insert(vec.begin(), arr, arr+3);
    std::cout << "\n- int arr[] = { 501,502,503 }";
    std::cout << "\n- vec.insert(vec.begin(), arr, arr+3) \n";
    print_simple_container(vec);
}

void test_erase() {
    print_test_header("test_erase");

    using container = circular_array<int>;

    int aa[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    container c;
    for (int ix = 0; ix < 10; ++ix) c.push_back(ix);

    std::cout << "- c is:  \n";
    print_simple_container(c);

    c.erase(c.begin());
    std::cout << "\n- c.erase(c.begin()) \n";
    print_simple_container(c);

    c.erase(c.begin()+2, c.begin()+5);
    std::cout << "\n- c.erase(c.begin()+2, c.begin()+5) \n";
    print_simple_container(c);

    // Erase all even numbers (C++11 and later)
    for (auto it = c.begin(); it != c.end(); ) {
        if (*it % 2 == 0) {
            it = c.erase(it);
        } else {
            ++it;
        }
    }
    std::cout << "\n- Erase all even numbers \n";
    print_simple_container(c);

    c.erase(c.end()-1);
    std::cout << "\n- c.erase(c.end()-1) \n";
    print_simple_container(c);

    c.erase(c.begin(), c.end());
    std::cout << "\n- c.erase(c.begin(), c.end()) \n";
    print_simple_container(c);

}

void test_resize() {
    print_test_header("test_resize");

    using container = circular_array<int>;
    int a[] = {1, 2, 3};
    container c;
    c.push_back(1);
    c.push_back(2);
    c.push_back(3);

    std::cout << "- The vector holds: \n";
    print_simple_container(c);
    std::cout << '\n';
    c.resize(5);
    std::cout << "- After resize up to 5: \n";
    print_simple_container(c);
    std::cout << '\n';
    c.resize(2);
    std::cout << "- After resize down to 2: \n";
    print_simple_container(c);
    std::cout << '\n';
    c.resize(6, 4);
    std::cout << "- After resize up to 6 (rvalue initializer = 4): \n";
    print_simple_container(c);
    int lvalue = 5;
    c.resize(8, lvalue);
    std::cout << "- After resize up to 8 (lvalue initializer = 5): \n";
    print_simple_container(c);
    std::cout << '\n';
}

void test_push_and_pop_back() {
    print_test_header("test_push_and_pop_back");

    using container = circular_array<int>;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    container c;

    for(auto & item : a) c.push_back(item);

    std::cout << "- The array holds after push_back: \n";
    print_simple_container(c);

    while(!c.empty()) c.pop_back();
    std::cout << "- The array holds after pop_back: \n";
    print_simple_container(c);
}

void test_push_and_pop_front() {
    print_test_header("test_push_and_pop_front");

    using container = circular_array<int>;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    container c;

    for(auto & item : a) c.push_front(item);

    std::cout << "- The array holds after push_front: \n";
    print_simple_container(c);

    while(!c.empty()) c.pop_front();
    std::cout << "- The array holds after pop_front: \n";
    print_simple_container(c);
}

void test_emplace_back_and_front() {
    print_test_header("test_emplace_back_and_front");

    using container = circular_array<dummy_t>;
    container c;

    c.emplace_back(4, 5);
    c.emplace_back(6, 7);
    std::cout << "- The array holds after emplace_back(1,2) and emplace_back(3,4): \n";
    print_simple_container(c);

    c.emplace_front(2, 3);
    c.emplace_front(0, 1);
    std::cout << "- The array holds after emplace_front(2, 3) and emplace_front(0, 1): \n";
    print_simple_container(c);
}

void test_clear() {
    print_test_header("test_clear");

    using container = circular_array<int>;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    container c;

    for(auto & item : a) c.push_front(item);

    std::cout << "- The array holds after push_front: \n";
    print_simple_container(c);

    c.clear();
    std::cout << "- The array holds after clear(): \n";
    print_simple_container(c);
}

void test_access_operator_and_at() {
    print_test_header("test_access_operator_and_at");

    using container = circular_array<int>;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    container c(a, a+10);
    std::cout << "- The array is: \n";
    print_simple_container(c);

    std::cout << "- The array after printing with [] operator: \n- (";
    for (int ix = 0; ix < c.size(); ++ix) { std::cout << c[ix] << ", "; }
    std::cout << ")\n";

    std::cout << "- The array after printing with .at() method: \n- (";
    for (int ix = 0; ix < c.size(); ++ix) { std::cout << c.at(ix)<< ", "; }
    std::cout << ")\n";

    std::cout << "- The array after changing second c[1]=50: ";
    c[1] = 50;
    print_simple_container(c);
}

void test_back_front() {
    print_test_header("test_back_front");

    using container = circular_array<int>;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    container c(a, a+10);
    std::cout << "- The array is: \n";
    print_simple_container(c);

    std::cout << "- printing front() and back() : (" << c.front() << ", " << c.back() << ")\n";

    std::cout << "- The array after changing c.front()=50, c.back()=100: ";
    c.front()=50; c.back()=100;
    print_simple_container(c);
}

void test_assign_copy_move_operator() {
    print_test_header("test_assign_copy_move_operator");

    using container = circular_array<int>;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    container c1(a, a+10), c2, c3;

    std::cout << "- c1=: "; print_simple_container(c1);
    std::cout << "- c2=c1: "; c2=c1; print_simple_container(c2);
    std::cout << "- c3=move(c2): "; c3=std::move(c2); print_simple_container(c3);
    std::cout << "- c2=: "; print_simple_container(c2);
}

void test_copy_and_move_ctors() {
    print_test_header("test_copy_and_move_ctors");

    using container = circular_array<int>;
    int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    container c1(a, a+10);

    std::cout << "- c1=: "; print_simple_container(c1);
    std::cout << "- container c2=c1: "; container c2=c1; print_simple_container(c2);
    std::cout << "- container c3=move(c2): "; container c3=std::move(c2); print_simple_container(c3);
    std::cout << "- c2=: "; print_simple_container(c2);
}

int main() {
    // modifiers
//    test_insert();
//    test_erase();
//    test_resize();
//    test_push_and_pop_back();
//    test_push_and_pop_front();
//    test_emplace_back_and_front();
//    test_clear();

    // Element Access
//    test_access_operator_and_at();
//    test_back_front();

    // Assign operator
//    test_assign_copy_move_operator();

    // Constructors
    test_copy_and_move_ctors();

}

