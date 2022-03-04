#include "src/test_utils.h"
#include <micro-containers/lru_pool.h>

using namespace std;

void test_iterator() {
    lru_pool<dummy_t, 4, unsigned int, microc::std_allocator<char>> pool(0.5f);
    pool.construct(3,5);
    const auto & const_ref = pool;
//
    for (int ix = 0; ix < 3200; ++ix)
        pool.get(ix);

    auto obj = pool.get(30);

    for (auto kv : const_ref) {
        std::cout << "{ k: " << kv.key << ", v: " << to_string(kv.value) << " },\n";
    }

    pool.print();
}

int main() {
    test_iterator();
}

