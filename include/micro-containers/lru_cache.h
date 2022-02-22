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

#include "bits_linear_probe_lru_pool.h"
#include "bits_robin_lru_pool.h"

namespace microc {
#define LRU_PRINT_SEQ 0
#define LRU_PRINT_ORDER_MRU 1
#define LRU_PRINT_ORDER_FREE_LIST 2
#define LRU_CACHE_ALLOW_PRINT

#ifdef LRU_CACHE_ALLOW_PRINT
#include <iostream>
#endif
    /**
     * LRU Cache and pool for integer values with constrained bits:
     * 1. upto 10 bits per value for 32 bits keys
     * 2. upto 21 bits per value for 64 bits keys
     * This uses a hash table with robin hood probing and in-place linked-list,
     * and is very conservative with memory, which allows CPU caches to load many entries at once.
     *
     * NOTES:
     * - Keys are assumed to be unique (if you are using hash function as a source for keys use a good one)
     * - for 32 bits keys, each entry is 64 bit.
     * - for 64 bits keys, each entry is 128 bit.
     * - The size of the cache is a power of 2 of the bits for value, which gives some optimizations.
     * - perfect for small caches: up to 1024 entries for 32 bit keys and 2,097,152 for 64 bit keys.
     * - perfect for storing integer indices.
     *
     * @tparam size_bits the integer bits size
     * @tparam machine_word the machine word type = short, int or long
     */
    template<class object_type, int size_bits=10,
            class machine_word=long, class Allocator=void>
    class lru_cache {
    public:
        using value_type = object_type;
        using size_type = int;
        using allocator_type = Allocator;
        using val_alloc = typename allocator_type::template rebind<value_type>::other;

    private:
        bits_robin_lru_pool<size_bits, machine_word, Allocator> _pool;
        value_type * _items;
        val_alloc _allocator;

    public:

        lru_cache(float load_factor=0.5f, const allocator_type & allocator = allocator_type()) :
                    _pool(load_factor, allocator) {
        }
        ~lru_cache() {
            _allocator.deallocate(_items);
        }

        allocator_type get_allocator() { return _allocator; }

        constexpr int capacity() const { return _pool.capacity(); }
        int size() const { return _pool.size(); }
        int maxSize() const { return _pool.maxSize(); }
        void print() { _pool.print(); }
        struct query_type {
            value_type * value;
            bool is_active;
        };

        bool has(machine_word key) const { return _pool.has(key); }

        value_type * get(machine_word key) {
            int val = _pool.get(key);
            return val==-1 ? nullptr : (_items + val);
        }

    private:
        template<class VV>
        void internal_put(machine_word key, VV && value) {
            const auto q = _pool.get_or_put(key);
            auto * val_mem = _items + q.value;
            if(q.is_active) { // if active, copy/move assign with forward
                *(val_mem) = microc::traits::forward<VV>(value);
            } else { // if free, copy/move-construct with emplace-new forward
                ::new(val_mem, microc_new::blah) value_type(microc::traits::forward<VV>(value));
            }
        }
    public:

        void put(machine_word key, const value_type & value) {
            internal_put(key, value);
        }
        void put(machine_word key, value_type && value) {
            internal_put(key, microc::traits::move(value));
        }

        bool remove(machine_word key) {
            int val = _pool.remove(key);
            if(val==-1) return true;
            // let's destruct the free item
            (_items + val)->~value_type();
            return true;
        }

        void clear() { _pool.clear(); }

    void print(char order=1, int how_many=-1) const {
#ifdef LRU_CACHE_ALLOW_PRINT
            const bool order_seq = order==LRU_PRINT_SEQ;
            const bool order_mru = order==LRU_PRINT_ORDER_MRU;
            const bool order_free = order==LRU_PRINT_ORDER_FREE_LIST;
            int start = order_seq ? 0 : order_mru ? _mru_list : _free_list;
            int stop = order_seq ? items_count : order_mru ? _mru_list : _free_list;
            const char * str_order = order_seq ? "SEQUENCE" : order_mru ? "MRU" : "FREE";
            std::cout << "\n====== Printing in " << str_order << " order \n"
                      << "- LRU head is " << _mru_list << ", free head is "
                      << _free_list << "\n";
            std::cout << "- MAX SIZE is " << _max_size << ", LOAD FACTOR is " << float(maxSize())/capacity()
                      << "\n";
            std::cout << "- LRU size is " << _mru_size << ", FREE size is "
                      << (capacity() - _mru_size) << ", CAPACITY is " << capacity() << '\n';
            std::cout << "- Printing " << (how_many==-1 ? "All" : std::to_string(how_many)) << " Items \n";
            std::cout << "[\n";
            if(start==-1) return;
            do {
                if(how_many--==0) break;
                const auto item = _items[start];
                const char * head_str = start == _mru_list ? "* " : start == _free_list ? "$ " : "";
                std::cout << head_str << start << " = ( K: " << item.key << ", V: "
                          << item.value() << ", free: " << item.is_free() << ", <-: "
                          << item.prev() << ", ->: " << item.next() << " ),\n";
                start = order_seq ? (start + 1) : item.next();
            } while(start != stop);
            std::cout << "]\n";
#endif
        }

    };

}