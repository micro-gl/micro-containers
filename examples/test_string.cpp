//#define MICRO_CONTAINERS_ENABLE_THROW
#include "src/test_utils.h"
#include <micro-containers/string.h>
#include <micro-containers/algorithm.h>

using namespace microc;
#include <string>

void print_string(const string & str) {
    std::cout << str.c_str() << std::endl;
}

void test_replace() {
    print_test_header("test_replace");

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

void test_append() {
    print_test_header("test_append");

    basic_string<char> str = "string";
    const char* cptr = "C-string";
    const char carr[] = "Two and one";

    string output;

    // 1) Append a char 3 times.
    // Notice, this is the only overload accepting chars.
    output.append(3, '*');
    std::cout << "1) " << output.c_str() << "\n";

    //  2) Append a whole string
    output.append(str);
    std::cout << "2) " << output.c_str() << "\n";

    // 3) Append part of a string (last 3 letters, in this case)
    output.append(str, 3, 3);
    std::cout << "3) " << output.c_str() << "\n";

    // 4) Append part of a C-string
    // Notice, because `append` returns *this, we can chain calls together
    output.append(1, ' ').append(carr, 4);
    std::cout << "4) " << output.c_str() << "\n";

    // 5) Append a whole C-string
    output.append(cptr);
    std::cout << "5) " << output.c_str() << "\n";

    // 6) Append range
    output.append(&carr[3], microc::end(carr)-1);
    std::cout << "6) " << output.c_str() << "\n";

    // 7) Append initializer list
    output.append(std::initializer_list<char>{ ' ', 'l', 'i', 's', 't' });
    std::cout << "7) " << output.c_str() << "\n";}

void test_substr() {
    print_test_header("test_substr");

    string a = "0123456789abcdefghij";

    std::cout << "- a= " << a.c_str() << std::endl;

    // count is npos, returns [pos, size())
    string sub1 = a.substr(10);
    std::cout << "- sub1= " << sub1.c_str() << '\n';

    // both pos and pos+count are within bounds, returns [pos, pos+count)
    string sub2 = a.substr(5, 3);
    std::cout << "- sub2 = a.substr(5, 3); sub2= " << sub2.c_str() << '\n';

    // pos is within bounds, pos+count is not, returns [pos, size())
    string sub4 = a.substr(a.size()-3, 50);
    std::cout << "- sub4 = a.substr(a.size()-3, 50); sub4= " << sub4.c_str() << '\n';
    // this is effectively equivalent to
    // string sub4 = a.substr(17, 3);
    // since a.size() == 20, pos == a.size()-3 == 17, and a.size()-pos == 3

#ifdef MICRO_CONTAINERS_ENABLE_THROW
    try {
        // pos is out of bounds, throws
        string sub5 = a.substr(a.size()+3, 50);
        std::cout << sub5.c_str() << '\n';
    } catch(string::out_of_bounds_exception) {
        std::cout << "pos exceeds string size\n";
    }
#endif
}

void test_copy() {
    print_test_header("test_copy");

    string foo("quuuux");
    char bar[7]{};
    foo.copy(bar, sizeof bar);
    std::cout << "- foo= " << foo.c_str() << '\n';
    std::cout << "- bar= " << bar << '\n';
}

void test_resize() {
    print_test_header("test_resize");

    const unsigned  desired_length{ 8 };
    string     long_string( "Where is the end?" );
    string     short_string( "Ha" );

    std::cout << "Basic functionality:\n"
              << "Shorten:\n"
              << "1. Before: " << long_string.c_str() << '\n';

    long_string.resize( desired_length );

    std::cout << "2. After:  " << long_string.c_str() << '\n';

    std::cout << "Lengthen:\n"
              << "3. Before: " << short_string.c_str() << '\n';

    short_string.resize( desired_length, 'a' );

    std::cout << "4. After:  " << short_string.c_str() <<  "\n\n";
}

void test_compare() {
    print_test_header("test_compare");

    // 1) Compare with other string
    {
        int compare_value{
                string{"Batman"}.compare(string{"Superman"})
        };
        std::cout << (
                compare_value < 0 ? "Batman comes before Superman\n" :
                compare_value > 0 ? "Superman comes before Batman\n" :
                "Superman and Batman are the same.\n"
        );
    }

    // 2) Compare substring with other string
    {
        int compare_value{
                string{"Batman"}.compare(3, 3, string{"Superman"})
        };
        std::cout << (
                compare_value < 0 ? "man comes before Superman\n" :
                compare_value > 0 ? "Superman comes before man\n" :
                "man and Superman are the same.\n"
        );
    }

    // 3) Compare substring with other substring
    {
        string a{"Batman"};
        string b{"Superman"};

        int compare_value{a.compare(3, 3, b, 5, 3)};

        std::cout << (
                compare_value < 0 ? "man comes before man\n" :
                compare_value > 0 ? "man comes before man\n" :
                "man and man are the same.\n"
        );
        // Compare substring with other substring
        // defaulting to end of other string
        assert(compare_value == a.compare(3, 3, b, 5));
    }

    // 4) Compare with char pointer
    {
        int compare_value{string{"Batman"}.compare("Superman")};

        std::cout << (
                compare_value < 0 ? "Batman comes before Superman\n" :
                compare_value > 0 ? "Superman comes before Batman\n" :
                "Superman and Batman are the same.\n"
        );
    }

    // 5) Compare substring with char pointer
    {
        int compare_value{string{"Batman"}.compare(3, 3, "Superman")};

        std::cout << (
                compare_value < 0 ? "man comes before Superman\n" :
                compare_value > 0 ? "Superman comes before man\n" :
                "man and Superman are the same.\n"
        );
    }

    // 6) Compare substring with char pointer substring
    {
        int compare_value{string{"Batman"}.compare(0, 3, "Superman", 5)};

        std::cout << (
                compare_value < 0 ? "Bat comes before Super\n" :
                compare_value > 0 ? "Super comes before Bat\n" :
                "Super and Bat are the same.\n"
        );
    }
}

void test_starts_with() {
    print_test_header("test_starts_with");

    auto s = string("hello world");
    string prefix = "hello";
    std::cout << '\'' << s.c_str() << "' starts with '" << prefix.c_str() << "': " <<
              s.starts_with(prefix) << '\n';
    prefix = "goodbye";
    std::cout << '\'' << s.c_str() << "' starts with '" << prefix.c_str() << "': " <<
              s.starts_with(prefix.c_str()) << '\n';
    std::cout << '\'' << s.c_str() << "' starts with char '" << 'h' << "': " <<
              s.starts_with('h') << '\n';
    std::cout << '\'' << s.c_str() << "' starts with char '" << 'x' << "': " <<
              s.starts_with('x') << '\n';
}

void test_ends_with() {
    print_test_header("test_ends_with");

    auto s = string("hello world");
    string prefix = "world";
    std::cout << '\'' << s.c_str() << "' ends with '" << prefix.c_str() << "': " <<
              s.ends_with(prefix) << '\n';
    prefix = "goodbye";
    std::cout << '\'' << s.c_str() << "' ends with '" << prefix.c_str() << "': " <<
              s.ends_with(prefix.c_str()) << '\n';
    std::cout << '\'' << s.c_str() << "' ends with char '" << 'd' << "': " <<
              s.ends_with('d') << '\n';
    std::cout << '\'' << s.c_str() << "' ends with char '" << 'x' << "': " <<
              s.ends_with('x') << '\n';
}

void test_contains() {
    print_test_header("test_contains");

    auto s = string("hello world");
    string prefix = "hello";
    std::cout << '\'' << s.c_str() << "' contains '" << prefix.c_str() << "': " <<
              s.contains(prefix) << '\n';
    prefix = "goodbye";
    std::cout << '\'' << s.c_str() << "' contains '" << prefix.c_str() << "': " <<
              s.contains(prefix.c_str()) << '\n';
    prefix = "llo w";
    std::cout << '\'' << s.c_str() << "' contains '" << prefix.c_str() << "': " <<
              s.contains(prefix.c_str()) << '\n';
    prefix = s;
    std::cout << '\'' << s.c_str() << "' contains '" << prefix.c_str() << "': " <<
              s.contains(prefix.c_str()) << '\n';
    std::cout << '\'' << s.c_str() << "' contains char '" << 'd' << "': " <<
              s.contains('d') << '\n';
    std::cout << '\'' << s.c_str() << "' contains char '" << 'x' << "': " <<
              s.contains('x') << '\n';
}

void test_find() {
    print_test_header("test_find");

    const auto print = [](string::size_type n, string const &s) {
        if (n == string::npos) std::cout << "not found\n";
        else std::cout << "found: \"" << s.substr(n).c_str() << '\n';
    };

    string::size_type n;
    string const s = "This is a string";
    std::cout << "- " << s.c_str() << std::endl;

    // search from beginning of string
    n = s.find("is");
    print(n, s);

    // search from position 5
    n = s.find("is", 5);
    print(n, s);

    // find a single character
    n = s.find('a');
    print(n, s);

    // find a single character
    n = s.find('q');
    print(n, s);
}

void test_rfind() {
    print_test_header("test_rfind");

    const auto print = [](string::size_type n, string const &s) {
        if (n == string::npos) std::cout << "not found\n";
        else std::cout << "found: \"" << s.substr(n).c_str() << "\" at " << n << '\n';
    };

    string::size_type n;
    string const s = "This is a string";
    std::cout << "- " << s.c_str() << std::endl;

    // search from beginning of string
    n = s.rfind("is");
    print(n, s);

    // search from position 4
    n = s.rfind("is", 4);
    print(n, s);

    // find a single character
    n = s.rfind('s');
    print(n, s);

    // find a single character
    n = s.rfind('q');
    print(n, s);
}

void test_find_first_of() {
    print_test_header("test_find_first_of");

    // the test string
    string str = string("Hello World!");

    // strings and chars to search for
    string search_str = string("o");
    const char* search_cstr = "Good Bye!";

    std::cout << str.find_first_of(search_str) << '\n';
    std::cout << str.find_first_of(search_str, 5) << '\n';
    std::cout << str.find_first_of(search_cstr) << '\n';
    std::cout << str.find_first_of(search_cstr, 0, 4) << '\n';
    // 'x' is not in "Hello World', thus it will return string::npos
    std::cout << str.find_first_of('x') << '\n';
}

void test_find_last_of() {
    print_test_header("test_find_last_of");

    const string path="/root/config";
    auto const pos=path.find_last_of('/');
    const auto leaf=path.substr(pos+1);

    std::cout << leaf.c_str() << '\n';
}

void test_find_first_not_of() {
    print_test_header("test_find_first_not_of");

    string to_search = "Some data with %MACROS to substitute";

    std::cout << "Before: " << to_search.c_str() << '\n';

    auto pos = string::npos;
    while ((pos = to_search.find('%')) != string::npos) {
        // Permit uppercase letters, lowercase letters and numbers in macro names
        const auto after = to_search.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                       "abcdefghijklmnopqrstuvwxyz"
                                                       "0123456789", pos + 1);

        // Now to_search[pos] == '%' and to_search[after] == ' ' (after the 'S')

        if(after != string::npos)
            to_search.replace(pos, after - pos, "some very nice macros");
    }

    std::cout << "After: " << to_search.c_str() << '\n';

}

