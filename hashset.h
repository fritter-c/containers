#ifndef HASHSET_H
#define HASHSET_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include "cassert"
#include "gtr_hash.h"
namespace gtr {
namespace containers {

template <typename Key> struct hashset_hash {
    std::size_t operator()(const Key &key) { return gtr::hash<Key>()(key); }
};
template <typename Key> struct hashset_internal_comp {
    bool operator()(const Key &k1, const Key &k2) { return k1 == k2; }
};

template <> struct hashset_internal_comp<const char *> {
    bool operator()(const char *k1, const char *k2) { return std::strcmp(k1, k2) == 0; }
};

enum hashset_bucket_flags : uint8_t { HASHSET_BUCKET_OCCUPIED = 1 << 0, HASHSET_BUCKET_TOMBSTONE = 1 << 1 };

template <typename Key, class HashFunc, class CompFunc, class Allocator> struct hashset;
template <typename Key> struct hashset_bucket {
    Key key;
    template <typename, class, class, class>
    friend struct hashset;

  private:
    uint8_t _flags;
    uint64_t _hash;
};
template <typename Key, class HashFunc = hashset_hash<Key>, class CompFunc = hashset_internal_comp<Key>,
          class Allocator = c_allocator<hashset_bucket<Key>>>
struct hashset : private _allocator_ebo<hashset_bucket<Key>, Allocator> {

    static constexpr float load_factor = 0.75f;
    static constexpr float growth_factor = 1.0f;

    using value_type = Key;
    using allocator_type = Allocator;

    struct iterator {
      private:
        hashset_bucket<Key> *pointer;
        hashset_bucket<Key> *end_ptr;

      public:
        friend struct hashset;
        inline constexpr iterator(hashset_bucket<Key> *ptr, hashset_bucket<Key> *end) : pointer(ptr), end_ptr(end) {
            if (pointer != end_ptr && !(pointer->_flags & (HASHSET_BUCKET_OCCUPIED | HASHSET_BUCKET_TOMBSTONE)))
                operator++();
        };

        inline iterator &operator++() {
            do {
                ++pointer;
            } while (pointer != end_ptr && !(pointer->_flags & (HASHSET_BUCKET_OCCUPIED | HASHSET_BUCKET_TOMBSTONE)));
            return *this;
        }

        inline iterator &operator++(int) {
            iterator temp = *this;
            do {
                ++pointer;
            } while (pointer != end_ptr && !(pointer->_flags & (HASHSET_BUCKET_OCCUPIED | HASHSET_BUCKET_TOMBSTONE)));
            return temp;
        }

        inline Key &operator*() const { return pointer->key; }

        inline Key *operator->() { return &pointer->key; }

        inline Key &first() { return pointer->key; }

        friend bool operator==(const iterator &a, const iterator &b) { return a.pointer == b.pointer; }

        friend bool operator!=(const iterator &a, const iterator &b) { return a.pointer != b.pointer; }
    };

    using bucket = hashset_bucket<Key>;

    bucket *data;
    std::size_t size;
    std::size_t capacity;

    /**
     * @brief Returns the size of a bucket in the hash set.
     *
     * This function provides the size of a single bucket used in the hash set.
     * It is a constexpr and inline function, ensuring that the size is computed
     * at compile time and has minimal overhead.
     *
     * @return std::size_t The size of a bucket in bytes.
     */
    constexpr inline std::size_t bucket_size() const { return sizeof(bucket); }

    /**
     * @brief Frees all allocated memory for the hash set.
     *
     * This function deallocates the memory used by the hash set's data array
     * and resets the capacity to zero. It should be called to clean up resources
     * when the hash set is no longer needed.
     */
    constexpr inline void _free_all() { this->free(data, capacity); }

    /**
     * @brief Constructs an empty hashset.
     *
     * This constructor initializes an empty hashset with no elements,
     * setting the internal data pointer to nullptr, size to 0, and capacity to 0.
     */
    inline hashset() : _allocator_ebo<hashset_bucket<Key>, Allocator>(), data(nullptr), size(0), capacity(0) {};

