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

#include "algorithm.h"

namespace microc {

    /**
     * A priority queue is a container adaptor that provides constant time lookup of the
     * largest (by default) element, at the expense of logarithmic insertion and extraction.
     * @tparam T item type
     * @tparam Container The type of the underlying container to use to store the elements.
     * The container must satisfy the requirements of SequenceContainer. Additionally, it must
     * provide the following functions with the usual semantics:
     * - back(), push_back(), pop_back()
     * The containers dynamic_array, static_array, vector and deque satisfy these requirements.
     * @tparam Compare A strict compare function interface
     */
    template<class T,
            class Container,
            class Compare = microc::less<typename Container::value_type>>
    class priority_queue {
    public:
        using value_type = typename Container::value_type;
        using value_compare = Compare;
        using container_type = Container;
        using reference = value_type &;
        using const_reference = const value_type &;
        using size_type = typename Container::size_type;

    protected:
        container_type c;
        value_compare comp;

    public:
        // ctors
        priority_queue() : priority_queue(Compare(), Container()) {}
        explicit priority_queue(const Compare& compare) : priority_queue(compare, Container()) {};
        priority_queue(const Compare& compare, const Container& cont) : comp(compare), c(cont) {
            microc::make_heap(c.begin(), c.end(), comp);
        };
        priority_queue(const Compare& compare, Container&& cont) :
                comp(compare), c(microc::traits::move(cont)) {
            microc::make_heap(c.begin(), c.end(), comp);
        }
        priority_queue(const priority_queue & other) : priority_queue(other.comp, other.c) {};
        priority_queue(priority_queue && other) noexcept :
                priority_queue(other.comp, microc::traits::move(other.c)) {}; //6
        template<class InputIt>
        priority_queue(InputIt first, InputIt last, const Compare& compare = Compare()) :
                            priority_queue(compare) {
            InputIt current = first;
            while(current != last) { push(*current); ++current; }
        }
        template<class InputIt>
        priority_queue(InputIt first, InputIt last, const Compare& compare, const Container& cont) :
                priority_queue(compare, cont) {
            InputIt current = first;
            while(current != last) { push(*current); ++current; }
        }
        template<class InputIt>
        priority_queue(InputIt first, InputIt last, const Compare& compare, const Container&& cont) :
                priority_queue(compare, microc::traits::move(cont)) {
            InputIt current = first;
            while(current != last) { push(*current); ++current; }
        } // 9
        template<class Alloc>
        explicit priority_queue(const Alloc& alloc) : priority_queue(Compare(), Container(alloc)) {}
        template<class Alloc>
        priority_queue(const Compare& compare, const Alloc& alloc) : priority_queue(compare, Container(alloc)) {}
        template<class Alloc>
        priority_queue(const Compare& compare, const Container& cont, const Alloc& alloc) :
                comp(compare), c(cont, alloc) {
            microc::make_heap(c.begin(), c.end(), comp);
        } // 12
        template<class Alloc>
        priority_queue(const Compare& compare, Container&& cont, const Alloc& alloc) :
                comp(compare), c(microc::traits::move(cont), alloc) {
            microc::make_heap(c.begin(), c.end(), comp);
        }
        template<class Alloc>
        priority_queue(const priority_queue& other, const Alloc& alloc) : c(other.c, alloc), comp(Compare()) {} // 14
        template<class Alloc>
        priority_queue(priority_queue&& other, const Alloc& alloc) :
                    c(microc::traits::move(other.c), alloc), comp(Compare()) {} // 15

        template<class InputIt, class Alloc>
        priority_queue(InputIt first, InputIt last, const Alloc& alloc) : c(alloc), comp(Compare()) {
            InputIt current = first;
            while(current != last) { push(*current); ++current; }
        };
        template<class InputIt, class Alloc>
        priority_queue(InputIt first, InputIt last, const Compare& compare, const Alloc& alloc) :
                c(alloc), comp(compare) {
            InputIt current = first;
            while(current != last) { push(*current); ++current; }
        };
        template<class InputIt, class Alloc>
        priority_queue(InputIt first, InputIt last, const Compare& compare, const Container& cont,
                       const Alloc& alloc) : c(cont, alloc), comp(compare) {
            microc::make_heap(c.begin(), c.end(), comp);
            InputIt current = first;
            while(current != last) { push(*current); ++current; }
        };
        template<class InputIt, class Alloc>
        priority_queue(InputIt first, InputIt last, const Compare& compare, Container&& cont,
                       const Alloc& alloc) : c(microc::traits::move(cont), alloc), comp(compare) {
            microc::make_heap(c.begin(), c.end(), comp);
            InputIt current = first;
            while(current != last) { push(*current); ++current; }
        };

        // assignment
        priority_queue& operator=(const priority_queue& other) { c=other.c; comp = other.comp; return *this; }
        priority_queue& operator=(priority_queue && other) noexcept {
            c=microc::traits::move(other.c); comp = microc::traits::move(other.comp); return *this; }

        // Element access
        const_reference top() const { return c.front(); }

        // Capacity
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }

    protected:
        void on_push() { microc::push_heap(c.begin(), c.end(), comp); }
        void on_pop() { microc::pop_heap(c.begin(), c.end(), comp); }

    public:
        // Modifiers
        void push(const value_type& value) {
            c.push_back(value); on_push();
        }
        void push(value_type && value) {
            c.push_back(microc::traits::move(value)); on_push();
        }
        template<class... Args>
        void emplace(Args&&... args) { c.emplace_back(std::forward<Args>(args)...); on_push(); }
        void pop() { on_pop(); c.pop_back(); }

        // meta
        container_type & get_container() { return c; }
        value_compare & get_comparator() { return comp; }
    };
}