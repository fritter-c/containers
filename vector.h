#ifndef VECTOR_H
#define VECTOR_H
#include <algorithm>
#include <cassert>
#include <concepts>
#include <initializer_list>
#include <limits>
#include <numeric>
#include <type_traits>
#include "allocator_base.h"
#include "allocators/allocators.h"

namespace gtr {
namespace containers {
template <typename U>
concept has_basic_comp = requires(U t) {
    { t == t } -> std::same_as<bool>;
    { t < t } -> std::same_as<bool>;
    { t > t } -> std::same_as<bool>;
};
template <class T, class Allocator = c_allocator<T>> struct vector : _allocator_ebo<T, Allocator> {
    using value_type = T;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    using allocator_type = Allocator;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = std::size_t;
    using diffence_type = std::ptrdiff_t;
    static constexpr size_type npos = static_cast<size_type>(-1);

    T *data;         ///< Pointer to the beginning of the allocated storage.
    T *data_end;     ///< Pointer to one past the last constructed element.
    T *capacity_end; ///< Pointer to one past the end of the allocated storage.

    /**
     * @brief Retrieves the allocator associated with the container.
     *
     * This function returns a reference to the allocator used by the container.
     * The allocator is responsible for managing the memory allocation and
     * deallocation for the elements stored in the container.
     *
     * @return Allocator& Reference to the allocator used by the container.
     */
    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }
    /**
     * @brief Retrieves the allocator used by the vector.
     *
     * This function returns a constant reference to the allocator
     * used by the vector. The allocator is responsible for managing
     * the memory allocation and deallocation for the elements of the vector.
     *
     * @return const Allocator& A constant reference to the allocator.
     */
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    /**
     * @brief Returns the current number of elements in the vector.
     *
     * This function computes the size by subtracting the `data` pointer from the `data_end` pointer.
     *
     * @return size_type The number of elements in the vector.
     */
    inline size_type size() const { return data_end - data; }

    /**
     * @brief Returns the capacity of the vector.
     *
     * This function computes the capacity by subtracting the `data` pointer from the `capacity_end` pointer.
     *
     * @return size_type The capacity of the vector.
     */
    inline size_type capacity() const { return capacity_end - data; }

    /**
     * @brief Returns the size of the vector in bytes.
     *
     * This function calculates the total memory size occupied by the elements
     * of the vector by multiplying the number of elements (size) by the size
     * of each element (sizeof(T)).
     *
     * @return size_type The size of the vector in bytes.
     */
    inline size_type size_in_bytes() const { return size() * sizeof(T); }

    /**
     * @brief Destroys all elements in the container.
     *
     * This function iterates over all elements in the container and explicitly
     * calls their destructor. It ensures that all resources held by the elements
     * are properly released. This function should be used with caution as it
     * manually invokes destructors, which is generally handled automatically
     * by the container's destructor.
     */
    inline void destroy_all() {
        if (data) {
            for (auto &c : *this) {
                c.~T();
            }
        }
    }

    /**
     * @brief Sets the metadata size for the allocator.
     *
     * This function sets the metadata size for the allocator associated with the vector.
     * It is only available when the allocator type is `meta_allocator<T>`.
     *
     * @param size The size of the metadata to set.
     */
    inline void set_metadata_size(size_type size)
        requires(std::is_same_v<Allocator, meta_allocator<T>>)
    {

        assert(empty());
        _free_all();
        data_end = capacity_end = data;
        allocator().metadata_size = size;
    }

    /**
     * @brief Retrieves the metadata size from the allocator.
     *
     * This function returns the metadata size from the allocator associated with the vector.
     * It is only available when the allocator type is `meta_allocator<T>`.
     *
     * @return std::size_t The size of the metadata.
     */
    inline std::size_t get_metadata_size() const
        requires(std::is_same_v<Allocator, meta_allocator<T>>)
    {
        return allocator().metadata_size;
    }

    /**
     * @brief Retrieves a pointer to the metadata.
     *
     * This function returns a pointer to the metadata associated with the vector.
     * It is only available when the allocator type is `meta_allocator<T>`.
     *
     * @tparam U The type of the metadata.
     * @return U* Pointer to the metadata, or nullptr if the vector is empty.
     */
    template <typename U>
    inline U *get_metadata()
        requires(std::is_same_v<Allocator, meta_allocator<T>> && std::is_standard_layout_v<U>)
    {
        if (data)
            return reinterpret_cast<U *>(allocator().get_metadata(data));
        return nullptr;
    }

    /**
     * @brief Retrieves a constant pointer to the object.
     *
     * This function returns a constant pointer to the object associated with the vector.
     * It is only available when the allocator type is `object_allocator<T, U>`.
     *
     * @tparam U The type of the object.
     * @return const U* Constant pointer to the object, or nullptr if the vector is empty.
     */
    template <typename Alloc = Allocator>
        requires(is_object_allocator_v<Alloc>)
    const inline typename Alloc::object_type *get_object() const {
        if (data)
            return allocator().get_object(data);
        return nullptr;
    }

