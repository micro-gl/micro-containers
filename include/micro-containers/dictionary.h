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

#include "avl_tree.h"
#include "traits.h"
#include "pair.h"

template<class Key>
struct dict_less {
    bool operator()(const Key& lhs, const Key& rhs) const
    {return lhs < rhs;}
};

/**
 * AVL Tree balanced tree, logarithmic complexity everything.
 * Notes:
 * - Most algorithms use recursion, so there is a stack memory cost of O(log(n))
 * - This class is Allocator-Aware
 * @tparam Key the key type, that the tree stores
 * @tparam Compare _compare structure or lambda
 * @tparam Allocator allocator type
 */
template<class Key, class T,
         class Compare=dict_less<Key>,
         class Allocator=micro_containers::traits::std_allocator<char>>
class dictionary {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = pair<const Key, T>;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    class value_compare {
        Compare _compare;
    protected:
        value_compare(Compare c) : _compare(c) {}
        bool operator()( const value_type& lhs, const value_type& rhs ) const
        { return _compare(lhs.first, rhs.first); }
    };

private:
    template<class value_reference_type, class tree_iterator>
    struct iterator_t {
        tree_iterator _pos;
        dictionary * ncn(const dictionary * val) const
        { return const_cast<dictionary *>(val); }
        template<class value_reference_type_t, tree_iterator>
        iterator_t(const iterator_t<value_reference_type_t, tree_iterator> & other) :
                _pos(other._pos) {}
        explicit iterator_t(const tree_iterator & start) : _pos(start) {}
        iterator_t& operator++() { ++_pos; return *this;}
        iterator_t& operator--() { --_pos; return *this;}
        iterator_t operator++(int) {iterator_t retval(_pos); ++(*this); return retval;}
        iterator_t operator--(int) {iterator_t retval(_pos); --(*this); return retval;}
        bool operator==(iterator_t other) const {return _pos == other._pos;}
        bool operator!=(iterator_t other) const {return !(*this == other);}
        value_reference_type operator*() const {return (*_pos).key; } // key in tree is pair<key, T> ;}
    };

public:
    using tree_type = avl_tree<value_type, value_compare, Allocator>;
    using node_type = typename tree_type::node_type;
    using iterator = iterator_t<value_type &, typename tree_type::iterator>;
    using const_iterator = iterator_t<const value_type &, typename tree_type::const_iterator>;
    using rebind_alloc = typename Allocator:: template rebind<node_type>::other;
    static constexpr unsigned long node_type_size = sizeof (node_type);

    // iterators
    iterator begin() noexcept { return iterator{_tree.begin(), this}; }
    const_iterator begin() const noexcept { return const_iterator{_tree.begin(), this}; }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator end() noexcept { return iterator(_tree.end()); }
    const_iterator end() const noexcept { return const_iterator(_tree.end()); }
    const_iterator cend() const noexcept { return end(); }

private:
    key_compare _key_compare;
    tree_type _tree;
    rebind_alloc _alloc;

public:

    dictionary(const Compare & comp,
               const Allocator & allocator=Allocator()) :
            _key_compare(comp), _alloc(allocator),
            _tree(value_compare(_key_compare), allocator) {}

    dictionary(const Allocator & allocator=Allocator()) :
            dictionary(Compare(), allocator) {};


    dictionary(const dictionary & other, const Allocator & allocator) :
            dictionary(other.get_allocator()) {
//        for(const auto & node : other)
//            insert_node(node.key);
    }
    dictionary(const dictionary & other) : dictionary(other, other.get_allocator()) {}
    dictionary(dictionary && other, const Allocator & allocator) :
            dictionary(allocator) {
//        const bool are_equal_allocators = _alloc==allocator;
//        if(are_equal_allocators) {
//            _root=other._root;
//            other._root=nullptr;
//        } else {
//            for(const auto & node : other)
//                insert_node(micro_containers::traits::move(node.key));
//        }
    }
    dictionary(dictionary && other)  noexcept :
            dictionary(micro_containers::traits::move(other), other.get_allocator()) {}
    ~dictionary() { clear(); }

    dictionary & operator=(const dictionary & other) {
        if(this!=&(other)) {
//            clear();
//            for (const auto & node: other)
//                insert_node(node.key);
        }
        return *this;
    }
    dictionary & operator=(dictionary && other) noexcept {
        if(this!=&(other)) {
//            clear();
//            const bool are_equal_allocators = _alloc==other.get_allocator();
//            if(are_equal_allocators) {
//                _root=other._root;
//                other._root=nullptr;
//            } else {
//                for(const auto & node : other)
//                    insert_node(micro_containers::traits::move(node.key));
//                other.clear();
//            }
        }
        return *this;
    }

    Allocator get_allocator() const { return Allocator(_alloc); }
    key_compare key_comp() const { return _key_compare; }
    value_compare value_comp() const { return _tree.get_comparator(); }

    // capacity
    bool empty() const noexcept { return _tree.empty(); }
    unsigned int size() const noexcept { return _tree.size(); }

    // element access
    T& at(const Key& key) {
        return _tree.find(value_type(key))->key.second;
    }
    const T& at(const Key& key) const {
        return _tree.find(value_type(key))->key.second;
    }
    T& operator[](const Key & key) {
        // todo
    }
    T& operator[](Key && key) {
        // todo
    }

    // Modifiers
    void clear() noexcept {
        //todo
    }
    pair<iterator, bool> insert(const value_type& value) {
        _tree.insert_node(value);
    }
};