void test_find_last_not_of() {
    print_test_header("test_find_last_not_of");

    const auto show_pos = [](const string& str, string::size_type found) {
        if (found != string::npos) std::cout << "[" << found << "] = \'" << str[found] << "\'\n";
        else std::cout << "not found" "\n";
    };

    string str { "abc_123" };
    char const* skip_set { "0123456789" };
    string::size_type str_last_pos { string::npos };

    show_pos(str, str.find_last_not_of(skip_set)); // [3] = '_'

    str_last_pos = 2;
    show_pos(str, str.find_last_not_of(skip_set, str_last_pos)); // [2] = 'c'

    str_last_pos = 2;
    show_pos(str, str.find_last_not_of('c', str_last_pos)); // [1] = 'b'

    const char arr[] { '3','4','5' };
    show_pos(str, str.find_last_not_of(arr)); // [5] = '2'

    str_last_pos = 2;
    string::size_type skip_set_size { 4 };
    show_pos(str, str.find_last_not_of(skip_set,
                                       str_last_pos,
                                       skip_set_size)); // [2] = 'c'

    show_pos(str, str.find_last_not_of("abc")); // [6] = '3'

    str_last_pos = 2;
    show_pos(str, str.find_last_not_of("abc", str_last_pos)); // not found
}

void test_assignment_operator() {
    print_test_header("test_assignment_operator");

    string from = "from";
    string from_copied = "from_copied";
    string from_moved = "from_moved";

    std::cout << "- from = " << from.c_str() << '\n';
    std::cout << "- from_copied = " << from_copied.c_str() << '\n';
    std::cout << "- from_moved = " << from_moved.c_str() << '\n';
    from_copied = from;
    std::cout << "- from_copied = from; from_copied = " << from_copied.c_str() << '\n';
    from_moved = std::move(from);
    std::cout << "- from_moved = std::move(from); from_moved = " << from_moved.c_str() << '\n';
    from = "from_string";
    std::cout << "- from = \"from_string\"; from = " << from.c_str() << '\n';
    from = 'C';
    std::cout << "- from = 'C'; from = " << from.c_str() << '\n';
}

