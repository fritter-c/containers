#ifndef DEQUE_H
#define DEQUE_H

#include <cassert>
#include <initializer_list>
#include <type_traits>
#include "allocator_base.h"
#include "allocators/allocators.h"
namespace gtr {
namespace containers {
/**
 * @brief Deque implementation using a block map.
 */
template <typename T, class Allocator = c_allocator<T>> struct deque : private _allocator_ebo<T, Allocator> {
    static constexpr std::size_t BLOCK_SIZE = 2048;
    using value_type = T;
    using allocator_type = Allocator;

    // Forward declaration of iterator classes
    class iterator;
    class const_iterator;

    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;
    using const_pointer = const T *;
    using const_reference = const T &;

    T **block_map;            ///< Pointer to the block map.
    std::size_t front_block;  ///< Index of the front block in the block map.
    std::size_t back_block;   ///< Index of the back block in the block map.
    std::size_t map_size;     ///< Total number of blocks in the block map.
    std::size_t start_offset; ///< Offset in the front block where elements start.
    std::size_t end_offset;   ///< Offset in the back block where elements end.

    /**
     * @brief Retrieves the allocator associated with the container.
     *
     * @return Allocator& Reference to the allocator used by the container.
     */
    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }
    /**
     * @brief Retrieves the allocator used by the deque.
     *
     * @return const Allocator& A constant reference to the allocator.
     */
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Default constructor.
     */
    inline deque() : _allocator_ebo<T, Allocator>() {
        block_map = nullptr;
        front_block = back_block = 0;
        map_size = 0;
        start_offset = end_offset = 0;
    }

    /**
     * @brief Constructor that initializes the deque with elements from an initializer list.
     *
     * @param init_list The initializer list of elements.
     */
    inline deque(std::initializer_list<T> init_list) : deque() {
        for (const auto &item : init_list) {
            push_back(item);
        }
    }

    /**
     * @brief Copy constructor.
     */
    deque(const deque &other) : _allocator_ebo<T, Allocator>(other.allocator()) {
        block_map = nullptr;
        front_block = back_block = 0;
        map_size = 0;
        start_offset = end_offset = 0;
        if (!other.empty()) {
            for (const T &value : other) {
                push_back(value);
            }
        }
    }

    /**
     * @brief Move constructor.
     */
    deque(deque &&other) noexcept : _allocator_ebo<T, Allocator>(std::move(other.allocator())) {
        block_map = other.block_map;
        map_size = other.map_size;
        front_block = other.front_block;
        back_block = other.back_block;
        start_offset = other.start_offset;
        end_offset = other.end_offset;

        other.block_map = nullptr;
        other.map_size = 0;
        other.front_block = other.back_block = 0;
        other.start_offset = other.end_offset = 0;
    }

    /**
     * @brief Destructor.
     */
    ~deque() {
        clear();
        if (block_map) {
            allocator().free(block_map, map_size * sizeof(T *));
        }
    }

