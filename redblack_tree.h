#ifndef REDBLACK_TREE_H
#define REDBLACK_TREE_H
#include <cstddef>
#include <functional>

namespace gtr {
namespace containers {

template <class T> struct redblack_tree {
    // Node structure: every node stores its value, pointers to its children and parent,
    // and a flag indicating if the node is red.
    struct node {
        T value;
        node *left;
        node *right;
        node *parent;
        bool is_red;
        // Constructor: every new node is initially red with no children.
        node(const T &value) : value(value), left(nullptr), right(nullptr), parent(nullptr), is_red(true) {}
    };

    // The root of the tree.
    node *root;

    // Constructor.
    redblack_tree() : root(nullptr) {}

    // ------------------------------------------------------------
    // Rotation Functions
    // ------------------------------------------------------------

    // Left-rotate around node x.
    void left_rotate(node *x) {
        node *y = x->right;
        x->right = y->left;
        if (y->left)
            y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    // Right-rotate around node x.
    void right_rotate(node *x) {
        node *y = x->left;
        x->left = y->right;
        if (y->right)
            y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;
        y->right = x;
        x->parent = y;
    }

    // ------------------------------------------------------------
    // Insertion Methods
    // ------------------------------------------------------------

    // Fix up the tree after insertion to restore red–black properties.
    void insert_fixup(node *z) {
        while (z != root && z->parent && z->parent->is_red) {
            if (z->parent == z->parent->parent->left) {
                node *y = z->parent->parent->right; // Uncle.
                if (y && y->is_red) {
                    // Case 1: Uncle is red.
                    z->parent->is_red = false;
                    y->is_red = false;
                    z->parent->parent->is_red = true;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        // Case 2: z is a right child.
                        z = z->parent;
                        left_rotate(z);
                    }
                    // Case 3: z is a left child.
                    z->parent->is_red = false;
                    z->parent->parent->is_red = true;
                    right_rotate(z->parent->parent);
                }
            } else {
                // Mirror image of the above.
                node *y = z->parent->parent->left; // Uncle.
                if (y && y->is_red) {
                    // Case 1: Uncle is red.
                    z->parent->is_red = false;
                    y->is_red = false;
                    z->parent->parent->is_red = true;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        // Case 2: z is a left child.
                        z = z->parent;
                        right_rotate(z);
                    }
                    // Case 3: z is a right child.
                    z->parent->is_red = false;
                    z->parent->parent->is_red = true;
                    left_rotate(z->parent->parent);
                }
            }
        }
        if (root)
            root->is_red = false;
    }

    // Inserts a pre-allocated node into the tree.
    void insert(node *new_node) {
        node *y = nullptr;
        node *x = root;
        while (x) {
            y = x;
            if (std::less<T>()(new_node->value, x->value))
                x = x->left;
            else
                x = x->right;
        }
        new_node->parent = y;
        if (!y)
            root = new_node; // Tree was empty.
        else if (std::less<T>()(new_node->value, y->value))
            y->left = new_node;
        else
            y->right = new_node;

        new_node->left = nullptr;
        new_node->right = nullptr;
        new_node->is_red = true;
        insert_fixup(new_node);
    }

    // ------------------------------------------------------------
    // Finder / Accessor Methods
    // ------------------------------------------------------------

    // Finds a node with the given value.
    node *find(const T &value) {
        node *current = root;
        while (current) {
            if (std::less<T>()(value, current->value))
                current = current->left;
            else if (std::less<T>()(current->value, value))
                current = current->right;
            else
                return current;
        }
        return nullptr;
    }

    // Returns the minimum node in the subtree rooted at n.
    node *minimum(node *n) {
        while (n && n->left) n = n->left;
        return n;
    }

    // Returns the maximum node in the subtree rooted at n.
    node *maximum(node *n) {
        while (n && n->right) n = n->right;
        return n;
    }

    // Returns the minimum node in the tree.
    node *find_min() { return minimum(root); }

    // Returns the maximum node in the tree.
    node *find_max() { return maximum(root); }

    // Finds the in-order successor of node n.
    node *successor(node *n) {
        if (!n)
            return nullptr;
        if (n->right)
            return minimum(n->right);
        node *p = n->parent;
        while (p && n == p->right) {
            n = p;
            p = p->parent;
        }
        return p;
    }

    // Finds the in-order predecessor of node n.
    node *predecessor(node *n) {
        if (!n)
            return nullptr;
        if (n->left)
            return maximum(n->left);
        node *p = n->parent;
        while (p && n == p->left) {
            n = p;
            p = p->parent;
        }
        return p;
    }

    // ------------------------------------------------------------
    // Erase (Deletion) Methods
    // ------------------------------------------------------------

    // Replaces the subtree rooted at u with the subtree rooted at v.
    // (v may be nullptr.)
    void transplant(node *u, node *v) {
        if (u->parent == nullptr)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v)
            v->parent = u->parent;
    }

