#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H
#include <utility>

namespace gtr {
namespace containers {

/**
 * @brief A circular buffer data structure that stores elements of type T.
 *
 * @tparam T The type of elements stored in the circular buffer.
 * Note: This is a fixed-size circular buffer, the buffer must be provided externally.
 * The buffer is not owned by the circular buffer and must be managed externally.
 * This container is not thread-safe by default, use a mutex to make it thread-safe.
 * To handle non-trivial stuctures this uses move semantics only.
 */
template <typename T> struct circular_buffer {
    struct circular_buffer_iterator {
        T *ptr;
        T *end;
        inline circular_buffer_iterator(T *p, T *e) : ptr(p), end(e) {}
        inline circular_buffer_iterator &operator++() {
            ptr = (ptr + 1) % end;
            return *this;
        }
        inline T &operator*() { return *ptr; }
        inline bool operator!=(const circular_buffer_iterator &other) const { return ptr != other.ptr; }
    };

    using iterator = circular_buffer_iterator;

    T *buffer; /**< The underlying array to store the elements. */
    unsigned int read;
    unsigned int write;
    unsigned int capacity;

    /**
     * @brief Constructs an empty circular buffer.
     */
    inline constexpr circular_buffer() : buffer(nullptr), read(0), write(0), capacity(0) {}

    /**
     * @brief Constructs a circular_buffer object with the given storage and capacity.
     *
     * @param storage A pointer to the storage array.
     * @param cap The capacity of the circular_buffer.
     */
    inline constexpr circular_buffer(T *storage, unsigned int cap) : buffer(storage), read(0), write(0), capacity(cap) {}

    /**
     * @brief Destroys the circular buffer.
     */
    inline ~circular_buffer() { clear(); };

    /**
     * @brief Adds an element atomically to the back of the circular buffer.
     * If the circular buffer is full, the oldest element is overwritten.
     *
     * @param value The value to be added to the circular buffer.
     */
    inline bool push_back(const T &value) {
        if ((write + 1) % capacity == read) {
            return false;
        }
        new (buffer + write) T(value);
        write = (write + 1) % capacity;
        return true;
    }

    /**
     * @brief Removes the element from the front of the circular buffer.
     * If the circular buffer is empty, no action is taken.
     */
    void clear() {
        while (read != write) {
            buffer[read].~T();
            read = (read + 1) % capacity;
        }
        read = write = 0;
    }

    /**
     * @brief Adds an element atomically to the back of the circular buffer.
     * If the circular buffer is full, the oldest element is overwritten.
     *
     * @param value The value to be added to the circular buffer.
     */
    inline bool push_back(T &&value) {
        if ((write + 1) % capacity == read) {
            return false;
        }
        new (buffer + write) T(std::move(value));
        write = (write + 1) % capacity;
        return true;
    }

    /**
     * @brief Removes the element from the front of the circular buffer.
     * If the circular buffer is empty, no action is taken.
     */
    inline bool pop_front(T &item) {
        if (read == write) {
            return false;
        }
        item = std::move(buffer[read]);
        buffer[read].~T();
        read = (read + 1) % capacity;
        return true;
    }

    /**
     * @brief Removes the element from the front of the circular buffer.
     * If the circular buffer is empty this will blow up.
     */
    T pop_front() {
        T item = std::move(buffer[read]);
        buffer[read].~T();
        read = (read + 1) % capacity;
        return item;
    }

    /**
     * @brief Checks if the circular buffer is empty.
     *
     * @return True if the circular buffer is empty, false otherwise.
     */
    bool empty() const { return read == write; }

    /**
     * @brief Checks if the circular buffer is full.
     *
     * @return True if the circular buffer is full, false otherwise.
     */
    bool full() const { return (write + 1) % capacity == read; }
    /**
     * @brief Returns an iterator to the beginning of the circular buffer.
     *
     * @return An iterator to the beginning of the circular buffer.
     */
    iterator begin() { return iterator(buffer + read, buffer + capacity); }

    /**
     * @brief Returns an iterator to the end of the circular buffer.
     *
     * @return An iterator to the end of the circular buffer.
     */
    iterator end() { return iterator(buffer + write, buffer + capacity); }

    /**
     * @brief Returns a constant iterator to the beginning of the circular buffer.
     *
     * @return A constant iterator to the beginning of the circular buffer.
     */
    const iterator begin() const { return iterator(buffer + read, buffer + capacity); }

    /**
     * @brief Returns a constant iterator to the end of the circular buffer.
     *
     * @return A constant iterator to the end of the circular buffer.
     */
    const iterator end() const { return iterator(buffer + write, buffer + capacity); }
};
} // namespace containers
} // namespace gtr

#endif // !CIRCULAR_BUFFER_H
