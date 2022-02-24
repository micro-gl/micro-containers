#include "src/test_utils.h"
#include <micro-containers/lru_cache.h>

void test_cache_robin_hood() {
    int bb = sizeof (long);
    //        bits_robin_lru_cache<4, unsigned int, std::allocator<char>> pool{0.125f};
    //        bits_robin_lru_cache<10, unsigned int, nitrogl::std_rebind_allocator<>> pool{0.125f};
    lru_cache<dummy_t, 4, unsigned int, microc::std_allocator<char>> cache{0.5f};
    cache.print();

    //    for (int ix = 0; ix < 15*2; ++ix) {
    //        pool.get_or_put(ix);
    //        pool.print();
    //     }
    //    pool.get_or_put(28);
    //    pool.print();

    //
    //        return;
    cache.put(0, dummy_t{0,0});
    cache.put(1, dummy_t{1,1});
    cache.put(2, dummy_t{2,2});
    cache.put(3, dummy_t{3,3});

    cache.print();
}

void test_iterator() {
    lru_cache<dummy_t, 4, unsigned int, microc::std_allocator<char>> cache{0.5f};
    cache.put(0, dummy_t{0,0});
    cache.put(1, dummy_t{1,1});
    cache.put(2, dummy_t{2,2});
    cache.put(3, dummy_t{3,3});

    const auto & const_ref = cache;

//    for (auto kv : const_ref) {
    for (auto kv : cache) {
        std::cout << "{ k: " << kv.key << ", v: " << to_string(kv.value) << " },\n";
    }
}

using namespace std;
int main() {
//    test_cache_robin_hood();
    test_iterator();
}

