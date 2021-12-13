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
    namespace buffer_traits {
        template< class T > struct remove_reference      {typedef T type;};
        template< class T > struct remove_reference<T&>  {typedef T type;};
        template< class T > struct remove_reference<T&&> {typedef T type;};
        template <class _Tp> inline typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept {
            typedef typename remove_reference<_Tp>::type _Up;
            return static_cast<_Up&&>(__t);
        }
        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type& __t) noexcept
        { return static_cast<_Tp&&>(__t); }

        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type&& __t) noexcept
        { return static_cast<_Tp&&>(__t); }
    }

    /**
     * A minimal buffer object that can:
     * - receive/transfer ownership over data
     * - be given memory with pointer and size
     * - be given an allocator to allocate it's own memory
     * @tparam T
     * @tparam Alloc
     */
    template<typename T, class Alloc>
    class buffer {
    public:
        using value_type = T;
        using size_type = microc::size_t;
        using allocator_type = Alloc;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using iterator = pointer;
        using const_iterator = const_pointer;

    private:
        using rebind_alloc = typename Alloc::template rebind<T>::other;

        rebind_alloc _allocator;
        T *_data;
        bool owner;
        size_type _size;

        static T * allocate_and_default_construct(const size_type size, rebind_alloc & allocator) {
            auto * mem = allocate(size, allocator);
            for (size_type ix = 0; ix < size; ++ix)
                ::new (mem+ix, microc_new::blah) T();
            return (T *)mem;
        }

        static T * allocate(const size_type size, rebind_alloc & allocator) {
            auto * mem = allocator.allocate(size);
            return (T *)mem;
        }

        void copy_from(const buffer & val) {
            destroyIfPossibleAndReset();
            _data = allocate(val.size(), _allocator); //new T[val.size()];
            _size = val.size(); owner = true;
            for (size_type ix = 0; ix < _size; ++ix) // copy-construct
                ::new (_data+ix, microc_new::blah) T(val._data[ix]);
        }

        void move_from(buffer & val) {
            const bool is_same_allocator = _allocator==val._allocator;
            if(is_same_allocator) {
                // same allocator then do classic fast move
                destroyIfPossibleAndReset();
                _data = val.data();
                _size = val.size();
                owner = val.owner;
                val.owner = false;
            } else {
                // NOT same allocator then move item by item.
                destroyIfPossibleAndReset();
                _data = allocate(val.size(), _allocator);
                _size = val.size(); owner = true;
                // move-construct item by item
                for (size_type ix = 0; ix < _size; ++ix)
                    ::new (_data+ix, microc_new::blah) T(buffer_traits::move(val._data[ix]));
            }
            val.destroyIfPossibleAndReset();
        }

    public:
        explicit buffer(size_type size, const Alloc & allocator) :
                _data(nullptr), _size(size), owner(true), _allocator(allocator) {
            _data = allocate_and_default_construct(size, _allocator);
        }
        buffer(T* $data, size_type size, const Alloc & allocator=Alloc()) :
                        _data($data), _size(size), owner(false), _allocator(allocator) {}
        buffer(const buffer & val) : _allocator(val._allocator), owner(true), _data(nullptr),
                        _size(0) { copy_from(val); }
        buffer(buffer && val) noexcept : _allocator(val._allocator), owner(false),
                                _data(nullptr), _size(0) { move_from(val); }
        ~buffer() { destroyIfPossibleAndReset(); }

        buffer & operator=(const buffer & val) {
            if(&val==this) return *this;
            copy_from(val);
            return *this;
        }
        buffer & operator=(buffer && val) noexcept { move_from(val); return *this; }

        void destroyIfPossibleAndReset() {
            if(owner) {
                for (size_type ix = 0; ix < size(); ++ix)
                    _data[ix].~T();
                _allocator.deallocate(_data, _size);
            }
            _data=nullptr; _size=0; owner=false;
        };
        size_type size() const { return _size; }
        reference readAt(size_type index) { return _data[index]; }
        const_reference readAt(size_type index) const { return _data[index]; }
        void writeAt(const T &value, size_type index) { _data[index] = value; }
        const_reference operator[](size_type index) const { return _data[index]; }
        reference operator[](size_type index) { return _data[index]; }
        pointer data() { return _data; }
        const_pointer data() const { return _data; }
        void fill(const T &value) {
            for (size_type ix = 0; ix<_size; ++ix) _data[ix] = value;
        }
        allocator_type get_allocator() { return _allocator; }

        iterator begin() { return _data; }
        const_iterator begin() const { return _data; }
        iterator end() { return _data + _size; }
        const_iterator end() const { return _data + _size; }
    };

    template<class T, class Allocator>
    bool operator==(const buffer<T, Allocator>& lhs,
                    const buffer<T, Allocator>& rhs ) {
        if(!(lhs.size()==rhs.size())) return false;
        using size_type = typename buffer<T, Allocator>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix)
            if(!(lhs[ix]==rhs[ix])) return false;
        return true;
    }

}