#ifndef STACK_H
#define STACK_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include <cassert>

namespace gtr {
namespace containers {

/**
 * @brief A stack data structure implementation.
 *
 * @tparam T The type of elements stored in the stack.
 * @tparam Allocator The allocator type used for memory management.
 */
template <typename T, class Allocator = c_allocator<T>> struct stack : private _allocator_ebo<T, Allocator> {
    using value_type = T;
    using allocator_type = Allocator;
    using iterator = value_type *;
    using const_iterator = const value_type *;

    T *data;              /**< Pointer to the underlying array. */
    std::size_t size;     /**< The number of elements currently in the stack. */
    std::size_t capacity; /**< The capacity of the stack. */

    /**
     * @brief Get a reference to the allocator used by the stack.
     *
     * @return Allocator& A reference to the allocator.
     */
    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Get a const reference to the allocator used by the stack.
     *
     * @return const Allocator& A const reference to the allocator.
     */
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Get the size of the stack in bytes.
     *
     * @return constexpr std::size_t The size of the stack in bytes.
     */
    inline constexpr std::size_t size_in_bytes() const { return size * sizeof(T); }

    /**
     * @brief Free all memory allocated by the stack.
     */
    inline constexpr void _free_all() { this->free(data, capacity); }

    /**
     * @brief Default constructor. Creates an empty stack with initial capacity of 1.
     */
    inline stack() : _allocator_ebo<T, Allocator>() {
        data = this->malloc(1);
        capacity = 1;
        size = 0;
    }

    /**
     * @brief Constructor that initializes the stack with elements from an initializer list.
     *
     * @param init_list The initializer list containing the elements to be added to the stack.
     */
    inline stack(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>() {
        std::size_t init_size = init_list.size();
        data = this->malloc(init_size);
        capacity = init_size;
        size = init_size;
        std::copy(init_list.begin(), init_list.end(), data);
    }

    /**
     * @brief Constructor that creates an empty stack with a specified capacity.
     *
     * @param _Length The initial capacity of the stack.
     */
    explicit inline stack(std::size_t _Length) : _allocator_ebo<T, Allocator>() {
        data = this->malloc(_Length);
        capacity = _Length;
        size = 0;
    }

    /**
     * @brief Copy constructor. Creates a new stack that is a copy of another stack.
     *
     * @param _Src The stack to be copied.
     */
    inline stack(const stack<T> &_Src) : _allocator_ebo<T, Allocator>(_Src.allocator()) {
        data = nullptr;
        operator=(_Src);
    }

    /**
     * @brief Move constructor. Creates a new stack by moving the contents of another stack.
     *
     * @param _Src The stack to be moved.
     */
    inline stack(stack<T> &&_Src) noexcept : _allocator_ebo<T, Allocator>(std::move(_Src.allocator())) {
        capacity = _Src.capacity;
        size = _Src.size;
        data = _Src.data;
        _Src.data = nullptr;
    }

    /**
     * @brief Copy assignment operator. Assigns the contents of another stack to this stack.
     *
     * @param _Src The stack to be copied.
     * @return stack<T>& A reference to this stack.
     */
    inline stack<T> &operator=(const stack<T> &_Src) {
        if (data)
            _free_all();
        data = this->malloc(_Src.capacity);
        std::memcpy(data, _Src.data, _Src.capacity * sizeof(T));
        size = _Src.size;
        capacity = _Src.capacity;
        return *this;
    }

    /**
     * @brief Move assignment operator. Moves the contents of another stack to this stack.
     *
     * @param _Src The stack to be moved.
     * @return stack<T>& A reference to this stack.
     */
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

    /**
     * @brief Destructor. Frees the memory allocated by the stack.
     */
    inline ~stack() {
        if (data)
            _free_all();
    }

    /**
     * @brief Pushes an element onto the top of the stack.
     *
     * @param value The value to be pushed onto the stack.
     */
    inline void push(const T &value) {
        if (size == capacity) {
            data = this->realloc(data, capacity * 2, capacity *= 2);
        }
        data[size++] = value;
    }

    /**
     * @brief Removes the top element from the stack.
     */
    inline void pop() {
        if (size > 0) {
            size--;
        }
    }

    /**
     * @brief Returns a reference to the top element of the stack.
     *
     * @return T& A reference to the top element of the stack.
     */
    inline T &top() {
        assert(size > 0);
        return data[size - 1];
    }
};

};     // namespace containers
};     // namespace gtr
#endif // !STACK_H
