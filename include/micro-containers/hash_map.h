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
#include "pair.h"

//#define MICRO_CONTAINERS_ENABLE_THROW
#ifdef MICRO_CONTAINERS_ENABLE_THROW
struct throw_hash_map_out_of_range {};
#endif

#ifndef MICRO_CONTAINERS_SIZE_TYPE
#define MICRO_CONTAINERS_SIZE_TYPE unsigned long
#endif

template<class Key> struct hash{};
template<> struct hash<unsigned> {
    MICRO_CONTAINERS_SIZE_TYPE operator()(unsigned const s) const noexcept { return s; }
};
template<> struct hash<signed> {
    MICRO_CONTAINERS_SIZE_TYPE operator()(signed const s) const noexcept { return s & ~(1<<((sizeof(s)<<3)-1)) ; }
};


/**
 * Dictionary is an ordered associative data structure also known as orederd_map
 * Notes:
 * - Most algorithms use recursion, so there is a stack memory cost of O(log(n))
 * - This class is Allocator-Aware
 * @tparam Key the key type, that the tree stores
 * @tparam T The mapped value type of a key
 * @tparam Compare compare structure or lambda for key
 * @tparam Allocator allocator type
 */
template<class Key, class T,
         class Hash=hash<Key>,
         class Allocator=micro_containers::traits::std_allocator<char>>
class hash_map {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = pair<Key, T>;
    using size_type = MICRO_CONTAINERS_SIZE_TYPE;
    using hasher = Hash;
    using allocator_type = Allocator;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;

private:
    struct node_t {
        node_t()=default;
        node_t(const value_type & $value) :
                prev(nullptr), next(nullptr), key_value($value) {}
        node_t(value_type && $value) :
                prev(nullptr), next(nullptr), key_value(micro_containers::traits::move($value)) {}
        template<class... Args>
        node_t(Args&&... args) : prev(nullptr), next(nullptr),
                                 key_value(micro_containers::traits::forward<Args>(args)...) {}
        node_t * prev;
        node_t * next;
        value_type key_value;
    };

    struct bucket_t {
        bucket_t(node_t * $list) : list($list) {};
        node_t * list;
        bool is_empty() const { return list==nullptr; }
        node_t * head() const { return list; }
        node_t * tail() const {
            const node_t * iter = list;
            while(iter && iter->next) iter=iter->next;
            return iter;
        }
    };

    static node_t * ncn(const node_t * node)
    { return const_cast<node_t *>(node); }


    template<class value_reference_type>
    struct iterator_t {
        const node_t * _n; // node
        const hash_map * _c; // container
        size_type _bi; // bucket index

        template<class value_reference_type_t>
        iterator_t(const iterator_t<value_reference_type_t> & o) : iterator_t(o._n, o._bi, o._c) {}
        explicit iterator_t(const node_t * n, size_type bi, const hash_map * c) : _n(n), _bi(bi), _c(c) {}
        iterator_t& operator++() {
            auto q = _c->internal_node_successor(_n, _bi);
            _n = q.node; _bi = q.bucket_index;
            return *this;
        }
        iterator_t& operator--() {
            auto q = _c->internal_node_predeccessor(_n, _bi);
            _n = q.node; _bi = q.bucket_index;
            return *this;
        }
        iterator_t operator+(int val) {
            node_query q(_n, _bi);
            for (int ix = 0; ix < val; ++ix)
                q = _c->internal_node_successor(q.node, q.bucket_index);
            return iterator_t(q.node, q.bucket_index, _c);
        }
        iterator_t operator++(int) { iterator_t ret(_n, _bi, _c); ++(*this); return ret; }
        iterator_t operator--(int) { iterator_t ret(_n, _bi, _c); --(*this); return ret; }
        bool operator==(iterator_t o) const { return _n==o._n; }
        bool operator!=(iterator_t o) const { return !(*this==o); }
        value_reference_type operator*() const {
            auto & aa =  (*ncn(_n)).key_value;
            std::cout << to_string(aa, true);
            return (*ncn(_n)).key_value;
        }
    };

public:
    using node_type = node_t;
    using bucket_type = bucket_t;
    using iterator = iterator_t<reference>;
    using const_iterator = iterator_t<const_reference>;
    using node_allocator = typename Allocator:: template rebind<node_type>::other;
    using bucket_allocator = typename Allocator:: template rebind<bucket_type>::other;
    static constexpr unsigned long node_type_size = sizeof (node_type);
    static constexpr unsigned long bucket_type_size = sizeof (bucket_type);
    static constexpr size_type DEFAULT_BUCKET_COUNT = 1;

