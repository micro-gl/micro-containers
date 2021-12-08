#include "src/test_utils.h"
#include <micro-containers/dynamic_array.h>

using namespace microc;

void test_insert() {
    print_test_header("test_insert");

    using c = dynamic_array<int>;
    c vec(3,100);
    print_simple_container(vec);

    auto it = vec.begin();
    it = vec.insert(it, 200);
    print_simple_container(vec);

    vec.insert(it, 2, 300);
    std::cout << "**\n";
    print_simple_container(vec);
    std::cout << "**\n";

    // "it" no longer valid, get a new one:
    it = vec.begin();

    c vec2(2, 400);
    vec.insert(it+2, vec2.begin(), vec2.end());
    print_simple_container(vec);

    int arr[] = { 501,502,503 };
    vec.insert(vec.begin(), arr, arr+3);
    print_simple_container(vec);
}

void test_erase() {
    print_test_header("test_erase");

    using container = dynamic_array<int>;

    int aa[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    container c;
    for (int ix = 0; ix < 10; ++ix) c.push_back(ix);

    print_simple_container(c);

    c.erase(c.begin());
    print_simple_container(c);

    c.erase(c.begin()+2, c.begin()+5);
    print_simple_container(c);

    // Erase all even numbers (C++11 and later)
    for (auto it = c.begin(); it != c.end(); ) {
        if (*it % 2 == 0) {
            it = c.erase(it);
        } else {
            ++it;
        }
    }
    print_simple_container(c);

    c.erase(c.end()-1);
    print_simple_container(c);

    c.erase(c.begin(), c.end());
    print_simple_container(c);

}

void test_emplace() {
    print_test_header("test_emplace");

}


int main() {
    // modifiers
//    test_insert();
    test_erase();
//    test_emplace();
}

