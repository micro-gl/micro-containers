/*========================================================================================
 Copyright (2021), Tomer Shalev (tomer.shalev@gmail.com, https://github.com/HendrixString).
 All Rights Reserved.
 License is a custom open source semi-permissive license with the following guidelines:
 1. unless otherwise stated, derivative work and usage of this file is permitted and
    should be credited to the project and the author of this project.
 2. Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
========================================================================================*/
#pragma once

#include "traits.h"
#include <cmath>

namespace microc {

    template<class CharT> struct char_traits {};

    template<class CharT, class Int_type, class Off_type, class Pos_type,
             class State_type, Int_type E_O_F>
    struct __common_char_traits {
        using char_type = CharT;
        using int_type = Int_type;
        using off_type = Off_type;
        using pos_type = Pos_type;
        using state_type = State_type;

        static void assign(char_type& r, const char_type& a) noexcept { r=a; }
        static char_type* assign(char_type* p, microc::size_t count, char_type a) {
            for (microc::size_t ix = 0; ix < count; ++ix) assign(*(p++), a);
        }
        static bool eq(char_type a, char_type b) { return a==b; }
        static bool lt(char_type a, char_type b) { return a<b; }
        static char_type* move(char_type* dest, const char_type* src, microc::size_t count) {
            if (count == 0) return dest;
            char_type* r = dest;
            if (dest < src) {
                for (; count; --count, ++dest, ++src) assign(*dest, *src);
            } else if (src < dest) {
                dest += count; src += count;
                for (; count; --count) assign(*--dest, *--src);
            }
            return r;
        }
        static char_type* copy(char_type* dest, const char_type* src, microc::size_t count) {
            for(microc::size_t ix = 0; ix < count; ++ix) { assign(*(dest++), *(src++)); }
            return dest;
        }
        static int compare(const char_type* s1, const char_type* s2, std::size_t count) {
            for(microc::size_t ix = 0; ix < count; ++ix, s1++, s2++) {
                if(lt(*s1, *s2)) return -1;
                if(lt(*s2, *s1)) return 1;
            }
            return 0;
        }
        static microc::size_t length(const char_type* s) {
            microc::size_t count = 0;
            while(*(s++)!=char(0)) { ++count;}
            return count;
        }
        static const char_type* find(const char_type* p, microc::size_t count, const char_type& ch) {
            for(microc::size_t ix = 0; ix < count; ++ix, ++p)
                if(eq(*p, ch)) return p;
        }
        static char_type to_char_type(int_type c) { return char_type(c); }
        static int_type to_int_type(char_type c) { return int_type(c); }
        static bool eq_int_type(int_type c1, int_type c2) { return c1==c2; }
        static int_type eof() { return E_O_F; }
        static int_type not_eof(int_type e) { return eq_int_type(e,eof()) ? ~eof() : e; }
    };

    template<> struct char_traits<char> :
            public __common_char_traits<char, int, microc::size_t, void, void, -1> {};

    template<> struct char_traits<unsigned char> :
            public __common_char_traits<unsigned char, int, microc::size_t, void, void, 0xFF> {};

    template<> struct char_traits<char16_t> :
            public __common_char_traits<char16_t, unsigned short, microc::size_t, void, void, 0xFFFF> {};

    template<> struct char_traits<char32_t> :
            public __common_char_traits<char32_t, unsigned int, microc::size_t, void, void, 0xFFFFFFFF> {};

    /**
     * Basic String, Allocator-aware, but does not propagate allocators on copy/move.
     * @tparam CharT The type for a character
     * @tparam Traits The implementation of basic character operations
     * @tparam Allocator The allocator
     */
    template<class CharT,
             class Traits = microc::char_traits<CharT>,
             class Allocator = microc::std_allocator<CharT>>
    class basic_string {
    public:
        using traits_type = Traits;
        using value_type = CharT;
        using allocator_type = Allocator;
        using size_type = microc::size_t;
        using difference_type = microc::ptrdiff_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using iterator = pointer;
        using const_iterator = const_pointer;
        static const size_type npos = ~size_type(0);
        struct out_of_bounds_exception {};
#define minnnn(a,b) ((a)<(b) ? (a) : (b))
    private:
        using rebind_allocator_type = typename allocator_type::template rebind<value_type>::other;
        void throw_out_of_bounds_exception_if_can() const {
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            throw out_of_bounds_exception();
#endif
        }
        struct iter_single {
            using dtype = difference_type;
            CharT * ch; size_type i;
            iter_single(CharT * ch, size_type index) : ch(ch), i(index) {}
            iter_single operator++() { return *this; }
            iter_single operator--() { return *this; }
            difference_type operator-(const iter_single & o) { return dtype(i) - dtype(o.i); }
            bool operator!=(const iter_single & o) const { return i!=o.i; }
            CharT & operator *() { return *ch; }
        };

        value_type * _data;
        rebind_allocator_type _alloc;
        size_type _cap;
        size_type _current;
        static CharT _null_char;