    //
    struct node_query {
        node_query(const node_t * $node=nullptr, size_type $bucket_index=0) :
                node($node), bucket_index($bucket_index) {}
        const node_t * node; size_type bucket_index;
    };

    node_query internal_node_predeccessor(const node_t * node, size_type bi) const {
        node = node ? node->prev : node;
        while(node==nullptr && bi>0) {
            bi-=1; // prev bucket
            node=_buckets[bi].tail(); // try tail
        } // if _n==nullptr, this is end signal
        node_query result;
        result.node = node; result.bucket_index=bi;
        return result;
    }

    node_query internal_node_successor(const node_t * node, size_type bi) const {
        node= node ? node->next : node;
        while(node==nullptr && bi<bucket_count()) {
            bi+=1; // next bucket
            node=_buckets[bi].list; // try head
        } // if _n==nullptr, this is end signal
        return node_query(node, bi);
    }

    node_query internal_node_first() const {
        // return first node in the leftmost non-empty bucket or nullptr indicating end
        node_query result;
        for (int ix = 0; ix < _bucket_count; ++ix) {
            auto * bucket_list_head = _buckets[ix].list;
            if(bucket_list_head) {
                result.node = bucket_list_head; result.bucket_index = ix;
                return result;
            }
        }
        result.bucket_index=_bucket_count; result.node=nullptr;
        return result;
    }

    node_query internal_node_last() const {
        // return last node from the right non-empty bucket or nullptr indicating end
        node_query result;
        for (int ix = _bucket_count-1; ix >=0; --ix) {
            auto * tail = _buckets[ix].tail();
            if(tail) {
                result.bucket_index=ix; result.node=tail;
                return result;
            }
        }
        result.bucket_index=_bucket_count; result.node=nullptr;
        return result;
    }

    // iterators
    iterator begin() noexcept {
        auto q=internal_node_first(); return iterator(q.node, q.bucket_index, this);
    }
    const_iterator begin() const noexcept {
        auto q=internal_node_first(); return const_iterator(q.node, q.bucket_index, this);
    }
    const_iterator cbegin() const noexcept { return begin(); }
    iterator end() noexcept { return iterator(nullptr, bucket_count(), this); }
    const_iterator end() const noexcept { return const_iterator(nullptr, bucket_count(), this); }
    const_iterator cend() const noexcept { return end(); }

private:
    // buckets
    bucket_t * _buckets;
    size_type _bucket_count;
    size_type _size;
    // hash
    hasher _hasher;
    float _max_load_factor;
    // other
    node_allocator _alloc_node;
    bucket_allocator _alloc_bucket;

public:

    // bucket interface
    size_type bucket_size(size_type n) const {
        const auto * bucket = _buckets[n].list;
        size_type count = 0;
        while(bucket) {
            ++count; bucket=bucket->next;
        }
        return count;
    }
    size_type bucket_count() const { return _bucket_count; }
    size_type bucket(const Key& key) const {
        const auto hash = _hasher(key);
        size_type bucket_index = hash % _bucket_count;
        return bucket_index;
    }
    size_type max_bucket_count() const { return ~size_type(0); }

