#include "src/test_utils.h"
#include <micro-containers/string.h>
#include <micro-containers/algorithm.h>

using namespace microc;
#include <string>
using string = basic_string<char, char_traits<char>>;

void print_string(const string & str) {
    std::cout << str.c_str() << std::endl;
}

void test_replace() {
    print_test_header("test_replace");

    std::string a1 = "tomer tomer tomer";
    std::string a2 = "ABC";
    a2.replace(1, 10, a1);

    string a1_ = "tomer tomer tomer";
    string a2_ = "ABC";
    std::cout << "- a1_=" << a1_.c_str() << std::endl;
    std::cout << "- a2_=" << a2_.c_str() << std::endl;
    a1_.replace(0, 10, a2_.c_str(), a2_.size());
    std::cout << "- a1_.replace(0, 10, a2_.c_str(), a2_.size()); a1_=" << a1_.c_str() << std::endl << std::endl;

    string a1_1 = "tomer tomer tomer";
    string a2_1 = "ABC";
    std::cout << "- a1_1=" << a1_1.c_str() << std::endl;
    std::cout << "- a2_1=" << a2_1.c_str() << std::endl;
    a1_1.replace(5, 1, a2_1.c_str(), a2_1.size());
    std::cout << "- a1_1.replace(5, 1, a2_1.c_str(), a2_1.size()); a1_1=" << a1_1.c_str() << std::endl << std::endl;

    string a1_2 = "tomer tomer tomer";
    std::cout << "- a1_2=" << a1_2.c_str() << std::endl;
    a1_2.replace(a1_2.begin(), a1_2.end() - 5, 5, 'A');
    std::cout << "- a1_2.replace(a1_2.begin(), a1_2.end() - 5, 5, 'A'); a1_2=" << a1_2.c_str() << std::endl;
}

void test_insert() {
    print_test_header("test_insert");

    std::string a1 = "tomer tomer tomer";
    std::string a2 = "ABC";
    a2.replace(1, 10, a1);

    string s = "tomer tomer tomer";
    string s2 = "ABC";
    std::cout << "- s= " << s.c_str() << std::endl;
    std::cout << "- s2= " << s2.c_str() << std::endl;

    s.insert(s.begin(), s2.begin(), s2.end());
    std::cout << "- s.insert(s.begin(), s2.begin(), s2.end()), s= " << s.c_str() << std::endl;

    s.insert(s.begin()+3, s2.begin(), s2.end());
    std::cout << "- s.insert(s.begin()+3, s2.begin(), s2.end()), s= " << s.c_str() << std::endl;

    s.insert(s.end(), s2.begin(), s2.end());
    std::cout << "- s.insert(s.end(), s2.begin(), s2.end()), s= " << s.c_str() << std::endl;

    s.insert(0, s);
    std::cout << "- s.insert(0, s), s= " << s.c_str() << std::endl;

    s.insert(0, s.data(), 6);
    std::cout << "- s.insert(0, s.data(), 6), s= " << s.c_str() << std::endl;

    string s_temp = s.insert(0, 3, '=');
    std::cout << "- s.insert(0, s.data(), 6), s= " << s.c_str() << std::endl;

    s.insert(0, s.data(), 3);
    std::cout << "- s.insert(0, s.data(), 3), s= " << s.c_str() << std::endl;
}

void test_clear() {
    print_test_header("test_clear");

    string s = "tomer tomer tomer";
    std::cout << "- s= " << s.c_str() << std::endl;
    s.clear();
    std::cout << "- s.clear(), s= " << s.c_str() << std::endl;
    assert(s.empty() && "s is not empty");
}

void test_erase() {
    print_test_header("test_erase");

    string s = "This Is An Example";
    std::cout << "- s= " << s.c_str() << std::endl;
    s.erase(7, 3); // erases " An" using overload (1)
    std::cout << "- s.erase(7, 3); s= " << s.c_str() << std::endl;

    s.erase(microc::find(s.begin(), s.end(), ' ')); // erases first ' '; overload (2)
    std::cout << "- microc::find(s.begin(), s.end(), ' '); s= " << s.c_str() << std::endl;

    s.erase(s.find(' ')); // trims from ' ' to the end of the string; overload (1)
    std::cout << "- s.erase(s.find(' ')); s= " << s.c_str() << std::endl;
}

void test_push_pop_back() {
    print_test_header("test_push_pop_back");

    string s = "5 + 5";
    std::cout << "- s= " << s.c_str() << std::endl;
    s.push_back(' ');
    s.push_back('=');
    s.push_back(' ');
    s.push_back('1');
    s.push_back('0');
    std::cout << "- after multiple push_back; s= " << s.c_str() << std::endl;

    for (int ix = 0; ix < 5; ix++) s.pop_back();
    std::cout << "- after 4 pop_back; s= " << s.c_str() << std::endl;
}

void test_operator_plus_assign() {
    print_test_header("test_operator_plus_assign");

    string s = "5 + 5";

    std::cout << "s= " << s.c_str() << std::endl;

    s += " = 10";
    std::cout << "- s += \" = 10\"; s= " << s.c_str() << std::endl;

    s+='*';s+='1';s+=';';
    std::cout << "- s+='*';s+='1'; s= " << s.c_str() << std::endl;

    s+=s.c_str();
    std::cout << "- s+=s.c_str(); s= " << s.c_str() << std::endl;

    s+=s;
    std::cout << "- s+=s; s= " << s.c_str() << std::endl;
}

int main() {
    // Operations
//    test_clear();
//    test_replace();
//    test_insert();
//    test_erase();
//    test_push_pop_back();
    test_operator_plus_assign();

//    test_erase();
//    test_resize();
}