    /**
     * @brief Retrieves a pointer to the object.
     *
     * This function returns a pointer to the object associated with the vector.
     * It is only available when the allocator type is `object_allocator<T, U>`.
     *
     * @tparam U The type of the object.
     * @return U* Pointer to the object, or nullptr if the vector is empty.
     */
    template <typename Alloc = Allocator>
        requires(is_object_allocator_v<Alloc>)
    inline typename Alloc::object_type *get_object() {
        if (data)
            return allocator().get_object(data);
        return nullptr;
    }

    /**
     * @brief Retrieves a constant pointer to the metadata.
     *
     * This function returns a constant pointer to the metadata associated with the vector.
     * It is only available when the allocator type is `meta_allocator<T>`.
     *
     * @tparam U The type of the metadata.
     * @return const U* Constant pointer to the metadata, or nullptr if the vector is empty.
     */
    template <typename U>
    inline const U *get_metadata() const
        requires(std::is_same_v<Allocator, meta_allocator<T>> && std::is_standard_layout_v<U>)
    {
        if (data)
            return reinterpret_cast<const U *>(allocator().get_metadata(data));
        return nullptr;
    }

    /**
     * @brief Creates the metadata for the vector.
     *
     * This function creates the metadata for the vector. It is only available when
     * the allocator type is `meta_allocator<T>`. The metadata is create by initializing
     * the vector with a single element.
     */
    inline void create_metadata()
        requires(std::is_same_v<Allocator, meta_allocator<T>>)
    {
        reserve(1);
    }

    /**
     * @brief Creates the object for the vector.
     *
     * This function creates the object for the vector. It is only available when
     * the allocator type is `object_allocator<T, T>`. The object is created by initializing
     * the vector with a single element.
     */
    template <typename U>
    inline void create_object()
        requires(std::is_same_v<Allocator, object_allocator<T, U>>)
    {
        reserve(1);
    }

    /**
     * @brief Frees all allocated memory for the vector.
     *
     * This function destroys all elements in the vector and then frees the memory
     * allocated for the vector's data.
     */
    inline void _free_all() {
        destroy_all();
        allocator().free(data, capacity());
        data = nullptr;
    }

    /**
     * @brief Default constructor for the vector class.
     *
     * This constructor initializes an empty vector with no elements.
     * It sets the internal data pointers to nullptr.
     * The allocator is also initialized using the default allocator.
     */
    inline vector() : _allocator_ebo<T, Allocator>(), data(nullptr), data_end(nullptr), capacity_end(nullptr) {}

    /**
     * @brief Constructs a vector with a specified initial capacity.
     *
     * This constructor initializes a vector with a given capacity, allocating
     * memory for the elements but not initializing them. The size of the vector
     * is set to 0.
     *
     * @param _Capacity The initial capacity of the vector.
     */
    inline vector(size_type _Capacity) : _allocator_ebo<T, Allocator>() {
        data = nullptr;
        data_end = nullptr;
        capacity_end = nullptr;
        resize(_Capacity);
    }

    /**
     * @brief Constructs a vector with a specified initial capacity and value.
     *
     * This constructor initializes a vector with a given capacity and value.
     * It allocates memory for the elements, sets the size to the capacity, and
     * constructs each element in the vector using the provided value.
     *
     * @param _Size The initial capacity of the vector.
     * @param value The value to initialize each element with.
     */
    inline vector(size_type _Size, const T &value) : _allocator_ebo<T, Allocator>() {
        data = allocator().malloc(_Size);
        data_end = data + _Size;
        capacity_end = data_end;
        for (size_type i = 0; i < _Size; i++) {
            new (data + i) T(value);
        }
    }

    /**
     * @brief Constructs a vector from an initializer list.
     *
     * This constructor initializes the vector with the elements provided in the
     * initializer list. It allocates memory for the elements, sets the initial
     * size to 0, and the capacity to the size of the initializer list. Each
     * element from the initializer list is then added to the vector.
     *
     * @param init_list The initializer list containing elements to initialize the vector with.
     */
    inline vector(std::initializer_list<T> init_list) : _allocator_ebo<T, Allocator>() {
        data = allocator().malloc(init_list.size());
        data_end = data;
        capacity_end = data + init_list.size();
        for (const auto &item : init_list) {
            new (data_end) T(item);
            ++data_end;
        }
    }

