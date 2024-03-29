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

    template<class T> struct greater {
        bool operator()(const T& a, const T& b) const{ return a>b; }
    };
    template<class T> struct less {
        bool operator()(const T& a, const T& b) const{ return a<b; }
    };
    template<class T> struct equal_to {
        bool operator()(const T& a, const T& b) const{ return a==b; }
    };

    // Heap methods
    inline microc::size_t __heap_parent_of(microc::size_t i) { return (i-1)>>1; }
    inline microc::size_t __heap_left_child_of(microc::size_t i) { return (i<<1)|1; }
    inline microc::size_t __heap_right_child_of(microc::size_t i) { return (i<<1)+2; }
    inline microc::size_t __heap_before_leafs(unsigned i) { return (i-1)>>1; }

    template<class RandomIt, class Compare>
    inline void __heapify(RandomIt first, RandomIt last, microc::size_t i, Compare c) {
        if(first==last) return;
        // bubble down
        using size_t =  microc::size_t;
        const auto heap_size = last-first;
        while (i<heap_size) {
            size_t largest; // temp next index
            size_t le = __heap_left_child_of(i);
            size_t ri = __heap_right_child_of(i);
//            if ((le<=heap_size) and (A[le]>A[i]))
            if ((le<heap_size) && (c(first[i], first[le])))
                largest = le;
            else
                largest = i;
//            if ((ri<=heap_size) and (A[ri]>A[largest]))
            if ((ri<heap_size) && (c(first[largest], first[ri])))
                largest = ri;
            if (largest != i)
            {
                // todo: use move swaps instead
                const auto temp = first[largest];
                first[largest] = first[i];
                first[i] = temp;
                i = largest;
            }
            else break;
        }
    }

    template<class RandomIt, class Compare>
    bool is_heap(RandomIt first, RandomIt last, Compare c) {
        auto size = last-first;
        if(size<=1) return true;
        auto before_leafs = __heap_before_leafs(size);
        for (microc::size_t ix = 0; ix <= before_leafs; ++ix) {
            // p>=l , p>=r
            const bool has_left_child = __heap_left_child_of(ix)<size;
            const bool has_right_child = __heap_right_child_of(ix)<size;
            if(has_left_child && c(first[ix], first[__heap_left_child_of(ix)]))
                return false;
            if(has_right_child && c(first[ix], first[__heap_right_child_of(ix)]))
                return false;
        }
        return true;
    }

    template<class RandomIt>
    bool is_heap(RandomIt first, RandomIt last) {
        using Compare=microc::less<traits::remove_const_and_reference_t<decltype(*first)>>;
        return microc::is_heap<RandomIt, Compare>(first, last, Compare());
    }

    template<class RandomIt, class Compare>
    void push_heap(RandomIt first, RandomIt last, Compare c) {
        if(first==last) return;
        auto size = last-first;
        auto i = size-1;
        // bubble up the item
        while ((i>0) && c(first[__heap_parent_of(i)], first[i])) {
            // todo: use move swap instead
            const auto temp = first[i];
            first[i] = first[__heap_parent_of(i)];
            first[__heap_parent_of(i)] = temp;
            i=__heap_parent_of(i);
        }
    }

    template<class RandomIt>
    void push_heap(RandomIt first, RandomIt last) {
        // item was inserted at position last-1 into the heap, we need to correct heap. We assume
        // [first, last-1) is a heap and we want to make [first, last) a heap.
        using Compare=microc::less<microc::traits::remove_const_and_reference_t<decltype(*first)>>;
        microc::push_heap<RandomIt, Compare>(first, last, Compare());
    }

    template<class RandomIt, class Compare>
    void pop_heap(RandomIt first, RandomIt last, Compare comp) {
        // pop the root element into the last position and correct sub heap.
        // Assumes [first, last) is already a heap, then make sub-range [first, last-1) into a heap.
        // Swaps the value in the position first and the value in the position last-1 and make it a heap.
        if(first==last) return;
        auto size = last-first;
        // todo: move swap
        const auto temp = first[0];
        first[0] = first[size-1];
        first[size-1] = temp;
        __heapify<RandomIt, Compare>(first, last-1, 0, comp);
    }

    template<class RandomIt>
    void pop_heap(RandomIt first, RandomIt last) {
        using Compare=microc::less<microc::traits::remove_const_and_reference_t<decltype(*first)>>;
        microc::pop_heap<RandomIt, Compare>(first, last, Compare());
    }

    template<class RandomIt, class Compare>
    void make_heap(RandomIt first, RandomIt last, Compare comp) {
        if(first==last) return;
        auto size = last-first;
        auto before_leafs = __heap_before_leafs(size);
        // unsigned count down to 0 included (with a trick)
        for (microc::size_t ix = before_leafs; ix<=before_leafs; --ix) {
            __heapify<RandomIt, Compare>(first, last, ix, comp);
        }
    }

    template<class RandomIt>
    void make_heap(RandomIt first, RandomIt last) {
        using Compare=microc::less<traits::remove_const_and_reference_t<decltype(*first)>>;
        microc::make_heap<RandomIt, Compare>(first, last, Compare());
    }

    template<class RandomIt, class Compare>
    void sort_heap(RandomIt first, RandomIt last, Compare comp) {
        if(first==last) return;
        auto size = last-first;
        // unsigned count down to 1 included
        for (microc::size_t ix = size-1; ix!=0 ; --ix) {
            // todo: move swap
            const auto temp = first[ix];
            first[ix] = first[0];
            first[0] = temp;
            __heapify<RandomIt, Compare>(first, --last, 0, comp);
        }
    }

    template<class RandomIt>
    void sort_heap(RandomIt first, RandomIt last) {
        using Compare=microc::less<microc::traits::remove_const_and_reference_t<decltype(*first)>>;
        microc::sort_heap<RandomIt, Compare>(first, last, Compare());
    }

    // Find iterator
    template<class InputIt, class T>
    InputIt find(InputIt first, InputIt last, const T& value) {
        for (; first != last; ++first)
            if (*first == value) { return first; }
        return last;
    }

    template<class InputIt, class UnaryPredicate>
    InputIt find_if(InputIt first, InputIt last, UnaryPredicate p) {
        for (; first != last; ++first)
            if (p(*first)) { return first; }
        return last;
    }

    template<class InputIt, class UnaryPredicate>
    InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate q) {
        for (; first != last; ++first)
            if (!q(*first)) { return first; }
        return last;
    }

    // iterators stuff
    template <class _Tp, size_t _Np>
    _Tp* begin(_Tp (&__array)[_Np]) { return __array; }
    template <class _Tp, size_t _Np>
    _Tp* end(_Tp (&__array)[_Np]) { return __array + _Np; }

    template <class _Cp>
    auto begin(_Cp& __c) -> decltype(__c.begin()) { return __c.begin(); }

    template <class _Cp>
    auto begin(const _Cp& __c) -> decltype(__c.begin()) { return __c.begin(); }

    template <class _Cp>
    auto end(_Cp& __c) -> decltype(__c.end()) { return __c.end(); }

    template <class _Cp>
    auto end(const _Cp& __c) -> decltype(__c.end()) { return __c.end(); }

    template<class InputIt, class UnaryFunction>
    UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction f) {
        for (; first != last; ++first) { f(*first); }
        return f; // implicit move since C++11
    }

    template<class InputIt, class Size, class UnaryFunction>
    InputIt for_each_n(InputIt first, Size n, UnaryFunction f) {
        for (Size i = 0; i < n; ++first, (void) ++i) { f(*first); }
        return first;
    }

    template<class InputIt, class OutputIt, class UnaryOperation>
    OutputIt transform(InputIt first1, InputIt last1, OutputIt d_first, UnaryOperation unary_op) {
        while (first1 != last1) { *d_first++ = unary_op(*first1++); }
        return d_first;
    }

    template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
    OutputIt transform(InputIt1 first1, InputIt1 last1, InputIt2 first2,
                       OutputIt d_first, BinaryOperation binary_op ) {
        while (first1 != last1) { *d_first++ = binary_op(*first1++, *first2++); }
        return d_first;
    }
}