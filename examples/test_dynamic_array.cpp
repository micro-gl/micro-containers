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

void test_emplace() {
    print_test_header("test_emplace");

}


int main() {
    // modifiers
    test_insert();
//    test_emplace();
}