    /**
     * @brief Copy constructor for the vector class.
     *
     * This constructor creates a new vector by copying the contents of another vector.
     * It allocates memory for the new vector, copies the size and capacity, and
     * constructs each element in the new vector by copying the corresponding element
     * from the other vector.
     *
     * @param other The vector to be copied.
     */
    inline vector(const vector<T, Allocator> &other)
        : _allocator_ebo<T, Allocator>(other.allocator())

    {
        data = allocator().malloc(other.capacity());
        capacity_end = data + other.capacity();
        data_end = data + other.size();
        for (size_type i = 0; i < size(); i++) {
            new (data + i) T(other.data[i]);
        }

        if constexpr (is_object_allocator_v<Allocator>) {
            if (other.capacity()) {
                auto *a = get_object();
                auto *b = other.get_object();
                *a = *b;
            }
        }
    }

    /**
     * @brief Move constructor for the vector class.
     *
     * This constructor initializes the vector by transferring ownership of the data
     * from another vector instance. The other vector is left in a valid but unspecified
     * state, typically empty.
     *
     * @param other The vector instance to move from.
     *
     * @note This constructor is marked noexcept, indicating that it does not throw exceptions.
     */
    inline vector(vector<T, Allocator> &&other) noexcept : _allocator_ebo<T, Allocator>(std::move(other.allocator())) {
        data = other.data;
        data_end = other.data_end;
        capacity_end = other.capacity_end;
        other.data = nullptr;
        other.data_end = nullptr;
        other.capacity_end = nullptr;
        // Even if the allocator is an object allocator, we can still move the object because its only a pointer
    }

    /**
     * @brief Assignment operator for the vector class.
     *
     * This operator assigns the contents of another vector to this vector.
     * It first checks for self-assignment and then deallocates any existing
     * data if necessary. It then allocates new memory and copies the elements
     * from the other vector to this vector.
     *
     * @param other The vector to be assigned to this vector.
     * @return A reference to this vector.
     */
    inline vector<T, Allocator> &operator=(const vector<T, Allocator> &other) {
        if (this != &other) {
            if (data) {
                _free_all();
            }
            allocator() = other.allocator();
            data = allocator().malloc(other.capacity());
            capacity_end = data + other.capacity();
            data_end = data + other.size();
            for (size_type i = 0; i < size(); i++) {
                new (data + i) T(other.data[i]);
            }
            if constexpr (is_object_allocator_v<Allocator>) {
                if (other.capacity()) {
                    auto *obj = other.get_object();
                    auto *this_obj = get_object();
                    *obj = *this_obj;
                }
            }
        }
        return *this;
    }

    /**
     * @brief Move assignment operator for the vector class.
     *
     * This operator transfers ownership of the resources from the given vector
     * to the current vector. It first checks for self-assignment and then
     * deallocates any existing resources. The allocator, data pointer, size,
     * and capacity are moved from the source vector to the current vector.
     * The source vector is left in a valid but unspecified state.
     *
     * @param other The vector to move from.
     * @return A reference to the current vector.
     */
    inline vector<T, Allocator> &operator=(vector<T, Allocator> &&other) noexcept {
        if (this != &other) {
            if (data) {
                _free_all();
            }
            allocator() = std::move(other.allocator());
            data = other.data;
            data_end = other.data_end;
            capacity_end = other.capacity_end;
            other.data = nullptr;
            other.data_end = nullptr;
            other.capacity_end = nullptr;
        }
        return *this;
    }

    /**
     * @brief Destructor for the vector class.
     *
     * This destructor is responsible for cleaning up the resources used by the vector.
     * If the vector contains data, it calls the _free_all() method to release all allocated memory.
     */
    inline ~vector() {
        if (data) {
            _free_all();
        }
        data = nullptr;
        data_end = nullptr;
        capacity_end = nullptr;
    }

    /**
     * @brief Adds a new element to the end of the vector.
     *
     * This function appends the given value to the end of the vector. If the
     * current size of the vector is equal to its capacity, the capacity is
     * increased (doubled or set to 1 if it was 0) to accommodate the new element.
     *
     * @param value The value to be added to the vector.
     */
    inline void push_back(const T &value) {
        if (data_end == capacity_end) {
            reserve(capacity() ? capacity() * 2 : 1);
        }
        new (data_end) T(value);
        ++data_end;
    }

    /**
     * @brief Adds a new element to the end of the vector using move semantics.
     *
     * This function appends the given value to the end of the vector by moving it.
     * If the current size of the vector is equal to its capacity, the capacity is
     * increased (doubled or set to 1 if it was 0) to accommodate the new element.
     *
     * @param value The value to be moved into the vector.
     */
    inline void push_back(T &&value) {
        if (data_end == capacity_end) {
            reserve(capacity() ? capacity() * 2 : 1);
        }
        new (data_end) T(std::move(value));
        ++data_end;
    }

