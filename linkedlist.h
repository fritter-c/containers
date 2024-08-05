#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "allocator_base.h"
#include "allocators/allocators.h"
namespace gtr {
namespace containers {
template <typename T> struct node {
    T data;
    node *next;
    node *prev;
};
template <typename T, class Allocator = c_allocator<node<T>>> struct linked_list : private _allocator_ebo<T, Allocator> {
    struct iterator {
        node<T> *current;
        inline iterator(node<T> *node) : current(node) {}
        inline iterator &operator++() {
            current = current->next;
            return *this;
        }
        inline iterator operator++(int) {
            iterator temp = *this;
            current = current->next;
            return temp;
        }
        inline T &operator*() { return current->data; }
        inline bool operator==(const iterator &other) { return current == other.current; }
        inline bool operator!=(const iterator &other) { return current != other.current; }
    };
    using value_type = T;

    node<T> *head;
    node<T> *tail;
    std::size_t size;

    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    inline linked_list() : _allocator_ebo<T, Allocator>() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    inline linked_list(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>() {
        head = nullptr;
        tail = nullptr;
        size = 0;
        for (auto &item : init_list) {
            push_back(item);
        }
    }

    inline linked_list(const linked_list<T> &other) : _allocator_ebo<T, Allocator>(other.allocator()) {
        head = nullptr;
        tail = nullptr;
        size = 0;
        for (auto &item : other) {
            push_back(item);
        }
    }

    inline linked_list(linked_list<T> &&other) noexcept : _allocator_ebo<T, Allocator>(std::move(other.allocator())) {
        head = other.head;
        tail = other.tail;
        size = other.size;
        other.head = nullptr;
        other.tail = nullptr;
        other.size = 0;
    }

    inline linked_list<T> &operator=(const linked_list<T> &other) {
        if (this != (void *)(&other)) {
            clear();
            for (auto &item : other) {
                push_back(item);
            }
        }
        return *this;
    }

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

    inline ~linked_list() { clear(); }

    inline void clear() {
        node<T> *current = head;
        while (current) {
            node<T> *next = current->next;
            allocator().free(current, 1);
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    inline void push_back(const T &value) {
        node<T> *new_node = allocator().malloc(1);
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

    inline void push_front(const T &value) {
        node<T> *new_node = allocator().malloc(1);
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
            allocator().free(old_tail, 1);
            size--;
        }
    }

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
            allocator().free(old_head, 1);
            size--;
        }
    }

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
                allocator().free(current);
                current = next;
                size--;
            } else {
                current = current->next;
            }
        }
    }

    inline iterator begin() { return head; }
    inline iterator end() { return nullptr; }
    inline const iterator begin() const { return head; }
    inline const iterator end() const { return nullptr; }

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

    inline T &front() { return head->data; }
    inline T &back() { return tail->data; }
};
}; // namespace containers
}; // namespace gtr

#endif // !LINKEDLIST_H
