#include "src/test_utils.h"
#include <micro-containers/array.h>


void test_1() {
    print_test_header("test_1");

    array<int, 2> a = {0,1};
    array<int, 2> b = {2,3};

    a=b;

    print_simple_container(a);
}

#include <string>
using namespace std;
int main() {
    test_1();

    int * aa = nullptr;
    aa = aa+1;
    string a = "abdwewewe";
    auto b = a.size();
    a.shrink_to_fit();
    a.pop_back();
    a.pop_back();
    a.push_back('c');
    a.pop_back();
    a.pop_back();
    a.pop_back();

}

