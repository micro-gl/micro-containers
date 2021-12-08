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
    struct throw_array_out_of_range {};
#endif

/**
 * This container is an aggregate type with the same semantics as a struct holding a
 * C-style array T[N] as its only non-static data member. Unlike a C-style array,
 * it doesn't decay to T* automatically. As an aggregate type, it can be initialized
 * with aggregate-initialization given at most N initializers that are convertible to
 * T: std::array<int, 3> a = {1,2,3};.
 * The struct combines the performance and accessibility of a C-style array with the benefits
 * of a standard container, such as knowing its own size, supporting assignment,
 * random access iterators, etc.
 *
 * Note:
 * - This is an aggregate type and is aggregate initialized, this is why everything works,
 *   without constructor or assignment operator.
 * @tparam T item type
 * @tparam N size of array
 */
    template <class T, microc::size_t N>
    struct array
    {
        using value_type = T;
        using reference = value_type &;
        using const_reference = const value_type &;
        using iterator = value_type *;
        using const_iterator = const value_type *;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using size_type = microc::size_t;

        T _data[N];

        // No explicit construct/copy/destroy for aggregate type
        void fill(const value_type& value) {
            for (int ix = 0; ix < N; ++ix)
                *(_data + ix) = value;
        }

        // iterators:
        iterator begin() noexcept {return iterator(data());}
        const_iterator begin() const noexcept {return const_iterator(data());}
        iterator end() noexcept {return iterator(data() + N);}
        const_iterator end() const noexcept {return const_iterator(data() + N);}

        const_iterator cbegin() const noexcept {return begin();}
        const_iterator cend() const noexcept {return end();}

        // capacity:
        constexpr size_type size() const noexcept {return N;}
        size_type max_size() const noexcept {return N;}
        bool empty() const noexcept {return N == 0;}

        // element access:
        reference operator[](size_type index) _NOEXCEPT {
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            if (index>=N) throw throw_array_out_of_range();
#endif
            return _data[index];
        }
        const_reference operator[](size_type index) const _NOEXCEPT {
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            if (index>=N) throw throw_array_out_of_range();
#endif
            return _data[index];
        }

        reference at(size_type index) {
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            if (index>=N) throw throw_array_out_of_range();
#endif
            return _data[index];
        }

        const_reference at(size_type index) const {
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            if (index>=N) throw throw_array_out_of_range();
#endif
            return _data[index];
        }

        reference front()             noexcept {return (*this)[0];}
        const_reference front() const noexcept {return (*this)[0];}
        reference back()              noexcept {return (*this)[N - 1];}
        const_reference back() const  noexcept {return (*this)[N - 1];}

        value_type* data() noexcept {return _data;}
        const value_type* data() const noexcept {return _data;}
    };

}