    // hash policy
    float load_factor() const { return float(size())/bucket_count(); }
    float max_load_factor() const { return _max_load_factor; }
    void max_load_factor(float ml) {
        _max_load_factor = ml;
        const auto factor = load_factor();
        if(factor<_max_load_factor) return;
        // else, let's rehash
        size_type new_buckets_count = minimal_required_buckets_count_for_valid_load_factor();
        rehash(new_buckets_count);
    }

    bool requires_rehash() const { return load_factor()>max_load_factor(); }

    // the minimal buckets count required to keep load factor below max load factor
    size_type minimal_required_buckets_count_for_valid_load_factor() {
        const auto suggested = size_type(float(size())/max_load_factor());
        return suggested ? suggested : DEFAULT_BUCKET_COUNT;
    }

    node_t * internal_insert_node_at_front_of_bucket(node_t * node, const size_type bucket_index) {
        auto & bucket = _buckets[bucket_index];
        auto * head = bucket.list;
        node->next = head; node->prev=nullptr;
        if(head) head->prev = node;
        bucket.list = node;
        return node;
    }
    node_query internal_insert_node(node_t * node) {
        const auto hash = _hasher(node->key_value.first);
        size_type bucket_index = hash % _bucket_count;
        return node_query(internal_insert_node_at_front_of_bucket(node, bucket_index),
                          bucket_index);
    }
    void rehash(size_type new_buckets_count) {
        bucket_type * old_buckets = _buckets;
        const size_type old_buckets_count = _bucket_count;
        if(new_buckets_count==old_buckets_count ||
                        new_buckets_count==0) return;
        if(new_buckets_count < minimal_required_buckets_count_for_valid_load_factor())
            new_buckets_count = minimal_required_buckets_count_for_valid_load_factor();
        // allocate and construct new buckets
        auto * new_buckets = _alloc_bucket.allocate(new_buckets_count);
        for (int ix = 0; ix < new_buckets_count; ++ix)
            ::new(new_buckets+ix) bucket_t(nullptr);
        // iterate all nodes
        for (int ix = 0; ix < old_buckets_count; ++ix) {
            auto & bucket = old_buckets[ix];
            while(bucket.list) {
                // remove head node from old list
                node_t * removed_node = bucket.list;
                bucket.list = removed_node->next;
                if(bucket.list) bucket.list->prev = nullptr;
                removed_node->prev=removed_node->next=nullptr;
                // compute hash again and re-assign bucket to new bucket
                const auto hash = _hasher(removed_node->key_value.first);
                size_type bucket_index = hash % new_buckets_count;
                auto & new_bucket = new_buckets[bucket_index];
                // add front in new bucket list
                removed_node->next = new_bucket.list; removed_node->prev=nullptr;
                if(removed_node->next) removed_node->next->prev = removed_node;
                new_bucket.list = removed_node;
            }
        }

        // destruct and deallocate old bucket
        for (int ix = 0; ix < old_buckets_count; ++ix)
            old_buckets[ix].~bucket_t();
        _alloc_bucket.deallocate(old_buckets);
        // assign new bucket info
        _buckets = new_buckets;
        _bucket_count = new_buckets_count;
    }

    hasher hash_function() const { return _hasher; }

    hash_map(size_type bucket_count,
             const Hash& hash = Hash(),
             const Allocator& allocator = Allocator()) :
            _max_load_factor(1.0f), _bucket_count(0), _size(0),
            _buckets(nullptr), _hasher(hash), _alloc_node(allocator), _alloc_bucket(allocator) {
        rehash(bucket_count);
    }
    hash_map() : hash_map(DEFAULT_BUCKET_COUNT, Hash(), Allocator()) {}
    explicit hash_map(const Allocator& alloc) : hash_map(DEFAULT_BUCKET_COUNT, Hash(), alloc) {};
    hash_map(size_type bucket_count, const Allocator& alloc ) : hash_map(bucket_count, Hash(), alloc) {}

