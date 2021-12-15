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
#include <string>

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
            char_type* __r = dest;
            if (dest < src) {
                for (; count; --count, ++dest, ++src) assign(*dest, *src);
            } else if (src < dest) {
                dest += count; src += count;
                for (; count; --count) assign(*--dest, *--src);
            }
            return __r;
        }
        static char_type* copy(char_type* dest, const char_type* src, microc::size_t count) {
            for(microc::size_t ix = 0; ix < count; ++ix) { assign(*(dest++), *(src++)); }
        }
        static int compare(const char_type* s1, const char_type* s2, std::size_t count) {
            for(microc::size_t ix = 0; ix < count; ++ix) {
                if(lt(*(s1++), *(s2++))) return -1;
                if(lt(*(s2++), *(s1++))) return 1;
            }
            return 0;
        }
        static microc::size_t length(const char_type* s) {
            microc::size_t count = 0;
            while(*(s++)!=char(0)) { ++count;}
            return count;
        }
        static const char_type* find(const char_type* p, microc::size_t count, const char_type& ch) {
            for(microc::size_t ix = 0; ix < count; ++ix, ++p) {
                if(eq(*p, ch)) return p;
            }
        }
        static char_type to_char_type(int_type c) { return char_type(c); }
        static int_type to_int_type(char_type c) { return int_type(c); }
        static bool eq_int_type(int_type c1, int_type c2) { return c1==c2; }
        static int_type eof() { return E_O_F; }
        static int_type not_eof(int_type e) { return eq_int_type(e,eof()) ? ~eof() : e; }
    };

    template<> struct char_traits<char> :
            public __common_char_traits<char, int, microc::size_t, void, void, -1> {};

    template<> struct char_traits<char16_t> :
            public __common_char_traits<char16_t, unsigned short, microc::size_t, void, void, 0xFFFF> {};

    template<> struct char_traits<char32_t> :
            public __common_char_traits<char32_t, unsigned int, microc::size_t, void, void, 0xFFFFFFFF> {};


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

    private:
        using rebind_allocator_type = typename allocator_type::template rebind<value_type>::other;
        void throw_out_of_bounds_exception_if_can() {
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            throw out_of_bounds_exception();
#endif
        }

        value_type * _data;
        rebind_allocator_type _alloc;
        size_type _cap;
        size_type _current;

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
        basic_string(const basic_string& other, size_type pos, size_type count,
                     const Allocator& alloc = Allocator()) : basic_string(alloc) {
            count = pos+count > other.size() ? other.size()-pos : count;
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

        // Assignment operator
        basic_string & operator=(const basic_string & other) noexcept {
            if(this!= &other) {
                clear();
                reserve(other.size());
                for(size_type ix = 0; ix < other.size(); ix++)
                    push_back(other[ix]);
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
        }
        basic_string& operator=(CharT ch) { clear(); reserve(2); push_back(ch); }
        template<class Iterable>
        basic_string& operator=(const Iterable & other) {
            clear(); reserve(other.size());
            for (const auto & item : other) push_back(item);
        }

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
        CharT* data() noexcept { return _data; }
        const CharT* data() const noexcept { return _data; }
        const CharT* c_str() const noexcept { return _data; }
        reference back() { return _data[_current-1]; }
        reference front() { return _data[0]; }
        const_reference back() const { return _data[_current-1]; }
        const_reference front() const { return _data[0]; }

        // Capacity
        bool empty() noexcept { return _current==0; }
        size_type size() const noexcept { return _current; }
        size_type capacity() const noexcept { return _cap; }
        void shrink_to_fit() { re_alloc_to(size()); }
        void reserve(size_type new_cap) {
            if(new_cap <= capacity()) return;
            CharT * _new_data = _alloc.allocate(new_cap);
            // move-construct old objects
            for (size_type ix = 0; ix < size(); ++ix)
                ::new (_new_data + ix, microc_new::blah) CharT(microc::traits::move(_data[ix]));
            _cap = new_cap;
            // no need to destruct because the items were moved
            if(_data) _alloc.deallocate(_data);
            _data = _new_data;
        }

        // Modifiers
    private:
        void alloc_(bool up) noexcept {
            const auto new_cap = up ? (_cap==0?1:_cap*2) : _cap/2;
            re_alloc_to(new_cap);
        }
        void re_alloc_to(size_type new_capacity) noexcept {
            // re-alloc to another capacity and move old elements
            const auto old_size = _current;
            const auto copy_size = old_size < new_capacity ? old_size : new_capacity;

            CharT * _new_data = _alloc.allocate(new_capacity);
            // move all previous objects into new location,
            // therefore we do not need to destruct because we move from same allocator
            for (size_type ix = 0; ix < copy_size; ++ix)
                ::new (_new_data + ix, microc_new::blah) CharT(microc::traits::move(_data[ix]));

            // de allocate old data
            _alloc.deallocate(_data, capacity());
            _data = _new_data;
            _cap = new_capacity;
        }
        template<class TT>
        void internal_push_back(TT && v) noexcept {
            if(size() >= capacity()) {
                // copy the value, edge case if v belongs
                // to the dynamic array
                CharT vv = microc::traits::forward<TT>(v); // copy-or-move ctor
                alloc_(true);
                ::new(_data + _current++, microc_new::blah) CharT(microc::traits::move<TT>(vv));
            } else ::new(_data + _current++, microc_new::blah) CharT(microc::traits::forward<TT>(v));;
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
        }

    public:
        basic_string& operator+=(const basic_string & str) {
            reserve(size() + str.size());
            for (const auto & item : str) push_back(item);
        }
        basic_string& operator+=(CharT ch) { reserve(size() + 1); push_back(ch);}
        basic_string& operator+=(const CharT* s) {
            auto len = traits_type::length(s);
            reserve(size() + len);
            for (; len ; --len) { push_back(*(s++)); }
        }
        template<class Iterable> basic_string& operator+=(const Iterable & other) {
            reserve(size() + other.size());
            for (const auto & item : other) push_back(item);
        }

        void push_back(const CharT & v) noexcept { internal_push_back(v); }
        void push_back(CharT && v) noexcept { internal_push_back(v); }
        void pop_back() {
            if(_current==0) return;
            _data[_current--].~T();
            if(_current < (_cap>>1)) alloc_(false);
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
            for (size_type ix = 0; ix < size(); ++ix) _data[ix].~T();
            _current = 0;
        }

        void drain() noexcept {
            clear();
            if(_data) _alloc.deallocate(_data, capacity());
            _data = nullptr; _cap = _current = 0;
        }

        // avoid overload resolution if InputIt is integral type, this can conflict with the
        // insert(const_iterator pos, size_type count, const T& value) overload for integral types
        template<class InputIt, typename bb = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        iterator insert(const_iterator pos, InputIt first, InputIt last, bool move=false) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            if(first==last) return const_cast<iterator>(pos);
            // insert range before pos
            const auto dist_to_first = pos-begin();
            const auto needed_extra_size = last-first;
            const auto how_many_to_shift_right = end()-pos;
            const auto how_many_to_construct = end()-pos;
            const bool requires_expend = size()+needed_extra_size > capacity();
            if(requires_expend) {
                re_alloc_to(capacity() + needed_extra_size + 1);
                // latest pos is no longer valid, memory might have changed
                pos = begin()+dist_to_first;
            }
            auto current_end = end();
            auto new_end = current_end + needed_extra_size;
            //
#define minnnn(a,b) ((a)<(b) ? (a) : (b))
            const auto how_many_veterans_require_move_construction =
                    minnnn(how_many_to_shift_right, needed_extra_size);
            const auto how_many_veterans_require_move_assignment =
                    how_many_to_shift_right-how_many_veterans_require_move_construction;
            const auto how_many_range_require_copy_construction =
                    needed_extra_size-how_many_veterans_require_move_construction;
#undef minnnn
            // move-construct at end the last elements, that belong to this
            for (size_type ix = 0; ix<how_many_veterans_require_move_construction; ++ix) { // count from [N..1]
                ::new (--new_end, microc_new::blah) CharT(microc::traits::move(*(--current_end)));
            } // move-assign at end the last elements, that belong to this
            for (size_type ix = 0; ix<how_many_veterans_require_move_assignment; ++ix) { // count from [N..1]
                *(--new_end) = microc::traits::move(*(--current_end));
            }
            // copy-construct some range elements
            for (size_type ix = 0; ix<how_many_range_require_copy_construction; ++ix) { // count from [N..1]
                if(!move) ::new (--new_end, microc_new::blah) CharT(*(--last));
                else ::new (--new_end, microc_new::blah) CharT(microc::traits::move(*(--last)));
            }
            do { // move-or-copy assign remaining items from iterator
                *(--new_end) = !move ? *(--last) : microc::traits::move(*(--last));
            } while(last!=first);
            _current += needed_extra_size;
            return const_cast<iterator>(pos);
        }
        iterator insert(const_iterator pos, const CharT& value) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            if(pos==end()) { push_back(value); return end(); }
            return insert<const_iterator>(pos, &value, (&value)+1);
        }
        iterator insert(const_iterator pos, CharT && value) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            if(pos==end()) { push_back(microc::traits::move(value)); return end(); }
            return insert<const_iterator>(pos, &value, (&value)+1, true);
        }
        iterator insert(const_iterator pos, size_type count, const CharT& value) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            // this is a very lazy batch insert, correct way it to implement a
            // range iterator that returns the same value
            auto last_pos = const_cast<iterator>(pos);
            for (size_type ix = 0; ix < count; ++ix) {
                last_pos = insert(last_pos, value);
            }
            return const_cast<iterator>(last_pos-count);
        }

        iterator erase(const_iterator pos) { return erase(pos, pos+1); }
        iterator erase(const_iterator first, const_iterator last) {
            auto count_to_erase = last-first;
            auto distance_to_first = first - begin();
            auto move_to_iter = const_cast<iterator>(first);
            auto last_copy_1 = const_cast<iterator>(last);
            while (last_copy_1!=end()) {
                *(move_to_iter) = microc::traits::move(*last_copy_1);
                ++last_copy_1; ++move_to_iter;
            }
            while(count_to_erase--!=0) pop_back();
            // pop back might invalidate iterators, so re-invent them
            return begin()+distance_to_first;
        }

        void resize(size_type count) { internal_resize(count, CharT()); }
        void resize(size_type count, const value_type& value) { internal_resize(count, value); }

        basic_string substr(size_type pos = 0, size_type count = npos) const {
            if(pos>size()) throw_out_of_bounds_exception_if_can();
            if(pos+count>size()) count=size()-pos;
            return basic_string(_data, pos, count, _alloc);
        }
        size_type copy(CharT* dest, size_type count, size_type pos = 0) const {
            if(pos>size()) throw_out_of_bounds_exception_if_can();
            if(pos+count>size()) count=size()-pos;
            for(; count; --count) { *(dest++) = *(_data + pos++); }
        }

        // query/search operations
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

        bool contains(const CharT* s) const { return find(s) != npos; }
        bool contains(CharT c) const noexcept { return find(c) != npos; }

        bool starts_with(CharT c) const noexcept { return find(c)==0; }
        bool starts_with(const CharT* s) const { return find(s)==0; }
        bool ends_with(CharT c) const noexcept { return find(c)==size()-1; }
        bool ends_with(const CharT* s) const {
            const auto str_len = traits_type::length(s);
            return find(s, 0, str_len)==size()-str_len;
        }


        // Iterators
        const_iterator begin() const noexcept {return _data;}
        const_iterator end() const noexcept {return _data + size();}
        iterator begin() noexcept {return _data;}
        iterator end()  noexcept {return _data + size();}

        // etc..
        Allocator get_allocator() const noexcept { return Allocator(_alloc); }
    };

    template<class CharT, class Traits, class Allocator>
    bool operator==(const basic_string<CharT, Traits, Allocator>& lhs,
                    const basic_string<CharT, Traits, Allocator>& rhs ) {
        if(!(lhs.size()==rhs.size())) return false;
        using size_type = typename basic_string<CharT, Traits, Allocator>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix)
            if(!(lhs[ix]==rhs[ix])) return false;
        return true;
    }
}