    public:
        // ctors
        basic_string() : basic_string(Allocator()) {}
        explicit basic_string(const Allocator& alloc) noexcept :
                _alloc(alloc), _data(nullptr), _cap(0), _current(0) {};
        basic_string(size_type count, CharT ch, const Allocator& alloc = Allocator()) :
                basic_string(alloc) {
            reserve(count);
            for (size_type ix = 0; ix < count; ++ix) push_back(ch);
        }
        basic_string(const basic_string& other, size_type pos, size_type count=npos,
                     const Allocator& alloc = Allocator()) : basic_string(alloc) {
            count = minnnn(count, other.size()-pos);
            reserve(count);
            for (; count; --count) push_back(other[pos++]);
        }
        basic_string(const CharT* s, size_type count, const Allocator& alloc = Allocator()) :
                basic_string(alloc) {
            reserve(count);
            for (size_type ix = 0; ix < count; ++ix) push_back(s[ix]);
        }
        basic_string(const CharT* s, const Allocator& alloc = Allocator()) :
                basic_string(s, traits_type::length(s), alloc) {}
        template<class InputIt, typename = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        basic_string(InputIt first, InputIt last, const Allocator& alloc = Allocator()) :
                basic_string(alloc) {
            reserve(last-first);
            for(;first!=last; ++first) { push_back(*first); }
        }
        basic_string(const basic_string& other, const Allocator& alloc) :
                        basic_string(other.begin(), other.end(), alloc) {}
        basic_string(const basic_string& other) : basic_string(other, other.get_allocator()) {}
        basic_string(basic_string&& other) noexcept {
            _data = other._data;
            _current = other._current;
            _cap = other._cap;
            other._data=nullptr;
            other._cap=0;
            other._current=0;
        }
        basic_string(basic_string&& other, const Allocator& alloc) : basic_string(alloc) {
            reserve(other.size());
            for (auto & item : other) push_back(microc::traits::move(item));
        }
        template<class Iterable>
        explicit basic_string(const Iterable &list, const Allocator & alloc= Allocator()) :
                basic_string(list.begin(), list.end(), alloc) {}

        ~basic_string() noexcept { drain(); }

        // Iterators
        const_iterator begin() const noexcept {return _data;}
        const_iterator end() const noexcept {return _data + size();}
        iterator begin() noexcept {return _data;}
        iterator end()  noexcept {return _data + size();}

        // etc..
        Allocator get_allocator() const noexcept { return Allocator(_alloc); }

        // Element access
        reference operator[](size_type i) { return _data[i]; }
        const_reference operator[](size_type i) const { return _data[i]; }
        reference at(size_type pos) {
            if(pos>=0 && pos < size()) return _data[pos];
            throw_out_of_bounds_exception_if_can();
        }
        const_reference at(size_type pos) const {
            if(pos>=0 && pos < size()) return _data[pos];
            throw_out_of_bounds_exception_if_can();
        }
        CharT* data() noexcept { return size() ? _data : &_null_char; }
        const CharT* data() const noexcept { return size() ? _data : &_null_char;; }
        const CharT* c_str() const noexcept { return data(); }
        reference back() { return _data[_current-1]; }
        reference front() { return _data[0]; }
        const_reference back() const { return _data[_current-1]; }
        const_reference front() const { return _data[0]; }

        // Capacity
        bool empty() noexcept { return _current==0; }
        size_type size() const noexcept { return _current; }
        size_type length() const noexcept { return _current; }
        size_type capacity() const noexcept { return _cap; }
        void shrink_to_fit() { re_alloc_to(size()); }
        void reserve(size_type new_cap) {
            if(new_cap <= capacity()) return;
            re_alloc_to(new_cap);
        }

        // Assignment
        basic_string & operator=(const basic_string & other) noexcept {
            if(this!= &other) {
                clear(); reserve(other.size());
                for(size_type ix = 0; ix < other.size(); ix++) push_back(other[ix]);
            }
            return (*this);
        }
        basic_string & operator=(basic_string && other) noexcept {
            // two cases:
            // 1. if the allocators are equal, then move the data completely.
            // 2. otherwise, move element by element
            const bool are_equal = _alloc == other.get_allocator();
            if(this==&other) return *this;
            clear();
            if(are_equal) {
                // clear and destruct current elements
                // deallocate the current memory
                _alloc.deallocate(_data, capacity());
                // move everything from other
                _data = other._data;
                _current = other._current;
                _cap = other._cap;
                // no need to de-allocate because we stole the memory
                other._data=nullptr;
                other._cap=0;
                other._current=0;
            } else {
                if(other.size() >= capacity()) {
                    _alloc.deallocate(_data, capacity()); // de allocate the chunk
                    _data = _alloc.allocate(other.capacity()); // create new chunk
                }
                // move other items into current memory
                for (size_type ix = 0; ix < other.size(); ++ix)
                    ::new (_data + ix, microc_new::blah) CharT(microc::traits::move(other[ix]));
                // no need to destruct other's items
                _current = other._current;
                _cap = other._cap;
                // different allocators, we do not de-allocate the other array.
            }
            return (*this);
        }
        basic_string& operator=(const CharT* s) {
            const auto len = traits_type::length(s);
            clear(); reserve(len);
            for (size_type ix = 0; ix < len; ++ix) push_back(s[ix]);
            return *this;
        }
        basic_string& operator=(CharT ch) { clear(); reserve(2); push_back(ch);  return *this; }
        template<class Iterable>
        basic_string& operator=(const Iterable & other) {
            clear(); reserve(other.size());
            for (const auto & item : other) push_back(item);
            return *this;
        }
        // assign
        template<class InputIt, typename = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        basic_string& assign(InputIt first, InputIt last) {
            const auto count = last-first;
            clear(); reserve(count);
            for(size_type ix = 0; ix < count; ix++, ++first) push_back(*first);
            return *this;
        }
        template<class Iterable> basic_string& assign(const Iterable & iterable) {
            return assign<decltype(iterable.begin())>(iterable.begin(), iterable.end());
        }
        basic_string& assign(const CharT* s, size_type count) { return assign<const CharT*>(s, s+count); }
        basic_string& assign(const CharT* s) { return assign(s, traits_type::length(s)); }
        basic_string& assign(basic_string&& str) noexcept { return this->operator=(microc::traits::move(str)); }
        basic_string& assign(const basic_string& str, size_type pos, size_type count = npos) {
            count = minnnn(count, str.size());
            return assign<const_iterator>(str.begin()+pos, str.begin()+pos+count);
        }
        basic_string& assign(const basic_string& str) { return this->operator=(str); }
        basic_string& assign(size_type count, CharT ch) {
            return assign<iter_single>(iter_single(&ch, 0), iter_single(&ch, count));
        }

