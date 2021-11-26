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

template<typename Key,
         typename Compare,
         class Allocator=micro_containers::traits::std_allocator<char>>
class avl_tree {
public:
    using key_type = Key;
    using compare_function = Compare;
    using allocator_type = Allocator;

private:
    struct node_t {
        Key key;
        int height;
        node_t *left, *right;
        explicit node_t(const Key &k) : key(k), left(nullptr), right(nullptr),
                height(-1) {}
    };

public:
    using node_type = node_t;
    using rebind_alloc = typename Allocator:: template rebind<node_type>::other;

private:
    Compare compare;
    node_t * _root;
    rebind_alloc _alloc;

public:

    avl_tree(const Allocator & allocator=Allocator()) :
                _root(nullptr), compare(Compare()), _alloc(allocator) {};
    avl_tree(const avl_tree & other, const Allocator & allocator) :
            avl_tree(allocator) {
        // todo
    }
    avl_tree(const avl_tree & other) : avl_tree(other, other.get_allocator()) {}
    avl_tree(avl_tree && other) : avl_tree(other.get_allocator()) {
        // todo
    }
    ~avl_tree() { clear(); }

    avl_tree & operator=(const avl_tree & other) {
        if(this==&(other)) return *this;
        // todo
    }
    avl_tree & operator=(avl_tree && other) noexcept {
        if(this==&(other)) return *this;
        // todo
    }

    Allocator get_allocator() const { return Allocator(_alloc); }
    const Compare & get_comparator() const { return compare; }
    const node_t * getRoot() const { return _root; }
    node_t * getRoot() { return _root; }
    bool empty() const { return getRoot() == nullptr; }
    const node_t * find(const Key &k) const { return find(getRoot(), k); }
    const node_t * find(const node_t * root, const Key &k) const {
        const node_t * iter = root;
        while (iter != nullptr) {
            if (isEqual(k, iter->key)) return iter;
            iter = isPreceding(k, iter->key) ? iter->left : iter->right;
        }
        return nullptr;
    }
    bool contains(const Key &k) const { return contains(getRoot(), k); }
    bool contains(const node_t * root, const Key &k) const { return find(root, k) != nullptr; }
    const node_t * findLowerBoundOf(const Key & key) const {
        const node_t * root = getRoot();
        node_t * candidate = nullptr;
        while(root!= nullptr) {
            bool has_left = root->left!= nullptr;
            bool has_right = root->right!= nullptr;
            // if _root key is before me, then he might be a candidate,
            // let's store it, and try to get better
            if(!isPreceding(key, root->key)) {
                candidate = root;
                // let's try to get a bigger lower bound
                if(has_right) root = root->right;
                else break;
            } else {
                if(has_left) root = root->left;
                else break;
            }
        }
        return candidate;
    }

    const node_t * findUpperBoundOf(const Key & key) const {
        const node_t * root = getRoot();
        node_t * candidate = nullptr;
        while(root!= nullptr) {
            bool has_left = root->left!= nullptr;
            bool has_right = root->right!= nullptr;
            // if _root key is before me, then he might be a candidate,
            // let's store it, and try to get better
            if(isPreceding(key, root->key)) {
                candidate = root;
                // let's try to get a bigger lower bound
                if(has_left) root = root->left;
                else break;
            } else {
                if(has_right) root = root->right;
                else break;
            }
        }
        return candidate;
    }

    const node_t * successor(const node_t * node) const
    { return successor(node, getRoot()); }
    const node_t * successor(const node_t * node, const node_t * root) const {
        // Step 1 of the above algorithm
        if (node->right) return minimum(node->right);
        const node_t * succ = nullptr;
        // Start from root and search for successor down the tree
        while (root) {
            if (isPreceding(node->key, root->key)) {
                succ = root; root = root->left;
            } else if (isPreceding(root->key, node->key))
                root = root->right;
            else break;
        }
        return succ;
    }

