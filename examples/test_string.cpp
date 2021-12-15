#include "src/test_utils.h"
#include <micro-containers/string.h>

using namespace microc;

void test_insert() {
    print_test_header("test_insert");

    using c = dynamic_array<int>;
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

    // "it" no longer valid, get a new one:
    it = vec.begin();

    c vec2(2, 400);
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

    using container = dynamic_array<int>;

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

    using container = dynamic_array<int>;
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


int main() {
    // modifiers
    test_insert();
//    test_erase();
//    test_resize();
}

