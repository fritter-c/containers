#ifndef QUEUE_H
#define QUEUE_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include "linkedlist.h"
#include <initializer_list>
namespace gtr {
namespace containers {
template <typename T, class Allocator = c_allocator<node<T>>> struct queue : private _allocator_ebo<T, Allocator> {
    using value_type = T;
    using allocator_type = Allocator;

    linked_list<T, Allocator> list;

    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    inline constexpr std::size_t size_in_bytes() const { return list.size * sizeof(T); }
    inline constexpr void _free_all() { list.clear(); }

    inline queue() : _allocator_ebo<T, Allocator>() {}

    inline queue(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>(), list(init_list) {}

    explicit inline queue(std::size_t _Lenght) : _allocator_ebo<T, Allocator>() {}

    inline queue(const queue<T> &_Src) : _allocator_ebo<T, Allocator>(_Src.allocator()) { list = _Src.list; }

    inline queue(queue<T> &&_Src) noexcept : _allocator_ebo<T, Allocator>(std::move(_Src.allocator())) { list = std::move(_Src.list); }

    inline queue<T> &operator=(const queue<T> &_Src) {
        if (this != &_Src) {
            list = _Src.list;
        }
        return *this;
    }

    inline queue<T> &operator=(queue<T> &&_Src) {
        if (this != &_Src) {
            list = std::move(_Src.list);
        }
        return *this;
    }

    inline void push(const T &value) { list.push_back(value); }

    inline void pop_front() { list.pop_front(); }

    inline void dequeue() { list.pop_front(); }

    inline void enqueue(const T &value) { list.push_back(value); }

    inline T &front() { return list.front(); }

    inline T &back() { return list.back(); }

    inline bool empty() { return list.empty(); }

    inline std::size_t size() { return list.size; }

    inline void clear() { list.clear(); }

    inline ~queue() { _free_all(); }
};
} // namespace containers
} // namespace gtr

#endif // !QUEUE_H
