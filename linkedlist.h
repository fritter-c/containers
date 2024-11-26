#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "allocator_base.h"
#include "allocators/allocators.h"

namespace gtr {
namespace containers {
/**
 * @brief A node in the linked list.
 *
 * @tparam T The type of data stored in the node.
 */
template <typename T> struct node {
    T data;     /**< The data stored in the node. */
    node *next; /**< Pointer to the next node in the list. */
    node *prev; /**< Pointer to the previous node in the list. */
};

/**
 * @brief A doubly linked list container.
 *
 * @tparam T The type of elements stored in the list.
 * @tparam Allocator The allocator type used for memory management.
 */
template <typename T, class Allocator = c_allocator<node<T>>> struct linked_list : private _allocator_ebo<node<T>, Allocator> {
    /**
     * @brief An iterator for traversing the linked list.
     */
    struct iterator {
        node<T> *current; /**< Pointer to the current node. */

        /**
         * @brief Constructs an iterator with a given node.
         *
         * @param node The node to point to.
         */
        inline iterator(node<T> *node) : current(node) {}

        /**
         * @brief Advances the iterator to the next node.
         *
         * @return Reference to the updated iterator.
         */
        inline iterator &operator++() {
            current = current->next;
            return *this;
        }

        /**
         * @brief Advances the iterator to the next node.
         *
         * @return A copy of the iterator before the increment.
         */
        inline iterator operator++(int) {
            iterator temp = *this;
            current = current->next;
            return temp;
        }

        /**
         * @brief Dereferences the iterator to access the data of the current node.
         *
         * @return Reference to the data of the current node.
         */
        inline T &operator*() { return current->data; }

        /**
         * @brief Compares two iterators for equality.
         *
         * @param other The iterator to compare with.
         * @return True if the iterators are equal, false otherwise.
         */
        inline bool operator==(const iterator &other) { return current == other.current; }

        /**
         * @brief Compares two iterators for inequality.
         *
         * @param other The iterator to compare with.
         * @return True if the iterators are not equal, false otherwise.
         */
        inline bool operator!=(const iterator &other) { return current != other.current; }
    };

    using value_type = T; /**< The type of elements stored in the list. */

    node<T> *head;    /**< Pointer to the first node in the list. */
    node<T> *tail;    /**< Pointer to the last node in the list. */
    std::size_t size; /**< The number of elements in the list. */

    /**
     * @brief Gets the allocator used by the linked list.
     *
     * @return Reference to the allocator.
     */
    Allocator &allocator() { return _allocator_ebo<node<T>, Allocator>::get_allocator(); }

    /**
     * @brief Gets the allocator used by the linked list (const version).
     *
     * @return Const reference to the allocator.
     */
    const Allocator &allocator() const { return _allocator_ebo<node<T>, Allocator>::get_allocator(); }

    /**
     * @brief Constructs an empty linked list.
     */
    inline linked_list() : _allocator_ebo<node<T>, Allocator>() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    /**
     * @brief Constructs a linked list with elements from an initializer list.
     *
     * @param init_list The initializer list of elements.
     */
    inline linked_list(std::initializer_list<T> init_list) : _allocator_ebo<node<T>, Allocator>() {
        head = nullptr;
        tail = nullptr;
        size = 0;
        for (auto &item : init_list) {
            push_back(item);
        }
    }

    /**
     * @brief Constructs a linked list by copying another linked list.
     *
     * @param other The linked list to copy from.
     */
    inline linked_list(const linked_list<T> &other) : _allocator_ebo<node<T>, Allocator>(other.allocator()) {
        head = nullptr;
        tail = nullptr;
        size = 0;
        for (auto &item : other) {
            push_back(item);
        }
    }

    /**
     * @brief Constructs a linked list by moving another linked list.
     *
     * @param other The linked list to move from.
     */
    inline linked_list(linked_list<T> &&other) noexcept : _allocator_ebo<node<T>, Allocator>(std::move(other.allocator())) {
        head = other.head;
        tail = other.tail;
        size = other.size;
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }

    /**
     * @brief Assigns the contents of another linked list to this linked list.
     *
     * @param other The linked list to assign from.
     * @return Reference to this linked list.
     */
    inline linked_list<T> &operator=(const linked_list<T> &other) {
        if (this != (void *)(&other)) {
            clear();
            for (auto &item : other) {
                push_back(item);
            }
        }
        return *this;
    }

    /**
     * @brief Assigns the contents of another linked list to this linked list by moving.
     *
     * @param other The linked list to move from.
     * @return Reference to this linked list.
     */
    inline linked_list<T> &operator=(linked_list<T> &&other) noexcept {
        if (this != (void *)(&other)) {
            clear();
            head = other.head;
            tail = other.tail;
            size = other.size;
            other.head = nullptr;
            other.tail = nullptr;
            other.size = 0;
            allocator() = std::move(other.allocator());
        }
        return *this;
    }

    /**
     * @brief Destroys the linked list and frees the allocated memory.
     */
    inline ~linked_list() { clear(); }

    /**
     * @brief Removes all elements from the linked list.
     */
    inline void clear() {
        node<T> *current = head;
        while (current) {
            node<T> *next = current->next;
            current->~node();
            allocator().free(current, 1);
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    /**
     * @brief Adds an element to the end of the linked list.
     *
     * @param value The value to add.
     */
    inline void push_back(const T &value) {
        node<T> *new_node = this->malloc(1);
        new_node->data = value;
        new_node->next = nullptr;
        new_node->prev = tail;
        if (tail) {
            tail->next = new_node;
        }
        tail = new_node;
        if (!head) {
            head = new_node;
        }
        size++;
    }

    /**
     * @brief Adds an element to the front of the linked list.
     *
     * @param value The value to add.
     */
    inline void push_front(const T &value) {
        node<T> *new_node = this->malloc(1);
        new (new_node) node<T>();
        new_node->data = value;
        new_node->next = head;
        new_node->prev = nullptr;
        if (head) {
            head->prev = new_node;
        }
        head = new_node;
        if (!tail) {
            tail = new_node;
        }
        size++;
    }

    /**
     * @brief Removes the last element from the linked list.
     */
    inline void pop_back() {
        if (tail) {
            node<T> *old_tail = tail;
            tail = tail->prev;
            if (tail) {
                tail->next = nullptr;
            } else {
                head = nullptr;
                tail = nullptr;
            }
            this->free(old_tail, 1);
            size--;
        }
    }

    /**
     * @brief Removes the first element from the linked list.
     */
    inline void pop_front() {
        if (head) {
            node<T> *old_head = head;
            head = head->next;
            if (head) {
                head->prev = nullptr;
            } else {
                head = nullptr;
                tail = nullptr;
            }
            old_head->~node();
            this->free(old_head, 1);
            size--;
        }
    }

    /**
     * @brief Removes all occurrences of a value from the linked list.
     *
     * @param value The value to remove.
     */
    inline void remove(const T &value) {
        node<T> *current = head;
        while (current) {
            if (current->data == value) {
                if (current->prev) {
                    current->prev->next = current->next;
                }
                if (current->next) {
                    current->next->prev = current->prev;
                }
                if (current == head) {
                    head = current->next;
                }
                if (current == tail) {
                    tail = current->prev;
                }
                node<T> *next = current->next;
                current->~node();
                this->free(current, 1);
                current = next;
                size--;
            } else {
                current = current->next;
            }
        }
    }

    /**
     * @brief Returns an iterator to the first element of the linked list.
     *
     * @return Iterator to the first element.
     */
    inline iterator begin() { return head; }

    /**
     * @brief Returns an iterator to the element following the last element of the linked list.
     *
     * @return Iterator to the element following the last element.
     */
    inline iterator end() { return nullptr; }

    /**
     * @brief Returns a const iterator to the first element of the linked list.
     *
     * @return Const iterator to the first element.
     */
    inline const iterator begin() const { return head; }

    /**
     * @brief Returns a const iterator to the element following the last element of the linked list.
     *
     * @return Const iterator to the element following the last element.
     */
    inline const iterator end() const { return nullptr; }

    /**
     * @brief Reverses the order of elements in the linked list.
     */
    inline void reverse() {
        node<T> *current = head;
        while (current) {
            node<T> *next = current->next;
            current->next = current->prev;
            current->prev = next;
            current = next;
        }
        node<T> *temp = head;
        head = tail;
        tail = temp;
    }

    /**
     * @brief Returns a reference to the first element of the linked list.
     *
     * @return Reference to the first element.
     */
    inline T &front() { return head->data; }

    /**
     * @brief Returns a reference to the last element of the linked list.
     *
     * @return Reference to the last element.
     */
    inline T &back() { return tail->data; }

    /**
     * @brief Checks if the linked list is empty.
     *
     * @return true if the linked list is empty, false otherwise.
     */
    inline bool empty() { return !head; }
};
} // namespace containers
} // namespace gtr
#endif // !LINKEDLIST_H
