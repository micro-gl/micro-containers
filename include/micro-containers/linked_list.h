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
    namespace linked_list_traits {

        template< class T > struct remove_reference      {typedef T type;};
        template< class T > struct remove_reference<T&>  {typedef T type;};
        template< class T > struct remove_reference<T&&> {typedef T type;};
        template <class _Tp> inline typename remove_reference<_Tp>::type&&
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
        template<bool, class _Tp = void> struct enable_if {};
        template<class _Tp> struct enable_if<true, _Tp> { typedef _Tp type; };

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
                ::new(p, microc_new::blah) U(linked_list_traits::forward<Args>(args)...);
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
     * Doubly-connected Circular Linked List like container, does not obey all of the propagate syntax that
     * Allocator Aware Container follows
     * @tparam T the type
     * @tparam Allocator the allocator type
     */
    template<typename T, class Allocator=linked_list_traits::std_allocator<T>>
    class linked_list {
    public:
        using value_type = T;
        using allocator_type = Allocator;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using size_type = MICRO_CONTAINERS_SIZE_TYPE;

    private:
        struct node_base_t {
            node_base_t() : prev(nullptr), next(nullptr) {}
            ~node_base_t() = default;
            node_base_t * prev;
            node_base_t * next;
        };
        struct node_t : public node_base_t {
            node_t()=default;
            ~node_t() = default;
            node_t(const value_type & $value) :
                    node_base_t(), value($value) {}
            node_t(value_type && $value) :
                    node_base_t(), value(linked_list_traits::move($value)) {}
            template<class... Args>
            node_t(Args&&... args) : node_base_t(),
                                     value(linked_list_traits::forward<Args>(args)...) {}
            value_type value;
        };

        template<class value_reference_type>
        struct iterator_t {
            node_base_t * _node;

            template<class value_reference_type_t>
            iterator_t(const iterator_t<value_reference_type_t> & other) : _node(other._node) {}
            explicit iterator_t(node_base_t * start) : _node(start) {}
            iterator_t& operator++() { _node=_node->next; return *this;}
            iterator_t& operator--() { _node=_node->prev; return *this;}
            iterator_t operator++(int) {iterator_t retval(_node); ++(*this); return retval;}
            iterator_t operator--(int) {iterator_t retval(_node); --(*this); return retval;}
            iterator_t operator+(size_type step) {
                node_base_t * current = _node;
                for (size_type ix = 0; ix < step; ++ix)
                    current=current ? current->next : nullptr;
                return iterator_t(current);
            }
            iterator_t operator-(size_type step) {
                node_base_t * current = _node;
                for (size_type ix = 0; ix < step; ++ix)
                    current=current ? current->prev : nullptr;
                return iterator_t(current);
            }
            bool operator==(iterator_t other) const {return _node == other._node;}
            bool operator!=(iterator_t other) const {return !(*this == other);}
            value_reference_type operator*() const {return static_cast<node_t *>(_node)->value ;}
        };

        node_base_t * non_const_node(const node_base_t * node) const
        { return const_cast<node_base_t *>(node); }

    public:
        using iterator = iterator_t<reference>;
        using const_iterator = iterator_t<const_reference>;
        using node_type = node_t;

        const_reference back() const noexcept { return *(--end()); }
        const_reference front() const noexcept { return *begin(); }
        reference back() noexcept { return *(--end()); }
        reference front() noexcept { return *begin(); }
        iterator begin() noexcept {return iterator(_sentinel_node.next);}
        const_iterator begin() const noexcept {return const_iterator(non_const_node(_sentinel_node.next));}
        const_iterator cbegin() const noexcept {return const_iterator(non_const_node(_sentinel_node.next));}
        iterator end() noexcept {return iterator(&_sentinel_node);}
        const_iterator end() const noexcept {return const_iterator(non_const_node(&_sentinel_node));}
        const_iterator cend() const noexcept {return const_iterator(non_const_node(&_sentinel_node));}
        Allocator get_allocator() const noexcept { return Allocator(_alloc); }
        bool empty() const noexcept { return size()==0; }
        size_type size() const noexcept { return _size; }

    private:
        using rebind_allocator_type = typename Allocator::template rebind<node_t>::other;

        node_base_t _sentinel_node; // _sentinel_node=end, _sentinel_node->next=begin
        rebind_allocator_type _alloc;
        size_type _size;

        void reset_sentinel() { _sentinel_node.prev = _sentinel_node.next = &_sentinel_node; }

    public:
        explicit linked_list(const Allocator & allocator = Allocator()) noexcept :
                _sentinel_node(), _size(0), _alloc{allocator} {
            reset_sentinel();
        }

        linked_list(const size_type count, const T & value, const Allocator & alloc = Allocator()) :
                linked_list(alloc) {
            for (size_type ix = 0; ix < count; ++ix) push_back(value);
        }

        linked_list(const size_type count, const Allocator & allocator = Allocator()) :
                linked_list(count, T(), allocator) {}

        template<class Iterable>
        linked_list(const Iterable &list, const Allocator & allocator= Allocator()) noexcept :
                linked_list(allocator) {
            for (const auto & item : list) push_back(item);
        }

        linked_list(const linked_list & other, const Allocator & allocator) noexcept :
                linked_list(allocator) {
            for (const auto & item : other) push_back(item);
        }

        linked_list(const linked_list & other) noexcept :
                linked_list(other, other.get_allocator()) {}

        linked_list(linked_list && other, const Allocator & allocator) noexcept :
                linked_list(allocator) {
            const bool are_equal_allocators = allocator==other.get_allocator();
            if(are_equal_allocators) {
                _sentinel_node.next = other.size() ? other._sentinel_node.next : &_sentinel_node;
                _sentinel_node.prev = other.size() ? other._sentinel_node.prev : &_sentinel_node;
                _sentinel_node.next->prev = &_sentinel_node;
                _sentinel_node.prev->next = &_sentinel_node;
                _size = other._size;
                other.reset_sentinel();
                other._size = 0;
            } else {
                for (auto & item : other)
                    push_back(linked_list_traits::move(item));
                other.clear();
            }
        }

        linked_list(linked_list && other) noexcept :
                    linked_list{linked_list_traits::move(other), other.get_allocator()} {}

        ~linked_list() noexcept { clear(); }

        linked_list & operator=(const linked_list & other) noexcept {
            if(this!= &other) {
                clear();
                for (const auto & item : other) push_back(item);
            }
            return (*this);
        }

        linked_list & operator=(linked_list && other) noexcept {
            // two cases:
            // 1. if the allocators are equal, then move the data completely.
            // 2. otherwise, move push_back element by element
            const bool are_equal_allocators = _alloc == other.get_allocator();
            const bool self_assign = this == &other;
            if(self_assign) return *this;
            clear();
            if(are_equal_allocators) {
                // clear and destruct current elements
                // move everything from other
                _sentinel_node.next = other.size() ? other._sentinel_node.next : &_sentinel_node;
                _sentinel_node.prev = other.size() ? other._sentinel_node.prev : &_sentinel_node;
                _sentinel_node.next->prev = &_sentinel_node;
                _sentinel_node.prev->next = &_sentinel_node;
                _size = other._size;
                other.reset_sentinel();
                other._size = 0;
            } else {
                for (auto & item : other)
                    push_back(linked_list_traits::move(item));
                other.clear();
            }
            return (*this);
        }

    private:
        node_t * create_node(const value_type & value = value_type()) {
            node_t * node = _alloc.allocate(1);
            ::new(node, microc_new::blah) node_t(value); // construct
            return node;
        }
        node_t * create_node(value_type && value) {
            node_t * node = _alloc.allocate(1);
            ::new(node, microc_new::blah) node_t(linked_list_traits::move(value)); // construct
            return node;
        }
        template<class... Args>
        node_t * create_node(Args &&... args) {
            node_t * node = _alloc.allocate(1);
            ::new (node, microc_new::blah) node_t(linked_list_traits::forward<Args>(args)...); // construct
            return node;
        }
        iterator insert_node_internal(const_iterator pos, node_t * node) {
            // insert_node a new node before pos
            auto * node_pos = pos._node;
            auto * node_before_pos = pos._node->prev;
            auto * node_new = node;
            node_before_pos->next = node_new;
            node_pos->prev = node_new;
            node_new->prev = node_before_pos;
            node_new->next = node_pos;
            _size+=1;
            return iterator(node_new);
        }

    public:
        iterator insert(const_iterator pos, const T & value) {
            // insert_node a new node before pos
            return insert_node_internal(pos, create_node(value));
        }
        iterator insert(const_iterator pos, T&& value) {
            // insert_node a new node before pos
            return insert_node_internal(pos, create_node(linked_list_traits::move(value)));
        }
        iterator insert(const_iterator pos, size_type count, const T & value) {
            if(count==0) return pos;
            auto first_pos = insert(pos, value);
            for (size_type ix = 1; ix < count; ++ix)
                insert(pos, value);
            return first_pos;
        }
        template<class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last ) {
            iterator iter(pos); --iter;
            do {
                insert(pos, *first);
            } while (++first!=last);
            return ++iter;
        }

    private:
        template<class... Args>
        iterator insert_emplace(const_iterator pos, Args&&... args) {
            // insert_node a new node before pos
            return insert_node_internal(pos,
                        create_node(linked_list_traits::forward<Args>(args)...));
        }

    public:
        iterator erase(const_iterator pos) {
            if(pos==end()) return pos;
            auto * node_to_erase = pos._node;
            auto * before = pos._node->prev;
            auto * after = pos._node->next;
            before->next = after;
            after->prev = before;
            // destruct, this will call base destructor as well
            reinterpret_cast<node_t *>(node_to_erase)->~node_t();
            // de-allocate
            _alloc.deallocate((node_t *)node_to_erase);
            // size update
            _size -= 1;
            return iterator(after);
        }

        iterator erase(const_iterator first, const_iterator last) {
            const bool is_last_end = last == cend();
            auto iter = first;
            while (iter!=last) iter = erase(iter);
            // If last==end() prior to removal, then the updated end() iterator is returned.
            return is_last_end ? cend() : last;
        }

        void push_back(const T & value) { insert(cend(), value); }
        void push_back(T && value) { insert(cend(), linked_list_traits::move(value)); }
        void push_front(const T & value) { insert(cbegin(), value); }
        void push_front(T && value) { insert(cbegin(), linked_list_traits::move(value)); }
        void pop_back() { if(empty()) return; erase(--cend()); }
        void pop_front() { if(empty()) return; erase(cbegin()); }

        template<typename... Args>
        iterator emplace(const_iterator pos, Args&&... args)
        { return insert_emplace(pos, linked_list_traits::forward<Args>(args)...); }
        template<typename... Args>
        iterator emplace_back(Args&&... args)
        { return emplace<Args...>(end(), linked_list_traits::forward<Args>(args)...); }
        template<typename... Args>
        iterator emplace_front(Args&&... args)
        { return emplace<Args...>(begin(), linked_list_traits::forward<Args>(args)...); }

        void clear() { while (size()) erase(--end()); reset_sentinel(); }
    };
}