void test_assign() {
    print_test_header("test_assign");

    string s;
    // assign(size_type count, CharT ch)
    s.assign(4, '=');
    std::cout << s.c_str() << '\n'; // "===="

    string const c("Exemplary");
    // assign(basic_string const& str)
    s.assign(c);
    std::cout << c.c_str() << " == " << s.c_str() <<'\n'; // "Exemplary == Exemplary"

    // assign(basic_string const& str, size_type pos, size_type count)
    s.assign(c, 0, c.length()-1);
    std::cout << s.c_str() << '\n'; // "Exemplar";

    // assign(basic_string&& str)
    s.assign(string("C++ by ") += "example");
    std::cout << s.c_str() << '\n'; // "C++ by example"

    // assign(charT const* s, size_type count)
    s.assign("C-style string", 7);
    std::cout << s.c_str() << '\n'; // "C-style"

    // assign(charT const* s)
    s.assign("C-style\0string");
    std::cout << s.c_str() << '\n'; // "C-style"

    char mutable_c_str[] = "C-style string";
    // assign(InputIt first, InputIt last)
    s.assign(microc::begin(mutable_c_str), microc::end(mutable_c_str)-1);
    std::cout << s.c_str() << '\n'; // "C-style string"

    // assign(std::initializer_list<charT> ilist)
    s.assign(std::initializer_list<char>{ 'C', '-', 's', 't', 'y', 'l', 'e' });
    std::cout << s.c_str() << '\n'; // "C-style"
}

