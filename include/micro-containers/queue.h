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

    /**
     * Queue is an Adaptor type, that uses other type for container
     * @tparam T item type
     * @tparam Container The type of the underlying container to use to store the elements.
     * The container must satisfy the requirements of SequenceContainer. Additionally, it must
     * provide the following functions with the usual semantics:
     * - back(), front(), push_back(), pop_front()
     * The standard containers deque, linked_list and circular_array satisfy these requirements.
     */
    template <class T, class Container>
    class queue {
    public:
        using value_type = typename Container::value_type;
        using container_type = Container;
        using reference = value_type &;
        using const_reference = const value_type &;
        using size_type = typename container_type::size_type;

    protected:
        container_type c;

    public:
        // ctors
        explicit queue(const Container& cont) : c(cont) {};
        queue() : queue(Container()) {}
        explicit queue(Container&& cont) : c(microc::traits::move(cont)) {}
        queue(const queue & other) : queue(other.c) {};
        queue(queue && other) noexcept : queue(microc::traits::move(other.c)) {};
        template< class InputIt >
        queue(InputIt first, InputIt last) : queue() {
            InputIt current = first;
            while(current != last) {
                push(*current); ++current;
            }
        }
        template<class Alloc>
        explicit queue(const Alloc& alloc) : queue(Container(alloc)) {}
        template<class Alloc>
        queue(const Container& other, const Alloc& alloc) : c(other, alloc) {}
        template<class Alloc>
        queue(Container&& other, const Alloc& alloc) : c(microc::traits::move(other), alloc) {}
        template<class Alloc>
        queue(const queue& other, const Alloc& alloc) : queue(other.c, alloc) {};
        template<class Alloc>
        queue(queue&& other, const Alloc& alloc) : queue(microc::traits::move(other.c), alloc) {};
        template<class InputIt, class Alloc>
        queue(InputIt first, InputIt last, const Alloc& alloc) : c(first, last, alloc) {};

        // assignment
        queue& operator=(const queue& other) { c=other.c; return *this; }
        queue& operator=(queue && other) noexcept { c=microc::traits::move(other.c); return *this; }

        // Element access
        reference front() { return c.front(); }
        const_reference front() const { return c.front(); }
        reference back() { return c.back(); }
        const_reference back() const { return c.back(); }

        // Capacity
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }

        // Modifiers
        void push(const value_type& value) { c.push_back(value); }
        void push(value_type && value) { c.push_back(microc::traits::move(value)); }
        template<class... Args>
        void emplace(Args&&... args) { c.emplace_back(std::forward<Args>(args)...); }
        void pop() { c.pop_front(); }

        // meta
        container_type & get_container() { return c; }
    };
}