    /**
     * @brief Constructs a hashset with a reserved capacity.
     *
     * This constructor initializes a hashset with a specified reserved capacity.
     * It allocates memory for the hashset buckets and sets the initial size to zero.
     * The allocated memory is then zero-initialized.
     *
     * @param _Reserved The number of buckets to reserve for the hashset.
     */
    inline explicit hashset(std::size_t _Reserved) : _allocator_ebo<hashset_bucket<Key>, Allocator>(), size(0), capacity(_Reserved) {
        data = this->malloc(capacity);
        std::memset((char *)(void *)data, 0, capacity * bucket_size());
    };

    /**
     * @brief Constructs a hashset from an initializer list of key-value pairs.
     *
     * This constructor initializes the hashset with the given initializer list.
     * It allocates memory for the hashset, sets the initial capacity to four times
     * the size of the initializer list, and initializes the memory to zero.
     * Then, it inserts each key-value pair from the initializer list into the hashset.
     *
     * @param init_list An initializer list of key-value pairs to initialize the hashset.
     */
    inline hashset(std::initializer_list<Key> init_list) : _allocator_ebo<hashset_bucket<Key>, Allocator>() {
        std::size_t init_size = init_list.size();
        data = this->malloc(init_size * 4);
        capacity = init_size * 4;
        size = 0;
        std::memset((char *)(void *)data, 0, capacity * bucket_size());
        for (const auto &pair : init_list) add(pair.first, pair.second);
    }

    /**
     * @brief Move constructor for the hashset class.
     *
     * This constructor initializes a hashset object by transferring ownership of the resources
     * from the source hashset (_Src) to the newly created hashset. The source hashset is left
     * in a valid but unspecified state.
     *
     * @param _Src The source hashset to move from.
     */
    inline hashset(hashset &&_Src) noexcept : _allocator_ebo<hashset_bucket<Key>, Allocator>(std::move(_Src.get_allocator())) {
        data = _Src.data;
        size = _Src.size;
        capacity = _Src.capacity;
        _Src.data = nullptr;
        _Src.size = 0;
        _Src.capacity = 0;
    }

    /**
     * @brief Deleted copy constructor to prevent copying of hashset instances.
     *
     * This constructor is explicitly deleted to avoid the copying of hashset objects,
     * ensuring that each instance is unique and not inadvertently duplicated.
     *
     * @param other The hashset instance to copy from (deleted).
     */
    inline hashset(const hashset &other) = delete;

    /**
     * @brief Deleted copy assignment operator.
     *
     * This operator is deleted to prevent copying of the hashset object.
     * Copying a hashset is not allowed to ensure unique instances and
     * avoid potential issues with resource management.
     *
     * @param other The hashset object to be copied from.
     * @return Reference to the current hashset object.
     */
    inline hashset &operator=(const hashset &other) = delete;

    /**
     * @brief Move assignment operator for the hashset class.
     *
     * This operator transfers ownership of the resources from the source hashset
     * (_Src) to the current hashset instance. It first checks for self-assignment,
     * then frees any existing resources in the current instance, and finally
     * transfers the data, size, capacity, and allocator from the source to the
     * current instance. The source hashset is left in a valid but empty state.
     *
     * @param _Src The source hashset to move from.
     * @return A reference to the current hashset instance.
     */
    inline hashset &operator=(hashset &&_Src) noexcept {
        if (this != &_Src) {
            _free_all();
            data = _Src.data;
            size = _Src.size;
            capacity = _Src.capacity;
            this->get_allocator() = std::move(_Src.get_allocator());
            _Src.data = nullptr;
            _Src.size = 0;
            _Src.capacity = 0;
        }
        return *this;
    }