void test_ctors() {
    print_test_header("test_ctors");

    {
        std::cout << "1) string(); ";
        string s;
        assert(s.empty() && (s.length() == 0) && (s.size() == 0));
        std::cout << "s.capacity(): " << s.capacity() << '\n'; // unspecified
    }

    {
        std::cout << "2) string(size_type count, charT ch): ";
        string s(4, '=');
        std::cout << s.c_str() << '\n'; // "===="
    }

    {
        std::cout << "3) string(const string& other, size_type pos, size_type count): ";
        string const other("Exemplary");
        string s(other, 0, other.length()-1);
        std::cout << s.c_str() << '\n'; // "Exemplar"
    }

    {
        std::cout << "4) string(const string& other, size_type pos): ";
        string const other("Mutatis Mutandis");
        string s(other, 8);
        std::cout << s.c_str() << '\n'; // "Mutandis", i.e. [8, 16)
    }

    {
        std::cout << "5) string(charT const* s, size_type count): ";
        string s("C-style string", 7);
        std::cout << s.c_str() << '\n'; // "C-style", i.e. [0, 7)
    }

    {
        std::cout << "6) string(charT const* s): ";
        string s("C-style\0string");
        std::cout << s.c_str() << '\n'; // "C-style"
    }

    {
        std::cout << "7) string(InputIt first, InputIt last): ";
        char mutable_c_str[] = "another C-style string";
        string s(std::begin(mutable_c_str)+8, std::end(mutable_c_str)-1);
        std::cout << s.c_str() << '\n'; // "C-style string"
    }

    {
        std::cout << "8) string(string&): ";
        string const other("Exemplar");
        string s(other);
        std::cout << s.c_str() << '\n'; // "Exemplar"
    }

    {
        std::cout << "9) string(string&&): ";
//        string s(string("C++ by ") + string("example"));
//        std::cout << s.c_str() << '\n'; // "C++ by example"
    }

    {
        std::cout << "α) string(std::initializer_list<charT>): ";
        string s(std::initializer_list<char>{ 'C', '-', 's', 't', 'y', 'l', 'e' });
        std::cout << s.c_str() << '\n'; // "C-style"
    }
    {    // before C++11, overload resolution selects string(InputIt first, InputIt last)
        // [with InputIt = int] which behaves *as if* string(size_type count, charT ch)
        // after C++11 the InputIt constructor is disabled for integral types and calls:
        std::cout << "β) string(size_type count, charT ch) is called: ";
        string s(3, 'A');
        std::cout << s.c_str() << '\n'; // "AAA"
    }
}

void test_concat_operator_plus() {
    print_test_header("test_concat_operator_plus");

    string s1 = "Hello";
    string s2 = "world";
    std::cout << (s1 + ' ' + s2 + "!\n" + s1.c_str() + ' ' + s2.c_str()).c_str();
}

