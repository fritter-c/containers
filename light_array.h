#ifndef LIGHTARRAY_H
#define LIGHTARRAY_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include <cassert>
#include <initializer_list>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

namespace gtr {
namespace containers {
template <typename T, class Allocator = c_allocator<T>> struct light_array : private _allocator_ebo<T, Allocator> {
    using value_type = T;
    using iterator = value_type *;
    using const_interator = const value_type *;
    using allocator_type = Allocator;

    T *data;
    std::size_t size;
    std::size_t capacity;

    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }

    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    inline constexpr std::size_t size_in_bytes() const { return size * sizeof(T); }

    inline constexpr void _free_all() { allocator().free(data, capacity); }

    // Constructors
    inline light_array() : _allocator_ebo<T, Allocator>() {
        data = allocator().malloc(1);
        capacity = 1;
        size = 0;
    }

    inline light_array(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>() {
        std::size_t init_size = init_list.size();
        data = allocator().malloc(init_size);
        capacity = init_size;
        size = init_size;
        std::copy(init_list.begin(), init_list.end(), data);
    }

    explicit inline light_array(std::size_t _Lenght) : _allocator_ebo<T, Allocator>() {
        data = allocator().malloc(_Lenght);
        capacity = _Lenght;
        size = 0;
    }

    inline light_array(const light_array<T> &_Src) : _allocator_ebo<T, Allocator>(_Src.allocator()) {
        data = nullptr;
        operator=(_Src);
    }

    inline light_array<T> &operator=(const light_array<T> &_Src) {
        if (data)
            _free_all();
        data = allocator().malloc(_Src.capacity);
        std::memcpy(data, _Src.data, _Src.capacity * sizeof(T));
        size = _Src.size;
        capacity = _Src.capacity;
        return *this;
    }

    inline light_array(light_array<T> &&_Src) noexcept : _allocator_ebo<T, Allocator>(std::move(_Src.allocator())) {
        size = _Src.size;
        capacity = _Src.capacity;
        data = _Src.data;
        _Src.data = nullptr;
        _Src.size = 0;
        _Src.capacity = 0;
    }

    inline light_array<T> &operator=(light_array<T> &&_Src) noexcept {
        if (this != &_Src) {
            if (data)
                _free_all();

            allocator() = std::move(_Src.allocator());
            size = _Src.size;
            capacity = _Src.capacity;
            data = _Src.data;
            _Src.data = nullptr;
            _Src.size = 0;
            _Src.capacity = 0;
        }
        return *this;
    }

    inline ~light_array() {
        if (data) {
            _free_all();
        }
        data = nullptr;
    }

    inline void destroy() {
        if (data) {
            for (auto &c : *this) {
                c.~T();
            }
        }
    }

    // Manipulation and acesses methods
    inline constexpr void push_back(const value_type &_Value) {
        if (size == capacity) {
            reserve(capacity * 2);
        }
        *end() = _Value;
        size++;
    }

    inline constexpr void push_back(value_type &&_Value) {
        if (size == capacity) {
            reserve(capacity * 2);
        }
        *((T *)std::memset(end(), 0, sizeof(T))) = std::move(_Value);
        size++;
    }

    inline void shrink_to_fit() {
        if (size < capacity) {
            data = allocator().realloc(data, size ? size : 1, capacity);
            capacity = size ? size : 1;
        }
    }

    inline constexpr void push_range(const_interator _Start, const_interator _End) {
        std::ptrdiff_t to_append = _End - _Start;
        if (to_append + size > capacity)
            reserve(to_append + size);
        std::memcpy(end(), _Start, to_append * sizeof(T));
        size += to_append;
    }

    inline void insert(const value_type &_Value, const std::size_t _Index) {
        push_back(_Value);
        std::memmove(data + _Index + 1, data + _Index, sizeof(T) * (size - _Index - 1));
        data[_Index] = _Value;
    }

    inline void insert_unordered(const value_type &_Value, const std::size_t _Index) {
        if (_Index < size) {
            push_back(data[_Index]);
            data[_Index] = _Value;
        } else {
            push_back(_Value);
        }
    }

    inline constexpr void pop() { size--; }

    inline constexpr void clear() { size = 0; }

    inline void reserve(const std::size_t _Reserve) {
        if (_Reserve > capacity) {
            data = allocator().realloc(data, _Reserve, capacity);
            capacity = _Reserve;
        }
    }

    inline void resize(const std::size_t _Reserve) {
        if (_Reserve > size) {
            reserve(_Reserve);
            size = _Reserve;
        }
    }

    inline value_type &last() {
        assert(size > 0);
        return data[size - 1];
    }

    inline value_type &first() {
        assert(size > 0);
        return data[0];
    }

    inline const value_type &last() const {
        assert(size > 0);
        return data[size - 1];
    }

    inline const value_type &first() const {
        assert(size > 0);
        return data[0];
    }

    inline bool contains(const value_type &_Element) const {
        for (auto &c : *this) {
            if (c == _Element)
                return true;
        }
        return false;
    }

    inline bool contains(const value_type &_Element, std::size_t &_Index) const {
        _Index = 0;
        for (std::size_t i = 0; i < size; i++) {
            if (data[i] == _Element) {
                _Index = i;
                return true;
            }
        }
        return false;
    }

    inline iterator begin() { return data; }

    inline const_interator begin() const { return data; }

    inline iterator end() { return data + size; }

    inline const_interator end() const { return data + size; }

    inline const value_type &operator[](std::size_t _Index) const {
        assert(_Index < size);
        return data[_Index];
    }

    inline value_type &operator[](std::size_t _Index) {
        assert(_Index < size);
        return data[_Index];
    }

    inline iterator erase(const_interator it) {
        if (it >= data && it < data + size) {
            const std::ptrdiff_t off = it - data;
            std::memmove(data + off, data + off + 1, (size - off - 1) * sizeof(T));
            size--;
            return data + off;
        }
        return data + size;
    }

    inline iterator erase(const_interator it, const_interator it_last) {
        if (it >= data && it < data + size && it_last >= it && it_last <= data + size) {
            const std::ptrdiff_t count = it_last - it;
            const std::ptrdiff_t off = it - data;
            std::memmove(data + off, data + off + count, (size - off - count) * sizeof(T));
            size -= count;
            return data + off;
        }
        return data + size;
    }

    inline iterator erase_unordered(const_interator it) {
        if (it >= data && it < data + size) {
            const std::ptrdiff_t off = it - data;
            if (it < data + size - 1) {
                std::memcpy(data + off, data + size - 1, sizeof(T));
            }
            size--;

            return data + off;
        }
        return data + size;
    }
};     // namespace containers
};     // namespace containers
};     // namespace gtr
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif // LIGHTARRAY_H
