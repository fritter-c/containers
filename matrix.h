#ifndef MATRIX_H
#define MATRIX_H
#include <cassert>
#include "allocator_base.h"
#include "allocators/allocators.h"

namespace gtr {
namespace containers {

/**
 * @brief A generic matrix container class.
 *
 * @tparam T The type of elements stored in the matrix.
 * @tparam Allocator The allocator type used for memory management.
 * NOTE: The matrix is stored in column-major order because this is the only way to make a dynamic matrix(I think).
 * that pushes elements in O(1) time. This is because the matrix is stored in a single buffer and the elements are
 * pushed in the buffer as rows.
 *
 */
template <typename T, typename Allocator = c_allocator<T>> struct matrix : private _allocator_ebo<T, Allocator> {
    static_assert(std::is_arithmetic<T>::value, "Matrix type must be arithmetic.");
    static_assert(std::is_trivially_copyable<T>::value, "Matrix type must be trivially copyable.");

    using value_type = T *;
    using iterator = value_type *;
    using const_interator = const value_type *;
    using allocator_type = Allocator;

    /**
     * @brief A helper struct representing a buffer in the matrix.
     */
    struct buffer {
        T *data;           /**< The pointer to the buffer data. */
        std::size_t width; /**< The width of the buffer. */
        std::size_t count; /**< The total number of elements in the buffer. */

        struct iterator {
            T *data;
            std::size_t width;
            std::size_t row;

            iterator(T *data, std::size_t width, std::size_t row) : data(data), width(width), row(row) {}

            T &operator*() { return data[row * width]; }

            iterator &operator++() {
                row++;
                return *this;
            }

            bool operator!=(const iterator &other) { return row != other.row; }
        };

        /**
         * @brief Constructs a buffer object.
         *
         * @param a_width The width of the buffer.
         * @param a_count The total number of elements in the buffer.
         * @param a_data The pointer to the buffer data.
         */
        inline constexpr buffer(std::size_t a_width, std::size_t a_count, T *a_data) : data(a_data), width(a_width), count(a_count) {};

        /**
         * @brief Pre-increment operator for the buffer.
         *
         * @return The incremented buffer object.
         */
        inline buffer operator++() {
            data += width;
            return *this;
        }

        /**
         * @brief Access operator for the buffer.
         *
         * @param row The row index.
         * @return The reference to the element at the specified row.
         */
        inline T &operator[](std::size_t row) {
            assert(row * width < count);
            return data[width * row];
        }

        /**
         * @brief Const access operator for the buffer.
         *
         * @param row The row index.
         * @return The const reference to the element at the specified row.
         */
        const inline T &operator[](std::size_t row) const {
            assert(row * width < count);
            return data[width * row];
        }

        /**
         * @brief Returns the number of rows in the buffer.
         *
         * @return The number of rows in the buffer.
         */
        std::size_t size() const { return count / width; }

        /**
         * @brief Returns an iterator to the beginning of the buffer.
         *
         * @return An iterator to the beginning of the buffer.
         */
        iterator begin() { return iterator(data, width, 0); }

        /**
         * @brief Returns an iterator to the end of the buffer.
         *
         * @return An iterator to the end of the buffer.
         */
        iterator end() { return iterator(data, width, size()); }

        /**
         * @brief Returns a const iterator to the beginning of the buffer.
         *
         * @return A const iterator to the beginning of the buffer.
         */
        const iterator begin() const { return iterator(data, width, 0); }

        /**
         * @brief Returns a const iterator to the end of the buffer.
         *
         * @return A const iterator to the end of the buffer.
         */
        const iterator end() const { return iterator(data, width, size()); }
    };

    T *data;              /**< The pointer to the matrix data. */
    std::size_t rows;     /**< The number of rows in the matrix. */
    std::size_t columns;  /**< The number of columns in the matrix. */
    std::size_t capacity; /**< The capacity of the matrix. */

    /**
     * @brief Returns the allocator used by the matrix.
     *
     * @return The allocator used by the matrix.
     */
    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Returns the const allocator used by the matrix.
     *
     * @return The const allocator used by the matrix.
     */
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Returns the size of the matrix in bytes.
     *
     * @return The size of the matrix in bytes.
     */
    inline constexpr std::size_t size_in_bytes() const { return columns * sizeof(T) * rows; }

    /**
     * @brief Frees all allocated memory in the matrix.
     */
    inline constexpr void _free_all() { this->free(data, capacity); }

    /**
     * @brief Returns the size of a single element in the matrix.
     *
     * @return The size of a single element in the matrix.
     */
    inline std::size_t _element_size() { return columns * sizeof(T); }

    /**
     * @brief Default constructor for the matrix.
     */
    inline constexpr matrix() {
        capacity = 0;
        rows = 0;
        columns = 0;
        data = nullptr;
    }

    /**
     * @brief Sets the size of the matrix.
     *
     * @param _rows The number of rows.
     * @param _columns The number of columns.
     */
    inline constexpr void set_matrix(std::size_t _rows, std::size_t _columns) {
        _free_all();
        *this = matrix<T>(_rows, _columns);
    }

    /**
     * @brief Checks if the matrix is set.
     *
     * @return True if the matrix is set, false otherwise.
     */
    inline bool is_set() { return data != nullptr; }

    /**
     * @brief Constructs a matrix with the specified number of rows and columns.
     *
     * @param rows The number of rows.
     * @param columns The number of columns.
     */
    inline constexpr matrix(std::size_t rows, std::size_t columns) {
        assert(rows && columns);
        data = this->malloc(columns * rows);
        capacity = rows;
        this->columns = columns;
        this->rows = rows;
    }

    /**
     * @brief Copy constructor for the matrix.
     *
     * @param other The matrix to copy from.
     */
    inline constexpr matrix(const matrix<T> &other) {
        data = nullptr;
        operator=(other);
    }

    /**
     * @brief Move constructor for the matrix.
     *
     * @param other The matrix to move from.
     */
    inline constexpr matrix(matrix<T> &&other) noexcept : _allocator_ebo<T, Allocator>(std::move(other.allocator())) {
        capacity = other.capacity;
        columns = other.columns;
        rows = other.rows;
        data = other.data;
        other.data = nullptr;
    }

    /**
     * @brief Destructor for the matrix.
     */
    inline ~matrix() {
        _free_all();
        data = nullptr;
    }

    /**
     * @brief Copy assignment operator for the matrix.
     *
     * @param other The matrix to copy from.
     * @return The reference to the assigned matrix.
     */
    inline matrix<T> &operator=(const matrix<T> &other) {
        _free_all();

        data = this->malloc(other.size_in_bytes());
        std::memcpy(data, other.data, other.size_in_bytes());
        rows = other.rows;
        columns = other.columns;
        capacity = other.capacity;
        return *this;
    }

    /**
     * @brief Move assignment operator for the matrix.
     *
     * @param other The matrix to move from.
     * @return The reference to the assigned matrix.
     */
    inline matrix<T> &operator=(matrix<T> &&other) noexcept {
        if (this != &other) {
            _free_all();
            allocator() = std::move(other.allocator());
            data = other.data;
            rows = other.rows;
            columns = other.columns;
            capacity = other.capacity;
            other.data = nullptr;
        }
        return *this;
    }

    /**
     * @brief Reserves memory for the matrix.
     *
     * @param _Reserve The number of elements to reserve.
     */
    inline constexpr void reserve(const std::size_t _Reserve) {
        if (_Reserve > capacity) {
            data = this->realloc(data, columns * _Reserve, columns * capacity);
            capacity = _Reserve;
        }
    }

    /**
     * @brief Resizes the matrix.
     *
     * @param _Rows The new number of rows.
     */
    inline constexpr void resize(const std::size_t _Rows) {
        if (_Rows > capacity) {
            reserve(_Rows);
        }
        rows = _Rows;
    }

    /**
     * @brief Adds an element to the end of the matrix.
     *
     * @param element The element to add.
     */
    constexpr inline void push_back(const T *element) {
        std::size_t new_size = rows + 1;
        if (rows == capacity) {
            reserve(capacity * 2);
        }
        std::memcpy(data + rows * columns, element, _element_size());
        rows = new_size;
    };

    /**
     * @brief Clears the matrix.
     */
    inline void clear() { rows = 0; }

    /**
     * @brief Removes the last element from the matrix.
     */
    inline void pop() {
        assert(rows);
        rows--;
    }

    /**
     * @brief Access operator for the matrix.
     *
     * @param column The column index.
     * @return The buffer object representing the specified column.
     */
    inline buffer operator[](std::size_t column) {
        assert(column < columns);
        return buffer(columns, columns * rows, data + column);
    }

    /**
     * @brief Const access operator for the matrix.
     *
     * @param column The column index.
     * @return The const buffer object representing the specified column.
     */
    inline buffer operator[](std::size_t column) const {
        assert(column < columns);
        return buffer(columns, columns * rows, data + column);
    }

    /**
     * @brief Conversion operator to const value_type*.
     *
     * @return The const pointer to the matrix data.
     */
    inline operator const value_type *() const { return data; }
};

}; // namespace containers
}; // namespace gtr

#endif // MATRIX_H
