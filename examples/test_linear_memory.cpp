#define MICRO_ALLOC_DEBUG

#include <micro-alloc/linear_memory.h>

using namespace micro_alloc;

void test_1() {
    using byte= unsigned char;
    const int size = 1024;
    byte memory[size];

    linear_memory<> alloc{memory, size};

    void  * p1 = alloc.malloc(512);
    void  * p2 = alloc.malloc(512);
    void  * p3 = alloc.malloc(512);

    alloc.free(p1);
    alloc.free(p2);
    alloc.free(p3);
    alloc.free(p3);
    alloc.free(p3);
//    alloc.reset();

    void  * p4 = alloc.malloc(512);
    void  * p5 = alloc.malloc(512);

    //    alloc.free((void *)(memory+256));

    //    alloc.free(p1);
    //    alloc.free(p1);
    linear_memory<> alloc2{memory, size};

    bool test_equal = alloc.is_equal(alloc2);

}

int main() {
    test_1();
}
