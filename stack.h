#ifndef STACK_H
#define STACK_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include <cassert>

namespace gtr {
namespace containers {
template <typename T, class Allocator = c_allocator<T>> struct stack : private _allocator_ebo<T, Allocator> {
    using value_type = T;
    using allocator_type = Allocator;
    using iterator = value_type *;
    using const_iterator = const value_type *;

    T *data;
    std::size_t size;
    std::size_t capacity;

    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }

    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    inline constexpr std::size_t size_in_bytes() const { return size * sizeof(T); }

    inline constexpr void _free_all() { allocator().free(data, capacity); }

    inline stack() : _allocator_ebo<T, Allocator>() {
        data = allocator().malloc(1);
        capacity = 1;
        size = 0;
    }

    inline stack(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>() {
        std::size_t init_size = init_list.size();
        data = allocator().malloc(init_size);
        capacity = init_size;
        size = init_size;
        std::copy(init_list.begin(), init_list.end(), data);
    }

    explicit inline stack(std::size_t _Lenght) : _allocator_ebo<T, Allocator>() {
        data = allocator().malloc(_Lenght);
        capacity = _Lenght;
        size = 0;
    }

    inline stack(const stack<T> &_Src) : _allocator_ebo<T, Allocator>(_Src.allocator()) {
        data = nullptr;
        operator=(_Src);
    }

    inline stack(stack<T> &&_Src) noexcept : _allocator_ebo<T, Allocator>(std::move(_Src.allocator())) {
        capacity = _Src.capacity;
        size = _Src.size;
        data = _Src.data;
        _Src.data = nullptr;
    }

    inline stack<T> &operator=(const stack<T> &_Src) {
        if (data)
            _free_all();
        data = allocator().malloc(_Src.capacity);
        std::memcpy(data, _Src.data, _Src.capacity * sizeof(T));
        size = _Src.size;
        capacity = _Src.capacity;
        return *this;
    }

    inline stack<T> &operator=(stack<T> &&_Src) noexcept {
        if (this != &_Src) {
            if (data)
                _free_all();
            data = _Src.data;
            size = _Src.size;
            capacity = _Src.capacity;
            allocator() = std::move(_Src.allocator());
            _Src.data = nullptr;
        }
        return *this;
    }

    inline ~stack() {
        if (data)
            _free_all();
    }

    inline void push(const T &value) {
        if (size == capacity) {
            data = allocator().realloc(data, capacity * 2, capacity *= 2);
        }
        data[size++] = value;
    }

    inline void pop() {
        if (size > 0) {
            size--;
        }
    }
    inline T &top() {
        assert(size > 0);
        return data[size - 1];
    }
};
};     // namespace containers
};     // namespace gtr
#endif // !STACK_H