    /**
     * @brief Appends the elements of another vector to the end of the current vector.
     *
     * This function takes another vector as an argument and appends its elements
     * to the end of the current vector. If the combined size of the current vector
     * and the other vector exceeds the current capacity, the function will reserve
     * additional space to accommodate the new elements.
     *
     * @param other The vector whose elements are to be appended to the current vector.
     */
    inline void push_back(const vector<T, Allocator> &other) {
        if (size() + other.size() > capacity()) {
            reserve(size() + other.size());
        }
        for (size_type i = 0; i < other.size(); i++) {
            new (data_end) T(other.data[i]);
            ++data_end;
        }
    }

    /**
     * @brief Appends the elements from another vector to the end of the current vector.
     *
     * This function takes an rvalue reference to another vector and appends its elements
     * to the current vector. If the combined size of the current vector and the other vector
     * exceeds the current capacity, the function reserves enough space to accommodate all elements.
     *
     * @param other The vector whose elements are to be appended. This vector will be moved from.
     */
    inline void push_back(vector<T, Allocator> &&other) {
        if (size() + other.size() > capacity()) {
            reserve(size() + other.size());
        }
        for (size_type i = 0; i < other.size(); i++) {
            new (data_end) T(std::move(other.data[i]));
            ++data_end;
        }
    }

    /**
     * @brief Removes the last element from the vector, reducing the size by one.
     *
     * This function calls the destructor of the last element and then decreases
     * the size of the vector. It is a no-op if the vector is already empty.
     */
    inline void pop_back() {
        if (data_end > data) {
            --data_end;
            data_end->~T();
        }
    }

    /**
     * @brief Removes the specified number of elements from the end of the vector.
     *
     * This function destructs the elements at the end of the vector and reduces the size of the vector by the specified count.
     * If the count is greater than the current size of the vector, it will remove all elements.
     *
     * @param count The number of elements to remove from the end of the vector.
     */
    inline void pop_back(size_type count) {
        if (count > size()) {
            count = size();
        }
        for (size_type i = 0; i < count; i++) {
            --data_end;
            data_end->~T();
        }
    }

    /**
     * @brief Clears the vector by destructing all elements and resetting the size to zero.
     *
     * This function iterates through the vector, calling the destructor for each element,
     * and then sets the size of the vector to zero. After calling this function, the vector
     * will be empty.
     */
    inline void clear() {
        while (data_end != data) {
            --data_end;
            data_end->~T();
        }
    }

    /**
     * @brief Reserves storage for at least the specified number of elements.
     *
     * This function increases the capacity of the container to at least the specified
     * number of elements. If the current capacity is already greater than or equal to
     * the specified number, no action is taken. Otherwise, the storage is reallocated
     * to accommodate the new capacity.
     *
     * @param new_capacity The minimum number of elements to reserve storage for.
     */
    inline void reserve(size_type new_capacity) {
        if constexpr (std::is_trivially_copyable<T>::value && !is_object_allocator_v<Allocator>) {
            if (new_capacity > capacity()) {
                size_type sz = size();
                data = allocator().realloc(data, new_capacity, capacity());
                data_end = data + sz;
                capacity_end = data + new_capacity;
            }
        } else {
            if (new_capacity > capacity()) {
                T *new_data = allocator().malloc(new_capacity);
                T *new_data_end = new_data;
                for (T *ptr = data; ptr != data_end; ++ptr, ++new_data_end) {
                    new (new_data_end) T(std::move(*ptr));
                    ptr->~T();
                }
                if (data) {
                    if constexpr (is_object_allocator_v<Allocator>) {
                        auto *obj = get_object();
                        auto *new_obj = allocator().get_object(new_data);
                        *new_obj = std::move(*obj);
                    }
                    allocator().free(data, capacity());
                }
                data = new_data;
                data_end = new_data_end;
                capacity_end = data + new_capacity;
            }
        }
    }

    /**
     * @brief Resizes the container to contain the specified number of elements.
     *
     * If the new size is greater than the current size, additional default-constructed elements are appended.
     * If the new size is less than the current size, elements are destroyed to reduce the size.
     *
     * @param new_size The new size of the container.
     */
    inline void resize(size_type new_size) {
        if (new_size > size()) {
            reserve(new_size);
            while (data_end != data + new_size) {
                new (data_end) T();
                ++data_end;
            }
        } else if (new_size < size()) {
            while (data_end != data + new_size) {
                --data_end;
                data_end->~T();
            }
        }
    }

    /**
     * @brief Resizes the container to contain the specified number of elements.
     *
     * If the new size is greater than the current size, additional copies of the specified value are appended.
     * If the new size is less than the current size, elements are destroyed to reduce the size.
     *
     * @param new_size The new size of the container.
     * @param value The value to be appended to the container.
     */
    inline void resize(size_type new_size, const T &value) {
        if (new_size > size()) {
            reserve(new_size);
            while (data_end != data + new_size) {
                new (data_end) T(value);
                ++data_end;
            }
        } else if (new_size < size()) {
            while (data_end != data + new_size) {
                --data_end;
                data_end->~T();
            }
        }
    }

