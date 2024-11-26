#ifndef STACK_H
#define STACK_H
#include <cassert>
#include "vector.h"

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

    vector<T, Allocator> data;

    /**
     * @brief Constructs an empty stack.
     */
    inline constexpr stack() : data() {}

    /**
     * @brief Constructs a stack with the given capacity.
     *
     * @param capacity The maximum capacity of the stack.
     */
    inline constexpr stack(std::size_t capacity) : data(capacity) {}

    /**
     * @brief Destroys the stack.
     */

    inline ~stack() = default;

    /**
     * @brief Copy constructor.
     *
     * @param other The stack object to be copied.
     */
    inline stack(const stack &other) : data(other.data) {}

    /**
     * @brief Move constructor.
     *
     * @param other The stack object to be moved.
     */
    inline stack(stack &&other) noexcept : data(std::move(other.data)) {}

    /**
     * @brief Move assignment operator.
     *
     * @param other The stack object to be moved.
     * @return Reference to the updated stack object.
     */
    inline stack &operator=(stack &&other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    /**
     * @brief Copy assignment operator.
     *
     * @param other The stack object to be copied.
     * @return Reference to the updated stack object.
     */
    inline stack &operator=(const stack &other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    };

    /**
     * @brief Pushes an element onto the stack.
     *
     * @param value The value to be pushed onto the stack.
     */
    inline void push(const T &value) { data.push_back(value); }

    /**
     * @brief Pops an element from the stack.
     */
    inline void pop() { data.pop_back(); }

    /**
     * @brief Returns the top element of the stack.
     *
     * @return The top element of the stack.
     */
    inline T &top() { return data.back(); }

    /**
     * @brief Returns the top element of the stack.
     *
     * @return The top element of the stack.
     */
    inline const T &top() const { return data.back(); }
};
}; // namespace containers
}; // namespace gtr
#endif // !STACK_H