        // Modifiers
    private:
        void alloc_(bool up) noexcept {
            const auto new_cap = up ? (_cap==0?1:_cap*2) : _cap/2;
            re_alloc_to(new_cap);
        }
        void re_alloc_to(size_type new_capacity) noexcept {
            if(capacity()==new_capacity) return;
            // if requested cap is not zero, then reserve another place for null-terminated char
            if(new_capacity>0) new_capacity+=1;
            // re-alloc to another capacity and move old elements
            const auto old_size = _current;
            const auto copy_size = old_size < new_capacity ? old_size : new_capacity-1;

            CharT * _new_data = nullptr;
            if(new_capacity) {
                _new_data = _alloc.allocate(new_capacity);
                // move all previous objects into new location,
                // therefore we do not need to destruct because we move from same allocator
                if(copy_size) {
                    for (size_type ix = 0; ix < copy_size; ++ix) // including null terminator
                        ::new (_new_data + ix, microc_new::blah) CharT(microc::traits::move(_data[ix]));
                }
                // add null termination chars between size and capacity in new memory
                for (int ix = copy_size; ix < new_capacity; ++ix)
                    ::new (_new_data + ix, microc_new::blah) CharT(0);
            }
            if(_data) { // de allocate old data
                // destruct suffix items in old memory, that didn't make it to new memory,
                // This happens when shrinking is happening.
                for (size_type ix = copy_size; ix < old_size; ++ix) (_data + ix)->~value_type();
                _alloc.deallocate(_data, capacity());
            }
            _data = _new_data;
            _cap = new_capacity;
        }
        void write_null_termination_at_end() { if (capacity()) traits_type::assign(*end(), value_type(0)); }
        template<class TT> void internal_push_back(TT && v) noexcept {
            if(size() >= capacity()) {
                // copy the value, edge case if v belongs
                // to the dynamic array
                CharT vv = microc::traits::forward<TT>(v); // copy-or-move ctor
                alloc_(true);
                ::new(_data + _current++, microc_new::blah) CharT(microc::traits::move<TT>(vv));
            } else ::new(_data + _current++, microc_new::blah) CharT(microc::traits::forward<TT>(v));
            write_null_termination_at_end();
        }
        template<class TT>
        void internal_resize(size_type count, TT && perfect_forwarded_value) {
            const size_type curr_size = size();
            if(count==curr_size) return;
            if(count < curr_size) { // reduce size
                const auto delta = curr_size-count;
                for (size_type ix = 0; ix < delta; ++ix) pop_back();
            } else { // increase capacity and fill extra items
                const auto delta = count-curr_size;
                reserve(curr_size+delta);
                for (size_type ix = 0; ix < delta; ++ix)
                    push_back(microc::traits::forward<TT>(perfect_forwarded_value));
            }
            write_null_termination_at_end();
        }

    public:
        basic_string& operator+=(const basic_string & str) {
            reserve(size() + str.size());
            for (const auto & item : str) push_back(item);
            return *this;
        }
        basic_string& operator+=(CharT ch) { reserve(size() + 1); push_back(ch); return *this;}
        basic_string& operator+=(const CharT* s) {
            auto len = traits_type::length(s);
            const CharT* s_tag=s;
            auto old_begin = begin();
            auto old_end = end();
            reserve(size() + len);
            {
                if(s>=old_begin and s<=old_end) { // belongs to me
                    // might have changed pointer because of reserve, so recalculate
                    auto delta = s-old_begin;
                    s_tag = begin()+delta;
                }
            }
            for (; len ; --len) { push_back(*(s_tag++)); }
            return *this;
        }
        template<class Iterable> basic_string& operator+=(const Iterable & other) {
            reserve(size() + other.size());
            for (const auto & item : other) push_back(item);
            return *this;
        }

        // append
        template<class InputIt, typename = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        basic_string& append(InputIt first, InputIt last) {
            const auto count = last-first;
            reserve(size() + count);
            for (size_type ix=0; ix < count; ++ix, ++first) push_back(*first);
            return *this;
        }
        template<class Iterable> basic_string& append(const Iterable & iterable) {
            return append<decltype(iterable.begin())>(iterable.begin(), iterable.end());
        }
        basic_string& append(const CharT* s, size_type count) { return append<const CharT*>(s, s+count); }
        basic_string& append(const CharT* s) { return append(s, traits_type::length(s)); }
        basic_string& append(const basic_string& str, size_type pos, size_type count = npos) {
            count = minnnn(count, str.size());
            return append<const_iterator>(str.begin()+pos, str.begin()+pos+count);
        }
        basic_string& append(const basic_string& str) { return this->operator+=(str); }
        basic_string& append(size_type count, CharT ch) {
            return append<iter_single>(iter_single(&ch, 0), iter_single(&ch, count));
        }