    template< class InputIt >
    hash_map(InputIt first, InputIt last, size_type bucket_count,
             const Hash& hash = Hash(), const Allocator& alloc = Allocator() )
            : hash_map(bucket_count, hash, alloc) {
        // todo :  iterate
    }
    template< class InputIt >
    hash_map(InputIt first, InputIt last, size_type bucket_count,
             const Allocator& alloc = Allocator() )
            : hash_map(first, last, bucket_count, Hash(), alloc) {}

    hash_map(const hash_map & other, const Allocator & allocator) :
            hash_map(other._bucket_count, other._hasher, other.get_allocator()) {
        for(const auto & item : other) insert(item);
    }
    hash_map(const hash_map & other) : hash_map(other, other.get_allocator()) {}

    hash_map(hash_map && other, const Allocator & allocator) :
            hash_map(size_type(0), other._hasher, other.get_allocator()) {
        // using 0 to mute main constructor table creation
        const bool are_equal_allocators = _alloc_node == allocator;
        _max_load_factor = other._max_load_factor;
        if(are_equal_allocators) {
            _bucket_count=other._bucket_count;
            _buckets = other._buckets;
            _size = other._size;
            other._buckets=nullptr;
            other._bucket_count=0;
            other._size=0;
        } else {
            rehash(other._bucket_count); // reserves a table
            for(auto & item : other)
                insert(micro_containers::traits::move(item));
            other.clear();
        }
    }
    hash_map(hash_map && other) noexcept :
            hash_map(micro_containers::traits::move(other), other.get_allocator()) {}
    ~hash_map() { clear(); }

    hash_map & operator=(const hash_map & other) {
        if(this==&other) return *this;
        clear();
        for(const auto & item : other) insert(item);
        return *this;
    }
    hash_map & operator=(hash_map && other) noexcept {
        if(this==&(other)) return *this;
        clear();
        const bool are_equal_allocators = _alloc_node == other.get_allocator();
        _max_load_factor = other._max_load_factor;
        if(are_equal_allocators) {
            _bucket_count=other._bucket_count;
            _buckets = other._buckets;
            _size = other._size;
            other._buckets=nullptr;
            other._bucket_count=0;
            other._size=0;
        } else {
            rehash(other._bucket_count); // reserves a table
            for(auto & item : other)
                insert(micro_containers::traits::move(item));
            other.clear();
        }
        return *this;
    }

    Allocator get_allocator() const { return Allocator(_alloc_node); }

    // capacity
    bool empty() const noexcept { return _size==0; }
    size_type size() const noexcept { return _size; }

    // lookup
    iterator find(const Key& key) {
        const auto bi = bucket(key);
        const auto & bucket = _buckets[bi];
        const node_t * iter = bucket.list;
        while(iter && !(iter->key_value.first==key)) { iter=iter->next; }
        return iter ? iterator(iter, bi, this) : end();
    }
    const_iterator find(const Key& key) const {
        const auto bi = bucket(key);
        const auto & bucket = _buckets[bi];
        const node_t * iter = bucket.list;
        while(iter && !(iter->key_value.first==key)) { iter=iter->next; }
        return iter ? iterator(iter, bi, this) : end();
    }
    bucket_t & internal_bucket_of_key(const Key & key) const {
        const auto bi = bucket(key);
        return _buckets[bi];
    }
    bool contains(const Key& key) const {
        return find(key)!=end();
    }

    // element access
    T& at(const Key& key) {
        auto iter = find(key);
#ifdef MICRO_CONTAINERS_ENABLE_THROW
        if(iter==end()) throw throw_hash_map_out_of_range();
#endif
        return (*iter).second;
    }
    const T& at(const Key& key) const {
        auto iter = find(key);
#ifdef MICRO_CONTAINERS_ENABLE_THROW
        if(iter==end()) throw throw_hash_map_out_of_range();
#endif
        return (*iter).second;
    }
    T & operator[](const Key & key) {
        auto iter = insert(value_type(key, T())).first;
        return (*iter).second;
    }
    T & operator[](Key && key) {
        auto iter = insert(value_type(micro_containers::traits::move(key),
                                      T())).first;
        return (*iter).second;
    }