void test_compare_operator() {
    print_test_header("test_compare_operator");

    string s1 = "Hello";
    string s2 = "world";
    std::cout << "- s1=" << s1.c_str() << std::endl;
    std::cout << "- s2=" << s2.c_str() << std::endl;
    std::cout << "- s1==s1 -> " << (s1==s1) << std::endl;
    std::cout << "- s1==s2 -> " << (s1==s2) << std::endl;
    std::cout << "- s1==\"Hello\" -> " << (s1=="Hello") << std::endl;
    std::cout << "- \"Hello\"==s1 -> " << ("Hello"==s1) << std::endl;
    std::cout << "- \"Hello\"==s2 -> " << ("Hello"==s2) << std::endl;
    std::cout << "- \"Hello\"!=s2 -> " << ("Hello"!=s2) << std::endl;
    std::cout << "- s1!=s2 -> " << (s1!=s2) << std::endl;
    std::cout << "- s1==s2 -> " << (s1==s2) << std::endl;
}

void test_strtol() {
    print_test_header("test_strtol");
    char a ='0';
    char a1 ='a';
    char a2 ='a';
    const char * number1 = "543";
    const char * number2 = "0xFF";
    const char * number3 = "0xFFFF";
    const char * number4 = "0xF1";
    const char * number5 = "0b111";
    std::cout << "- string = " << number1 << ", number = " << microc::strtol(number1, nullptr) << std::endl;
    std::cout << "- string = " << number2 << ", number = " << microc::strtol(number2, nullptr, 0) << std::endl;
    std::cout << "- string = " << number3 << ", number = " << microc::strtol(number3, nullptr, 0) << std::endl;
    std::cout << "- string = " << number4 << ", number = " << microc::strtol(number4, nullptr, 0) << std::endl;
    std::cout << "- string = " << number5 << ", number = " << microc::strtol(number5, nullptr, 0) << std::endl;
}

void test_stoi_l_ll() {
    print_test_header("test_stoi_l_ll");
    char a ='0';
    char a1 ='a';
    char a2 ='a';
    string number1 = "-543";
    string number2 = "-0xFF";
    string number3 = "0xFFFF";
    string number4 = "+0xF1";
    string number5 = "-0b111";
    std::cout << "- string = " << number1.c_str() << ", number = " << microc::stoi(number1, nullptr) << std::endl;
    std::cout << "- string = " << number2.c_str() << ", number = " << microc::stoi(number2, nullptr, 0) << std::endl;
    std::cout << "- string = " << number3.c_str() << ", number = " << microc::stoi(number3, nullptr, 0) << std::endl;
    std::cout << "- string = " << number4.c_str() << ", number = " << microc::stoi(number4, nullptr, 0) << std::endl;
    std::cout << "- string = " << number5.c_str() << ", number = " << microc::stoi(number5, nullptr, 0) << std::endl;
}

void test_hash() {
    print_test_header("test_hash");

    string s1 = "Stand back! I've got jimmies!";
    string s2 = "ab";
    string s3 = "ba";
    string s4 = "0";

    std::cout << microc::hash<string>{}(s1) << '\n';
    std::cout << microc::hash<string>{}(s2) << '\n';
    std::cout << microc::hash<string>{}(s3) << '\n';
    std::cout << microc::hash<string>{}(s4) << '\n';
}

void test_to_string() {
    print_test_header("test_to_string");

    for (const float f : {23.01f, -50.05f})
        std::cout << "std::cout: " << f << '\n'
                  << "to_string: " << microc::to_string(f).c_str() << "\n\n";

    for (const int f : {23, -23, 12345})
        std::cout << "std::cout: " << f << '\n'
                  << "to_string: " << microc::to_string(f).c_str() << "\n\n";
}

int main() {
    test_to_string();
    return 0 ;

    // Operations

    test_clear();
    test_insert();
    test_erase();
    test_push_pop_back();
    test_operator_plus_assign();
    test_append();
    test_replace();
    test_substr();
    test_copy();
    test_resize();

    // Search / Queries

    test_compare();
    test_starts_with();
    test_ends_with();
    test_contains();
    test_find();
    test_rfind();
    test_find_first_of();
    test_find_last_of();
    test_find_first_not_of();
    test_find_last_not_of();

    // Copy / Move Assign operator

    test_assignment_operator();
    test_assign();

    // Constructors

    test_ctors();

    // misc

    test_concat_operator_plus();
    test_compare_operator();

    // stoi

    test_strtol();
    test_stoi_l_ll();
    test_to_string();

    // Hash
    test_hash();

}

