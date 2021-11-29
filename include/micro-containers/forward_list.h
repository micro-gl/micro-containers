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

enum class micro { blah };

#ifndef MICRO_CONTAINERS_SIZE_TYPE
inline void* operator new (unsigned long n, void* ptr, enum micro) noexcept {
    return ptr;
}
#else
inline void* operator new (MICRO_CONTAINERS_SIZE_TYPE n, void* ptr, enum micro) noexcept {
    return ptr;
}
#endif

namespace forward_list_traits {

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

    template<typename _Tp, typename _Up = _Tp&&>
    _Up __declval(int);  // (1)

    template<typename _Tp>
    _Tp __declval(long); // (2)

    template<typename _Tp>
    auto declval() noexcept -> decltype(__declval<_Tp>(0));

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
            new(p) U(forward_list_traits::forward<Args>(args)...);
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
 * Singly-connected Linked List like container, more space efficient than doubly
 * connected linked list If you do not need bi-directional iteration and can
 * settle for single forward iteration.
 * Allocator Aware Container follows
 * @tparam T the type
 * @tparam Allocator the allocator type
 */
template<typename T, class Allocator=forward_list_traits::std_allocator<T>>
class forward_list {
public:
    using value_type = T;
    using allocator_type = Allocator;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using index = unsigned int;
    using uint = unsigned int;

private:
    struct node_t {
        node_t()=default;
        node_t(const value_type & $value) :
                    next(nullptr), value($value) {}
        node_t(value_type && $value) :
                next(nullptr), value(forward_list_traits::move($value)) {}
        template<class... Args>
        node_t(Args&&... args) : next(nullptr),
                    value(forward_list_traits::forward<Args>(args)...) {}
        node_t * next;
        value_type value;
    };

    template<class value_reference_type>
    struct iterator_t {
        node_t * _node;

        template<class value_reference_type_t>
        iterator_t(const iterator_t<value_reference_type_t> & other) : _node(other._node) {}
        explicit iterator_t(node_t * start) : _node(start) {}
        iterator_t& operator++() { _node=_node->next; return *this;}
        iterator_t operator++(int) {iterator_t retval(_node); ++(*this); return retval;}
        iterator_t operator+(int val) {
            node_t * result = _node;
            for (int ix = 0; ix < val; ++ix) result = result->next;
            return iterator_t(result);
        }
        bool operator==(iterator_t other) const {return _node == other._node;}
        bool operator!=(iterator_t other) const {return !(*this == other);}
        value_reference_type operator*() const {return _node->value ;}
    };

    node_t * non_const_node(const node_t * node) const
    { return const_cast<node_t *>(node); }

public:
    using iterator = iterator_t<reference>;
    using const_iterator = iterator_t<const_reference>;
    using node_type = node_t;

    const_reference front() const noexcept { return *begin(); }
    reference front() noexcept { return *begin(); }
    iterator begin() noexcept {return iterator(_sentinel_node.next);}
    iterator before_begin() noexcept {return end();}
    const_iterator begin() const noexcept {return const_iterator(non_const_node(_sentinel_node.next));}
    const_iterator before_begin() const noexcept {return end();}
    const_iterator cbegin() const noexcept {return begin();}
    iterator end() noexcept {return iterator(&_sentinel_node);}
    const_iterator end() const noexcept {return cend();}
    const_iterator cend() const noexcept {return const_iterator(non_const_node(&_sentinel_node));}
    Allocator get_allocator() const noexcept { return Allocator(_alloc); }
    bool empty() const noexcept { return size()==0; }
    index size() const noexcept { return _size; }

private:
    using rebind_allocator_type = typename Allocator::template rebind<node_t>::other;

    node_t _sentinel_node; // _sentinel_node=end, _sentinel_node->next=begin
    rebind_allocator_type _alloc;
    index _size = 0u;

    void reset_sentinel() { _sentinel_node.next = &_sentinel_node; }

public:
    explicit forward_list(const Allocator & allocator = Allocator()) noexcept : _alloc{allocator} {
        reset_sentinel();
    }

    forward_list(const uint count, const T & value, const Allocator & alloc = Allocator()) :
            forward_list(alloc) {
        iterator current = before_begin();
        for (int ix = 0; ix < count; ++ix) current=insert_after(current, value);
    }

    forward_list(const uint count, const Allocator & allocator = Allocator()) :
            forward_list(count, T(), allocator) {}

    template<class Iterable>
    forward_list(const Iterable &list, const Allocator & allocator= Allocator()) noexcept :
            forward_list(allocator) {
        iterator current = before_begin();
        for (const auto & item : list) current=insert_after(current, item);
    }

    forward_list(const forward_list & other, const Allocator & allocator) noexcept :
            forward_list(allocator) {
        iterator current = before_begin();
        for (const auto & item : other) current=insert_after(current, item);
    }

    forward_list(const forward_list & other) noexcept :
            forward_list(other, other.get_allocator()) {}

