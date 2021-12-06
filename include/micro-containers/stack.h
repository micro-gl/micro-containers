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
 * Stack is an Adaptor type, that uses other type for container
 * @tparam T item type
 * @tparam Container The type of the underlying container to use to store the elements.
 * The container must satisfy the requirements of SequenceContainer. Additionally, it must
 * provide the following functions with the usual semantics:
 * - back(), push_back(), pop_back()
 * The standard containers vector, deque and linked_list satisfy these requirements. By default,
 * if no container class is specified for a particular stack class instantiation,
 * the standard container std::deque is used.
 */
    template <class T,
              class Container>
    class stack
    {
    public:
        using value_type = typename Container::value_type;
        using container_type = Container;
        using reference = value_type &;
        using const_reference = const value_type &;
        using size_type = MICRO_CONTAINERS_SIZE_TYPE;

    protected:
        container_type c;

    public:
        // ctors
        explicit stack(const Container& cont) : c(cont) {};
        stack() : stack(Container()) {}
        explicit stack(Container&& cont) : c(microc::traits::move(cont)) {}
        stack(const stack & other) : stack(other.c) {};
        stack(stack && other) noexcept : stack(microc::traits::move(other.c)) {};
        template< class InputIt >
        stack(InputIt first, InputIt last) : stack() {
            InputIt current = first;
            while(current != last) {
                push(*current); ++current;
            }
        }
        template<class Alloc>
        explicit stack(const Alloc& alloc) : stack(Container(alloc)) {}
        template<class Alloc>
        stack(const Container& other, const Alloc& alloc) : c(other, alloc) {}
        template<class Alloc>
        stack(Container&& other, const Alloc& alloc) : c(microc::traits::move(other), alloc) {}
        template<class Alloc>
        stack(const stack& other, const Alloc& alloc) : stack(other.c, alloc) {};
        template<class Alloc>
        stack(stack&& other, const Alloc& alloc) : stack(microc::traits::move(other.c), alloc) {};
        template<class InputIt, class Alloc>
        stack(InputIt first, InputIt last, const Alloc& alloc) : c(first, last, alloc) {};

        // assignment
        stack& operator=(const stack& other) { c=other.c; return *this; }
        stack& operator=(stack && other) noexcept { c=microc::traits::move(other.c); return *this; }

        // Element access
        reference top() { return c.back(); }
        const_reference top() const { return c.back(); }

        // Capacity
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }

        // Modifiers
        void push(const value_type& value) { c.push_back(value); }
        void push(value_type && value) { c.push_back(microc::traits::move(value)); }
        template<class... Args>
        void emplace(Args&&... args) { c.emplace_back(std::forward<Args>(args)...); }
        void pop() { c.pop_back(); }

        // meta
        container_type & get_container() { return c; }
    };

}
