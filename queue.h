#ifndef QUEUE_H
#define QUEUE_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include "linkedlist.h"
#include <initializer_list>

namespace gtr {
namespace containers {

/**
 * @brief A queue data structure implementation.
 *
 * @tparam T The type of elements stored in the queue.
 * @tparam Allocator The allocator type used for memory management.
 */
template <typename T, class Allocator = c_allocator<node<T>>> struct queue : private _allocator_ebo<T, Allocator> {
    using value_type = T;
    using allocator_type = Allocator;

    linked_list<T, Allocator> list; /**< The underlying linked list used to store the elements. */

    /**
     * @brief Get the allocator object.
     *
     * @return Allocator& The allocator object.
     */
    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Get the allocator object (const version).
     *
     * @return const Allocator& The allocator object.
     */
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Get the size of the queue in bytes.
     *
     * @return constexpr std::size_t The size of the queue in bytes.
     */
    inline constexpr std::size_t size_in_bytes() const { return list.size * sizeof(T); }

    /**
     * @brief Free all elements in the queue.
     */
    inline constexpr void _free_all() { list.clear(); }

    /**
     * @brief Default constructor.
     */
    inline queue() : _allocator_ebo<T, Allocator>() {}

    /**
     * @brief Constructor that initializes the queue with elements from an initializer list.
     *
     * @param init_list The initializer list of elements.
     */
    inline queue(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>(), list(init_list) {}

    /**
     * @brief Constructor that initializes the queue with a specified length.
     *
     * @param _Length The length of the queue.
     */
    explicit inline queue(std::size_t _Length) : _allocator_ebo<T, Allocator>() {}

    /**
     * @brief Copy constructor.
     *
     * @param _Src The source queue to copy from.
     */
    inline queue(const queue<T> &_Src) : _allocator_ebo<T, Allocator>(_Src.allocator()) { list = _Src.list; }

    /**
     * @brief Move constructor.
     *
     * @param _Src The source queue to move from.
     */
    inline queue(queue<T> &&_Src) noexcept : _allocator_ebo<T, Allocator>(std::move(_Src.allocator())) { list = std::move(_Src.list); }

    /**
     * @brief Copy assignment operator.
     *
     * @param _Src The source queue to copy from.
     * @return queue<T>& Reference to the updated queue.
     */
    inline queue<T> &operator=(const queue<T> &_Src) {
        if (this != &_Src) {
            list = _Src.list;
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     *
     * @param _Src The source queue to move from.
     * @return queue<T>& Reference to the updated queue.
     */
    inline queue<T> &operator=(queue<T> &&_Src) {
        if (this != &_Src) {
            list = std::move(_Src.list);
        }
        return *this;
    }

    /**
     * @brief Push an element to the back of the queue.
     *
     * @param value The value to push.
     */
    inline void push(const T &value) { list.push_back(value); }

    /**
     * @brief Remove the element at the front of the queue.
     */
    inline void pop_front() { list.pop_front(); }

    /**
     * @brief Remove the element at the front of the queue (alias for pop_front).
     */
    inline void dequeue() { list.pop_front(); }

    /**
     * @brief Add an element to the back of the queue (alias for push).
     *
     * @param value The value to enqueue.
     */
    inline void enqueue(const T &value) { list.push_back(value); }

    /**
     * @brief Get a reference to the element at the front of the queue.
     *
     * @return T& Reference to the front element.
     */
    inline T &front() { return list.front(); }

    /**
     * @brief Get a reference to the element at the back of the queue.
     *
     * @return T& Reference to the back element.
     */
    inline T &back() { return list.back(); }

    /**
     * @brief Check if the queue is empty.
     *
     * @return bool True if the queue is empty, false otherwise.
     */
    inline bool empty() { return list.empty(); }

    /**
     * @brief Get the size of the queue.
     *
     * @return std::size_t The size of the queue.
     */
    inline std::size_t size() { return list.size; }

    /**
     * @brief Clear the queue (remove all elements).
     */
    inline void clear() { list.clear(); }

    /**
     * @brief Destructor.
     */
    inline ~queue() { _free_all(); }
};

} // namespace containers
} // namespace gtr

#endif // !QUEUE_H