    forward_list(forward_list && other, const Allocator & allocator) noexcept :
            forward_list(allocator) {
        const bool are_equal_allocators = allocator==other.get_allocator();
        if(are_equal_allocators) {
            other.find_last_node()->next = &_sentinel_node;
            _sentinel_node.next = other._sentinel_node.next;
            _size = other._size;
            other.reset_sentinel();
            other._size = 0;
        } else {
            iterator current = before_begin();
            for (auto & item : other)
                current=insert_after(current, forward_list_traits::move(item));
            other.clear();
        }
    }

    forward_list(forward_list && other) noexcept :
                forward_list{forward_list_traits::move(other), other.get_allocator()} {}

    ~forward_list() noexcept { clear(); }

    forward_list & operator=(const forward_list & other) noexcept {
        if(this!= &other) {
            clear();
            iterator current = before_begin();
            for (const auto & item : other) current = insert_after(current, item);
        }
        return (*this);
    }

    forward_list & operator=(forward_list && other) noexcept {
        // two cases:
        // 1. if the allocators are equal, then move the data completely.
        // 2. otherwise, move push_back element by element
        const bool are_equal_allocators = _alloc == other.get_allocator();
        const bool self_assign = this == &other;
        if(self_assign) return *this;
        clear();
        if(are_equal_allocators) {
            other.find_last_node()->next = &_sentinel_node;
            _sentinel_node.next = other._sentinel_node.next;
            _size = other._size;
            other.reset_sentinel();
            other._size = 0;
        } else {
            iterator current = before_begin();
            for (auto & item : other)
                current = insert_after(current, forward_list_traits::move(item));
            other.clear();
        }
        return (*this);
    }

private:
    node_t * find_last_node() {
        node_t * sentinel = &_sentinel_node;
        node_t * current = sentinel;
        while(current->next!=sentinel) {
            current = current->next;
        }
        return current;
    }
    node_t * create_node(const value_type & value = value_type()) {
        node_t * node = _alloc.allocate(1);
        ::new(node, micro::blah) node_t(value); // construct
        return node;
    }
    node_t * create_node(value_type && value) {
        node_t * node = _alloc.allocate(1);
        ::new(node, micro::blah) node_t(forward_list_traits::move(value)); // construct
        return node;
    }
    template<class... Args>
    node_t * create_node(Args &&... args) {
        node_t * node = _alloc.allocate(1);
        ::new (node, micro::blah) node_t(forward_list_traits::forward<Args>(args)...); // construct
        return node;
    }
    iterator insert_node_after_internal(const_iterator pos, node_t * node) {
        // insert a new node after pos
        auto * node_pos = pos._node;
        auto * node_pos_next = node_pos->next;
        node_pos->next = node;
        node->next = node_pos_next;
        _size+=1;
        return iterator(node);
    }

public:
    iterator insert_after(const_iterator pos, const T & value) {
        // insert_node a new node after pos, returns new node pos iterator
        return insert_node_after_internal(pos, create_node(value));
    }
    iterator insert_after(const_iterator pos, T&& value) {
        // insert_node a new node after pos
        return insert_node_after_internal(pos, create_node(forward_list_traits::move(value)));
    }
    iterator insert_after(const_iterator pos, uint count, const T & value) {
        if(count==0) return pos;
        iterator current = pos;
        for (int ix = 0; ix < count; ++ix)
            current=insert_after(current, value);
        return current;
    }
    // todo: test this
    template<class InputIt, typename A = decltype(forward_list_traits::declval<InputIt>().operator *())>
    iterator insert_after(const_iterator pos, InputIt first, InputIt last ) {
        iterator my_iter = pos;
        InputIt current = first;
        while(current != last) {
            my_iter = insert_after(my_iter, *current);
        }
        return my_iter;
    }

public:
    // returns iterator to the following the erased one
    iterator erase_after(const_iterator pos) {
        if((pos+1)==end()) return pos;
        auto * node_at_pos = pos._node;
        auto * node_to_erase = pos._node->next;
        auto * node_following_erase = node_to_erase->next;
        node_at_pos->next = node_following_erase;
        // destruct
        node_to_erase->~node_t();
        // de-allocate
        _alloc.deallocate((node_t *)node_to_erase);
        // size update
        _size -= 1;
        return iterator(node_following_erase);
    }

    iterator erase_after(const_iterator first, const_iterator last) {
        auto iter = first;
        if((iter+1)==last) return last;
        while (iter!=last) iter = erase_after(first);
        return last;
    }

    void push_front(const T & value) { insert_after(before_begin(), value); }
    void push_front(T && value) { insert_after(before_begin(), forward_list_traits::move(value)); }
    void pop_front() { if(empty()) return; erase_after(before_begin()); }

    template<class... Args>
    iterator emplace_after(const_iterator pos, Args&&... args) {
        auto iter_pos = insert_node_after_internal(pos,
                create_node<Args...>(forward_list_traits::forward<Args>(args)...));
        return iter_pos;
    }
    template<typename... Args>
    reference emplace_front(Args&&... args) {
        return *(emplace_after<Args...>(before_begin(), forward_list_traits::forward<Args>(args)...));
    }

    void clear() { while (size()) erase_after(before_begin()); reset_sentinel(); }
};