        // push back
        void push_back(const CharT & v) noexcept { internal_push_back(v); }
        void push_back(CharT && v) noexcept { internal_push_back(v); }
        void pop_back() {
            if(_current==0) return;
            _data[_current--].~value_type();
            if(size()==0 or (size() < (capacity()>>1))) alloc_(false);
            write_null_termination_at_end();
        }

        template<class... Args>
        iterator emplace(const_iterator pos, Args&&... args) {
            return insert(pos, T(microc::traits::forward<Args>(args)...));
        }
        template<typename... Args>
        reference emplace_back(Args&&... args) {
            if(_current + 1> _cap) alloc_(true);
            ::new (_data + _current++, microc_new::blah) CharT(microc::traits::forward<Args>(args)...);
            return back();
        }

        void clear() noexcept {
//            for (size_type ix = 0; ix < size(); ++ix) _data[ix].~value_type();
            // we assume CharT is non-allocating structure
            _current = 0;
            write_null_termination_at_end();
        }

        void drain() noexcept {
            clear();
            if(_data) _alloc.deallocate(_data, capacity());
            _data = nullptr; _cap = _current = 0;
        }

        // avoid overload resolution if InputIt is integral type, this can conflict with the
        // insert(const_iterator pos, size_type count, const T& value) overload for integral types
        template<class InputIt, typename = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        iterator insert(const_iterator pos, InputIt first, InputIt last) {
            auto delta = pos-begin();
            replace<InputIt>(pos, pos, first, last);
            return begin() + delta;
        }
        template<>
        iterator insert<iterator>(const_iterator pos, iterator first, iterator last) {
            return insert<const_iterator>(pos, const_iterator(first), const_iterator(last));
        }
        template<>
        iterator insert<const_iterator>(const_iterator pos, const_iterator first, const_iterator last) {
            auto delta = pos-begin();
            if(first>=begin() and last<=end()) { // belongs to me
                basic_string temp(first, last, _alloc);
                replace<const_iterator>(pos, pos, temp.begin(), temp.end());
            } else
                replace<const_iterator>(pos, pos, first, last);
            return begin() + delta;
        }
        iterator insert(const_iterator pos, CharT ch) {
            auto delta = pos-begin();
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            replace<const_iterator>(begin()+pos, begin()+pos, 1, ch);
            return begin()+delta;
        }
        // todo: this is ambigious with the index overload until I make iterator a type
        iterator insert2(const_iterator pos, size_type count, CharT ch) {
            auto delta = pos-begin();
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            replace(pos, pos, count, ch);
            return begin()+delta;
        }
        basic_string& insert(size_type index, const basic_string& str, size_type index_str,
                             size_type count = npos) {
            // insert sub_str = str[index_str, index_str+count) in this[index]
            count = minnnn(count, str.size()-index_str);
            if(str.begin()>=begin() and str.end()<=end()) { // belongs to me
                basic_string temp = str.substr(index_str, count);
                insert<const_iterator>(begin()+index, temp.begin(), temp.end());
            } else
                insert<const_iterator>(begin()+index, str.begin()+index_str, str.begin()+index_str+count);
            return *this;
        }
        basic_string& insert(size_type index, const basic_string& str) {
            return insert(index, str, 0, str.size());
        }
        basic_string& insert(size_type index, const CharT* s, size_type count) {
            insert<const_iterator>(begin()+index, s, s+count);
            return *this;
        }
        basic_string& insert(size_type index, const CharT* s) {
            if(s>=begin() && s<=end()) { // belongs to me
                basic_string temp(s, traits_type::length(s), _alloc);
                insert<const_iterator>(begin()+index, temp.begin(), temp.end());
            } else
                insert<const_iterator>(begin()+index, s, s+traits_type::length(s));
            return *this;
        }
        basic_string& insert(size_type index, size_type count, CharT ch) {
            insert2(const_iterator(begin())+index, count, ch);
            return *this;
        }

        basic_string& erase(size_type index = 0, size_type count = npos) {
            if(index>size()) throw_out_of_bounds_exception_if_can();
            count = minnnn(count, size()-index);
            erase(begin()+index, begin()+index+count);
            return *this;
        }
        iterator erase(const_iterator pos) { return erase(pos, pos+1); }
        iterator erase(const_iterator first, const_iterator last) {
            difference_type last_index = last-begin();
            difference_type how_many_to_erase = last-first;
            const_iterator dummy;
            replace<const_iterator>(first, last, dummy, dummy);
            return begin() + last_index - how_many_to_erase;
        }

        void resize(size_type count) { internal_resize(count, CharT()); }
        void resize(size_type count, const value_type& value) { internal_resize(count, value); }