    // Modifiers
    void clear() noexcept {
        const auto bucket_count = _bucket_count;
        // destroy nodes
        for (int ix = 0; ix < bucket_count; ++ix) {
            auto & bucket = _buckets[ix];
            while(bucket.list) {
                // remove head node from old list
                node_t * removed_node = bucket.list;
                bucket.list = removed_node->next;
                if(bucket.list) bucket.list->prev = nullptr;
                removed_node->prev=removed_node->next=nullptr;
                // destroy and deallocate
                removed_node->~node_t();
                _alloc_node.deallocate(removed_node);
            }
        }
        // destroy buckets and deallocate
        for (int ix = 0; ix < bucket_count; ++ix)
            _buckets[ix].~bucket_t(); // this is useless, trivial destructor
        _alloc_bucket.deallocate(_buckets);
        // reset values
        _buckets=nullptr; _bucket_count=_size=0;
    }

    pair<iterator, bool> insert(const value_type& value) {
        {
            iterator iter = find(value.first);
            if (iter!=end()) return pair<iterator, bool>(iter, false);
        }
        auto * node = _alloc_node.allocate(1);
        ::new (node) node_t(value); _size+=1;
        node_query q = internal_insert_node(node);
        return pair<iterator, bool>(iterator(q.node, q.bucket_index, this), true);
    }
    pair<iterator, bool> insert(value_type && value) {
        {
            iterator iter = find(value.first);
            if (iter!=end()) return pair<iterator, bool>(iter, false);
        }
        auto * node = _alloc_node.allocate(1);
        ::new (node) node_t(micro_containers::traits::move(value)); _size+=1;
        node_query q = internal_insert_node(node);
        return pair<iterator, bool>(iterator(q.node, q.bucket_index, this), true);
    }
private:
    template<class A, class B>
    using match_t = micro_containers::traits::enable_if_t<
            micro_containers::traits::is_same<A,
                    micro_containers::traits::remove_reference_t<B>>::value, bool>;
    template<class A, class B>
    using non_match_t = micro_containers::traits::enable_if_t<
            !micro_containers::traits::is_same<A,
                    micro_containers::traits::remove_reference_t<B>>::value, bool>;
public:
    template<class InputIt, typename Non_Key = non_match_t<Key, InputIt>>
    void insert(InputIt first, InputIt last) {
        InputIt current(first);
        while(current!=last) { insert(*current); ++current; }
    }
    template<class KK, class TT, typename AA = match_t<KK, Key>, typename BB = match_t<TT, T>>
    pair<iterator, bool> insert(KK && key, TT && value) {
        return insert(value_type(micro_containers::traits::forward<KK>(key),
                   micro_containers::traits::forward<TT>(value)));
    }
    iterator internal_erase(const Key & key) {
        auto iter = find(key);
        const bool found = iter!=end();
        if(!found) return end();
        auto iter_next = iter+1;
        // re-wire
        auto * node = const_cast<node_t *>(iter._n);
        auto bi = iter._bi;
        auto & bucket = _buckets[bi];
        // node is head
        if(node == bucket.head()) {
            bucket.list = node->next;
            if(bucket.list) bucket.list->prev = nullptr;
        } else {
            node->prev->next = node->next;
            if(node->next) // in case it is not tail
                node->next->prev = node->prev;
        }
        node->prev=node->next=nullptr;

        // destroy and deallocate
        node->~node_t();
        _alloc_node.deallocate(node);
        _size-=1;
        return iter_next;
    }

    size_type erase(const Key& key) {
        auto iter_next = internal_erase(key);
        return iter_next==end() ? 0 : 1;
    }
    iterator erase(iterator pos) { return iterator(internal_erase((*pos).first)); }
    iterator erase(const_iterator pos) { return iterator(internal_erase((*pos).first)); }
    iterator erase(const_iterator first, const_iterator last) {
        const_iterator current(first);
        while (current!=last) current=erase(current);
        return current;
    }
};