    const node_t * predecessor(const node_t * node) const
    { return predecessor(node, getRoot()); }
    const node_t * predecessor(const node_t * node, const node_t * root) const {
        // Step 1 of the above algorithm
        if (node->left) return maximum(node->left);
        const node_t * pred = nullptr;
        // Start from root and search for predecessor down the tree
        while (root) {
            if (isPreceding(node->key, root->key)) {
                root = root->left;
            } else if (isPreceding(root->key, node->key)) {
                pred = root; root = root->right;
            } else break;
        }
        return pred;
    }

    const node_t * minimum() const { return minimum(getRoot()); }
    static const node_t * minimum(const node_t * node) {
        const node_t* current = node;
        while (current->left != nullptr) current = current->left;
        return current;
    }

    const node_t* maximum() const { return maximum(getRoot()); }
    static const node_t* maximum(const node_t * node) {
        const node_t * iter = node;
        while (iter && iter->right) iter = iter->right;
        return iter;
    }

    void clear() { while(!empty()) remove(getRoot()->key); }

    // inserts
    const node_t * insert(const Key &k) { return _root = insert(getRoot(), k); }
    const node_t * insert(Key &&k) {
        return _root = insert(getRoot(), Key(micro_containers::traits::move(k)));
    }
    template<class... Args>
    const node_t * insert_emplace(Args&&... args) {
        return _root = insert(getRoot(), Key(micro_containers::traits::forward<Args>(args)...));
    }

    node_t * insert(node_t * node, const Key & k) {
        if(node==nullptr) {
            auto * mem = _alloc.allocate(1);
            ::new (mem) node_t(k);
            return mem;
        } else if(isPreceding(k, node->key)) {
            node->left = insert(node->left, k);
        } else if(isSucceeding(k, node->key)) {
            node->right = insert(node->right, k);
        } else {
            // duplicate keys
            return node;
        }
        return re_balance(node);
    }

    node_t* remove(const Key &k) { return _root = remove(getRoot(), k); }
    // make this iterative with a stack
    node_t* remove(node_t* node, const Key &k) { // deleting k key from node tree
        if(node==nullptr) return nullptr;
        else if(isPreceding(k, node->key)) {
            node->left = remove(node->left, k);
        } else if(isPreceding(node->key, k)) {
            node->right = remove(node->right, k);
        } else {
            if (node->left == nullptr || node->right == nullptr) {
                //  if is leaf
                node = (node->left == nullptr) ? node->right : node->left;
            } else {
                //  find the successor = left most in right tree
                auto * successor_node = const_cast<node_t *>(minimum(node->right));
                node->key = successor_node->key;
                node->right = remove(node->right, node->key);
            }
        }
        if (node) node = re_balance(node);
        return node;
    }

    // compare keys
    bool isPreceding(const Key &lhs, const Key &rhs) const {
        return compare(lhs, rhs);
    }
    bool isPrecedingOrEqual(const Key &lhs, const Key &rhs) const {
        return compare(lhs, rhs) || isEqual(lhs, rhs);
    }
    bool isSucceeding(const Key &lhs, const Key &rhs) const {
        return compare(rhs, lhs);
    }
    bool isEqual(const Key &lhs, const Key &rhs) const {
        return !compare(lhs, rhs) && !compare(rhs, lhs);
    }

    int height_of_node(node_t* node) const { return node ? node->height : -1; }

private:

    int balance_factor(node_t * node) const {
        return node==nullptr ? 0 : (height_of_node(node->right) - height_of_node(node->left));
    }

    void fix_height(node_t * node) {
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
    node_t * rotate_right(node_t * y) {
        node_t * x = y->left;
        node_t * z = x->right;
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
    node_t * rotate_left(node_t * y) {
        node_t * x = y->right;
        node_t * z = x->left;
        x->left = y;
        y->right = z;
        fix_height(y);
        fix_height(x);
        return x;
    }

    node_t * re_balance(node_t * node) { // balancing the node node_t
        fix_height(node);
        auto balance = balance_factor(node);
        if(balance > 1 )
        {
            if(height_of_node(node->right->right) > height_of_node(node->right->left))
                node = rotate_left(node);
            else {
                node->right = rotate_right(node->right);
                node = rotate_left(node);
            }
        } else if(balance < -1) {
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