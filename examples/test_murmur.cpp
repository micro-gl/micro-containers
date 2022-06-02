#include "src/test_utils.h"
#include <micro-containers/murmur.h>

using namespace std;
using namespace microc;

void test_murmur_32() {
    iterative_murmur<uint32_t> murmur;
    murmur.begin(0);
    for (int ix = 0; ix < 3; ++ix)
        murmur.next(ix);
    auto res = murmur.end();

    std::cout << res << '\n';
}

int main() {
    test_murmur_32();
}