    /**
     * @brief Destructor for the hashset class.
     *
     * This inline destructor is responsible for cleaning up the resources
     * used by the hashset. It calls the destroy() method to perform any
     * necessary cleanup operations and then frees all allocated memory
     * by calling the _free_all() method. Finally, it sets the data pointer
     * to nullptr to avoid dangling pointers.
     */
    inline ~hashset() {
        destroy();
        _free_all();
        data = nullptr;
    }

    /**
     * @brief Returns an iterator to the beginning of the hash set.
     *
     * This function provides an iterator that points to the first element
     * in the hash set. The iterator can be used to traverse the elements
     * in the hash set from the beginning to the end.
     *
     * @return iterator An iterator to the beginning of the hash set.
     */
    inline iterator begin() { return iterator(data, data + capacity); }

    /**
     * @brief Returns an iterator to the end of the hash set.
     *
     * This function provides an iterator pointing to the position past the
     * last element in the hash set. The end iterator is used to denote the
     * end of the hash set and should not be dereferenced.
     *
     * @return iterator An iterator to the end of the hash set.
     */
    inline iterator end() { return iterator(data + capacity, data + capacity); }

    /**
     * @brief Returns a constant iterator to the beginning of the container.
     *
     * This function provides a constant iterator pointing to the first element
     * in the container. The iterator can be used to traverse the elements in
     * the container from the beginning to the end.
     *
     * @return A constant iterator to the beginning of the container.
     */
    inline const iterator begin() const { return iterator(data, data + capacity); }

    /**
     * @brief Returns a constant iterator to the end of the container.
     *
     * This function provides a constant iterator pointing to the position
     * past the last element in the container. The end iterator is used to
     * denote the end of the container and should not be dereferenced.
     *
     * @return A constant iterator to the end of the container.
     */
    inline const iterator end() const { return iterator(data + capacity, data + capacity); }

    /**
     * @brief Returns an iterator to the end of the container.
     *
     * This function provides a constant iterator pointing to the position
     * past the last element in the container. The end iterator is used to
     * denote the end of the container and should not be dereferenced.
     *
     * @return A constant iterator to the end of the container.
     */
    inline bucket *get_bucket(const Key &key, std::size_t &hash_created) const {
        if (hash_created == static_cast<std::size_t>(-1))
            hash_created = hash(key);

        std::size_t index = hash_created % capacity;
        auto bucket = data + index;
        hashset_bucket<Key> *first_tombstone = nullptr;
        std::size_t key_probe = probe(hash_created);
        std::size_t probes = 0;

        while (probes < capacity) {
            if (bucket->_flags & HASHSET_BUCKET_OCCUPIED) {
                if (bucket->_hash == hash_created && CompFunc()(bucket->key, key)) {
                    // Found existing key
                    return bucket;
                }
            } else if (bucket->_flags & HASHSET_BUCKET_OCCUPIED) {
                // Record the first tombstone to reuse it later
                if (!first_tombstone) {
                    first_tombstone = bucket;
                }
            } else {
                // Empty bucket found
                if (first_tombstone) {
                    // Use the first tombstone found
                    bucket = first_tombstone;
                }
                bucket->_hash = hash_created;
                return bucket;
            }

            // Move to the next bucket
            index = (index + key_probe) % capacity;
            key_probe++;
            bucket = data + index;
            probes++;
        }

        // All buckets probed; check for a tombstone to reuse
        if (first_tombstone) {
            first_tombstone->_hash = hash_created;
            return first_tombstone;
        }

        // Hashmap is full (no empty buckets and no tombstones)
        return nullptr;
    }

    /**
     * @brief Retrieves the bucket corresponding to the given key.
     *
     * This function calculates the hash of the provided key and then
     * retrieves the bucket associated with that hash.
     *
     * @param key The key for which the bucket is to be retrieved.
     * @return A pointer to the bucket corresponding to the given key.
     */
    inline bucket *get_bucket(const Key &key) const {
        std::size_t hash_created = hash(key);
        return get_bucket(key, hash_created);
    }

