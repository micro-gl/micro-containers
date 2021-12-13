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
     * Interface for key comparison
     */
    template<class Key>
    struct avl_less {
        bool operator()(const Key &lhs, const Key &rhs) const { return lhs < rhs; }
    };

    /**
     * This function extracts reference to the key of a stored item
     */
    template<class Item, class Key>
    struct avl_key_extract {
        using key = Key;
        using item = Item;
        const Key & operator()(const Item &whole) const { return whole; }
    };

    /**
     * AVL Tree balanced tree, logarithmic complexity everything.
     * Notes:
     * - Most algorithms use recursion, so there is a stack memory cost of O(log(n))
     * - This class is Allocator-Aware
     * - We give user an option to break down and define what is a key, as usually the stored item is the key itself,
     *   BUT, many data structure only use a key which is a partial data of the stored item such as pairs of (key, value).
     *   In this case, query/find/erase induce a cost of temporarily creating the value in the pair which is undesirable,
     *   therefore, I give an option to use keys of different types then stored item, as long as they can be computed, or
     *   referenced from the stored type as well.
     *   KeyExtractFunction type is a function struct, that given a stored type, can compute a reference to a comparable key
     * @tparam StoreItemType The item type, that the tree stores
     * @tparam Key The key type, that is used to compare StoreItemType, usually a sub-type of StoreItemType or StoreItemType itself
     * @tparam Compare compare structure to compare keys less than binary relation (key_1 < key_2)
     * @tparam KeyExtractFunction (Optional) Extract a key from the stored item object, by default the StoreItemType is the key
     * @tparam Allocator allocator type
     */
    template<class StoreItemType,
             class Key=StoreItemType,
             class Compare=avl_less<Key>,
             class KeyExtractFunction=avl_key_extract<StoreItemType, Key>,
             class Allocator=microc::std_allocator<char>>
    class avl_tree {
    public:
        using store_item_type = StoreItemType;
        using key_type = Key;
        using compare_function = Compare;
        using key_extract_function = KeyExtractFunction;
        using allocator_type = Allocator;
        using size_type = microc::size_t;

    private:
        struct node_t {
            StoreItemType item;
            int height;
            node_t *left, *right;

            explicit node_t(const StoreItemType &k) : item(k), left(nullptr),
                                             right(nullptr), height(-1) {}

            explicit node_t(StoreItemType &&k) : item(microc::traits::move(k)), left(nullptr),
                                        right(nullptr), height(-1) {}
        };

        template<class value_reference_type>
        struct iterator_t {
            const node_t *_n; // node, nullptr node is end signal
            const avl_tree *_t; // tree

            node_t *ncn(const node_t *node) const { return const_cast<node_t *>(node); }

            template<class value_reference_type_t>
            iterator_t(const iterator_t<value_reference_type_t> &o) : iterator_t(o._n, o._t) {}

            explicit iterator_t(const node_t *n, const avl_tree *t) : _n(n), _t(t) {}

            iterator_t &operator++() {
                _n = _t->successor(_n);
                return *this;
            }
            iterator_t &operator--() {
                _n = _n ? _t->predecessor(_n) : _t->maximum_node(_t->root());
                return *this;
            }
            iterator_t operator++(int) {
                iterator_t ret(_n, _t);
                ++(*this);
                return ret;
            }
            iterator_t operator--(int) {
                iterator_t ret(_n, _t);
                --(*this);
                return ret;
            }
            bool operator==(iterator_t o) const { return _n == o._n; }
            bool operator!=(iterator_t o) const { return !(*this == o); }
            value_reference_type operator*() const { return (*ncn(_n)).item; }
        };

    public:
        using node_type = node_t;
        using iterator = iterator_t<StoreItemType &>;
        using const_iterator = iterator_t<const StoreItemType &>;
        using rebind_alloc = typename Allocator::template rebind<node_type>::other;
        using insert_result = struct insert_result_t {
            const_iterator first; bool second;
            insert_result_t(const const_iterator &a, bool b) : first(a), second(b) {}
        };
        static constexpr unsigned long node_type_size = sizeof(node_type);

        // iterators
        iterator begin() noexcept { return iterator{minimum()}; }
        const_iterator begin() const noexcept { return minimum(); }
        const_iterator cbegin() const noexcept { return begin(); }
        iterator end() noexcept { return iterator{nullptr, this}; }
        const_iterator end() const noexcept { return const_iterator{nullptr, this}; }
        const_iterator cend() const noexcept { return end(); }
        const key_type & extract_key(const StoreItemType & item) const
        { return _partial(item); }

    private:
        Compare _compare;
        key_extract_function _partial;
        node_t *_root;
        rebind_alloc _alloc;
        size_type _size;

    public:
        avl_tree(const Compare &comp,
                 const Allocator &allocator = Allocator()) :
                _partial(), _compare(comp), _alloc(allocator), _root(nullptr), _size(0) {};
        avl_tree(const Allocator &allocator = Allocator()) :
                avl_tree(Compare(), allocator) {};
        avl_tree(const avl_tree &other, const Allocator &allocator) :
                avl_tree(allocator) {
            for (const auto &key: other) insert(key);
        }
        avl_tree(const avl_tree &other) : avl_tree(other, other.get_allocator()) {}
        avl_tree(avl_tree &&other, const Allocator &allocator) :
                avl_tree(allocator) {
            const bool are_equal_allocators = _alloc == allocator;
            if (are_equal_allocators) {
                _root = other._root;
                _size = other._size;
                other._root = nullptr;
                other._size = 0;
            } else {
                for (auto &key: other)
                    insert(microc::traits::move(key));
                other.clear();
            }
        }

        avl_tree(avl_tree &&other) noexcept:
                avl_tree(microc::traits::move(other), other.get_allocator()) {}

        ~avl_tree() { clear(); }

        avl_tree &operator=(const avl_tree &other) {
            if (this != &(other)) {
                clear();
                for (const auto &key: other) insert(key);
            }
            return *this;
        }
        avl_tree &operator=(avl_tree &&other) noexcept {
            if (this != &(other)) {
                clear();
                const bool are_equal_allocators = _alloc == other.get_allocator();
                if (are_equal_allocators) {
                    _root = other._root;
                    _size = other._size;
                    other._root = nullptr;
                    other._size = 0;
                } else {
                    for (auto &key: other)
                        insert(microc::traits::move(key));
                    other.clear();
                }
            }
            return *this;
        }

        Allocator get_allocator() const { return Allocator(_alloc); }
        const Compare &get_comparator() const { return _compare; }
        const node_t *root() const { return _root; }
        node_t *root() { return _root; }
        bool empty() const { return root() == nullptr; }
        size_type size() const { return _size; }
        const_iterator find(const StoreItemType &k) const { return const_iterator(find_node(root(), extract_key(k)), this); }
        const_iterator find_by_key(const key_type &k) const { return const_iterator(find_node_by_key(root(), k), this); }
        bool contains(const StoreItemType &k) const { return internal_contains(root(), extract_key(k)); }
        bool contains_by_key(const key_type &k) const { return internal_contains_by_key(root(), k); }

        const_iterator findLowerBoundOf(const StoreItemType &key) const {
            const node_t *root = root();
            node_t *candidate = nullptr;
            while (root != nullptr) {
                bool has_left = root->left != nullptr;
                bool has_right = root->right != nullptr;
                // if _root item is before me, then he might be a candidate,
                // let's store it, and try to get better
                if (!isPreceding(key, root->item)) {
                    candidate = root;
                    // let's try to get a bigger lower bound
                    if (has_right) root = root->right;
                    else break;
                } else {
                    if (has_left) root = root->left;
                    else break;
                }
            }
            return const_iterator(candidate, this);
        }
        const_iterator findUpperBoundOf(const StoreItemType &key) const {
            const node_t *root = root();
            node_t *candidate = nullptr;
            while (root != nullptr) {
                bool has_left = root->left != nullptr;
                bool has_right = root->right != nullptr;
                // if _root item is before me, then he might be a candidate,
                // let's store it, and try to get better
                if (isPreceding(key, root->item)) {
                    candidate = root;
                    // let's try to get a bigger lower bound
                    if (has_left) root = root->left;
                    else break;
                } else {
                    if (has_right) root = root->right;
                    else break;
                }
            }
            return const_iterator(candidate, this);
        }

        const_iterator minimum() const { return const_iterator(minimum_node(root()), this); }
        const_iterator maximum() const { return const_iterator(maximum_node(root()), this); }

        void clear() {
            while (!empty()) { _root = remove_node_by_key(root(), nullptr, extract_key(root()->item)); }
            _size = 0;
        }

        // inserts, return the new root
        // todo: make it proper for move semantics with template, that constructs
        insert_result insert(const StoreItemType &k) {
            node_t *new_node = nullptr;
            bool has_succeeded = false;
            _root = insert_node(root(), k, &new_node, has_succeeded, false);
            return insert_result(const_iterator(new_node, this), has_succeeded);
        }
        insert_result insert(StoreItemType &&item) {
            node_t *new_node = nullptr;
            bool has_succeeded = false;
            _root = insert_node(root(), item,
                                &new_node, has_succeeded, true);
            return insert_result(const_iterator(new_node, this), has_succeeded);
        }
        template<class... Args>
        insert_result insert_emplace(Args &&... args) {
            node_t *new_node = nullptr;
            bool has_succeeded = false;
            _root = insert_node(root(), StoreItemType(microc::traits::forward<Args>(args)...),
                                &new_node, has_succeeded, true);
            return insert_result(const_iterator(new_node, this), has_succeeded);
        }

        // returns the new root
        const_iterator remove(const StoreItemType &item) {
            return remove_by_key(extract_key(item));
        }
        const_iterator remove_by_key(const key_type &k) {
            // todo: take advantage of find node result to make remove_node_by_partial_key faster
            const node_t *node = find_node_by_key(root(), k);
            const bool has_found_node = node != nullptr;
            const node_t *next_node = nullptr;
            if (has_found_node) {
                next_node = successor(node, root());
                _root = remove_node_by_key(root(), nullptr, k);
                _size -= 1;
            }
            return const_iterator(next_node, this);
        }

        // _compare keys
        bool isPreceding(const key_type &lhs, const key_type &rhs) const { return _compare(lhs, rhs); }
        bool isPrecedingOrEqual(const key_type &lhs, const key_type &rhs) const
        { return _compare(lhs, rhs) || isEqual(lhs, rhs); }
        bool isSucceeding(const key_type &lhs, const key_type &rhs) const { return _compare(rhs, lhs); }
        bool isEqual(const key_type &lhs, const key_type &rhs) const { return !_compare(lhs, rhs) && !_compare(rhs, lhs); }

    private:
        const node_t *find_node_by_key(const node_t *root, const key_type &k) const {
            const node_t *iter = root;
            while (iter != nullptr) {
                if (isEqual(k, extract_key(iter->item))) return iter;
                iter = isPreceding(k, extract_key(iter->item)) ? iter->left : iter->right;
            }
            return nullptr;
        }
        const node_t *find_node(const node_t *root, const StoreItemType &item) const {
            return find_node_by_key(root, extract_key(item));
        }
        bool internal_contains(const node_t *root, const StoreItemType &k) const { return internal_contains_by_key(root, extract_key(k)); }
        bool internal_contains_by_key(const node_t *root, const key_type &k) const { return find_node_by_key(root, k) != nullptr; }
        const node_t *successor(const node_t *node) const { return successor(node, root()); }
        const node_t *successor(const node_t *node, const node_t *root) const {
            if (node == nullptr) return nullptr;
            if (node->right) return minimum_node(node->right);
            const node_t *succ = nullptr;
            // Start from root and search for successor down the tree
            while (root) {
                if (isPreceding(extract_key(node->item), extract_key(root->item))) {
                    succ = root;
                    root = root->left;
                } else if (isPreceding(extract_key(root->item), extract_key(node->item)))
                    root = root->right;
                else break;
            }
            return succ;
        }
        const node_t *predecessor(const node_t *node) const { return predecessor(node, root()); }
        const node_t *predecessor(const node_t *node, const node_t *root) const {
            // find predecessor of node in root tree
            if (node == nullptr) return nullptr;
            if (node->left) return maximum_node(node->left);
            const node_t *pred = nullptr;
            // Start from root and search for predecessor down the tree
            while (root) {
                if (isPreceding(node->item, root->item)) {
                    root = root->left;
                } else if (isPreceding(root->item, node->item)) {
                    pred = root;
                    root = root->right;
                } else break;
            }
            return pred;
        }
        static const node_t *minimum_node(const node_t *node) {
            const node_t *current = node;
            while (current && current->left) current = current->left;
            return current;
        }
        struct pair_node { node_t *first, *second; };
        static pair_node minimum_node_with_parent(node_t *node, node_t *parent) {
            node_t *current = node;
            pair_node result;
            result.first = node;
            result.second = parent;
            while (current && current->left) {
                result.second = current;
                current = current->left;
                result.first = current;
            }
            return result;
        }
        static const node_t *maximum_node(const node_t *node) {
            const node_t *iter = node;
            while (iter && iter->right) iter = iter->right;
            return iter;
        }
        /**
         * Insert a item
         * @param root Tree root
         * @param item item
         * @param new_node New node or existing if item is already present
         * @param has_succeeded True if new node created. False, otherwise;
         * @return The new root of the tree
         */
        node_t *insert_node(node_t *root, const StoreItemType &item, // root is a sub tree root
                            node_t **new_node, bool &has_succeeded, const bool move_ctor) {
            if (root == nullptr) {
                auto *mem = _alloc.allocate(1);
                if (move_ctor) ::new(mem, microc_new::blah) node_t(microc::traits::move(const_cast<StoreItemType &>(item)));
                else ::new(mem, microc_new::blah) node_t(item);
                has_succeeded = true;
                *new_node = mem;
                _size += 1;
                return mem;
            } else if (isPreceding(extract_key(item), extract_key(root->item))) {
                root->left = insert_node(root->left, item, new_node, has_succeeded, move_ctor);
            } else if (isSucceeding(extract_key(item), extract_key(root->item))) {
                root->right = insert_node(root->right, item, new_node, has_succeeded, move_ctor);
            } else {
                has_succeeded = false;
                *new_node = root;
                return root;
            } // duplicate keys
            return re_balance(root);
        }

        pair_node swap_nodes(node_t *a, node_t *a_parent,
                             node_t *b, node_t *b_parent) {
            bool a_has_parent = a_parent != nullptr;
            bool b_has_parent = b_parent != nullptr;
            bool a_on_left = a_has_parent && a_parent->left == a;
            bool b_on_left = b_has_parent && b_parent->left == b;
            bool a_is_left_child_of_b = b && a && b->left == a;
            bool a_is_right_child_of_b = b && a && b->right == a;
            bool b_is_left_child_of_a = b && a && a->left == b;
            bool b_is_right_child_of_a = b && a && a->right == b;
            node_t *temp_left = a->left, *temp_right = a->right;
            node_t *temp_p = a;
            a->left = b->left;
            a->right = b->right;
            b->left = temp_left;
            b->right = temp_right;
            a = b;
            b = temp_p;
            if (a_has_parent) {
                if (a_is_left_child_of_b) b->left = a;
                else if (a_is_right_child_of_b) b->right = a;
                else if (a_on_left) a_parent->left = a;
                else a_parent->right = a;
            }
            if (b_has_parent) {
                if (b_is_left_child_of_a) a->left = b;
                else if (b_is_right_child_of_a) a->right = b;
                else if (b_on_left) b_parent->left = b;
                else b_parent->right = b;
            }
            pair_node result;
            result.first = a;
            result.second = b;
            return result;
        }

        /**
         * Remove a root
         * @param root Tree root
         * @param k item
         * @param next_node Next root after removal
         * @return The new root
         */
        node_t *remove_node(node_t *root, node_t *root_parent, const StoreItemType &k) {
            return remove_node_by_key(root, root_parent, extract_key(k));
        }
        node_t *remove_node_by_key(node_t *root, node_t *root_parent, const key_type &k) {
            if (root == nullptr) return nullptr;
            else if (isPreceding(k, extract_key(root->item))) {
                root->left = remove_node_by_key(root->left, root, k);
            } else if (isPreceding(extract_key(root->item), k)) {
                root->right = remove_node_by_key(root->right, root, k);
            } else {
                if (root->left == nullptr || root->right == nullptr) {
                    //  if is leaf
                    auto *node_to_remove = root;
                    root = (root->left) ? root->left : root->right;
                    node_to_remove->~node_t();
                    _alloc.deallocate(node_to_remove);
                } else {
                    //  replace with successor = left most in right tree, and then remove_node_by_partial_key successor
                    pair_node min = minimum_node_with_parent(const_cast<node_t *>(root->right),
                                                             const_cast<node_t *>(root));
                    auto *successor = min.first;
                    auto *og_successor_parent = min.second;
                    auto result = swap_nodes(root, root_parent, successor, og_successor_parent);
                    root = result.first;
                    root->right = remove_node_by_key(root->right, root, k);
                }
            }
            if (root) root = re_balance(root);
            return root;
        }

        int height_of_node(node_t *node) const { return node ? node->height : -1; }
        int balance_factor(node_t *node) const {
            return node == nullptr ? 0 : (height_of_node(node->right) - height_of_node(node->left));
        }
        void fix_height(node_t *node) {
            auto hl = height_of_node(node->left);
            auto hr = height_of_node(node->right);
            node->height = (hl > hr ? hl : hr) + 1;
        }

        /**
         * Right rotation
         *          Y                   X
         *         / \                 / \
         *        X  (**)      ===>  (*)  Y
         *       / \                     / \
         *     (*)  Z                   Z  (**)
         */
        node_t *rotate_right(node_t *y) {
            node_t *x = y->left;
            node_t *z = x->right;
            x->right = y;
            y->left = z;
            fix_height(y);
            fix_height(x);
            return x;
        }

        /**
         * Left rotation
         *          Y                   X
         *         / \                 / \
         *       (*)  X      ===>     Y  (**)
         *           / \             / \
         *          Z  (**)        (*)  Z
         */
        node_t *rotate_left(node_t *y) {
            node_t *x = y->right;
            node_t *z = x->left;
            x->left = y;
            y->right = z;
            fix_height(y);
            fix_height(x);
            return x;
        }

        node_t *re_balance(node_t *node) { // balancing the node node_t
            fix_height(node);
            auto balance = balance_factor(node);
            if (balance > 1) {
                if (height_of_node(node->right->right) > height_of_node(node->right->left))
                    node = rotate_left(node);
                else {
                    node->right = rotate_right(node->right);
                    node = rotate_left(node);
                }
            } else if (balance < -1) {
                if (height_of_node(node->left->left) > height_of_node(node->left->right))
                    node = rotate_right(node);
                else {
                    node->left = rotate_left(node->left);
                    node = rotate_right(node);
                }
            }
            return node; // balancing is not required
        }
    };

    template<class StoreItemType, class Key, class Compare,
            class KeyExtractFunction, class Allocator>
    bool operator==(const avl_tree<StoreItemType, Key, Compare, KeyExtractFunction, Allocator>& lhs,
                    const avl_tree<StoreItemType, Key, Compare, KeyExtractFunction, Allocator>& rhs ) {
        if(!(lhs.size()==rhs.size())) return false;
        using size_type = typename avl_tree<StoreItemType, Key, Compare, KeyExtractFunction, Allocator>::size_type;
        for (size_type ix = 0; ix < lhs.size(); ++ix)
            if(!(lhs[ix]==rhs[ix])) return false;
        return true;
    }
}