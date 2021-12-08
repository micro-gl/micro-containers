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
    namespace dynamic_array_traits {

        template< class T > struct remove_reference      {typedef T type;};
        template< class T > struct remove_reference<T&>  {typedef T type;};
        template< class T > struct remove_reference<T&&> {typedef T type;};

        template <class _Tp>
        inline typename remove_reference<_Tp>::type&&
        move(_Tp&& __t) noexcept
        {
            typedef typename remove_reference<_Tp>::type _Up;
            return static_cast<_Up&&>(__t);
        }

        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type& __t) noexcept
        {
            return static_cast<_Tp&&>(__t);
        }

        template <class _Tp> inline _Tp&&
        forward(typename remove_reference<_Tp>::type&& __t) noexcept
        {
            return static_cast<_Tp&&>(__t);
        }

        /**
         * standard allocator
         * @tparam T the allocated object type
         */
        template<typename T>
        class std_allocator {
        public:
            using value_type = T;
            using size_t = unsigned long;
        public:
            template<class U>
            explicit std_allocator(const std_allocator<U> & other) noexcept { };
            explicit std_allocator()=default;

            template <class U, class... Args>
            void construct(U* p, Args&&... args) {
                ::new(p, microc_new::blah) U(dynamic_array_traits::forward<Args>(args)...);
            }

            T * allocate(size_t n) { return (T *)operator new(n * sizeof(T)); }
            void deallocate(T * p, size_t n=0) { operator delete (p); }

            template<class U> struct rebind {
                typedef std_allocator<U> other;
            };
        };

        template<class T1, class T2>
        bool operator==(const std_allocator<T1>& lhs, const std_allocator<T2>& rhs ) noexcept {
            return true;
        }
    }

    /**
     * Minimal vector like container, does not obey all of the propagate syntax that
     * Allocator Aware Container follows
     * @tparam T the type
     * @tparam Alloc the allocator type
     */
    template<typename T, class Alloc=dynamic_array_traits::std_allocator<T>>
    class dynamic_array {
        using const_dynamic_array_ref = const dynamic_array<T, Alloc> &;
    public:
        using value_type = T;
        using size_type = MICRO_CONTAINERS_SIZE_TYPE;
        using allocator_type = Alloc;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using iterator = pointer;
        using const_iterator = const_pointer;

    private:
        using rebind_allocator_type = typename Alloc::template rebind<value_type>::other;

        T *_data = nullptr;
        rebind_allocator_type _alloc;
        size_type _current = 0u;
        size_type _cap = 0u;

    public:
        explicit dynamic_array(const Alloc & alloc = Alloc()) noexcept : _alloc{alloc} {}
        dynamic_array() noexcept : dynamic_array(Alloc()) {}

        dynamic_array(const size_type count, const T & value, const Alloc & alloc = Alloc()) :
                dynamic_array(alloc) {
            reserve(count);
            for (size_type ix = 0; ix < count; ++ix) push_back(value);
        }

        explicit dynamic_array(size_type count, const Alloc& alloc = Alloc()) :
                            dynamic_array(count, T(), alloc) {}

        template<class Iterable>
        explicit dynamic_array(const Iterable &list, const Alloc & alloc= Alloc()) :
                dynamic_array(alloc) {
            reserve(list.size());
            for (const auto & item : list) push_back(item);
        }

        template<class InputIt, typename bb = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        dynamic_array(InputIt first, InputIt last, const Alloc & alloc= Alloc()) :
                dynamic_array(alloc) {
            reserve(last-first);
            while(first!=last) { push_back(*first); ++first; }
        }

        dynamic_array(const dynamic_array & other, const Alloc & alloc) noexcept :
                dynamic_array(alloc) {
            reserve(other.size());
            for (const auto & item : other) push_back(item);
        }

        dynamic_array(const dynamic_array & other) noexcept :
                dynamic_array(other, other.get_allocator()) {
        }

        dynamic_array(dynamic_array && other, const Alloc & alloc) noexcept :
                dynamic_array(alloc) {
            reserve(other.size());
            for (auto & item : other) push_back(dynamic_array_traits::move(item));
        }

        dynamic_array(dynamic_array && other) noexcept : dynamic_array{other.get_allocator()} {
            _data = other._data;
            _current = other._current;
            _cap = other._cap;
            other._data=nullptr;
            other._cap=0;
            other._current=0;
        }

        ~dynamic_array() noexcept { drain(); }

        struct out_of_bounds_exception {};
        // Element access
        reference operator[](size_type i) { return _data[i]; }
        const_reference operator[](size_type i) const { return _data[i]; }
        reference at(size_type pos) {
            if(pos>=0 && pos < size()) return _data[pos];
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            throw out_of_bounds_exception();
#endif
        }
        const_reference at(size_type pos) const {
            if(pos>=0 && pos < size()) return _data[pos];
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            throw out_of_bounds_exception();
#endif
        }
        T* data() noexcept { return _data; }
        const T* data() const noexcept { return _data; }
        reference back() { return _data[_current-1]; }
        reference front() { return _data[0]; }
        const_reference back() const { return _data[_current-1]; }
        const_reference front() const { return _data[0]; }

        // Capacity
        bool empty() noexcept { return _current==0; }
        size_type size() const noexcept { return _current; }
        size_type capacity() const noexcept { return _cap; }

        void reserve(size_type new_cap) {
            bool avoid = new_cap <= capacity();
            if(avoid) return;
            T * _new_data = _alloc.allocate(new_cap);
            // move-construct old objects
            for (size_type ix = 0; ix < size(); ++ix)
                ::new (_new_data + ix, microc_new::blah) T(dynamic_array_traits::move(_data[ix]));
            _cap = new_cap;
            // no need to destruct because the items were moved
            if(_data!=nullptr) _alloc.deallocate(_data);
            _data = _new_data;
        }

        // Assignment operator
        dynamic_array & operator=(const dynamic_array & other) noexcept {
            if(this!= &other) {
                clear();
                reserve(other.size());
                for(size_type ix = 0; ix < other.size(); ix++)
                    push_back(other[ix]);
            }
            return (*this);
        }

        dynamic_array & operator=(dynamic_array && other) noexcept {
            // two cases:
            // 1. if the allocators are equal, then move the data completely.
            // 2. otherwise, move element by element
            const bool are_equal = _alloc == other.get_allocator();
            const bool self_assign = this == &other;
            if(self_assign) return *this;

            if(are_equal) {
                // clear and destruct current elements
                clear();
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
                    clear(); // clear and destruct current objects
                    _alloc.deallocate(_data, capacity()); // de allocate the chunk
                    _data = _alloc.allocate(other.size()); // create new chunk
                }
                // move other items into current memory
                for (size_type ix = 0; ix < other.size(); ++ix)
                    ::new (_data + ix, microc_new::blah) T(dynamic_array_traits::move(other[ix]));
                // no need to destruct other's items
                _current = other._current;
                _cap = other._cap;
                // different allocators, we do not de-allocate the other array.
            }

            return (*this);
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

            T * _new_data = _alloc.allocate(new_capacity);
            // move all previous objects into new location,
            // therefore we do not need to destruct because we move from same allocator
            for (size_type ix = 0; ix < copy_size; ++ix)
                ::new (_new_data + ix, microc_new::blah) T(dynamic_array_traits::move(_data[ix]));

            // de allocate old data
            _alloc.deallocate(_data, capacity());
            _data = _new_data;
            _cap = new_capacity;
        }

    public:
        void push_back(const T & v) noexcept {
            if(_current + 1 > _cap) {
                // copy the value, edge case if v belongs
                // to the dynamic array
                const T vv = v;
                alloc_(true);
                ::new(_data + _current++, microc_new::blah) T(vv);
            } else ::new(_data + _current++, microc_new::blah) T(v);;
        }
        void push_back(T && v) noexcept {
            if(_current + 1 > _cap) {
                // copy the value, edge case if v belongs
                // to the dynamic array
                T vv = dynamic_array_traits::move(v);
                alloc_(true);
                ::new(_data + _current++, microc_new::blah) T(dynamic_array_traits::move(vv));
            } else ::new(_data + _current++, microc_new::blah) T(dynamic_array_traits::move(v));
        }
        void pop_back() {
            if(_current==0) return;
            _data[_current--].~T();
            if(_current < (_cap>>1)) alloc_(false);
        }

        // todo: this is wrong
        template<class... Args>
        iterator emplace(const_iterator pos, Args&&... args) {
            return ::new (pos, microc_new::blah) T(dynamic_array_traits::forward<Args>(args)...);
        }
        template<typename... Args>
        reference emplace_back(Args&&... args) {
            if(_current + 1> _cap) alloc_(true);
            ::new (_data + _current++, microc_new::blah) T(dynamic_array_traits::forward<Args>(args)...);
            return back();
        }

        void clear() noexcept {
            for (size_type ix = 0; ix < capacity(); ++ix) _data[ix].~T();
            _current = 0;
        }

        void drain() noexcept {
            clear();
            if(_data!=nullptr)
                _alloc.deallocate(_data, capacity());
            _data = nullptr;
            _cap = 0;
            _current = 0;
        }

