#include "src/test_utils.h"
#include <vector>
#include <micro-containers/priority_queue.h>

template<typename T>
void print_queue(T q) { // NB: pass by value so the print uses a copy
    while(!q.empty()) {
        std::cout << q.top() << ' ';
        q.pop();
    }
    std::cout << '\n';
}

void test_1() {
    print_test_header("test_1");

    microc::priority_queue<int, std::vector<int>> q;

    const auto data = {1,8,5,6,3,4,0,9,7,2};

    for(int n : data) q.push(n);

    print_queue(q);

    microc::priority_queue<int, std::vector<int>, std::less<int>>
            q2(data.begin(), data.end());

    print_queue(q2);

    // Using lambda to compare elements.
    auto cmp = [](int left, int right) { return (left ^ 1) < (right ^ 1); };
    microc::priority_queue<int, std::vector<int>, decltype(cmp)> q3(cmp);

    for(int n : data)
        q3.push(n);

    print_queue(q3);
}

int main() {
    test_1();
}