    // Fixes up the tree after deletion.
    // x may be nullptr; in such cases, x_parent is used.
    void erase_fixup(node *x, node *x_parent) {
        while ((x != root) && (x == nullptr || !x->is_red)) {
            if (x == (x_parent ? x_parent->left : nullptr)) {
                node *w = x_parent->right; // Sibling.
                if (w && w->is_red) {
                    // Case 1.
                    w->is_red = false;
                    x_parent->is_red = true;
                    left_rotate(x_parent);
                    w = x_parent->right;
                }
                bool w_left_black = (w->left == nullptr || !w->left->is_red);
                bool w_right_black = (w->right == nullptr || !w->right->is_red);
                if (w_left_black && w_right_black) {
                    // Case 2.
                    w->is_red = true;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w_right_black) {
                        // Case 3.
                        if (w->left)
                            w->left->is_red = false;
                        w->is_red = true;
                        right_rotate(w);
                        w = x_parent->right;
                    }
                    // Case 4.
                    w->is_red = x_parent->is_red;
                    x_parent->is_red = false;
                    if (w->right)
                        w->right->is_red = false;
                    left_rotate(x_parent);
                    x = root;
                    break;
                }
            } else {
                // Mirror image: x is the right child.
                node *w = x_parent->left; // Sibling.
                if (w && w->is_red) {
                    // Case 1.
                    w->is_red = false;
                    x_parent->is_red = true;
                    right_rotate(x_parent);
                    w = x_parent->left;
                }
                bool w_left_black = (w->left == nullptr || !w->left->is_red);
                bool w_right_black = (w->right == nullptr || !w->right->is_red);
                if (w_left_black && w_right_black) {
                    // Case 2.
                    w->is_red = true;
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (w_left_black) {
                        // Case 3.
                        if (w->right)
                            w->right->is_red = false;
                        w->is_red = true;
                        left_rotate(w);
                        w = x_parent->left;
                    }
                    // Case 4.
                    w->is_red = x_parent->is_red;
                    x_parent->is_red = false;
                    if (w->left)
                        w->left->is_red = false;
                    right_rotate(x_parent);
                    x = root;
                    break;
                }
            }
        }
        if (x)
            x->is_red = false;
    }

    // Erase method:
    // Removes node z from the tree while preserving red–black properties.
    // Returns the removed node (which is no longer linked in the tree)
    // so that the caller can free its memory later.
    node *erase(node *z) {
        node *removed = z; // This will eventually be the node removed from the tree.
        bool removed_original_color = removed->is_red;
        node *x = nullptr;
        node *x_parent = nullptr;

        if (z->left == nullptr) {
            x = z->right;
            x_parent = z->parent;
            transplant(z, z->right);
        } else if (z->right == nullptr) {
            x = z->left;
            x_parent = z->parent;
            transplant(z, z->left);
        } else {
            // z has two children. Use its in-order successor.
            node *y = minimum(z->right);
            removed_original_color = y->is_red;
            removed = y; // The node actually removed is y.
            x = y->right;
            if (y->parent == z) {
                if (x)
                    x->parent = y;
                x_parent = y;
            } else {
                x_parent = y->parent;
                transplant(y, y->right);
                y->right = z->right;
                if (y->right)
                    y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            if (y->left)
                y->left->parent = y;
            y->is_red = z->is_red;
        }
        if (!removed_original_color)
            erase_fixup(x, x_parent);
        return removed;
    }

    // ------------------------------------------------------------
    // Iterator Implementation (In-Order Traversal)
    // ------------------------------------------------------------
    //
    // This iterator traverses the tree in in-order.
    struct iterator {
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        node *current;
        const redblack_tree *tree_ptr;

        // Constructor: takes the current node and a pointer to the tree.
        iterator(node *n, const redblack_tree *t) : current(n), tree_ptr(t) {}

        // Dereference.
        reference operator*() const { return current->value; }
        pointer operator->() const { return &(current->value); }

        // Pre-increment: move to the in-order successor.
        iterator &operator++() {
            if (current != nullptr) {
                if (current->right != nullptr)
                    current = iterator::minimum(current->right);
                else {
                    node *p = current->parent;
                    while (p && current == p->right) {
                        current = p;
                        p = p->parent;
                    }
                    current = p;
                }
            }
            return *this;
        }

        // Post-increment.
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }

        // Pre-decrement: move to the in-order predecessor.
        iterator &operator--() {
            if (current == nullptr)
                current = iterator::maximum(tree_ptr->root);
            else if (current->left != nullptr)
                current = iterator::maximum(current->left);
            else {
                node *p = current->parent;
                while (p && current == p->left) {
                    current = p;
                    p = p->parent;
                }
                current = p;
            }
            return *this;
        }

        // Post-decrement.
        iterator operator--(int) {
            iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const iterator &other) const { return current == other.current; }
        bool operator!=(const iterator &other) const { return current != other.current; }

        // Helper: returns the minimum node in the subtree rooted at n.
        static node *minimum(node *n) {
            while (n && n->left) n = n->left;
            return n;
        }

        // Helper: returns the maximum node in the subtree rooted at n.
        static node *maximum(node *n) {
            while (n && n->right) n = n->right;
            return n;
        }
    };

    // Returns an iterator to the smallest element.
    iterator begin() const { return iterator(iterator::minimum(root), this); }

    // Returns an iterator representing one past the last element.
    iterator end() const { return iterator(nullptr, this); }

    iterator upper_bound(const T &value) {
        node *current = root;
        node *upper_bound = nullptr;
        while (current) {
            if (std::less<T>()(value, current->value)) {
                upper_bound = current;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return iterator(upper_bound, this);
    };

    iterator lower_bound(const T &key) const {
        node *candidate = nullptr;
        node *current = root;
        while (current) {
            // If current->value is less than key, then all nodes in its left subtree are too.
            if (std::less<T>()(current->value, key))
                current = current->right;
            else {
                candidate = current;
                current = current->left;
            }
        }
        return iterator(candidate, this);
    }
};

} // namespace containers
} // namespace gtr

#endif // REDBLACK_TREE_H