#define max___(a,b) ((a)<(b) ? (b) : (a))
#define min___(a,b) ((a)<(b) ? (a) : (b))

        template<class InputIt, typename bb = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        iterator insert(const_iterator pos, InputIt first, InputIt last) {
            if(first==last) return const_cast<iterator>(pos);
            // insert range before pos
            const auto dist_to_first = pos-begin();
            const auto needed_extra_size = last-first;
            const auto how_many_to_shift_right = end()-pos;
            const auto how_many_to_construct = end()-pos;
            const bool requires_expend = size()+needed_extra_size > capacity();
            const bool range_belongs_to_me = first>=begin() && last<=end();
            if(range_belongs_to_me) {return nullptr;}
            if(requires_expend) {
                re_alloc_to(capacity() + needed_extra_size + 1);
                // latest pos is no longer valid, memory might have changed
                pos = begin()+dist_to_first;
            }

            auto current_end = end();
            const auto current_size = size();
            auto new_end = current_end + needed_extra_size;
            //
            const auto how_many_veterans_require_move_construction =
                    min___(how_many_to_shift_right, needed_extra_size);
            const auto how_many_veterans_require_move_assignment =
//                    how_many_veterans_require_move_construction < how_many_to_shift_right ?
                    how_many_to_shift_right-how_many_veterans_require_move_construction;
//                    : 0;
            const auto how_many_range_require_copy_construction =
                    needed_extra_size-how_many_veterans_require_move_construction;

            // move-construct at end the last elements, that belong to this
            for (size_type ix = 1; ix<=how_many_veterans_require_move_construction; ++ix) { // count from [N..1]
                ::new (--new_end, microc_new::blah)
                        T(dynamic_array_traits::move(*(--current_end)));
            }
            for (size_type ix = 1; ix<=how_many_veterans_require_move_assignment; ++ix) { // count from [N..1]
                *(--new_end) = dynamic_array_traits::move(*(--current_end));
            }
            // copy-construct some range elements
            for (size_type ix = 1; ix<=how_many_range_require_copy_construction; ++ix) { // count from [N..1]
                ::new (--new_end, microc_new::blah) T(*(--last));
            }
            do {
                *(--new_end) = *(--last);
            } while(last!=first);

            _current += needed_extra_size;

            return const_cast<iterator>(pos);
        }
        iterator insert(const_iterator pos, const T& value) {
            if(pos==end()) { push_back(value); return end(); }
            return insert<const_iterator>(pos, &value, (&value)+1);
        }
        iterator insert(const_iterator pos, size_type count, const T& value) {
            // this is a very lazy batch insert, correct way it to implement a
            // range iterator that returns the same value
            iterator last_pos = const_cast<iterator>(pos);
            for (int ix = 0; ix < count; ++ix) {
                last_pos = insert(last_pos, value);
            }
            return const_cast<iterator>(last_pos-count);
        }

        // Iterators
        const_iterator begin() const noexcept {return _data;}
        const_iterator end() const noexcept {return _data + size();}
        iterator begin() noexcept {return _data;}
        iterator end()  noexcept {return _data + size();}

        // etc..
        Alloc get_allocator() const noexcept { return Alloc(_alloc); }
    };

    template<class T, class Alloc>
    bool operator==(const dynamic_array<T,Alloc>& lhs,
                    const dynamic_array<T,Alloc>& rhs ) {
        const bool equals_sizes = lhs.size()==rhs.size();
        if(!equals_sizes) return false;
        using size_type = typename dynamic_array<T,Alloc>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix) {
            if(!(lhs[ix]==rhs[ix]))
                return false;
        }
        return true;
    }
}