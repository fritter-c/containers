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

/**
 * @brief A lightweight array container with customizable allocator.
 *
 * @tparam T The type of elements stored in the array.
 * @tparam Allocator The allocator type used to allocate and deallocate memory for the array.
 */
template <typename T, class Allocator = c_allocator<T>> struct light_array : private _allocator_ebo<T, Allocator> {
    using value_type = T;
    using iterator = value_type *;
    using const_interator = const value_type *;
    using allocator_type = Allocator;

    T *data;              /**< Pointer to the underlying data array. */
    std::size_t size;     /**< The number of elements currently stored in the array. */
    std::size_t capacity; /**< The current capacity of the array. */

    /**
     * @brief Get a reference to the allocator used by the array.
     *
     * @return Allocator& A reference to the allocator.
     */
    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Get a const reference to the allocator used by the array.
     *
     * @return const Allocator& A const reference to the allocator.
     */
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Get the size of the array in bytes.
     *
     * @return constexpr std::size_t The size of the array in bytes.
     */
    inline constexpr std::size_t size_in_bytes() const { return size * sizeof(T); }

    /**
     * @brief Free all allocated memory.
     */
    inline constexpr void _free_all() { this->free(data, capacity); }

    // Constructors

    /**
     * @brief Default constructor. Creates an empty array with initial capacity of 1.
     */
    inline light_array() : _allocator_ebo<T, Allocator>() { std::memset(this, 0, sizeof(*this)); }

    /**
     * @brief Construct an array from an initializer list.
     *
     * @param init_list The initializer list to initialize the array with.
     */
    inline light_array(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>() {
        std::size_t init_size = init_list.size();
        data = this->malloc(init_size);
        capacity = init_size;
        size = init_size;
        std::copy(init_list.begin(), init_list.end(), data);
    }

    /**
     * @brief Construct an array with a specified length.
     *
     * @param _Length The length of the array.
     */
     inline light_array(std::size_t _Length) : _allocator_ebo<T, Allocator>() {
        data = this->malloc(_Length);
        capacity = _Length;
        size = 0;
    }

    /**
     * @brief Copy constructor. Creates a copy of another array.
     *
     * @param _Src The array to copy from.
     */
    inline constexpr light_array(const light_array<T> &_Src) : _allocator_ebo<T, Allocator>(_Src.allocator()) {
        data = nullptr;
        operator=(_Src);
    }

    /**
     * @brief Copy assignment operator. Assigns the contents of another array to this array.
     *
     * @param _Src The array to copy from.
     * @return light_array<T>& A reference to this array.
     */
    inline constexpr light_array<T> &operator=(const light_array<T> &_Src) {
        if (data)
            _free_all();
        data = this->malloc(_Src.capacity);
        std::memcpy(data, _Src.data, _Src.capacity * sizeof(T));
        size = _Src.size;
        capacity = _Src.capacity;
        return *this;
    }

    /**
     * @brief Move constructor. Moves the contents of another array to this array.
     *
     * @param _Src The array to move from.
     */
    inline constexpr light_array(light_array<T> &&_Src) noexcept : _allocator_ebo<T, Allocator>(std::move(_Src.allocator())) {
        size = _Src.size;
        capacity = _Src.capacity;
        data = _Src.data;
        _Src.data = nullptr;
        _Src.size = 0;
        _Src.capacity = 0;
    }

    /**
     * @brief Move assignment operator. Moves the contents of another array to this array.
     *
     * @param _Src The array to move from.
     * @return light_array<T>& A reference to this array.
     */
    inline constexpr light_array<T> &operator=(light_array<T> &&_Src) noexcept {
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

    /**
     * @brief Destructor. Frees the allocated memory.
     */
    inline ~light_array() {
        if (data) {
            _free_all();
        }
        data = nullptr;
    }

    /**
     * @brief Destroy all elements in the array.
     */
    inline void destroy() {
        if (data) {
            for (auto &c : *this) {
                c.~T();
            }
        }
        size = 0;
    }

    // Manipulation and access methods

    /**
     * @brief Add an element to the end of the array.
     *
     * @param _Value The value to add.
     */
    inline constexpr void push_back(const value_type &_Value) {
        if (size == capacity) {
            reserve(capacity ? capacity * 2 : 1);
        }
        *end() = _Value;
        size++;
    }

    /**
     * @brief Add a move-constructed element to the end of the array.
     *
     * @param _Value The value to add.
     */
    inline constexpr void push_back(value_type &&_Value) {
        if (size == capacity) {
            reserve(capacity ? capacity * 2 : 1);
        }
        *((T *)std::memset(end(), 0, sizeof(T))) = std::move(_Value);
        size++;
    }

    /**
     * @brief Add the elements from another array to the end of this array.
     *
     * @param _Src The array to add.
     */
    inline void push_back(const light_array<T> &_Src) {
        if (size + _Src.size > capacity) {
            reserve(size + _Src.size);
        }
        std::memcpy(end(), _Src.data, _Src.size * sizeof(T));
        size += _Src.size;
    }

    /**
     * @brief Add a copy-constructed element to the end of the array.
     *
     * @param _Value The value to add.
     */
    inline constexpr void push_back_construct(const value_type &_Value) {
        if (size == capacity) {
            reserve(capacity ? capacity * 2 : 1);
        }
        new (end()) T(_Value);
        size++;
    }

    /**
     * @brief Emplace a new element at the end of the array.
     *
     * @tparam Args The types of the arguments to forward to the element's constructor.
     * @param _Args The arguments to forward to the element's constructor.
     */
    template <typename... Args> inline void emplace_back(Args &&..._Args) {
        if (size == capacity) {
            reserve(capacity ? capacity * 2 : 1);
        }
        new (end()) T(std::forward<Args>(_Args)...);
        size++;
    }

    /**
     * @brief Reduce the capacity of the array to match the size.
     */
    inline void shrink_to_fit() {
        if (size < capacity) {
            data = this->realloc(data, size ? size : 1, capacity);
            capacity = size ? size : 1;
        }
    }

    /**
     * @brief Add a range of elements to the end of the array.
     *
     * @param _Start Iterator to the start of the range.
     * @param _End Iterator to the end of the range.
     */
    inline constexpr void push_range(const_interator _Start, const_interator _End) {
        std::ptrdiff_t to_append = _End - _Start;
        if (to_append + size > capacity)
            reserve(to_append + size);
        std::memcpy(end(), _Start, to_append * sizeof(T));
        size += to_append;
    }

    /**
     * @brief Insert an element at a specified index in the array.
     *
     * @param _Value The value to insert.
     * @param _Index The index at which to insert the value.
     */
    inline void insert(const value_type &_Value, const std::size_t _Index) {
        push_back(_Value);
        std::memmove(data + _Index + 1, data + _Index, sizeof(T) * (size - _Index - 1));
        data[_Index] = _Value;
    }

    /**
     * @brief Insert an element at a specified index in the array, potentially overwriting an existing element.
     *
     * @param _Value The value to insert.
     * @param _Index The index at which to insert the value.
     */
    inline void insert_unordered(const value_type &_Value, const std::size_t _Index) {
        if (_Index < size) {
            push_back(data[_Index]);
            data[_Index] = _Value;
        } else {
            push_back(_Value);
        }
    }

    /**
     * @brief Remove the last element from the array.
     */
    inline constexpr void pop() { size--; }

    /**
     * @brief Remove the last element from the array and call its destructor.
     */
    inline constexpr void pop_destruct() {
        data[size - 1].~T();
        size--;
    }

    /**
     * @brief Clear the array, removing all elements.
     */
    inline constexpr void clear() { size = 0; }

    /**
     * @brief Reserve memory for a specified number of elements.
     *
     * @param _Reserve The number of elements to reserve memory for.
     */
    inline void reserve(const std::size_t _Reserve) {
        if (_Reserve > capacity) {
            data = allocator().realloc(data, _Reserve, capacity);
            capacity = _Reserve;
        }
    }

    /**
     * @brief Resize the array to a specified number of elements.
     *
     * @param _Reserve The number of elements to resize the array to.
     */
    inline void resize(const std::size_t _Reserve) {
        if (_Reserve > size) {
            reserve(_Reserve);
            size = _Reserve;
        }
    }

    /**
     * @brief Get a reference to the last element in the array.
     *
     * @return value_type& A reference to the last element.
     */
    inline value_type &last() {
        assert(size > 0);
        return data[size - 1];
    }

    /**
     * @brief Get a reference to the first element in the array.
     *
     * @return value_type& A reference to the first element.
     */
    inline value_type &first() {
        assert(size > 0);
        return data[0];
    }

    /**
     * @brief Get a const reference to the last element in the array.
     *
     * @return const value_type& A const reference to the last element.
     */
    inline const value_type &last() const {
        assert(size > 0);
        return data[size - 1];
    }

    /**
     * @brief Get a const reference to the first element in the array.
     *
     * @return const value_type& A const reference to the first element.
     */
    inline const value_type &first() const {
        assert(size > 0);
        return data[0];
    }

    /**
     * @brief Check if the array contains a specific element.
     *
     * @param _Element The element to check for.
     * @return bool True if the element is found, false otherwise.
     */
    inline bool contains(const value_type &_Element) const {
        for (auto &c : *this) {
            if (c == _Element)
                return true;
        }
        return false;
    }

    /**
     * @brief Check if the array contains a specific element and get its index.
     *
     * @param _Element The element to check for.
     * @param _Index The index of the element if found.
     * @return bool True if the element is found, false otherwise.
     */
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

    /**
     * @brief Get an iterator to the beginning of the array.
     *
     * @return iterator An iterator to the beginning of the array.
     */
    inline iterator begin() { return data; }

    /**
     * @brief Get a const iterator to the beginning of the array.
     *
     * @return const_interator A const iterator to the beginning of the array.
     */
    inline const_interator begin() const { return data; }

    /**
     * @brief Get an iterator to the end of the array.
     *
     * @return iterator An iterator to the end of the array.
     */
    inline iterator end() { return data + size; }

    /**
     * @brief Get a const iterator to the end of the array.
     *
     * @return const_interator A const iterator to the end of the array.
     */
    inline const_interator end() const { return data + size; }

    /**
     * @brief Get a const reference to an element at a specified index.
     *
     * @param _Index The index of the element.
     * @return const value_type& A const reference to the element.
     */
    inline const value_type &operator[](std::size_t _Index) const {
        assert(_Index < size);
        return data[_Index];
    }

    /**
     * @brief Get a reference to an element at a specified index.
     *
     * @param _Index The index of the element.
     * @return value_type& A reference to the element.
     */
    inline value_type &operator[](std::size_t _Index) {
        assert(_Index < size);
        return data[_Index];
    }

    /**
     * @brief Erase an element from the array and call its destructor.
     *
     * @param it An iterator pointing to the element to erase.
     * @return iterator An iterator pointing to the next element after the erased element.
     */
    inline iterator erase_destroy(const_interator it) {
        if (it >= data && it < data + size) {
            const std::ptrdiff_t off = it - data;
            data[off].~T();
            std::memmove(data + off, data + off + 1, (size - off - 1) * sizeof(T));
            size--;
            return data + off;
        }
        return data + size;
    }

    /**
     * @brief Erase an element from the array.
     *
     * @param it An iterator pointing to the element to erase.
     * @return iterator An iterator pointing to the next element after the erased element.
     */
    inline iterator erase(const_interator it) {
        if (it >= data && it < data + size) {
            const std::ptrdiff_t off = it - data;
            std::memmove(data + off, data + off + 1, (size - off - 1) * sizeof(T));
            size--;
            return data + off;
        }
        return data + size;
    }

    /**
     * @brief Erase a range of elements from the array.
     *
     * @param it An iterator pointing to the first element to erase.
     * @param it_last An iterator pointing to the last element to erase.
     * @return iterator An iterator pointing to the next element after the erased range.
     */
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

    /**
     * @brief Erase an element from the array without maintaining order.
     *
     * @param it An iterator pointing to the element to erase.
     * @return iterator An iterator pointing to the next element after the erased element.
     */
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
}; // namespace containers
}; // namespace containers
}; // namespace gtr
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif // LIGHTARRAY_H
