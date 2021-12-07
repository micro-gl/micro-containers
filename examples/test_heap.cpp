#include "src/test_utils.h"
#include <micro-containers/algorithm.h>
#include <micro-containers/static_array.h>
#include <vector>

void test_is_heap() {
    print_test_header("test_is_heap");

    std::vector<int> v_not_max_heap {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9 };
    std::vector<int> v_max_heap {9 , 6, 9, 5, 5, 9, 7, 1, 1, 3, 5, 8, 3, 4, 2 };

    std::cout << "- v_not_max_heap is:\n";
    print_simple_container(v_not_max_heap);
    std::cout << "- v_max_heap is:\n";
    print_simple_container(v_max_heap);

    std::cout << "- is v_not_max_heap MAX heap: " << microc::is_heap(v_not_max_heap.begin(), v_not_max_heap.end()) << std::endl;
    std::cout << "- is v_max_heap MAX heap: " << microc::is_heap(v_max_heap.begin(), v_max_heap.end()) << std::endl;
}

void test_make_heap() {
    print_test_header("test_make_heap");

    std::vector<int> v { 3, 2, 4, 1, 5, 9 };

    std::cout << "- v is:\n";
    print_simple_container(v);

    microc::make_heap(v.begin(), v.end(), microc::less<int>());
    std::cout << "- v after max make_heap is:\n";

    print_simple_container(v);
    std::cout << "- is v max heap: " << microc::is_heap(v.begin(), v.end(), microc::less<int>()) << std::endl;

    microc::make_heap(v.begin(), v.end(), microc::greater<int>());
    std::cout << "- v after MIN make_heap is:\n";

    print_simple_container(v);
    std::cout << "- is v MIN heap: " << microc::is_heap(v.begin(), v.end(), microc::greater<int>()) << std::endl;
}

int main() {
    test_is_heap();
//    test_make_heap();
}

