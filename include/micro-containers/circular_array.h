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
     * Minimal vector like container, does not obey all of the propagate syntax that
     * Allocator Aware Container follows
     * @tparam T the type
     * @tparam Alloc the allocator type
     */
    template<typename T, class Alloc=microc::std_allocator<T>>
    class circular_array {
    public:
        using value_type = T;
        using size_type = microc::size_t;
        using difference_type = microc::ptrdiff_t;
        using allocator_type = Alloc;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        struct out_of_bounds_exception {};

    private:
        using rebind_allocator_type = typename Alloc::template rebind<value_type>::other;

        void throw_out_of_bounds_exception_if_can() {
#ifdef MICRO_CONTAINERS_ENABLE_THROW
            throw out_of_bounds_exception();
#endif
        }

        T *_data;
        rebind_allocator_type _alloc;
        size_type _cap;
        difference_type _front;
        difference_type _back;

    public:
        explicit circular_array(const Alloc & alloc) noexcept :
                _data(nullptr), _front(0), _back(0), _cap(0), _alloc(alloc) {}
        circular_array() noexcept : circular_array(Alloc()) {}
        circular_array(const size_type count, const T & value, const Alloc & alloc = Alloc()) :
                circular_array(alloc) {
            reserve(count);
            for (size_type ix = 0; ix < count; ++ix) push_back(value);
        }
        explicit circular_array(size_type count, const Alloc& alloc = Alloc()) :
                circular_array(count, T(), alloc) {}
        template<class Iterable>
        explicit circular_array(const Iterable &list, const Alloc & alloc= Alloc()) :
                circular_array(alloc) {
            reserve(list.size());
            for (const auto & item : list) push_back(item);
        }
        template<class InputIt, typename bb = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        circular_array(InputIt first, InputIt last, const Alloc & alloc= Alloc()) :
                circular_array(alloc) {
            reserve(last-first);
            while(first!=last) { push_back(*first); ++first; }
        }
        circular_array(const circular_array & other, const Alloc & alloc) noexcept :
                circular_array(alloc) {
            reserve(other.size());
            for (const auto & item : other) push_back(item);
        }
        circular_array(const circular_array & other) noexcept :
                circular_array(other, other.get_allocator()) {
        }
        circular_array(circular_array && other, const Alloc & alloc) noexcept :
                circular_array(alloc) {
            reserve(other.size());
            for (auto & item : other) push_back(microc::traits::move(item));
        }
        circular_array(circular_array && other) noexcept : circular_array{other.get_allocator()} {
            _data = other._data;
            _front = other._front;
            _back = other._back;
            _cap = other._cap;
            other._data=nullptr;
            other._cap=0;
            other._back=other._front=0;
        }
        ~circular_array() noexcept { drain(); }

    private:
        template<class value_reference_type>
        struct iterator_t {
            difference_type i;
            const circular_array * c;

            template<class value_reference_type_t>
            iterator_t(const iterator_t<value_reference_type_t> & other) : i(other.i), c(other.c) {}
            explicit iterator_t(difference_type idx, const circular_array * c) : i(idx), c(c) {}
            iterator_t& operator++() { ++i; return *this;}
            iterator_t& operator--() { --i; return *this;}
            iterator_t operator++(int) {iterator_t retval(i); ++(*this); return retval;}
            iterator_t operator--(int) {iterator_t retval(i); --(*this); return retval;}
            iterator_t operator+(difference_type step) const { return iterator_t(i+step, c); }
            iterator_t operator-(difference_type step) const { return iterator_t(i-step, c); }
            difference_type operator-(iterator_t other) const {
                auto delta = difference_type(i)-difference_type(other.i);
                return c->_back>c->_front ? delta : -delta;
            }
            bool operator>(iterator_t other) const {return ((*this)-other)>0;}
            bool operator>=(iterator_t other) const {return ((*this)-other)>=0;}
            bool operator<(iterator_t other) const {return ((*this)-other)<0;}
            bool operator<=(iterator_t other) const {return ((*this)-other)<=0;}
            bool operator==(iterator_t other) const {return i == other.i;}
            bool operator!=(iterator_t other) const {return !(*this == other);}
            value_reference_type operator*() const {
                auto * data = const_cast<circular_array *>(c)->_data;
                auto where = c->mf(i);
                return data[where];
            }
        };

    public:
        using iterator = iterator_t<reference>;
        using const_iterator = iterator_t<const_reference>;

        // Iterators
        const_iterator begin() const noexcept {return iterator(_front, this);}
        const_iterator end() const noexcept {return iterator(_back, this);}
        iterator begin() noexcept {return iterator(_front, this);}
        iterator end()  noexcept {return iterator(_back, this);}

        // Element access
        size_type front_index() const { return _front; }
        size_type back_index() const { return _back; }
        reference operator[](size_type i) { return *(begin()+i); }
        const_reference operator[](size_type i) const { return *(begin()+i); }
        reference at(size_type pos) {
            if(pos>=0 && pos < size()) return this->operator[](pos);
            throw_out_of_bounds_exception_if_can();
        }
        const_reference at(size_type pos) const {
            if(pos>=0 && pos < size()) return this->operator[](pos);
            throw_out_of_bounds_exception_if_can();
        }
        T* data() noexcept { return _data; }
        const T* data() const noexcept { return _data; }
        reference back() { return *(end()-1); }
        reference front() { return *(begin()); }
        const_reference back() const { return *(end()-1); }
        const_reference front() const { return *(begin()); }

        // Capacity
        bool empty() noexcept { return size()==0; }
        size_type size() const noexcept {
            difference_type diff=(end()-begin())-0;
            return diff;
        }
        size_type capacity() const noexcept { return _cap; }
        void reserve(size_type new_cap) {
            if(new_cap <= capacity()) return;
            T * _new_data = _alloc.allocate(new_cap);
            // move-construct old objects
            for (size_type ix = 0; ix < size(); ++ix)
                ::new (_new_data + ix, microc_new::blah) T(microc::traits::move(this->operator[](ix)));
            _cap = new_cap; _front=0; _back=size()+0;
            // no need to destruct because the items were moved
            if(_data) _alloc.deallocate(_data);
            _data = _new_data;
        }

        // Assignment operator
        circular_array & operator=(const circular_array & other) noexcept {
            if(this!= &other) {
                clear(); reserve(other.size());
                for (const auto & item :other) push_back(item);
            }
            return (*this);
        }

        circular_array & operator=(circular_array && other) noexcept {
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
                _front = other._front;
                _back = other._back;
                _cap = other._cap;
                // no need to de-allocate because we stole the memory
                other._data=nullptr;
                other._front=other._back=0;
                other._cap=0;
            } else {
                if(other.size() >= capacity()) {
                    _alloc.deallocate(_data, capacity()); // de allocate the chunk
                    _data = _alloc.allocate(other.capacity()); // create new chunk
                }
                // move other items into current memory
                for (size_type ix = 0; ix < other.size(); ++ix)
                    ::new (_data + ix, microc_new::blah) T(microc::traits::move(other[ix]));
                // no need to destruct other's items
                _front = 0; _back = size()+0;
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
            const size_type old_size = size();
            const auto copy_size = old_size < new_capacity ? old_size : new_capacity;

            T * _new_data = _alloc.allocate(new_capacity);
            // move all previous objects into new location,
            // therefore we do not need to destruct because we move from same allocator
            for (size_type ix = 0; ix < copy_size; ++ix)
                ::new (_new_data + ix, microc_new::blah) T(microc::traits::move(this->operator[](ix)));

            // de allocate old data
            _alloc.deallocate(_data, capacity());
            _front=0; _back=old_size;
            _data = _new_data;
            _cap = new_capacity;
        }
        size_type mf(const difference_type pos) const {
            difference_type ccap = difference_type(_cap);
            return _cap ? ((pos%ccap)+ccap) % ccap : 0;
        }
        template<class TT> void internal_push_back(TT && v) noexcept {
            if(size()>=capacity()) {
                // copy the value, edge case if v belongs
                // to the dynamic array
                T vv = microc::traits::forward<TT>(v); // copy-or-move ctor
                alloc_(true);
                ::new(_data + mf(_back++), microc_new::blah) T(microc::traits::move<TT>(vv));
            } else ::new(_data + mf(_back++), microc_new::blah) T(microc::traits::forward<TT>(v));
        }
        template<class TT> void internal_push_front(TT && v) noexcept {
            if(size()>=capacity()) {
                // copy the value, edge case if v belongs
                // to the dynamic array
                T vv = microc::traits::forward<TT>(v); // copy-or-move ctor
                alloc_(true);
                ::new(_data + mf(--_front), microc_new::blah) T(microc::traits::move<TT>(vv));
            } else {
                ::new(_data + mf(--_front), microc_new::blah) T(microc::traits::forward<TT>(v));
            }
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
                for (size_type ix = 0; ix < delta; ++ix) push_back(microc::traits::forward<TT>(perfect_forwarded_value));
            }
        }

    public:
        void push_back(const T & v) noexcept { internal_push_back(v); }
        void push_back(T && v) noexcept { internal_push_back(v); }
        void pop_back() {
            if(size()==0) return;
            _data[--_back].~T();
            if(size() < (_cap>>1)) alloc_(false);
        }

        void push_front(const T & v) noexcept { internal_push_front(v); }
        void push_front(T && v) noexcept { internal_push_front(v); }
        void pop_front() {
            if(size()==0) return;
            _data[_front++].~T();
            if(size() < (_cap>>1)) alloc_(false);
        }

        template<class... Args>
        iterator emplace(const_iterator pos, Args&&... args) {
            return insert(pos, T(microc::traits::forward<Args>(args)...));
        }
        template<typename... Args>
        reference emplace_back(Args&&... args) {
            if(size()>=capacity()) alloc_(true);
            ::new (_data + mf(_back++), microc_new::blah) T(microc::traits::forward<Args>(args)...);
            return back();
        }
        template<typename... Args>
        reference emplace_front(Args&&... args) {
            if(size()>=capacity()) alloc_(true);
            ::new (_data + mf(--_front), microc_new::blah) T(microc::traits::forward<Args>(args)...);
            return back();
        }

        void clear() noexcept {
            for (size_type ix = 0; ix < size(); ++ix) (_data + mf(_front + ix))->~T();
            _front=0; _back=0;
        }

        void drain() noexcept {
            clear();
            if(_data) _alloc.deallocate(_data, capacity());
            _data = nullptr; _cap = 0;
        }

        // avoid overload resolution if InputIt is integral type, this can conflict with the
        // insert(const_iterator pos, size_type count, const T& value) overload for integral types
        template<class InputIt, typename bb = microc::traits::enable_if_t<!microc::traits::is_integral<InputIt>::value>>
        iterator insert(const_iterator pos, InputIt first, InputIt last, bool move=false) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            if(first==last) return pos;
            // insert range before pos
            const auto dist_to_first = pos-begin();
            const auto needed_extra_size = last-first;
            const auto how_many_to_shift_right = end()-pos;
            const auto how_many_to_construct = end()-pos;
            const bool requires_expend = size()+needed_extra_size > capacity();
            if(requires_expend) {
                re_alloc_to(capacity() + needed_extra_size + 0);
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
                ::new (&(*(--new_end)), microc_new::blah) T(microc::traits::move(*(--current_end)));
            } // move-assign at end the last elements, that belong to this
            for (size_type ix = 0; ix<how_many_veterans_require_move_assignment; ++ix) { // count from [N..1]
                *(--new_end) = microc::traits::move(*(--current_end));
            }
            // copy-construct some range elements
            for (size_type ix = 0; ix<how_many_range_require_copy_construction; ++ix) { // count from [N..1]
                if(!move) ::new (&(*(--new_end)), microc_new::blah) T(*(--last));
                else ::new (&(*(--new_end)), microc_new::blah) T(microc::traits::move(*(--last)));
            }
            do { // move-or-copy assign remaining items from iterator
                *(--new_end) = !move ? *(--last) : microc::traits::move(*(--last));
            } while(last!=first);
            _back+=needed_extra_size;
            return pos;
        }
        iterator insert(const_iterator pos, const T& value) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            if(pos==end()) { push_back(value); return end(); }
            return insert<const T *>(pos, &value, (&value)+1);
        }
        iterator insert(const_iterator pos, T && value) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            if(pos==end()) { push_back(microc::traits::move(value)); return end(); }
            return insert<const T *>(pos, &value, (&value)+1, true);
        }
        iterator insert(const_iterator pos, size_type count, const T& value) {
            if(pos>end() or pos<begin()) {
                throw_out_of_bounds_exception_if_can();
                return end();
            }
            // this is a very lazy batch insert, correct way it to implement a
            // range iterator that returns the same value
            auto last_pos = pos;
            for (size_type ix = 0; ix < count; ++ix) {
                last_pos = insert(last_pos, value);
            }
            return last_pos-count;
        }

        iterator erase(const_iterator pos) { return erase(pos, pos+1); }
        iterator erase(const_iterator first, const_iterator last) {
            auto count_to_erase = last-first;
            auto distance_to_first = first - begin();
            auto move_to_iter = iterator(first);
            auto last_copy_1 = last;
            while (last_copy_1!=end()) {
                *(move_to_iter) = microc::traits::move(*last_copy_1);
                ++last_copy_1; ++move_to_iter;
            }
            while(count_to_erase--!=0) pop_back();
            // pop back might invalidate iterators, so re-invent them
            return begin()+distance_to_first;
        }

        void resize(size_type count) { internal_resize(count, T()); }
        void resize(size_type count, const value_type& value) { internal_resize(count, value); }

        // etc..
        Alloc get_allocator() const noexcept { return Alloc(_alloc); }
    };

    template<class T, class Alloc>
    bool operator==(const circular_array<T,Alloc>& lhs,
                    const circular_array<T,Alloc>& rhs ) {
        if(!(lhs.size()==rhs.size())) return false;
        using size_type = typename circular_array<T,Alloc>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix) {
            if(!(lhs[ix]==rhs[ix])) return false;
        }
        return true;
    }
}