        basic_string substr(size_type pos = 0, size_type count = npos) const {
            if(pos>size()) throw_out_of_bounds_exception_if_can();
            count = minnnn(count, size()-pos);
            return basic_string(*this, pos, count, _alloc);
        }
        size_type copy(CharT* dest, size_type count, size_type pos = 0) const {
            if(pos>size()) throw_out_of_bounds_exception_if_can();
            count = minnnn(count, size()-pos);
            for(; count; --count) { traits_type::copy(dest, _data + pos, count); }
            return count;
        }

        // replace
        basic_string& replace(size_type pos, size_type count, const basic_string& str)
        { return replace(begin()+pos, begin()+pos+count, str.begin(), str.end()); }
        basic_string& replace(const_iterator first, const_iterator last, const basic_string& str)
        { return replace(first, last, str.begin(), str.end()); }
        basic_string& replace(size_type pos, size_type count, const basic_string& str,
                              size_type pos2, size_type count2 = npos ) {
            if(count2>str.size()-pos2) count2=str.size()-pos2;
            return replace(begin()+pos, begin()+pos+count, str.data()+pos2, str.data()+pos2+count2);
        }
        basic_string& replace(size_type pos, size_type count, const CharT* cstr, size_type count2)
        { return replace(begin()+pos, begin()+pos + count, cstr, cstr+count2); }
        basic_string& replace(size_type pos, size_type count, const CharT* cstr)
        { return replace(begin()+pos, begin()+pos+count, cstr, cstr + traits_type::length(cstr)); }
        basic_string& replace(const_iterator first, const_iterator last, const CharT* cstr)
        { return replace(first, last, cstr, traits_type::length(cstr)); }
        basic_string& replace(const_iterator first, const_iterator last, size_type count2, CharT ch) {
            return replace<iter_single>(first, last, iter_single(&ch, 0),  iter_single(&ch, count2));
        }
        template<class InputIt> basic_string& replace(const_iterator first, const_iterator last,
                                                      InputIt first2, InputIt last2) {
            auto count = last-first;
            auto count2 = last2-first2;
            auto pos = first-begin();
            count = minnnn(count, size()-pos);
            const bool is_expending = count2 > count;
            if(is_expending) reserve(size() + count2 - count);
            traits_type::move(_data+pos+count2, _data+pos+count, size()-pos-count);
            for (;count2; --count2, ++_current, ++first2) {
                traits_type::assign(*(_data+pos++), *first2);
            }
            _current -= count;
            write_null_termination_at_end();
            return *this;
        }

        /////
        // query/search operations
        /////
        size_type find(const CharT* s, size_type pos, size_type count) const {
            // Finds the first substring equal to the range [s, s+count) in [_data+pos, _data+size())
            if((pos+count)>size() or (count==0)) return npos;
            auto counter = count;
            for (size_type ix = pos; ix < size() - count + 1 ; ++ix, counter=count) {
                for (size_type jx = 0; jx < count ; ++jx) {
                    const auto & a = *(_data + ix + jx);
                    const auto & b = *(s + jx);
                    counter -= traits_type::eq(a, b) ? 1 : 0;
                }
                if(counter==0) return ix;
            }
            return npos;
        }
        size_type find(const basic_string& str, size_type pos = 0) const noexcept
        { return find(str.data(), pos, str.size()); }
        size_type find(const CharT* s, size_type pos = 0) const
        { return find(s, pos, traits_type::length(s)); }
        size_type find(CharT ch, size_type pos = 0) const { return find(&ch, pos, 1); }
        size_type rfind(const CharT* s, size_type pos, size_type count) const {
            // Finds the last substring equal to the range [s, s+count) in [_data+0, _data+pos]
            if(count==0 or count>size()) return npos;
            pos = minnnn(pos, size()-1);
            auto counter = count;
            for (size_type ix = pos+1; ix; --ix, counter=count) {
                for (size_type jx = 0; jx < count ; ++jx) {
                    const auto & a = *(_data + (ix-1) + jx);
                    const auto & b = *(s + jx);
                    counter -= traits_type::eq(a, b) ? 1 : 0;
                }
                if(counter==0) return ix-1;
            }
            return npos;
        }
        size_type rfind(const basic_string& str, size_type pos = npos) const noexcept
        { return rfind(str.data(), pos, str.size()); }
        size_type rfind(const CharT* s, size_type pos = npos) const
        { return rfind(s, pos, traits_type::length(s)); }
        size_type rfind(CharT ch, size_type pos = npos) const { return rfind(&ch, pos, 1); }

        bool contains(const basic_string & str) const { return find(str, 0) != npos; }
        bool contains(const CharT* s) const { return find(s) != npos; }
        bool contains(CharT c) const noexcept { return find(c) != npos; }

        bool starts_with(CharT c) const noexcept { return find(c)==0; }
        bool starts_with(const CharT* s) const { return find(s)==0; }
        bool starts_with(const basic_string & str) const { return find(str, 0)==0; }
        bool ends_with(const basic_string & str) const {
            if(str.size()>size()) return false;
            const auto pos = size()-str.size();
            return find(str, pos)==pos;
        }
        bool ends_with(CharT c) const noexcept { return find(c, size()-1)==size()-1; }
        bool ends_with(const CharT* s) const {
            const auto str_len = traits_type::length(s);
            if(str_len>size()) return false;
            return find(s, size()-str_len, str_len)==size()-str_len;
        }

