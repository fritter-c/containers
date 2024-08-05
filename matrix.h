#ifndef MATRIX_H
#define MATRIX_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include <cassert>

namespace gtr {
namespace containers {
template <typename T, typename Allocator = c_allocator<T>> struct matrix : private _allocator_ebo<T, Allocator> {
    using value_type = T *;
    using iterator = value_type *;
    using const_interator = const value_type *;
    using allocator_type = Allocator;

    T *data;
    std::size_t rows;
    std::size_t columns;
    std::size_t capacity;

    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    inline constexpr std::size_t size_in_bytes() const { return columns * sizeof(T) * rows; }
    inline constexpr void _free_all() {
        if (data)
            allocator().free(data, capacity);
    }
    inline std::size_t _element_size() { return columns * sizeof(T); }

    inline matrix(std::size_t columns) {
        data = allocator().malloc(columns);
        capacity = 1;
        rows = 0;
        this->columns = columns;
    }

    inline matrix() {
        capacity = 0;
        rows = 0;
        columns = 0;
        data = nullptr;
    }

    inline void set_matrix(std::size_t rows, std::size_t columns) {
        _free_all();
        *this = matrix<T>(rows, columns);
    }

    inline bool is_set() { return data != nullptr; }

    inline matrix(std::size_t rows, std::size_t columns) {
        data = allocator().malloc(columns * rows);
        capacity = rows;
        this->columns = columns;
        this->rows = rows;
    }

    inline matrix(const matrix<T> &other) {
        data = nullptr;
        operator=(other);
    }

    inline matrix(matrix<T> &&other) noexcept : _allocator_ebo<T, Allocator>(std::move(other.allocator())) {
        capacity = other.capacity;
        columns = other.columns;
        rows = other.rows;
        data = other.data;
        other.data = nullptr;
    }

    inline ~matrix() {
        if (data)
            _free_all();
        data = nullptr;
    }

    inline matrix<T> &operator=(const matrix<T> &other) {
        if (data)
            _free_all();
        data = std::memcpy(allocator().malloc(other.columns * other.capacity), other.data, other.rows * other.capacity);
        rows = other.rows;
        columns = other.columns;
        capacity = other.capacity;
        return *this;
    }

    inline matrix<T> &operator=(matrix<T> &&other) noexcept {
        if (this != other) {
            if (data)
                _free_all();
            allocator() = std::move(other.allocator());
            data = other.data;
            rows = other.rows;
            columns = other.columns;
            capacity = other.capacity;
            other.data = nullptr;
            return *this;
        }
    }

    inline void reserve(const std::size_t _Reserve) {
        if (_Reserve > capacity) {
            data = allocator().realloc(data, columns * _Reserve, columns * capacity);
            capacity = _Reserve;
        }
    }

    constexpr inline void push_back(const T *element) {
        std::size_t new_size = rows + 1;
        if (rows == capacity) {
            reserve(capacity * 2);
        }
        std::memcpy(data + rows * columns, element, _element_size());
        rows = new_size;
    };

    inline void clear() { rows = 0; }

    inline value_type &last() {
        assert(rows);
        return data[(rows - 1) * columns];
    }

    inline value_type &first() {
        assert(rows);
        return data[0];
    }

    inline const value_type &last() const {
        assert(rows);
        return data[(rows - 1) * columns];
    }
    inline const value_type &first() const {
        assert(rows);
        return data[0];
    }

    inline void pop() {
        assert(rows);
        rows--;
    }

    inline value_type operator[](std::size_t row) {
        assert(row >= 0 && row < rows);
        return data + row * columns;
    }

    inline const value_type operator[](std::size_t row) const {
        assert(row >= 0 && row < rows);
        return data + row * columns;
    }
    inline operator const value_type *() const { return data; }
};
}; // namespace containers
} // namespace gtr
#endif
