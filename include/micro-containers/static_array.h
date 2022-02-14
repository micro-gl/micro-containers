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

namespace microc {
    #ifdef MICRO_CONTAINERS_ENABLE_THROW
    struct throw_static_array_out_of_range {};
    #endif

    namespace static_array_traits {

        template< class T > struct remove_reference      {typedef T type;};
        template< class T > struct remove_reference<T&>  {typedef T type;};
        template< class T > struct remove_reference<T&&> {typedef T type;};

        template <class _Tp> inline
        typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept {
            typedef typename remove_reference<_Tp>::type _Up;
            return static_cast<_Up&&>(__t);
        }
        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type& __t) noexcept {
            return static_cast<_Tp&&>(__t);
        }
        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type&& __t) noexcept {
            return static_cast<_Tp&&>(__t);
        }

        template<typename T=unsigned char>
        class void_allocator {
        public:
            using value_type = T;
            using size_t = microc::size_t;
            template<class U>
            explicit void_allocator(const void_allocator<U> & other) noexcept { };
            explicit void_allocator()=default;
            template <class U, class... Args>
            void construct(U* p, Args&&... args) {}
            T * allocate(size_t n) { return nullptr; }
            void deallocate(T * p, size_t n=0) { }
            template<class U> struct rebind { typedef void_allocator<U> other; };
        };

        template<class T1, class T2>
        bool operator==(const void_allocator<T1>& lhs, const void_allocator<T2>& rhs ) noexcept {
            return true;
        }
    }

    /**
     * Static array, That behaves like a static vector, this is cool for debugging. This is
     * NOT equivalent to std::array, but with some mods:
     * 1. Contains FAKE allocator interface, so it can be compatible with many allocator-aware algorithms.
     * 2. you can push to back
     * 3. all elements are destructed at construction
     *
     * @tparam T item type
     * @tparam N the fixed capacity
     * @tparam fake_allocator A fake allocator
     */
    template<class T, unsigned N, class fake_allocator=static_array_traits::void_allocator<>>
    class static_array {
    public:
        using value_type = T;
        using allocator_type = fake_allocator;
        using size_type = microc::size_t;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;

    private:
        T _data[N];
        size_type _current;

    public:
        static_array() : _current(0), _data() {
            // destruct all the elements
            for (size_type ix = 0; ix < N; ++ix)
                (_data + ix)->~T();
        }

        template<class Iterable>
        static_array(const Iterable & list) : static_array() {
            for (const auto & item : list) push_back(item);
        }
        static_array(const allocator_type & allocator) : static_array()  {}
        static_array(const size_type count, const T & value = T(),
                     const allocator_type & alloc = allocator_type()) : static_array()  {
            for (size_type ix = 0; ix < count; ++ix) push_back(value);
        }
        static_array(const static_array & other) : static_array()  {
            for(auto ix = 0; ix < other.size(); ix++)
                push_back(other[ix]);
        }
        static_array(static_array && other) noexcept : static_array() {
            for(auto ix = 0; ix < other.size(); ix++)
                push_back(static_array_traits::move(other[ix]));
        }

        static_array & operator=(const static_array & other) {
            if(this==&other) return (*this);
            clear();
            for(size_type ix = 0; ix < other.size(); ix++)
                push_back(other[ix]);
            return (*this);
        }
        static_array & operator=(static_array && other) noexcept {
            if(this==&other) return (*this);
            clear();
            for(size_type ix = 0; ix < other.size(); ix++)
                push_back(static_array_traits::move(other[ix]));
            other.clear();
            return (*this);
        }

        T& operator[](const size_type i) { return _data[i]; }
        const T& operator[](const size_type i) const { return _data[i]; }
        T* data() { return _data; }
        const T* data() const { return _data; }

        void push_back(const T & v) {
            if(_current==N) return;
            ::new(_data + _current++, microc_new::blah) T(v);
        }
        void push_back(T && v) {
            if(_current==N) return;
            ::new(_data + _current++, microc_new::blah) T(static_array_traits::move(v));
        }

        template <class... Args>
        void emplace_back(Args&&... args) {
            if(_current==N) return;
            ::new(_data + _current++, microc_new::blah) T(static_array_traits::forward<Args>(args)...);
        }
        void pop_back() {
            if(_current==0) return;
            _data[_current--].~T();
        }

        T& back() noexcept { return _data[_current-1]; }
        T& front() noexcept { return _data[0]; }

        void clear() {
            for (size_type ix = 0; ix < capacity(); ++ix)
                (_data + ix)->~T();
            _current = 0;
        }
        size_type size() const { return _current; }
        constexpr size_type capacity() const { return N; }
        allocator_type get_allocator() const { return allocator_type(); }

        T* begin() { return _data; }
        T* end() { return _data + _current; }
        const T* begin() const { return _data; }
        const T* end() const { return _data + _current; }
    };

    template<class T, unsigned N, class fake_allocator>
    bool operator==(const static_array<T, N, fake_allocator>& lhs,
                    const static_array<T, N, fake_allocator>& rhs ) {
        if(!(lhs.size()==rhs.size())) return false;
        using size_type = typename static_array<T, N, fake_allocator>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix)
            if(!(lhs[ix]==rhs[ix])) return false;
        return true;
    }

}