        int compare(size_type pos1, size_type count1, const CharT* s, size_type count2) const {
            // Compares a [pos1, pos1+count1) substring of this string to the characters in the
            // range [s, s + count2). If count1 > size() - pos1 the substring is [pos1, size()).
            // (Note: the characters in the range [s, s + count2) may include null characters.)
            size_type sz = size();
            if (pos1 > sz || count2 == npos) {
                throw_out_of_bounds_exception_if_can();
                return 2;
            }
            size_type rlen = minnnn(count1, sz - pos1);
            int r = traits_type::compare(data() + pos1, s, minnnn(rlen, count2));
            if (r == 0) {
                if (rlen < count2) r=-1;
                else if (rlen > count2) r=1;
            }
            return r;
        }
        int compare(size_type pos1, size_type count1, const CharT* s) const
        { return compare(pos1, count1, s, traits_type::length(s)); }
        int compare(const CharT* s) const {
            auto len = traits_type::length(s);
            return compare(0, len, s, len);
        }
        int compare(size_type pos1, size_type count1, const basic_string& str,
                    size_type pos2, size_type count2 = npos ) const {
            count2 = minnnn(count2, str.size()-pos2);
            return compare(pos1, count1, str.data()+pos2, count2);
        }
        int compare(size_type pos1, size_type count1, const basic_string& str) const
        { return compare(pos1, count1, str.data(), str.size()); }
        int compare(const basic_string& str) const noexcept
        { return compare(0, size(), str.data(), str.size()); }
        // find_first_of
        size_type find_first_of(const CharT* s, size_type pos, size_type count) const {
            for(size_type ix = pos; ix < size(); ++ix)
                for(size_type jx = 0; jx < count; ++jx)
                    if(traits_type::eq(*(_data+ix), *(s+jx))) return ix;
            return npos;
        }
        size_type find_first_of(const CharT* s, size_type pos = 0) const
        { return find_first_of(s, pos, traits_type::length(s)); }
        size_type find_first_of(CharT ch, size_type pos = 0) const
        { return find_first_of(&ch, pos, 1); }
        size_type find_first_of(const basic_string& str, size_type pos = 0) const
        { return find_first_of(str.data(), pos, str.size()); }
        // find_last_of
        size_type find_last_of(const CharT* s, size_type pos, size_type count) const {
            pos=minnnn(pos, size()-1);
            for(size_type ix = pos+1; ix; --ix)
                for(size_type jx = 0; jx < count; ++jx)
                    if(traits_type::eq(*(_data+ix-1), *(s+jx))) return (ix-1);
            return npos;
        }
        size_type find_last_of(const CharT* s, size_type pos = npos) const
        { return find_last_of(s, pos, traits_type::length(s)); }
        size_type find_last_of(CharT ch, size_type pos = npos) const
        { return find_last_of(&ch, pos, 1); }
        size_type find_last_of(const basic_string& str, size_type pos = npos) const
        { return find_last_of(str.data(), pos, str.size()); }
        // find_first_not_of
        size_type find_first_not_of(const CharT* s, size_type pos, size_type count) const {
            for(size_type ix = pos; ix < size(); ++ix) {
                bool found = true;
                for (size_type jx = 0; jx < count; ++jx)
                    if (traits_type::eq(*(_data + ix), *(s + jx))) {
                        found=false;break;
                    }
                if(found) return ix;
            }
            return npos;
        }
        size_type find_first_not_of(const CharT* s, size_type pos = 0) const
        { return find_first_not_of(s, pos, traits_type::length(s)); }
        size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
        { return find_first_not_of(str.data(), pos, str.size()); }
        size_type find_first_not_of(CharT ch, size_type pos = 0) const
        { return find_first_not_of(&ch, pos, 1); }
        // find_last_not_of
        size_type find_last_not_of(const CharT* s, size_type pos, size_type count) const {
            // Finds the last character equal to none of the characters in the given character sequence.
            // The search considers only the interval [0, pos]. If the character is not present in
            // the interval, npos will be returned.
            pos = minnnn(pos, size()-1);
            for(size_type ix = pos+1; ix; --ix) {
                bool found = true;
                for (size_type jx = 0; jx < count; ++jx)
                    if (traits_type::eq(*(_data + ix-1), *(s + jx))) {
                        found=false;break;
                    }
                if(found) return ix-1;
            }
            return npos;
        }
        size_type find_last_not_of(const CharT* s, size_type pos = npos) const
        { return find_last_not_of(s, pos, traits_type::length(s)); }
        size_type find_last_not_of(const basic_string& str, size_type pos = npos) const
        { return find_last_not_of(str.data(), pos, str.size()); }
        size_type find_last_not_of(CharT ch, size_type pos = npos) const
        { return find_last_not_of(&ch, pos, 1); }

#undef minnnn
    };

    template<class CharT, class Traits, class Allocator>
    CharT basic_string<CharT, Traits, Allocator>::_null_char = CharT(0);

    using string = basic_string<char, char_traits<char>, microc::std_allocator<char>>;
    using u8string = basic_string<unsigned char, char_traits<unsigned char>, microc::std_allocator<unsigned char>>;
    using u16string = basic_string<char16_t, char_traits<char16_t>, microc::std_allocator<char16_t>>;
    using u32string = basic_string<char32_t, char_traits<char32_t>, microc::std_allocator<char32_t>>;

