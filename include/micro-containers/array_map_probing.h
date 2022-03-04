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
    //#define MICRO_CONTAINERS_ENABLE_THROW
    #ifdef MICRO_CONTAINERS_ENABLE_THROW
    struct throw_hash_map_out_of_range {};
    #endif

    /**
     * Hash-map is an un-ordered associative data structure also known as Hash-Table
     * Notes:
     * - This class is Allocator-Aware
     * @tparam Key the item type, that the tree stores
     * @tparam T The mapped value type of a item
     * @tparam Hash The hash struct/function must implement `size_type operator()(const Key & item) const `
     * @tparam Allocator allocator type
     */
    template<class Key, class T,
             class Hash=microc::hash<Key>,
             class Allocator=microc::std_allocator<char>>
    class array_map_probing {
    public:
        using key_type = Key;
        using mapped_type = T;
        using value_type = pair<Key, T>;
        using size_type = microc::size_t;
        using hasher = Hash;
        using allocator_type = Allocator;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;

    private:
        static array_map_probing * ncn(const array_map_probing * node)
        { return const_cast<array_map_probing *>(node); }

        template<class value_reference_type>
        struct iterator_t {
            using pointer = typename microc::traits::remove_reference_t<value_reference_type> *;
            const array_map_probing * _c; // container
            size_type _i; // bucket index

            template<class value_reference_type_t>
            iterator_t(const iterator_t<value_reference_type_t> & o) : iterator_t(o._i, o._c) {}
            explicit iterator_t(size_type i, const array_map_probing * c) : _i(i), _c(c) {}
            iterator_t& operator++() {
                _i=_c->internal_next_used(_i+1); return *this;
            }
            iterator_t& operator--() {
                _i=_c->internal_prev_used(_i-1); return *this;
            }
            iterator_t operator+(size_type val) {
                size_type pos = _i;
                for (size_type ix = 0; ix < val; ++ix)
                    pos = _c->internal_next_used(pos+1);
                return iterator_t(pos, _c);
            }
            iterator_t operator++(int) { iterator_t ret(_i, _c); ++(*this); return ret; }
            iterator_t operator--(int) { iterator_t ret(_i, _c); --(*this); return ret; }
            bool operator==(iterator_t o) const { return _i==o._i; }
            bool operator!=(iterator_t o) const { return !(*this==o); }
            value_reference_type operator*() const { return (*ncn(_c))._kvs[_i]; }
            pointer operator->() const { return &(*ncn(_c))._kvs[_i]; }
        };

    public:
        using iterator = iterator_t<reference>;
        using const_iterator = iterator_t<const_reference>;
        using node_allocator = typename Allocator:: template rebind<value_type>::other;
        using stat_allocator = typename Allocator:: template rebind<char>::other;
        static constexpr size_type DEFAULT_BUCKET_COUNT = size_type(1)<<4;

    private:
        static constexpr size_type FREE = 0;
        static constexpr size_type TOMBSTONE = 1;
        static constexpr size_type USED = 2;
        inline bool is_free(size_type idx) const { return _stats[idx]==FREE; }
        inline bool is_tombstone(size_type idx) const { return _stats[idx]==TOMBSTONE; }
        inline bool is_used(size_type idx) const { return _stats[idx]==USED; }
        inline Key & key_of(size_type idx) const { return _kvs[idx].first; }
        inline value_type & kv_of(size_type idx) const { return _kvs[idx]; }
        inline value_type & kv_of(size_type idx) { return _kvs[idx]; }
        inline T & value_of(size_type idx) const { return _kvs[idx].second; }
        inline T & value_of(size_type idx) { return _kvs[idx].second; }
        size_type internal_first_used() const { return internal_next_used(0); }
        size_type internal_next_used(size_type start) const {
            for (size_type ix = start; ix < capacity(); ++ix)
                if(is_used(ix)) return ix;
            return capacity();
        }
        size_type internal_prev_used(size_type start) const {
            for (size_type ix = start+1; ix; --ix)
                if(is_used(ix-1)) return (ix-1);
            return capacity();
        }
        inline int k2p(const Key & key) const {
            // when size is power of 2, we can get_or_put modulo with
            // bit-wise operation
            return mod(_hasher(key));
        }
        inline int mod(size_type idx) const {
            // when size is power of 2, we can get_or_put modulo with
            // bit-wise operation
            return ( idx & (_cap-1));
        }

        inline int distance_to_home_of(const Key & key, int current_home) const {
            // this is to avoid branching due to current home wraping around
            return k2p((current_home - k2p(key)) + _cap);
        }

        size_type internal_pos_of(const Key & key) const {
            auto start = k2p(key);
            const auto cap = capacity();
            for (size_type step = 0; step < cap; ++step) {
                auto pos = mod(step + start); // modulo
                const auto stat = _stats[pos];
                const auto _is_tombstone = stat==TOMBSTONE;
                if (_is_tombstone) continue; // important that this is first
                const auto _is_free = stat==FREE;
                if (_is_free) return cap;
                if (key_of(pos) == key) return pos; // found the item
            }
            return cap;
        }

        size_type pow2_upper(size_type val) {
            size_type exp=1;
            for (; exp < val; exp<<=1) {}
            return exp;
        }

        // the minimal buckets count required to keep load factor below max load factor
        size_type minimal_required_cap_for_valid_load_factor() {
            const auto suggested = size_type(0.5f + float(size())/max_load_factor());
            return pow2_upper(suggested);
        }

    public:
        // iterators
        iterator begin() noexcept {
            return iterator(internal_first_used(), this);
        }
        const_iterator begin() const noexcept {
            return const_iterator(internal_first_used(), this);
        }
        const_iterator cbegin() const noexcept { return begin(); }
        iterator end() noexcept { return iterator(capacity(), this); }
        const_iterator end() const noexcept { return const_iterator(capacity(), this); }
        const_iterator cend() const noexcept { return end(); }

    private:
        // buckets
        size_type _cap;
        size_type _size;
        // hash
        hasher _hasher;
        float _max_load_factor;
        // allocators
        node_allocator _alloc_kv;
        stat_allocator _alloc_status;
        // data
        value_type * _kvs;
        char * _stats;

    public:
        // hash policy
        float load_factor() const { return float(size())/capacity(); }
        float max_load_factor() const { return _max_load_factor; }
        void max_load_factor(float ml) {
            _max_load_factor = ml;
            if(load_factor()<max_load_factor()) return;
            // else, let's rehash
            size_type new_cap = minimal_required_cap_for_valid_load_factor();
            internal_rehash(new_cap);
        }

        bool requires_rehash() const { return load_factor()>max_load_factor(); }

    private:
        void internal_rehash(size_type new_cap) {
            if(new_cap<_size) return;
            // new_cap is a power of 2
            const size_type old_cap = _cap;
            if(new_cap == old_cap || new_cap == 0) return;
            // allocate and construct new buckets
            auto * new_key_vals = _alloc_kv.allocate(new_cap);
            auto * new_stats = _alloc_status.allocate(new_cap);
            for (size_type ix = 0; ix < new_cap; ++ix)
                new_stats[ix] = FREE; // set it free
            // iterate all nodes
            for (size_type ix = 0; ix < old_cap; ++ix) {
                const auto stat = _stats[ix];
                if(stat!=USED) continue; // free or tombstone become free

                value_type & item = _kvs[ix];
                // compute hash again and re-assign bucket to new bucket
                const auto hash = _hasher(item.first);
                size_type new_idx = hash & (new_cap-1);
                // move construct old item
                ::new(new_key_vals + new_idx, microc_new::blah)
                                value_type (microc::traits::move(item));
                new_stats[new_idx] = USED; // occupied
            }
            // items were moved, we only need to de-allocate old things
            if(_kvs) _alloc_kv.deallocate(_kvs);
            if(_stats) _alloc_status.deallocate(_stats);
            // assign new bucket info
            _kvs = new_key_vals;
            _stats = new_stats;
            _cap = new_cap;
        }

    public:
        void rehash(size_type suggested_cap) {
            internal_rehash(pow2_upper(suggested_cap));
        }

        array_map_probing(size_type initial_capacity,
                 const Hash& hash = Hash(),
                 const Allocator& allocator = Allocator()) :
                _max_load_factor(.5f), _cap(0), _size(0),
                _hasher(hash), _alloc_kv(allocator), _alloc_status(allocator),
                _kvs(nullptr), _stats(nullptr) {
            rehash(initial_capacity);
        }
        array_map_probing() : array_map_probing(DEFAULT_BUCKET_COUNT, Hash(), Allocator()) {}
        explicit array_map_probing(const Allocator& alloc) : array_map_probing(DEFAULT_BUCKET_COUNT, Hash(), alloc) {};
        array_map_probing(size_type initial_capacity, const Allocator& alloc) : array_map_probing(initial_capacity, Hash(), alloc) {}

        template<class InputIt>
        array_map_probing(InputIt first, InputIt last, size_type initial_capacity,
                 const Hash& hash = Hash(), const Allocator& alloc = Allocator() )
                 : array_map_probing(initial_capacity, hash, alloc) {
            InputIt current(first);
            while(current!=last) { insert(*current); ++current; }
        }
        template< class InputIt >
        array_map_probing(InputIt first, InputIt last, size_type initial_capacity,
                 const Allocator& alloc = Allocator() )
                 : array_map_probing(first, last, initial_capacity, Hash(), alloc) {}

        array_map_probing(const array_map_probing & other, const Allocator & allocator) :
                    array_map_probing(0, other._hasher, other.get_allocator()) {
            internal_rehash(other.capacity());
            for (size_type ix = 0; ix < capacity(); ++ix) {
                ::new (_kvs+ix, microc_new::blah) value_type(other._kvs[ix]);
                _stats[ix] = other._stats[ix];
            }
            _size = other._size;
        }
        array_map_probing(const array_map_probing & other) : array_map_probing(other, other.get_allocator()) {}

        array_map_probing(array_map_probing && other, const Allocator & allocator) :
                    array_map_probing(size_type(0), other._hasher, other.get_allocator()) {
            // using 0 to mute main constructor table creation
            const bool are_equal_allocators = _alloc_kv == allocator;
            _max_load_factor = other._max_load_factor;
            if(are_equal_allocators) {
                _cap=other._cap;
                _size = other._size;
                _kvs = other._kvs;
                _stats = other._stats;
                other._size=0;
                other._cap=0;
                other._kvs= nullptr;
                other._stats= nullptr;
            } else {
                internal_rehash(other._cap); // reserves a table
                for (size_type ix = 0; ix < capacity(); ++ix) {
                    ::new (_kvs+ix, microc_new::blah)
                                value_type(microc::traits::move(other._kvs[ix]));
                    _stats[ix] = other._stats[ix];
                }
                _size = other._size;
                other.shutdown();
            }
        }
        array_map_probing(array_map_probing && other) noexcept :
                array_map_probing(microc::traits::move(other), other.get_allocator()) {}
        ~array_map_probing() { shutdown(); }

        array_map_probing & operator=(const array_map_probing & other) {
            if(this==&other) return *this;
            clear();
            _max_load_factor = other.max_load_factor();
            internal_rehash(other.capacity());
            for (size_type ix = 0; ix < capacity(); ++ix) {
                ::new (_kvs+ix, microc_new::blah) value_type(other._kvs[ix]);
                _stats[ix] = other._stats[ix];
            }
            _size = other._size;
            return *this;
        }
        array_map_probing & operator=(array_map_probing && other) noexcept {
            if(this==&(other)) return *this;
            const bool are_equal_allocators = _alloc_kv == other.get_allocator();
            _max_load_factor = other._max_load_factor;
            if(are_equal_allocators) {
                shutdown();
                _cap=other._cap;
                _size = other._size;
                _kvs = other._kvs;
                _stats = other._stats;
                other._size=0;
                other._cap=0;
                other._kvs= nullptr;
                other._stats= nullptr;
            } else {
                clear();
                internal_rehash(other.capacity()); // reserves a table
                for (size_type ix = 0; ix < capacity(); ++ix) {
                    ::new (_kvs+ix, microc_new::blah)
                            value_type(microc::traits::move(other._kvs[ix]));
                    _stats[ix] = other._stats[ix];
                }
                _size = other._size;
                other.shutdown();
            }
            return *this;
        }

        Allocator get_allocator() const { return Allocator(_alloc_kv); }
        hasher hash_function() const { return _hasher; }

        // capacity
        bool empty() const noexcept { return _size==0; }
        size_type size() const noexcept { return _size; }
        size_type capacity() const noexcept { return _cap; }

        // lookup
        iterator find(const Key& key) {
            return iterator(internal_pos_of(key), this);
        }
        const_iterator find(const Key& key) const {
            return const_iterator(internal_pos_of(key), this);
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
            return iter->second;
        }
        const T& at(const Key& key) const {
            auto iter = find(key);
    #ifdef MICRO_CONTAINERS_ENABLE_THROW
            if(iter==end()) throw throw_hash_map_out_of_range();
    #endif
            return iter->second;
        }
        T & operator[](const Key & key) {
            auto iter = insert(value_type(key, T())).first;
            return iter->second;
        }
        T & operator[](Key && key) {
            auto iter = insert(value_type(microc::traits::move(key),
                                          T())).first;
            return iter->second;
        }

        // Modifiers
        void shutdown() {
            clear();
            if(_kvs) _alloc_kv.deallocate(_kvs);
            if(_stats) _alloc_status.deallocate(_stats);
            // reset values
            _kvs=nullptr; _stats= nullptr; _cap=0;
        }
        void clear() noexcept {
            // destroy items
            for (size_type ix = 0; ix < capacity(); ++ix) {
                _kvs[ix].~value_type();
                _stats[ix] = FREE;
            }
            _size=0;
        }

    private:
        template<class KV>
        size_type internal_insert(KV && kv) {
            if(requires_rehash()) internal_rehash(_cap<<1);
            // with probing we have to first search for the item
            // and then to insert if not found in the first free/tombstone we encountered.
            auto & key = kv.first;
            auto & value = kv.second;
            auto start = k2p(key);
            const auto cap = capacity();
            // first iterations to find if item exists and record where we first saw
            // a free/tombstone place.
            size_type first_free_pos=cap;
            for (size_type step = 0; step < cap; ++step) {
                auto pos = mod(step+start); // modulo
                const auto stat = _stats[pos];
                if (stat==TOMBSTONE) { // tombstone
                    // skip over tombstone but remember
                    if(first_free_pos==cap) first_free_pos = pos;
                    continue; // important that this is first
                }
                if (stat==FREE) { // free place, bail out, item not found
                    if(first_free_pos==cap) first_free_pos = pos;
                    break;
                } // important that this is first
                if (key_of(pos) == key) { // found the item, do nothing and return
                    return pos;
                }
            }
            // error, key not found and no free place to allocate
            if(first_free_pos==cap) return cap;
            // else, let's forward-construct. Free and Tombstones are always destructed or previously moved-abandoned.
            ::new(_kvs + first_free_pos, microc_new::blah) value_type(microc::traits::forward<KV>(kv));
            _stats[first_free_pos] = USED; // mark occupied
            ++_size;
            return first_free_pos;
        }

    public:
        pair<iterator, bool> insert(const value_type& value) {
            size_type pos = internal_insert(value);
            return pair<iterator, bool>(iterator(pos, this), true);
        }
        pair<iterator, bool> insert(value_type && value) {
            size_type pos = internal_insert(microc::traits::move(value));
            return pair<iterator, bool>(iterator(pos, this), true);
        }

    private:
        template<class A, class B>
        using match_t = microc::traits::enable_if_t<
                microc::traits::is_same<A,
                        microc::traits::remove_reference_t<B>>::value, bool>;
        template<class A, class B>
        using non_match_t = microc::traits::enable_if_t<
                !microc::traits::is_same<A,
                        microc::traits::remove_reference_t<B>>::value, bool>;

    public:
        template<class InputIt, typename Non_Key = non_match_t<Key, InputIt>>
        void insert(InputIt first, InputIt last) {
            InputIt current(first);
            while(current!=last) { insert(*current); ++current; }
        }
        template<class KK, class TT, typename AA = match_t<KK, Key>, typename BB = match_t<TT, T>>
        pair<iterator, bool> insert(KK && key, TT && value) {
            return insert(value_type(microc::traits::forward<KK>(key),
                                     microc::traits::forward<TT>(value)));
        }

    private:
        size_type internal_erase(const Key & key) {
            const auto cap = capacity();
            auto pos = internal_pos_of(key);
            if(pos==cap) return cap;
            _kvs[pos].~value_type();
            _stats[pos] = TOMBSTONE;
            return pos;
        }

        iterator internal_erase_return_iterator(const Key & key) {
            size_type pos = internal_erase(key);
            return iterator(internal_next_used(pos), this);
        }
    public:

        size_type erase(const Key& key) {
            auto pos = internal_erase(key);
            return pos==capacity() ? 0 : 1;
        }
        iterator erase(iterator pos) { return internal_erase_return_iterator(pos->first); }
        iterator erase(const_iterator pos) { return internal_erase_return_iterator(pos->first); }
        iterator erase(const_iterator first, const_iterator last) {
            const_iterator current(first);
            while (current!=last and current!=end()) current=erase(current);
            return current;
        }
        template< class... Args >
        pair<iterator, bool> emplace(Args&&... args) {
            return insert(value_type(microc::traits::forward<Args>(args)...));
        }

#define MICROC_PRINT_SEQ 0
#define MICROC_PRINT_USED 1
#define MICROC_ALLOW_PRINT
        void print(char order=0, int how_many=-1) const {
#ifdef MICROC_ALLOW_PRINT
            const char * str_order = order==0 ? "SEQUENCE" : "USED";

            std::cout << "\n- Printing in " << str_order << " order \n";
            std::cout << "- SIZE is " << _size << ", CAPACITY is " << _cap
                      << ", LOAD FACTOR is " << load_factor()
                      << ", MAX LOAD FACTOR is " << max_load_factor() << "\n";
            std::cout << "- Printing " << (how_many==-1 ? "All" : std::to_string(how_many)) << " Items \n";
            if(empty()) {
                std::cout << "- EMPTY !!! \n\n";
                return;
            }

            if(order==MICROC_PRINT_USED) {
                for (const value_type & item : *this) {
                    std::cout << "{ k: " << std::to_string(item.first) << ", v: "
                            << std::to_string(item.second) << " },\n";
                }
            }
            else {
                for (size_type ix = 0; ix < capacity(); ++ix) {
                    if(is_free(ix)) {
                        std::cout << ix << " = FREE, \n";
                    } else if(is_tombstone(ix)) {
                        std::cout << ix << " = TOMBSTONE, \n";
                    } else {
                        std::cout << ix << " = { k: " << std::to_string(_kvs[ix].first)
                        << ", v: " << std::to_string(_kvs[ix].second) << " },\n";
                    }
                }
            }
            std::cout << '\n';
#endif
        }
    };

    template<class Key, class T, class Hash, class Allocator>
    bool operator==(const array_map_probing<Key, T, Hash, Allocator>& lhs,
                    const array_map_probing<Key, T, Hash, Allocator>& rhs ) {
        if(!(lhs.size()==rhs.size())) return false;
        using size_type = typename array_map_probing<Key, T, Hash, Allocator>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix)
            if(!(lhs[ix]==rhs[ix])) return false;
        return true;
    }
}