    /**
     * @brief Assigns new contents to the vector, replacing its current contents.
     *
     * This function replaces the current contents of the vector with the elements
     * from the provided initializer list. It first clears the vector and then appends
     * the elements from the initializer list.
     *
     * @param init_list The initializer list containing the elements to assign to the vector.
     */
    void assign_buffer(T *buffer, size_type size)
        requires std::is_same_v<Allocator, c_allocator<T>>
    {
        if (data) {
            _free_all();
        }
        data = buffer;
        data_end = data + size;
        capacity_end = data_end;
    }

    /**
     * @brief Assigns the contents of another vector to this vector.
     *
     * This function replaces the current contents of the vector with the elements
     * from the provided vector. It first checks for self-assignment and then deallocates
     * any existing data if necessary. It then allocates new memory and copies the elements
     * from the other vector to this vector.
     *
     * @param other The vector whose contents are to be assigned to this vector.
     */
    inline void assign(const vector<T, Allocator> &other) {
        if (this != &other) {
            if (data) {
                _free_all();
            }
            allocator() = other.allocator();
            data = allocator().malloc(other.capacity());
            capacity_end = data + other.capacity();
            data_end = data + other.size();
            for (size_type i = 0; i < size(); i++) {
                new (data + i) T(other.data[i]);
            }
        }
    }

    /**
     * @brief Assigns the contents of an initializer list to this vector.
     *
     * This function replaces the current contents of the vector with the elements
     * from the provided initializer list. It first clears the vector and then appends
     * the elements from the initializer list.
     *
     * @param init_list The initializer list containing the elements to assign to the vector.
     */
    inline void assign(std::initializer_list<T> init_list) {
        clear();
        reserve(init_list.size());
        for (const auto &item : init_list) {
            new (data_end) T(item);
            ++data_end;
        }
    }

    /**
     * @brief Assigns the specified number of copies of a value to the vector.
     *
     * This function replaces the current contents of the vector with the specified
     * number of copies of the given value. It first clears the vector and then appends
     * the specified number of copies of the value.
     *
     * @param count The number of copies to assign to the vector.
     * @param value The value to be assigned to the vector.
     */
    inline void assign(size_type count, const T &value) {
        clear();
        reserve(count);
        for (size_type i = 0; i < count; i++) {
            new (data_end) T(value);
            ++data_end;
        }
    }

    /**
     * @brief Reverses the order of the elements in the vector.
     *
     * This function reverses the order of the elements in the vector in place.
     * After calling this function, the first element will become the last, the second
     * element will become the second to last, and so on.
     */
    inline void reverse() { std::reverse(begin(), end()); }

    /**
     * @brief Accesses the element at the specified index.
     *
     * This operator provides direct access to the element at the given index
     * in the container. It does not perform bounds checking.
     *
     * @param index The position of the element to access.
     * @return T& Reference to the element at the specified index.
     */
    inline T &operator[](size_type index) { return data[index]; }
    inline const T &operator[](size_type index) const { return data[index]; }

    /**
     * @brief Returns a pointer to the beginning of the container.
     *
     * This function provides access to the first element in the container.
     *
     * @return T* Pointer to the first element in the container.
     */
    inline T *begin() { return data; }
    /**
     * @brief Returns a pointer to the end of the container.
     *
     * This function provides a pointer to the element following the last element
     * of the container. This pointer can be used to iterate over the container
     * in a range-based for loop or other iteration constructs.
     *
     * @return T* Pointer to the element following the last element of the container.
     */
    inline T *end() { return data_end; }
    /**
     * @brief Returns a constant pointer to the beginning of the vector.
     *
     * This function provides a way to access the first element of the vector
     * in a read-only manner. It returns a constant pointer to the data stored
     * in the vector.
     *
     * @return const T* Pointer to the first element of the vector.
     */
    inline const T *begin() const { return data; }
    inline const T *end() const { return data_end; }

    /**
     * @brief Returns a reference to the first element in the vector.
     *
     * This function asserts that the vector is not empty and then returns
     * a reference to the first element in the underlying data array.
     *
     * @return T& Reference to the first element in the vector.
     * @throws Assertion failure if the vector is empty.
     */
    inline T &front() {
        assert(size() > 0);
        return *data;
    }

    /**
     * @brief Returns a reference to the last element in the vector.
     *
     * This function asserts that the vector is not empty and then returns
     * a reference to the last element in the vector.
     *
     * @return T& Reference to the last element in the vector.
     * @throws Assertion failure if the vector is empty.
     */
    inline T &back() {
        assert(size() > 0);
        return *(data_end - 1);
    }

