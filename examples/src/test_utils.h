#pragma once

#include <iostream>
#include <sstream>

using std::to_string;

struct dummy_t {
    int a, b;
    explicit dummy_t(int $a=0, int $b=1) : a($a), b($b) {
        int aa;
    }
};

std::string to_string(const dummy_t& value) {
    std::ostringstream os;
    os << "{ a: " << value.a << ", b: " << value.b << "}";
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