    /**
     * @brief Reserves space in the hashset to accommodate at least the specified number of elements.
     *
     * This function ensures that the hashset has enough capacity to store the given number of elements.
     * If the requested capacity is greater than the current capacity, a new hashset with the specified
     * capacity is created, and all existing elements are rehashed and moved to the new hashset.
     *
     * @param reserve The number of elements to reserve space for in the hashset.
     */
    inline void reserve(std::size_t reserve) {
        if (reserve > capacity) {
            hashset map(reserve);
            for (std::size_t i = 0; i < capacity; i++) {
                if (data[i]._flags & HASHSET_BUCKET_OCCUPIED) {
                    map.add_with_hash(data[i].key, data[i]._hash);
                }
            }
            *this = std::move(map);
        }
    }

    /**
     * @brief Increases the capacity of the hash set.
     *
     * This function calculates a new capacity based on the current capacity and a growth factor.
     * If the current capacity is zero, it sets the new capacity to 64. Otherwise, it multiplies
     * the current capacity by (1.0 + growth_factor) to determine the new capacity. It then reserves
     * the new capacity for the hash set.
     */
    inline void grow() {
        std::size_t new_capacity = static_cast<std::size_t>(static_cast<float>(capacity) * (1.0f + growth_factor));
        reserve(capacity ? new_capacity : 64);
    }

    /**
     * @brief Clears the hash set by destroying its contents and resetting its size.
     *
     * This function destroys all elements in the hash set, resets the size to zero,
     * and sets all bytes in the data array to zero.
     */
    inline void clear() {
        destroy();
        size = 0;
        std::memset((char *)(void *)data, 0, capacity * bucket_size());
    }

    /**
     * @brief Computes the hash value for a given key.
     *
     * This function uses the provided hash function object to compute
     * the hash value of the specified key.
     *
     * @param key The key for which the hash value is to be computed.
     * @return The computed hash value of the key.
     */
    inline std::size_t hash(const Key &key) const { return HashFunc()(key); };

    /**
     * @brief Computes the probe step for a given hash value.
     *
     * This function calculates the step size for probing in a hash table
     * using open addressing. The step size is determined by the hash value
     * and the capacity of the hash table.
     *
     * @param hash The hash value for which to compute the probe step.
     * @return The probe step size.
     */
    inline std::size_t probe(std::size_t hash) const { return 1LL + hash % (capacity - 1LL); }

    /**
     * @brief Adds a key to the hash set with a precomputed hash value.
     *
     * This function attempts to add the specified key to the hash set using the provided
     * hash value. If the current size of the hash set plus one exceeds the product of the
     * capacity and the load factor, the hash set will grow to accommodate the new element.
     *
     * @param key The key to be added to the hash set.
     * @param hash The precomputed hash value of the key.
     * @return An iterator pointing to the newly added element, or the end iterator if the
     *         element could not be added.
     */
    inline iterator add_with_hash(const Key &key, std::size_t hash) {
        if ((size + 1) >= (capacity * load_factor)) {
            grow();
        }
        std::size_t key_hash = hash == static_cast<std::size_t>(-1) ? this->hash(key) : hash;
        auto bucket = get_bucket(key, key_hash);
        if (bucket && !(bucket->_flags & HASHSET_BUCKET_OCCUPIED)) {
            // Insert new key
            new (&bucket->key) Key(key);
            bucket->_hash = key_hash;
            bucket->_flags = HASHSET_BUCKET_OCCUPIED;
            size++;
            return iterator(bucket, data + capacity);
        } else if (bucket && (bucket->_flags & HASHSET_BUCKET_OCCUPIED)) {
            // Key already exists
            return iterator(bucket, data + capacity);
        }
        return end();
    }