    /**
     * @brief Returns a constant reference to the first element in the vector.
     *
     * This function asserts that the vector is not empty and then returns a
     * constant reference to the first element in the vector.
     *
     * @return A constant reference to the first element in the vector.
     * @throws Assertion failure if the vector is empty.
     */
    inline const T &front() const {
        assert(size() > 0);
        return *data;
    }

    /**
     * @brief Returns a constant reference to the last element in the vector.
     *
     * This function asserts that the vector is not empty and then returns a
     * constant reference to the last element in the vector.
     *
     * @return A constant reference to the last element in the vector.
     * @throws Assertion failure if the vector is empty.
     */
    inline const T &back() const {
        assert(size() > 0);
        return *(data_end - 1);
    }

    /**
     * @brief Searches for the first occurrence of a value in the vector.
     *
     * This function searches for the first occurrence of the specified value
     * in the vector. If the value is found, the function returns the index of
     * the first occurrence; otherwise, it returns npos.
     *
     * @param value The value to search for in the vector.
     * @param ordered Whether the vector is ordered (sorted) or not.
     * @return The index of the first occurrence of the value, or npos if not found.
     */
    inline size_type find(const T &value, bool ordered = false) const
        requires has_basic_comp<T>
    {
        if (!ordered) {
            for (size_type i = 0; i < size(); i++) {
                if (data[i] == value) {
                    return i;
                }
            }
            return npos;
        } else {
            size_type low = 0;
            size_type high = size() - 1;
            while (low <= high) {
                size_type mid = low + (high - low) / 2;
                if (data[mid] == value) {
                    return mid;
                }
                if (data[mid] < value) {
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }
            return npos;
        }
    }

    /**
     * @brief Finds the index of the largest element in the vector.
     *
     * This function iterates through the vector to find the index of the largest element.
     * It requires that the elements of the vector support basic comparison operations.
     *
     * @return size_type The index of the largest element in the vector.
     *         If the vector is empty, returns npos.
     */
    inline size_type biggest() const
        requires has_basic_comp<T>
    {
        size_type index = 0;
        for (size_type i = 1; i < size(); i++) {
            if (data[i] > data[index]) {
                index = i;
            }
        }

        return size() ? index : npos;
    }

    /**
     * @brief Finds the index of the smallest element in the vector.
     *
     * This function iterates through the vector to find the index of the smallest element.
     * It requires that the elements of the vector support basic comparison operations.
     *
     * @return size_type The index of the smallest element in the vector.
     *         If the vector is empty, returns npos.
     */
    inline size_type smallest() const
        requires has_basic_comp<T>
    {
        size_type index = 0;
        for (size_type i = 1; i < size(); i++) {
            if (data[i] < data[index]) {
                index = i;
            }
        }

        return size() ? index : npos;
    }

    /**
     * @brief Computes the sum of all elements in the vector.
     *
     * This function calculates the sum of all elements in the vector.
     * It requires that the elements of the vector are of an arithmetic type.
     *
     * @return T The sum of all elements in the vector.
     */
    inline T sum() const
        requires std::is_arithmetic_v<T>
    {
        return std::accumulate(begin(), end(), T(0));
    }

    /**
     * @brief Computes the product of all elements in the vector.
     *
     * This function calculates the product of all elements in the vector.
     * It requires that the elements of the vector are of an arithmetic type.
     *
     * @return T The product of all elements in the vector.
     */
    inline T product() const
        requires std::is_arithmetic_v<T>
    {
        return std::accumulate(begin(), end(), T(1), std::multiplies<T>());
    }

    /**
     * @brief Computes the mean (average) of all elements in the vector.
     *
     * This function calculates the mean of all elements in the vector.
     * It requires that the elements of the vector are of an arithmetic type.
     *
     * @return T The mean of all elements in the vector.
     */
    inline T mean() const
        requires std::is_arithmetic_v<T>
    {
        return sum() / size();
    }

    /**
     * @brief Computes the median of all elements in the vector.
     *
     * This function calculates the median of all elements in the vector.
     * It requires that the elements of the vector are of an arithmetic type.
     *
     * @return T The median of all elements in the vector.
     */
    inline T median() const
        requires std::is_arithmetic_v<T>
    {
        vector<T, Allocator> copy(*this);
        std::sort(copy.begin(), copy.end());
        if (size() % 2 == 0) {
            return (copy[size() / 2 - 1] + copy[size() / 2]) / 2;
        }
        return copy[size() / 2];
    }

    /**
     * @brief Constructs an element in-place at the end of the container.
     *
     * This function constructs a new element at the end of the container using the provided arguments.
     * If the current size of the container is equal to its capacity, the capacity is increased (doubled or set to 1 if it was 0).
     * The new element is constructed in-place using placement new and the provided arguments.
     *
     * @tparam Args Types of the arguments to be forwarded to the constructor of the element.
     * @param _Args Arguments to be forwarded to the constructor of the element.
     */
    template <typename... Args> inline void emplace_back(Args &&..._Args) {
        if (data_end == capacity_end) {
            reserve(capacity() ? capacity() * 2 : 1);
        }
        new (data_end) T(std::forward<Args>(_Args)...);
        ++data_end;
    }

    /**
     * @brief Reduces the capacity of the container to fit its size.
     *
     * This function reduces the capacity of the container to match its current size.
     * If the current size is less than the capacity, it reallocates the internal
     * storage to free up unused memory.
     */
    inline void shrink_to_fit() {
        if constexpr (std::is_trivially_copyable<T>::value) {
            if (capacity() > size()) {
                size_type sz = size();
                data = allocator().realloc(data, sz);
                data_end = data + sz;
                capacity_end = data_end;
            }
        } else {
            if (size() < capacity()) {
                T *new_data = allocator().malloc(size());
                T *new_data_end = new_data;
                for (T *ptr = data; ptr != data_end; ++ptr, ++new_data_end) {
                    new (new_data_end) T(std::move(*ptr));
                    ptr->~T();
                }
                if (data) {
                    allocator().free(data, capacity());
                }
                data = new_data;
                data_end = new_data_end;
                capacity_end = data + size();
            }
        }
    }

    /**
     * @brief Erases the element at the specified index from the vector.
     *
     * This function removes the element at the given index by calling its destructor,
     * then shifts all subsequent elements one position to the left to fill the gap.
     * The size of the vector is decreased by one.
     *
     * @param index The index of the element to be erased.
     * @note If the index is out of bounds (greater than or equal to the current size),
     *       the function does nothing.
     */
    inline void erase(size_type index) {
        if constexpr (std::is_trivially_copyable<T>::value) {
            if (index < size()) {
                memmove(data + index, data + index + 1, (size() - index - 1) * sizeof(T));
                --data_end;
            }
        } else {
            if (index < size()) {
                data[index].~T();
                for (size_type i = index; i < size() - 1; ++i) {
                    new (data + i) T(std::move(data[i + 1]));
                    data[i + 1].~T();
                }
                --data_end;
            }
        }
    }

    /**
     * @brief Erases elements from the vector within the specified range.
     *
     * This function removes elements in the range [start_index, end_index) from the vector.
     * The elements in the specified range are destroyed, and the remaining elements
     * are shifted to fill the gap. The size of the vector is reduced accordingly.
     *
     * @param start_index The starting index of the range to erase (inclusive).
     * @param end_index The ending index of the range to erase (exclusive).
     */
    inline void erase(size_type start_index, size_type end_index) {
        if constexpr (std::is_trivially_copyable<T>::value) {
            if (start_index < size() && end_index <= size() && start_index < end_index) {
                memmove(data + start_index, data + end_index, (size() - end_index) * sizeof(T));
                data_end -= end_index - start_index;
            } else {
                if (start_index < size() && end_index <= size() && start_index < end_index) {
                    for (size_type i = start_index; i < end_index; ++i) {
                        data[i].~T();
                    }
                    size_type move_count = size() - end_index;
                    for (size_type i = 0; i < move_count; ++i) {
                        new (data + start_index + i) T(std::move(data[end_index + i]));
                        data[end_index + i].~T();
                    }
                    data_end -= (end_index - start_index);
                }
            }
        }
    }

    /**
     * @brief Erases the element at the specified iterator position.
     *
     * This function removes the element at the specified iterator position by calling
     * its destructor, then shifts all subsequent elements one position to the left to fill the gap.
     * The size of the vector is decreased by one.
     *
     * @param it The iterator pointing to the element to be erased.
     */
    inline void erase(iterator it) { erase(it - begin()); }

    /**
     * @brief Erases elements from the vector within the specified range.
     *
     * This function removes elements in the range [start_it, end_it) from the vector.
     * The elements in the specified range are destroyed, and the remaining elements
     * are shifted to fill the gap. The size of the vector is reduced accordingly.
     *
     * @param start_it The starting iterator of the range to erase (inclusive).
     * @param end_it The ending iterator of the range to erase (exclusive).
     */
    inline void erase(iterator start_it, iterator end_it) { erase(start_it - begin(), end_it - begin()); }

    /**
     * @brief Adds a range of elements to the vector.
     *
     * This function inserts elements from the range defined by the pointers
     * `start_ptr` and `end_ptr` into the vector. If the current capacity of the vector
     * is insufficient to hold the new elements, the capacity is increased.
     *
     * @param start_ptr Pointer to the beginning of the range of elements to be added.
     * @param end_ptr Pointer to the end of the range of elements to be added.
     */
    inline void push_range(const T *start_ptr, const T *end_ptr) {
        if constexpr (std::is_trivially_copyable<T>::value) {
            size_type count = end_ptr - start_ptr;
            if (size() + count > capacity()) {
                reserve(size() + count);
            }
            memcpy(data_end, start_ptr, count * sizeof(T));
            data_end += count;
        } else {
            size_type count = end_ptr - start_ptr;
            if (size() + count > capacity()) {
                reserve(size() + count);
            }
            for (const T *ptr = start_ptr; ptr != end_ptr; ++ptr) {
                new (data_end) T(*ptr);
                ++data_end;
            }
        }
    }

    /**
     * @brief Inserts an element at the specified index in the vector.
     *
     * This function inserts the given value at the specified index in the vector.
     * If the vector's size is equal to its capacity, the capacity is doubled (or set to 1 if it was 0).
     * Elements from the specified index to the end of the vector are shifted one position to the right
     * to make space for the new element.
     *
     * @param index The position at which to insert the new element.
     * @param value The value to be inserted.
     */
    inline void insert(size_type index, const T &value) {
        if (index <= size()) {
            if (data_end == capacity_end) {
                reserve(capacity() ? capacity() * 2 : 1);
            }
            new (data_end) T(); // Default construct at the end to extend the size
            ++data_end;
            for (size_type i = size() - 1; i > index; --i) {
                data[i] = std::move(data[i - 1]);
            }
            data[index] = value;
        }
    }

    /**
     * @brief Swaps the elements at the specified indices.
     *
     * This function swaps the elements located at indices `a` and `b` within the container.
     * It ensures that the indices are within the valid range before performing the swap.
     *
     * @param a The index of the first element to swap.
     * @param b The index of the second element to swap.
     */
    inline void swap_elements(size_type a, size_type b) {
        if (a < size() && b < size()) {
            T temp = std::move(data[a]);
            data[a] = std::move(data[b]);
            data[b] = std::move(temp);
        }
    }

    /**
     * @brief Swaps the contents of this vector with another vector.
     *
     * This function exchanges the contents of the current vector with those of the
     * provided vector `other`. After the swap, the current vector will contain the
     * elements that were in `other` and vice versa.
     *
     * @param other The vector to swap contents with.
     */
    inline void swap(vector<T, Allocator> &other) {
        std::swap(data, other.data);
        std::swap(data_end, other.data_end);
        std::swap(capacity_end, other.capacity_end);
        std::swap(allocator(), other.allocator());
    }

    /**
     * @brief Checks if the vector is empty.
     *
     * This function returns true if the vector is empty (i.e., it contains no elements),
     * and false otherwise.
     *
     * @return bool True if the vector is empty, false otherwise.
     */
    inline bool empty() const { return data == data_end; }

    /**
     * @brief Overloaded equality operator for comparing vectors.
     *
     * This operator compares the elements of the current vector with those of another vector
     * for equality. It returns true if the size and elements of both vectors are equal, and
     * false otherwise.
     *
     * @param other The vector to compare with.
     * @return bool True if the vectors are equal, false otherwise.
     */
    bool operator==(const vector<T, Allocator> &other) const {
        if (size() != other.size()) {
            return false;
        }
        for (size_type i = 0; i < size(); i++) {
            if (data[i] != other.data[i]) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Overloaded inequality operator for comparing vectors.
     *
     * This operator compares the elements of the current vector with those of another vector
     * for inequality. It returns true if the size or elements of the vectors are not equal,
     * and false otherwise.
     *
     * @param other The vector to compare with.
     * @return bool True if the vectors are not equal, false otherwise.
     */
    bool operator!=(const vector<T, Allocator> &other) const { return !(*this == other); }

    /**
     * @brief Compares this vector with another vector for ordering.
     *
     * This operator performs a lexicographical comparison between the elements
     * of this vector and another vector. It first compares the sizes of the vectors.
     * If the sizes are different, the comparison is based on the sizes. If the sizes
     * are the same, it compares the elements one by one using the three-way comparison
     * operator (<=>). The comparison stops as soon as a difference is found.
     *
     * @param other The vector to compare with.
     * @return std::strong_ordering The result of the comparison:
     *         - std::strong_ordering::less if this vector is less than the other vector,
     *         - std::strong_ordering::equal if this vector is equal to the other vector,
     *         - std::strong_ordering::greater if this vector is greater than the other vector.
     */
    auto operator<=>(const vector<T, Allocator> &other) const {
        if (size() != other.size()) {
            return size() <=> other.size();
        }
        for (size_type i = 0; i < size(); i++) {
            if (data[i] != other.data[i]) {
                return data[i] <=> other.data[i];
            }
        }
        return std::strong_ordering::equal;
    }
};

} // namespace containers
} // namespace gtr

#endif // !VECTOR_H
