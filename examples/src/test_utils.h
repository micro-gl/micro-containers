#pragma once

#include <iostream>
#include <sstream>

#include <micro-containers/pair.h>
using namespace microc;

using std::to_string;

struct dummy_t {
    int a, b;

    dummy_t(int aa, int bb) : a(aa), b(bb) {}

    ~dummy_t() {
        std::cout << "destructed !!\n";
    }
    bool operator<(const dummy_t& rhs) const {
        return (a < rhs.a) || (a==rhs.a && b<rhs.b);
    }

};

std::string to_string(const dummy_t& value) {
    std::ostringstream os;
    os << "{ a: " << value.a << ", b: " << value.b << "}";
    return os.str();
}

template<class U1, class U2>
std::string to_string(const pair<U1, U2>& value, bool compact=false) {
    std::ostringstream os;
    if(!compact)
        os << "\n{\n" << "  k: \n    " << to_string(value.first) << ", \n  v: \n    "
           << to_string(value.second) << "\n},";
    else
        os << "{" << "k: " << to_string(value.first) << ", v: "
           << to_string(value.second) << "}";
    return os.str();
}

template<class Container>
void print_simple_container(const Container & container) {
    std::cout << "(";
    for (const auto & item : container) {
        std::cout << to_string(item) << ", ";
    }
    std::cout << ")" << std::endl;
}

void print_test_header(std::string && name) {
    std::cout << std::endl << "==== " << name << " ====" << std::endl;
}