    /**
     * @brief Retrieves an iterator to the element with the specified key.
     *
     * This function searches for the element with the given key in the hash set.
     * If the element is found, it returns an iterator pointing to it. If the
     * element is not found or the hash set is empty, it returns an iterator to
     * the end of the hash set.
     *
     * @param key The key of the element to search for.
     * @return iterator An iterator to the element if found, otherwise an iterator to the end.
     */
    inline iterator get(const Key &key) const {
        if (capacity == 0)
            return end();
        std::size_t key_hash = (std::size_t)-1;
        auto bucket = get_bucket(key, key_hash);
        return bucket ? (bucket->_flags & HASHSET_BUCKET_OCCUPIED) ? iterator(bucket, data + capacity) : end() : end();
    }
    /**
     * @brief Retrieves an iterator to the element with the specified key.
     *
     * This function searches for the element with the given key in the hash set.
     * If the element is found, it returns an iterator pointing to it. If the
     * element is not found or the hash set is empty, it returns an iterator to
     * the end of the hash set.
     *
     * @param key The key of the element to search for.
     * @return iterator An iterator to the element if found, otherwise an iterator to the end.
     */
    inline iterator find(const Key &key) const {
        if (size == 0)
            return end();

        std::size_t key_hash = hash(key);
        std::size_t index = key_hash % capacity;
        std::size_t probes = 0;
        auto bucket = data + index;

        std::size_t key_probe = probe(key_hash); // Step size remains constant
        while (probes < capacity) {
            if (bucket->_flags & HASHSET_BUCKET_OCCUPIED) {
                if (bucket->_hash == key_hash && CompFunc()(bucket->key, key)) {
                    return iterator(bucket, data + capacity);
                }
            } else if (!(bucket->_flags & HASHSET_BUCKET_TOMBSTONE)) {
                // Empty bucket, key not found
                return end();
            }
            // Move to the next bucket
            index = (index + key_probe) % capacity;
            bucket = data + index;
            probes++;
        }
        return end();
    }

    /**
     * @brief Adds a key to the hash set with a precomputed hash value.
     *
     * This function attempts to add the specified key to the hash set using the provided
     * hash value. If the current size of the hash set plus one exceeds the product of the
     * capacity and the load factor, the hash set will grow to accommodate the new element.
     *
     * @param key The key to be added to the hash set.
     * @param hash The precomputed hash value of the key.
     * @return An iterator pointing to the newly added element, or the end iterator if the
     *         element could not be added.
     */

    inline iterator add(const Key &key) { return add_with_hash(key, (std::size_t)-1); }

    /**
     * @brief Removes the element with the specified key from the hash set.
     *
     * This function searches for the element with the given key and removes it
     * from the hash set if it exists. The element's destructor is called, and
     * the bucket is marked as a tombstone. The size of the hash set is
     * decremented.
     *
     * @param key The key of the element to be removed.
     * @return An iterator to the removed element if it was found, or an
     *         iterator to the end of the hash set if the element was not found.
     */
    inline iterator remove(const Key &key) {
        iterator it = find(key);
        if (it != end()) {
            it.pointer->key.~Key();
            it.pointer->_flags = HASHSET_BUCKET_TOMBSTONE;
            size--;
            return it;
        }
        return end();
    }

    /**
     * @brief Checks if the hash set contains the specified key.
     *
     * This function determines whether the given key exists in the hash set.
     *
     * @param key The key to be checked for existence in the hash set.
     * @return true if the key is found in the hash set, false otherwise.
     */
    inline bool contains(const Key &key) { return find(key) != end(); }

    /**
     * @brief Destroys the hash set by calling the destructor of each occupied bucket's key.
     *
     * This function iterates over all buckets in the hash set and checks if each bucket is occupied.
     * If a bucket is occupied, it explicitly calls the destructor of the key stored in that bucket.
     * This ensures that any resources held by the keys are properly released.
     *
     * @note This function assumes that the hash set's data structure is properly initialized and that
     *       the keys have a valid destructor.
     */
    inline void destroy() {
        if (data) {
            for (auto it = begin(); it != end(); ++it) {
                if (it.pointer->_flags & HASHSET_BUCKET_OCCUPIED) {
                    it.pointer->key.~Key();
                }
            }
        }
    }
};
} // namespace containers
} // namespace gtr

#endif