    // outside equality compare operator
    template<class CharT, class Traits, class Alloc>
    bool operator==(const microc::basic_string<CharT, Traits, Alloc>& lhs,
                    const microc::basic_string<CharT, Traits, Alloc>& rhs) {
        if(lhs.size()!=rhs.size()) return false;
        using size_type = typename microc::basic_string<CharT, Traits, Alloc>::size_type;
        using traits_type = typename microc::basic_string<CharT,Traits, Alloc>::traits_type;
        return traits_type::compare(lhs.c_str(), rhs.c_str(), lhs.size())==0;
    }
    template<class CharT, class Traits, class Alloc>
    bool operator!=(const microc::basic_string<CharT, Traits, Alloc>& lhs,
                    const microc::basic_string<CharT, Traits, Alloc>& rhs) {
        return !(lhs==rhs);
    }

    template<class CharT, class Traits, class Alloc>
    bool operator==(const microc::basic_string<CharT,Traits,Alloc>& lhs, const CharT* rhs) {
        using traits_type = typename microc::basic_string<CharT,Traits,Alloc>::traits_type;
        const auto len = traits_type::length(rhs);
        if(lhs.size()!=len) return false;
        return traits_type::compare(lhs.c_str(), rhs, lhs.size())==0;
    }

    template<class CharT, class Traits, class Alloc>
    bool operator==(const CharT* lhs, const microc::basic_string<CharT,Traits,Alloc>& rhs) {
        using traits_type = typename microc::basic_string<CharT,Traits,Alloc>::traits_type;
        const auto len = traits_type::length(lhs);
        if(rhs.size()!=len) return false;
        return traits_type::compare(lhs, rhs.c_str(), rhs.size())==0;
    }

    template<class CharT, class Traits, class Alloc>
    bool operator!=(const microc::basic_string<CharT,Traits,Alloc>& lhs, const CharT* rhs) {
        return !(lhs==rhs);
    }

    template<class CharT, class Traits, class Alloc>
    bool operator!=(const CharT* lhs, const microc::basic_string<CharT,Traits,Alloc>& rhs) {
        return !(lhs==rhs);
    }

