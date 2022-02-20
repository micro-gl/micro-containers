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
     * Array set is an un-ordered associative data structure also known as Hash-Table
     * Notes:
     * - This class is Allocator-Aware
     * - Uses a round-robin linear probing
     * @tparam Key the Key type, that the tree stores
     * @tparam Hash The hash struct/function must implement `size_type operator()(const Key & item) const `
     * @tparam Allocator allocator type
     */
    template<class Key,
             class Hash=microc::hash<Key>,
             class Allocator=microc::std_allocator<char>>
    class array_set_robin {
    public:
        using key_type = Key;
        using value_type = Key;
        using size_type = microc::size_t;
        using hasher = Hash;
        using allocator_type = Allocator;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;

    private:
        static array_set_robin * ncn(const array_set_robin * node)
        { return const_cast<array_set_robin *>(node); }

        template<class value_reference_type>
        struct iterator_t {
            using pointer = typename microc::traits::remove_reference_t<value_reference_type> *;
            const array_set_robin * _c; // container
            size_type _i; // index

            template<class value_reference_type_t>
            iterator_t(const iterator_t<value_reference_type_t> & o) : iterator_t(o._i, o._c) {}
            explicit iterator_t(size_type i, const array_set_robin * c) : _i(i), _c(c) {}
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
            value_reference_type operator*() const { return (*ncn(_c))._keys[_i]; }
            pointer operator->() const { return &(*ncn(_c))._keys[_i]; }
        };

    public:
        using iterator = iterator_t<reference>;
        using const_iterator = iterator_t<const_reference>;
        using node_allocator = typename Allocator:: template rebind<value_type>::other;
        using stat_allocator = typename Allocator:: template rebind<char>::other;
        static constexpr size_type DEFAULT_BUCKET_COUNT = size_type(1)<<4;

    private:
        static constexpr size_type FREE = 0;
        static constexpr size_type USED = 1;

        inline bool is_free(size_type idx) const { return _stats[idx]==FREE; }
        inline Key & key_of(size_type idx) const { return _keys[idx]; }
        size_type internal_first_used() const {
            return internal_next_used(0);
        }
        size_type internal_next_used(size_type start) const {
            for (size_type ix = start; ix < capacity(); ++ix)
                if(_stats[ix]==USED) return ix;
            return capacity();
        }
        size_type internal_prev_used(size_type start) const {
            for (size_type ix = start+1; ix; --ix)
                if(_stats[ix-1]==USED) return (ix-1);
            return capacity();
        }
        inline size_type k2p(const Key & key) const {
            // when size is power of 2, we can get modulo with
            // bit-wise operation
            return mod(_hasher(key));
        }
        inline size_type mod(size_type idx) const {
            // when size is power of 2, we can get modulo with
            // bit-wise operation
            return ( idx & (_cap-1));
        }

        inline size_type distance_to_home_of(const Key & key, size_type current_home) const {
            // this is to avoid branching due to current home wraping around
            return k2p((current_home - k2p(key)) + _cap);
        }

        size_type internal_pos_of(const Key & key) const {
            auto start = k2p(key);
            const auto cap = capacity();
            for (size_type step = 0; step < cap; ++step) {
                auto pos = mod(step + start); // modulo
                const auto _is_free = is_free(pos);
                if (_is_free) return cap; // important that this is first
                if (key_of(pos) == key) return pos; // found the item with high probability
                if (distance_to_home_of(key, pos) < step) {
                    // early stop detection, we found a non-free, that was closer to home,
                    return cap;
                }
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
        node_allocator _alloc_keys;
        stat_allocator _alloc_status;
        // data
        value_type * _keys;
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
            rehash(new_cap);
        }

        bool requires_rehash() const { return load_factor()>max_load_factor(); }

    private:
        void internal_rehash(size_type new_cap) {
            if(new_cap<_size) return;
            // new_cap is power of 2
            const size_type old_cap = _cap;
            if(new_cap == old_cap || new_cap == 0) return;
            // allocate and construct new buckets
            auto * new_key_vals = _alloc_keys.allocate(new_cap);
            auto * new_stats = _alloc_status.allocate(new_cap);
            for (size_type ix = 0; ix < new_cap; ++ix)
                new_stats[ix] = 0; // set it free
                // iterate all nodes
            for (size_type ix = 0; ix < old_cap; ++ix) {
                if(is_free(ix)) continue;
                value_type & item_key = key_of(ix);
                // compute hash again and re-assign bucket to new bucket
                const auto hash = _hasher(item_key);
                size_type new_idx = hash & (new_cap-1);
                // move construct old item_key
                ::new(new_key_vals + new_idx, microc_new::blah)
                                value_type (microc::traits::move(item_key));
                new_stats[new_idx] = 1; // occupied

            }
            // items were moved, we only need to de-allocate old things
            if(_keys) _alloc_keys.deallocate(_keys);
            if(_stats) _alloc_status.deallocate(_stats);
            // assign new bucket info
            _keys = new_key_vals;
            _stats = new_stats;
            _cap = new_cap;
        }

    public:
        void rehash(size_type suggested_cap) {
            internal_rehash(pow2_upper(suggested_cap));
        }

        array_set_robin(size_type initial_capacity,
                 const Hash& hash = Hash(),
                 const Allocator& allocator = Allocator()) :
                _max_load_factor(.5f), _cap(0), _size(0),
                _hasher(hash), _alloc_keys(allocator), _alloc_status(allocator),
                _keys(nullptr), _stats(nullptr) {
            rehash(initial_capacity);
        }
        array_set_robin() : array_set_robin(DEFAULT_BUCKET_COUNT, Hash(), Allocator()) {}
        explicit array_set_robin(const Allocator& alloc) : array_set_robin(DEFAULT_BUCKET_COUNT, Hash(), alloc) {};
        array_set_robin(size_type initial_capacity, const Allocator& alloc) : array_set_robin(initial_capacity, Hash(), alloc) {}

        template<class InputIt>
        array_set_robin(InputIt first, InputIt last, size_type initial_capacity,
                 const Hash& hash = Hash(), const Allocator& alloc = Allocator() )
                 : array_set_robin(initial_capacity, hash, alloc) {
            InputIt current(first);
            while(current!=last) { insert(*current); ++current; }
        }
        template< class InputIt >
        array_set_robin(InputIt first, InputIt last, size_type initial_capacity,
                 const Allocator& alloc = Allocator() )
                 : array_set_robin(first, last, initial_capacity, Hash(), alloc) {}

        array_set_robin(const array_set_robin & other, const Allocator & allocator) :
                    array_set_robin(0, other._hasher, other.get_allocator()) {
            internal_rehash(other.capacity());
            for (size_type ix = 0; ix < capacity(); ++ix) {
                ::new (_keys + ix, microc_new::blah) value_type(other._keys[ix]);
                _stats[ix] = other._stats[ix];
            }
            _size = other._size;
        }
        array_set_robin(const array_set_robin & other) : array_set_robin(other, other.get_allocator()) {}

        array_set_robin(array_set_robin && other, const Allocator & allocator) :
                    array_set_robin(size_type(0), other._hasher, other.get_allocator()) {
            // using 0 to mute main constructor table creation
            const bool are_equal_allocators = _alloc_keys == allocator;
            _max_load_factor = other._max_load_factor;
            if(are_equal_allocators) {
                _cap=other._cap;
                _size = other._size;
                _keys = other._keys;
                _stats = other._stats;
                other._size=0;
                other._cap=0;
                other._keys= nullptr;
                other._stats= nullptr;
                other.shutdown();
            } else {
                internal_rehash(other._cap); // reserves a table
                for (size_type ix = 0; ix < capacity(); ++ix) {
                    ::new (_keys + ix, microc_new::blah)
                                value_type(microc::traits::move(other._keys[ix]));
                    _stats[ix] = other._stats[ix];
                }
                _size = other._size;
                other.shutdown();
            }
        }
        array_set_robin(array_set_robin && other) noexcept :
                array_set_robin(microc::traits::move(other), other.get_allocator()) {}
        ~array_set_robin() { shutdown(); }

        array_set_robin & operator=(const array_set_robin & other) {
            if(this==&other) return *this;
            _max_load_factor = other.max_load_factor();
            clear();
            internal_rehash(other.capacity());
            for (size_type ix = 0; ix < capacity(); ++ix) {
                ::new (_keys + ix, microc_new::blah) value_type(other._keys[ix]);
                _stats[ix] = other._stats[ix];
            }
            _size = other._size;
            return *this;
        }
        array_set_robin & operator=(array_set_robin && other) noexcept {
            if(this==&(other)) return *this;
            const bool are_equal_allocators = _alloc_keys == other.get_allocator();
            _max_load_factor = other._max_load_factor;
            if(are_equal_allocators) {
                shutdown();
                _cap=other._cap;
                _size = other._size;
                _keys = other._keys;
                _stats = other._stats;
                other._size=0;
                other._cap=0;
                other._keys= nullptr;
                other._stats= nullptr;
            } else {
                clear();
                rehash(other.capacity()); // reserves a table
                for (size_type ix = 0; ix < capacity(); ++ix) {
                    ::new (_keys + ix, microc_new::blah)
                            value_type(microc::traits::move(other._keys[ix]));
                    _stats[ix] = other._stats[ix];
                }
                _size = other._size;
                other.shutdown();
            }
            return *this;
        }

        Allocator get_allocator() const { return Allocator(_alloc_keys); }
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

        // Modifiers
        void shutdown() {
            clear();
            if(_keys) _alloc_keys.deallocate(_keys);
            if(_stats) _alloc_status.deallocate(_stats);
            // reset values
            _keys=nullptr; _stats= nullptr; _cap=0;
        }
        void clear() noexcept {
            // destroy items
            for (size_type ix = 0; ix < capacity(); ++ix) {
                _keys[ix].~value_type();
                _stats[ix] = FREE;
            }
            _size=0;
        }

    private:
        template<class VV>
        size_type internal_insert(VV && key) {
            if(requires_rehash()) {
                rehash(_cap<<1);
            }
            auto start = k2p(key);
            size_type base_dist=0;
            // first iterations to find a spot
            for (size_type step = 0; step < capacity(); ++step) {
                const auto pos = mod(start + step); // modulo
                if (is_free(pos)) { // free item_key, let's conquer
                    // put the item_key
                    ::new(_keys + pos, microc_new::blah) value_type(microc::traits::forward<VV>(key));
                    _stats[pos] = USED;
                    ++_size;
                    return pos;
                }
                auto & item_key = key_of(pos);
                if (item_key == key) { // found, let's return value
                    // item_key = microc::traits::forward<VV>(k);
                    return pos;
                }
                base_dist = distance_to_home_of(item_key, pos);
                if (base_dist < step) { // let's robin hood
                    // swap
                    value_type next_displaced_item = microc::traits::move(item_key);
                    item_key = microc::traits::forward<VV>(key);
                    start = pos;
                    ++_size;

                    // start displace procedure
                    bool has_pending_displace=true;
                    while(has_pending_displace) {
                        has_pending_displace=false;
                        for (size_type step2 = 1; step2 < capacity(); ++step2) {
                            const auto pos2 = mod(start + step2); // modulo
                            value_type & item2_key = key_of(pos2);
                            if (is_free(pos2)) { // free item_key, let's conquer
                                // now item_key was copied, he has linked-list info but his
                                // pred/succ do not point to him, so let's fix it
                                item2_key = microc::traits::move(next_displaced_item);
                                _stats[pos2] = USED;
                                return pos;
                            }
                            size_type item_dist = distance_to_home_of(item2_key, pos2);
                            if (item_dist < base_dist+step2) { // let's robin hood
                                // before all, current displaced item_key might have wanted to move
                                // to one of its siblings(prev/next), so make a copy and update them.
                                auto temp = microc::traits::move(next_displaced_item);
                                ::new(&next_displaced_item) value_type(microc::traits::move(item2_key));
                                item2_key = microc::traits::move(temp);
                                // prepare for next iteration
                                base_dist = item_dist;
                                start = pos2;
                                has_pending_displace=true;
                                break;
                            }
                        }
                    }
                    //

                    break;
                }
            }
            // now displacements, this is not in the above loop because lru cache
            // requires some mods. NONE of the displaced items can be heads.
            return capacity(); // this shouldnt happen
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

    private:
        size_type internal_erase_by_key(const Key & key) {
            auto start = internal_pos_of(key);
            return internal_erase_by_index(start);
        }
        size_type internal_erase_by_index(size_type start) {
            // returns pos of deleted index, or capacity()
            const auto cap = capacity();
            if(start==cap) return cap;
            //
            (_keys + start)->~value_type(); // destruct
            _stats[start] = FREE; // free
            --_size;
            // begin back shifting procedure
            for (size_type step = 1; step < cap; ++step) {
                auto pos = mod(start + step); // modulo
                // we are done when the item_key in question is free or it's distance
                // from home is 0
                if(is_free(pos)) return start;
                value_type & item_key = key_of(pos);
                if(distance_to_home_of(item_key, pos) == 0) return start;
                // other-wise, we need to move it left because it's left sibling is empty
                const auto pos_predecessor = mod(start + step - 1); // modulo
                // move-construct the item_key to predecessor place, which is free and destructed
                ::new (_keys + pos_predecessor) value_type(microc::traits::move(item_key));
                // we don't need to destruct the moved item_key because it was moved, but
                // we need to update the stats
                _stats[pos_predecessor] = USED;
                _stats[pos] = FREE;
            }
            return start;
        }

        iterator internal_erase_by_key_return_iterator(const Key & key) {
            size_type pos = internal_erase_by_key(key);
            return iterator(internal_next_used(pos), this);
        }
    public:

        size_type erase(const Key& key) {
            auto pos = internal_erase_by_key(key);
            return pos==capacity() ? 0 : 1;
        }
        iterator erase(iterator pos) { return internal_erase_by_key_return_iterator(*pos); }
        iterator erase(const_iterator pos) { return internal_erase_by_key_return_iterator(*pos); }
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
    void print(char order=0, size_type how_many=-1) const {
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
                    std::cout << "{ k: " << std::to_string(item) << " },\n";
                }
            }
            else {
                for (size_type ix = 0; ix < capacity(); ++ix) {
                    if(is_free(ix)) {
                        std::cout << ix << " = FREE, \n";
                    } else {
                        std::cout << ix << " = { k: " << std::to_string(_keys[ix])
                                  << " },\n";
                    }
                }
            }
            std::cout << '\n';
#endif
        }
    };

    template<class Key, class T, class Hash, class Allocator>
    bool operator==(const array_set_robin<Key, Hash, Allocator>& lhs,
                    const array_set_robin<Key, Hash, Allocator>& rhs ) {
        if(!(lhs.size()==rhs.size())) return false;
        using size_type = typename array_set_robin<Key, Hash, Allocator>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix)
            if(!(lhs[ix]==rhs[ix])) return false;
        return true;
    }
}