    /**
     * @brief Copy assignment operator.
     */
    deque &operator=(const deque &other) {
        if (this != &other) {
            clear();
            allocator() = other.allocator();
            if (!other.empty()) {
                for (const T &value : other) {
                    push_back(value);
                }
            }
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     */
    deque &operator=(deque &&other) noexcept {
        if (this != &other) {
            clear();
            allocator() = std::move(other.allocator());
            block_map = other.block_map;
            map_size = other.map_size;
            front_block = other.front_block;
            back_block = other.back_block;
            start_offset = other.start_offset;
            end_offset = other.end_offset;

            other.block_map = nullptr;
            other.map_size = 0;
            other.front_block = other.back_block = 0;
            other.start_offset = other.end_offset = 0;
        }
        return *this;
    }

    /**
     * @brief Clears the contents of the deque.
     */
    void clear() {
        if (block_map) {
            // Destroy all the elements
            for (std::size_t block = front_block; block <= back_block; ++block) {
                T *curr_block = block_map[block];
                if (curr_block) {
                    std::size_t start = (block == front_block) ? start_offset : 0;
                    std::size_t end = (block == back_block) ? end_offset : BLOCK_SIZE;
                    for (std::size_t i = start; i < end; ++i) {
                        curr_block[i].~T();
                    }
                    allocator().free(curr_block, BLOCK_SIZE * sizeof(T));
                    block_map[block] = nullptr;
                }
            }
            allocator().free(block_map, map_size * sizeof(T *));
            block_map = nullptr;
        }
        front_block = back_block = 0;
        start_offset = end_offset = 0;
        map_size = 0;
    }

    /**
     * @brief Checks if the deque is empty.
     */
    bool empty() const { return size() == 0; }

    /**
     * @brief Returns the number of elements in the deque.
     */
    std::size_t size() const {
        if (block_map == nullptr) {
            return 0;
        }
        std::size_t total_size = 0;
        if (front_block == back_block) {
            total_size = end_offset - start_offset;
        } else {
            total_size += BLOCK_SIZE - start_offset;
            total_size += end_offset;
            total_size += (back_block - front_block - 1) * BLOCK_SIZE;
        }
        return total_size;
    }

    /**
     * @brief Adds an element to the end of the deque.
     */
    void push_back(const T &value) {
        if (block_map == nullptr) {
            initialize_map();
        } else if (end_offset == BLOCK_SIZE) {
            if (back_block + 1 == map_size) {
                resize_map();
            }
            back_block++;
            block_map[back_block] = allocator().malloc(BLOCK_SIZE * sizeof(T));
            end_offset = 0;
        }
        new (&(block_map[back_block][end_offset])) T(value);
        end_offset++;
    }

    /**
     * @brief Adds an element to the front of the deque.
     */
    void push_front(const T &value) {
        if (block_map == nullptr) {
            initialize_map();
        }
        if (start_offset == 0) {
            // If we are at the very front and cannot move left, resize the map first
            if (front_block == 0) {
                resize_map();
            }
            front_block--;
            block_map[front_block] = allocator().malloc(BLOCK_SIZE * sizeof(T));
            start_offset = BLOCK_SIZE;
        }
        start_offset--;
        new (&(block_map[front_block][start_offset])) T(value);
    }

    /**
     * @brief Removes the last element of the deque.
     */
    void pop_back() {
        if (empty()) {
            return;
        }
        end_offset--;
        block_map[back_block][end_offset].~T();
        if (end_offset == 0 && back_block != front_block) {
            allocator().free(block_map[back_block], BLOCK_SIZE * sizeof(T));
            block_map[back_block] = nullptr;
            back_block--;
            end_offset = BLOCK_SIZE;
        }
    }

    /**
     * @brief Removes the first element of the deque.
     */
    void pop_front() {
        if (empty()) {
            return;
        }
        block_map[front_block][start_offset].~T();
        start_offset++;
        if (start_offset == BLOCK_SIZE && front_block != back_block) {
            allocator().free(block_map[front_block], BLOCK_SIZE * sizeof(T));
            block_map[front_block] = nullptr;
            front_block++;
            start_offset = 0;
        }
    }

    /**
     * @brief Accesses the specified element with bounds checking.
     */
    T &at(std::size_t index) {
        if (index >= size()) {
            throw std::out_of_range("deque::at");
        }
        return (*this)[index];
    }

    const T &at(std::size_t index) const {
        if (index >= size()) {
            throw std::out_of_range("deque::at");
        }
        return (*this)[index];
    }

    /**
     * @brief Accesses the specified element without bounds checking.
     */
    T &operator[](std::size_t index) {
        assert(index < size());
        std::size_t block = front_block;
        std::size_t offset = start_offset + index;
        if (offset >= BLOCK_SIZE) {
            block += offset / BLOCK_SIZE;
            offset %= BLOCK_SIZE;
        }
        return block_map[block][offset];
    }

    const T &operator[](std::size_t index) const {
        assert(index < size());
        std::size_t block = front_block;
        std::size_t offset = start_offset + index;
        if (offset >= BLOCK_SIZE) {
            block += offset / BLOCK_SIZE;
            offset %= BLOCK_SIZE;
        }
        return block_map[block][offset];
    }

    /**
     * @brief Accesses the first element.
     */
    T &front() {
        assert(!empty());
        return block_map[front_block][start_offset];
    }

    const T &front() const {
        assert(!empty());
        return block_map[front_block][start_offset];
    }

    /**
     * @brief Accesses the last element.
     */
    T &back() {
        assert(!empty());
        std::size_t offset = end_offset == 0 ? BLOCK_SIZE - 1 : end_offset - 1;
        std::size_t block = end_offset == 0 ? back_block - 1 : back_block;
        return block_map[block][offset];
    }

    const T &back() const {
        assert(!empty());
        std::size_t offset = end_offset == 0 ? BLOCK_SIZE - 1 : end_offset - 1;
        std::size_t block = end_offset == 0 ? back_block - 1 : back_block;
        return block_map[block][offset];
    }

    /**
     * @brief Returns an iterator to the beginning.
     */
    iterator begin() { return iterator(this, front_block, start_offset); }

    /**
     * @brief Returns an iterator to the end.
     */
    iterator end() { return iterator(this, back_block, end_offset); }

    /**
     * @brief Returns a const iterator to the beginning.
     */
    const_iterator begin() const { return const_iterator(this, front_block, start_offset); }

    /**
     * @brief Returns a const iterator to the end.
     */
    const_iterator end() const { return const_iterator(this, back_block, end_offset); }

    /**
     * @brief Iterator class for deque.
     */
    class iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        iterator() : deque_ptr(nullptr), block_index(0), offset(0) {}
        iterator(deque *dq, std::size_t blk_idx, std::size_t off) : deque_ptr(dq), block_index(blk_idx), offset(off) {}

        reference operator*() const { return deque_ptr->block_map[block_index][offset]; }

        pointer operator->() const { return &(deque_ptr->block_map[block_index][offset]); }

        iterator &operator++() {
            if (++offset == BLOCK_SIZE) {
                offset = 0;
                block_index++;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator &operator--() {
            if (offset == 0) {
                offset = BLOCK_SIZE - 1;
                block_index--;
            } else {
                --offset;
            }
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        iterator &operator+=(difference_type n) {
            difference_type total_offset = (block_index - deque_ptr->front_block) * BLOCK_SIZE + offset - deque_ptr->start_offset + n;
            block_index = deque_ptr->front_block + (total_offset + deque_ptr->start_offset) / BLOCK_SIZE;
            offset = (total_offset + deque_ptr->start_offset) % BLOCK_SIZE;
            return *this;
        }

        iterator operator+(difference_type n) const {
            iterator tmp = *this;
            return tmp += n;
        }

        iterator &operator-=(difference_type n) { return *this += -n; }

        iterator operator-(difference_type n) const {
            iterator tmp = *this;
            return tmp -= n;
        }

        difference_type operator-(const iterator &other) const {
            difference_type this_pos = (block_index - deque_ptr->front_block) * BLOCK_SIZE + offset - deque_ptr->start_offset;
            difference_type other_pos = (other.block_index - deque_ptr->front_block) * BLOCK_SIZE + other.offset - deque_ptr->start_offset;
            return this_pos - other_pos;
        }

        bool operator==(const iterator &other) const {
            return deque_ptr == other.deque_ptr && block_index == other.block_index && offset == other.offset;
        }

        bool operator!=(const iterator &other) const { return !(*this == other); }

        bool operator<(const iterator &other) const {
            return block_index < other.block_index || (block_index == other.block_index && offset < other.offset);
        }

        bool operator>(const iterator &other) const { return other < *this; }

        bool operator<=(const iterator &other) const { return !(other < *this); }

        bool operator>=(const iterator &other) const { return !(*this < other); }

      private:
        deque *deque_ptr;
        std::size_t block_index;
        std::size_t offset;
    };

    /**
     * @brief Const iterator class for deque.
     */
    class const_iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;

        const_iterator() : deque_ptr(nullptr), block_index(0), offset(0) {}
        const_iterator(const deque *dq, std::size_t blk_idx, std::size_t off) : deque_ptr(dq), block_index(blk_idx), offset(off) {}

        const_iterator(const iterator &it) : deque_ptr(it.deque_ptr), block_index(it.block_index), offset(it.offset) {}

        reference operator*() const { return deque_ptr->block_map[block_index][offset]; }

        pointer operator->() const { return &(deque_ptr->block_map[block_index][offset]); }

        const_iterator &operator++() {
            if (++offset == BLOCK_SIZE) {
                offset = 0;
                block_index++;
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        const_iterator &operator--() {
            if (offset == 0) {
                offset = BLOCK_SIZE - 1;
                block_index--;
            } else {
                --offset;
            }
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        const_iterator &operator+=(difference_type n) {
            difference_type total_offset = (block_index - deque_ptr->front_block) * BLOCK_SIZE + offset - deque_ptr->start_offset + n;
            block_index = deque_ptr->front_block + (total_offset + deque_ptr->start_offset) / BLOCK_SIZE;
            offset = (total_offset + deque_ptr->start_offset) % BLOCK_SIZE;
            return *this;
        }

        const_iterator operator+(difference_type n) const {
            const_iterator tmp = *this;
            return tmp += n;
        }

        const_iterator &operator-=(difference_type n) { return *this += -n; }

        const_iterator operator-(difference_type n) const {
            const_iterator tmp = *this;
            return tmp -= n;
        }

        difference_type operator-(const const_iterator &other) const {
            difference_type this_pos = (block_index - deque_ptr->front_block) * BLOCK_SIZE + offset - deque_ptr->start_offset;
            difference_type other_pos = (other.block_index - deque_ptr->front_block) * BLOCK_SIZE + other.offset - deque_ptr->start_offset;
            return this_pos - other_pos;
        }

        bool operator==(const const_iterator &other) const {
            return deque_ptr == other.deque_ptr && block_index == other.block_index && offset == other.offset;
        }

        bool operator!=(const const_iterator &other) const { return !(*this == other); }

        bool operator<(const const_iterator &other) const {
            return block_index < other.block_index || (block_index == other.block_index && offset < other.offset);
        }

        bool operator>(const const_iterator &other) const { return other < *this; }

        bool operator<=(const const_iterator &other) const { return !(other < *this); }

        bool operator>=(const const_iterator &other) const { return !(*this < other); }

      private:
        const deque *deque_ptr;
        std::size_t block_index;
        std::size_t offset;
    };

    /**
     * @brief Initializes the block map.
     */
    void initialize_map() {
        map_size = 4;
        block_map = reinterpret_cast<T **>(allocator().malloc(map_size * sizeof(T *)));
        std::memset(block_map, 0, map_size * sizeof(T *));
        front_block = back_block = map_size / 2;
        block_map[front_block] = allocator().malloc(BLOCK_SIZE * sizeof(T));
        start_offset = end_offset = 0;
    }

#
    /**
     * @brief Resizes the block map.
     */
    void resize_map() {
        std::size_t new_map_size = map_size * 2;
        T **new_block_map = reinterpret_cast<T **>(allocator().malloc(new_map_size * sizeof(T *)));
        std::memset(new_block_map, 0, new_map_size * sizeof(T *));

        std::size_t blocks_in_use = back_block - front_block + 1;

        // Ensure we have at least one free block at the front after resizing
        std::size_t new_front_block = (new_map_size - blocks_in_use) / 2;
        if (new_front_block == 0) {
            new_front_block = 1;
        }

        std::memcpy(new_block_map + new_front_block, block_map + front_block, blocks_in_use * sizeof(T *));

        allocator().free(block_map, map_size * sizeof(T *));
        block_map = new_block_map;
        front_block = new_front_block;
        back_block = new_front_block + blocks_in_use - 1;
        map_size = new_map_size;
    }
};
} // namespace containers
} // namespace gtr

#endif // DEQUE_H