    // outside concat operator
    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(const microc::basic_string<CharT,Traits,Alloc>& lhs,
              const microc::basic_string<CharT,Traits,Alloc>& rhs) {
        auto str = lhs; str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(const microc::basic_string<CharT,Traits,Alloc>& lhs,
              const CharT* rhs) {
        auto str = lhs; str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(const microc::basic_string<CharT,Traits,Alloc>& lhs, CharT rhs) {
        auto str = lhs; str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(const CharT* lhs,
              const microc::basic_string<CharT,Traits,Alloc>& rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(rhs.get_allocator()); str=lhs; str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(CharT lhs, const microc::basic_string<CharT,Traits,Alloc>& rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(rhs.get_allocator()); str=lhs; str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(microc::basic_string<CharT,Traits,Alloc>&& lhs,
              microc::basic_string<CharT,Traits,Alloc>&& rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(microc::traits::move(lhs)); str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(microc::basic_string<CharT,Traits,Alloc>&& lhs,
              const microc::basic_string<CharT,Traits,Alloc>& rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(microc::traits::move(lhs)); str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(microc::basic_string<CharT,Traits,Alloc>&& lhs,
              const CharT* rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(microc::traits::move(lhs)); str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(microc::basic_string<CharT,Traits,Alloc>&& lhs,
              CharT rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(microc::traits::move(lhs)); str+=rhs; return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(const microc::basic_string<CharT,Traits,Alloc>& lhs,
              microc::basic_string<CharT,Traits,Alloc>&& rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(microc::traits::move(rhs)); str.insert(0, lhs); return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(const CharT* lhs,
              microc::basic_string<CharT,Traits,Alloc>&& rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(microc::traits::move(rhs)); str.insert(0, lhs); return str;
    }

    template<class CharT, class Traits, class Alloc> microc::basic_string<CharT,Traits,Alloc>
    operator+(CharT lhs,
              microc::basic_string<CharT,Traits,Alloc>&& rhs) {
        using str_t = basic_string<CharT,Traits,Alloc>;
        str_t str(microc::traits::move(rhs)); str.insert(str.begin(), lhs); return str;
    }

    template<class Integer=long>
    Integer __cstr_to_integral(const char *str, char **str_end=nullptr, unsigned char base=10) {
        using size_type = microc::size_t;
        Integer sign = 1;
        // find start, skip white spaces
        for(; *str==' '; ++str) {}
        // resolve sign if any, do it with fail safe for unsigned types
        if(*str=='-') { if(Integer(-1)<sign) sign=-1; str++; }
        else if(*str=='+') { str++; }
        // resolve base if any
        if((base==16 or base==0) and
                (*str=='0' and (*(str+1)=='x' or *(str+1)=='X'))) { base=16; str+=2; }
        else if((base==2 or base==0) and
                (*str=='0' and (*(str+1)=='b' or *(str+1)=='B'))) { base=2; str+=2; }
        else if((base==8 or base==0) and *str=='0') { base=8; str++; }
        else if(base==0) base=10;
        // find end
        const Integer nopos = base;
        const auto c2n = [nopos](char ch) -> Integer {
            // returns [0, base) for valid, or base for invalid
            Integer r = ch;
            if(ch>='0' and ch<='9') return r - Integer('0');
            else if(ch>='a' and ch<='z') return r - Integer('a') + 10;
            else if(ch>='A' and ch<='Z') return r - Integer('A') + 10;
            return nopos;
        };
        const char * e = str;
        for(; c2n(*e) < Integer(base); ++e) {}
        *str_end=const_cast<char *>(e);
        if(str==e) return 0;
        // find length
        const size_type len = e-str;
        // start counting
        Integer result = 0;
        Integer mul = 1;
        for(e-=1; e!=(str-1); --e, mul*=Integer(base)) {
            result += mul*c2n(*e);
        }
        return result*sign;
    }

    long strtol(const char *str, char **str_end=nullptr, unsigned char base=10)
    { return __cstr_to_integral<long>(str, str_end, base); }
    long long strtoll(const char *str, char **str_end=nullptr, unsigned char base=10)
    { return __cstr_to_integral<long long>(str, str_end, base); }
    unsigned long strtoul(const char *str, char **str_end=nullptr, unsigned char base=10)
    { return __cstr_to_integral<unsigned long>(str, str_end, base); }
    unsigned long long strtoull(const char *str, char **str_end=nullptr, unsigned char base=10)
    { return __cstr_to_integral<unsigned long long>(str, str_end, base); }

    template<class Integer>
    Integer __str_to_integer(const microc::string& str, microc::size_t* pos = nullptr, int base = 10) {
        char * str_end;
        auto res = __cstr_to_integral<Integer>(str.c_str(), &str_end, base);
        if(pos) *pos = str_end - str.c_str();
        return Integer(res);
    }
    int stoi(const microc::string& str, microc::size_t* pos = nullptr, int base = 10)
    { return __str_to_integer<int>(str, pos, base); }
    long stol(const microc::string& str, microc::size_t* pos = nullptr, int base = 10)
    { return __str_to_integer<long>(str, pos, base); }
    long long stoll(const microc::string& str, microc::size_t* pos = nullptr, int base = 10)
    { return __str_to_integer<long long>(str, pos, base); }
    unsigned long stoul(const microc::string& str, microc::size_t* pos = nullptr, int base = 10)
    { return __str_to_integer<unsigned long>(str, pos, base); }
    unsigned long long stoull(const microc::string& str, microc::size_t* pos = nullptr, int base = 10)
    { return __str_to_integer<unsigned long long>(str, pos, base); }

    template<class char_type, class size_type, size_type p=31>
    size_type __simple_hash_cstr(const char_type * s, size_type count) {
        size_type m = 1;
        size_type hash_code = 0;
        for (const char_type * e = s+count; s<e; ++s) {
            hash_code += size_type(*s) * m; m*=p;
        }
        return hash_code;
    }

    template<> struct hash<string> {
        string::size_type operator()(const string & s) const noexcept
        { return __simple_hash_cstr<string::value_type, string::size_type>(s.c_str(), s.size()); }
    };
    template<> struct hash<u16string> {
        u16string::size_type operator()(const u16string & s) const noexcept
        { return __simple_hash_cstr<u16string::value_type, u16string::size_type>(s.c_str(), s.size()); }
    };
    template<> struct hash<u8string> {
        u8string::size_type operator()(const u8string & s) const noexcept
        { return __simple_hash_cstr<u8string::value_type, u8string::size_type>(s.c_str(), s.size()); }
    };
    template<> struct hash<u32string> {
        u32string::size_type operator()(const u32string & s) const noexcept
        { return __simple_hash_cstr<u32string::value_type, u32string::size_type>(s.c_str(), s.size()); }
    };

    microc::string to_string(float fVal) {
        // a very naive float to string
        const int BUFF_SIZE=40;
        char storage[BUFF_SIZE] = {0};
        int dVal, dec;
        char * it = storage-1;
        const float sign = fVal < 0 ? -1.0f : 1.0f;
        dVal = fVal = fVal < 0 ? -fVal : fVal;

        int max_digits_dec = 7;
        {
            int mod = 1;
            for (int ix = 0; ix < max_digits_dec; ++ix, mod*=10) {}
            dec = (int)(fVal * float(mod)) % mod;
            bool encountered_non_zero=false;
            for (int ix = 0; ix < max_digits_dec; ++ix, dec/=10, --it) {
                *it = (dec % 10) + '0';
                // replace trailing '0' with null-terminating char 0
                if(ix+1!=max_digits_dec and *it=='0' and
                        !encountered_non_zero) *it=0;
                else encountered_non_zero=true;
            }
            *(it--) = '.';
        }
        if(dVal==0) *(it--) = '0';
        else for(; dVal!=0;  dVal/=10, --it) { *it = (dVal%10) + '0'; }
        if(sign<0) *(it--) = '-';
        const char * r = it+1;
        return microc::string(r, storage-r);
    }

    template<class int_t>
    microc::string to_string(int_t val) {
        constexpr char SZ = sizeof (val);
        constexpr int BUFF_SIZE=SZ==1 ? 4 : (SZ==2 ? 6 : (SZ==4 ? 11 : 22));
        char storage[BUFF_SIZE] = {0};
        char * it = storage-1;
        const int_t sign = val < 0 ? -1 : 1;
        int_t dVal = val < 0 ? -val : val;
        if(dVal==0) *it-- = '0';
        else {
            while (dVal) {
                *it-- = (dVal % 10) + '0';
                dVal /= 10;
            }
        }
        if(val<0) *it-- = '-';
        const char * r = it+1;
        return microc::string(r, storage-r);
    }
}