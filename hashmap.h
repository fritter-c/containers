#ifndef HASHMAP_H
#define HASHMAP_H
#include "allocator_base.h"
#include "allocators/allocators.h"
#include "cassert"
#include "gtr_hash.h"
namespace gtr {
namespace containers {

template <typename Key> struct hashmap_hash {
    std::size_t operator()(const Key &key) { return gtr::hash<Key>()(key); }
};
template <typename Key> struct hashmap_internal_comp {
    bool operator()(const Key &k1, const Key &k2) { return k1 == k2; }
};

template <> struct hashmap_internal_comp<const char *> {
    bool operator()(const char *k1, const char *k2) { return std::strcmp(k1, k2) == 0; }
};

enum hashmap_bucket_flags : uint8_t { HASHMAP_BUCKET_OCCUPIED = 1 << 0, HASHMAP_BUCKET_TOMBSTONE = 1 << 1 };


template <typename Key, typename T, class HashFunc, class CompFunc, class Allocator> struct hashmap;

template <typename Key, typename T> struct hashmap_bucket {
    Key key;
    T value;
    template <typename, class, class, class, class>
    friend struct hashmap;
    inline bool ocupied() { return _flags & HASHMAP_BUCKET_OCCUPIED; }
    inline bool tombstone() { return _flags & HASHMAP_BUCKET_TOMBSTONE; }
    inline bool empty() { return !(_flags & (HASHMAP_BUCKET_OCCUPIED | HASHMAP_BUCKET_TOMBSTONE)); }
    inline bool available() { return !(_flags & HASHMAP_BUCKET_OCCUPIED); }
private:
    uint8_t _flags;
    uint64_t _hash;
};

template <typename Key, typename T, class HashFunc = hashmap_hash<Key>, class CompFunc = hashmap_internal_comp<Key>,
          class Allocator = c_allocator<hashmap_bucket<Key, T>>>
struct hashmap : private _allocator_ebo<hashmap_bucket<Key, T>, Allocator> {
    static constexpr float load_factor = 0.75f;
    static constexpr float growth_factor = 1.0f;
    struct iterator {
        hashmap_bucket<Key, T> *pointer;
        hashmap_bucket<Key, T> *end_ptr;

        inline constexpr iterator(hashmap_bucket<Key, T> *ptr, hashmap_bucket<Key, T> *end) : pointer(ptr), end_ptr(end) {
            if (pointer != end_ptr && !(pointer->ocupied()))
                operator++();
        }

        inline hashmap_bucket<Key, T> &operator*() const { return *pointer; }

        inline hashmap_bucket<Key, T> *operator->() { return pointer; }

        inline iterator &operator++() {
            do {
                ++pointer;
            } while (pointer != end_ptr && !pointer->ocupied());
            return *this;
        }

        inline iterator operator++(int) {
            iterator temp = *this;
            do {
                ++pointer;
            } while (pointer != end_ptr && !pointer->ocupied());
            return temp;
        }

        inline iterator &operator--() {
            do {
                --pointer;
            } while (pointer != end_ptr && !pointer->ocupied());
            return *this;
        }

        inline iterator operator--(int) {
            iterator temp = *this;
            do {
                --pointer;
            } while (pointer != end_ptr && !pointer->ocupied());
            return temp;
        }

        inline Key &key() { return pointer->key; }

        inline T &value() { return pointer->value; }

        friend bool operator==(const iterator &a, const iterator &b) { return a.pointer == b.pointer; }

        friend bool operator!=(const iterator &a, const iterator &b) { return a.pointer != b.pointer; }
    };

    using bucket = hashmap_bucket<Key, T>;

    bucket *data;
    std::size_t size;
    std::size_t capacity;

    /**
     * @brief Returns the size of a bucket in the hashmap.
     *
     * This function calculates and returns the size of a single bucket
     * in the hashmap. The size is determined using the sizeof operator.
     *
     * @return std::size_t The size of a bucket in bytes.
     */
    constexpr inline std::size_t bucket_size() { return sizeof(bucket); }

    /**
     * @brief Frees all allocated memory in the hashmap.
     *
     * This function releases the memory allocated for the hashmap's data
     * based on its current capacity. It is intended to be used internally
     * to ensure proper memory management.
     */
    constexpr inline void _free_all() { this->free(data, capacity); }

    /**
     * @brief Constructs an empty hashmap with default parameters.
     *
     * This constructor initializes an empty hashmap with the following default parameters:
     * - `data` is set to `nullptr`.
     * - `size` is set to `0`.
     * - `capacity` is set to `0`.
     * - `load_factor` is set to `0.6f`.
     * - `growth_factor` is set to `1.0f`.
     *
     * The allocator is also initialized using the default constructor of the allocator type.
     * Use it for raw data or if you know what you are doing. This is not a replacement for std::unordered_map.
     */
    inline hashmap() : _allocator_ebo<hashmap_bucket<Key, T>, Allocator>(), data(nullptr), size(0), capacity(0) {};

    /**
     * @brief Constructor for the hashmap with reserved capacity.
     *
     * Initializes the hashmap with the specified reserved capacity.
     *
     * @param _Reserved The reserved capacity for the hashmap.
     */
    inline explicit hashmap(std::size_t _Reserved) : _allocator_ebo<hashmap_bucket<Key, T>, Allocator>(), size(0), capacity(_Reserved) {
        data = this->malloc(capacity);
        std::memset((char *)(void *)data, 0, capacity * bucket_size());
    };

    /**
     * @brief Constructor for the hashmap with initializer list.
     *
     * Initializes the hashmap with the key-value pairs from the initializer list.
     *
     * @param init_list The initializer list of key-value pairs.
     */
    inline hashmap(std::initializer_list<std::pair<Key, T>> init_list) : _allocator_ebo<hashmap_bucket<Key, T>, Allocator>() {
        std::size_t init_size = init_list.size();
        data = this->malloc(init_size * 4);
        capacity = init_size * 4;
        size = 0;
        std::memset((char *)(void *)data, 0, capacity * bucket_size());
        for (const auto &pair : init_list) add(pair.first, pair.second);
    }

    /**
     * @brief Move constructor for the hashmap.
     *
     * Initializes the hashmap by moving the data from another hashmap.
     *
     * @param _Src The source hashmap to move from.
     */
    inline hashmap(hashmap &&_Src) noexcept : _allocator_ebo<hashmap_bucket<Key, T>, Allocator>(std::move(_Src.get_allocator())) {
        data = _Src.data;
        size = _Src.size;
        capacity = _Src.capacity;
        _Src.data = nullptr;
        _Src.size = 0;
        _Src.capacity = 0;
    }

    /**
     * @brief Deleted copy constructor for the hashmap.
     *
     * Copying a hashmap is not allowed.
     *
     * @param other The source hashmap to copy from.
     */
    inline hashmap(const hashmap &other) = delete;

    /**
     * @brief Deleted copy assignment operator for the hashmap.
     *
     * Copying a hashmap is not allowed.
     *
     * @param other The source hashmap to copy from.
     * @return hashmap& The reference to the updated hashmap.
     */
    inline hashmap &operator=(const hashmap &other) = delete;

    /**
     * @brief Move assignment operator for the hashmap.
     *
     * Moves the data from another hashmap to this hashmap.
     *
     * @param _Src The source hashmap to move from.
     * @return hashmap& The reference to the updated hashmap.
     */
    inline hashmap &operator=(hashmap &&_Src) noexcept {
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
     * @brief Destructor for the hashmap.
     *
     * Frees all memory used by the hashmap.
     */
    inline ~hashmap() {
        destroy();
        _free_all();
        data = nullptr;
    }

    /**
     * @brief Returns an iterator to the beginning of the hashmap.
     *
     * @return iterator An iterator to the first element in the hashmap.
     */
    inline iterator begin() { return iterator(data, data + capacity); }

    /**
     * @brief Returns an iterator to the end of the hashmap.
     *
     * @return iterator An iterator to the element following the last element in the hashmap.
     */
    inline iterator end() { return iterator(data + capacity, data + capacity); }

    /**
     * @brief Returns a constant iterator to the beginning of the hashmap.
     *
     * @return const iterator A constant iterator to the first element in the hashmap.
     */
    inline const iterator begin() const { return iterator(data, data + capacity); }

    /**
     * @brief Returns a constant iterator to the end of the hashmap.
     *
     * @return const iterator A constant iterator to the element following the last element in the hashmap.
     */
    inline const iterator end() const { return iterator(data + capacity, data + capacity); }

    /**
     * @brief Retrieves the bucket for a given key.
     *
     * @param key The key to search for.
     * @param hash_created The hash value of the key.
     * @return bucket* A pointer to the bucket containing the key.
     */
    inline bucket *get_bucket(const Key &key, std::size_t &hash_created) const {
        if (hash_created == static_cast<std::size_t>(-1))
            hash_created = hash(key);

        std::size_t index = hash_created % capacity;
        auto bucket = data + index;
        hashmap_bucket<Key, T> *first_tombstone = nullptr;
        std::size_t key_probe = probe(hash_created);
        std::size_t probes = 0;

        while (probes < capacity) {
            if (bucket->ocupied()) {
                if (bucket->_hash == hash_created && CompFunc()(bucket->key, key)) {
                    // Found existing key
                    return bucket;
                }
            } else if (bucket->tombstone()) {
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
     * @brief Retrieves the bucket for a given key.
     *
     * @param key The key to search for.
     * @return bucket* A pointer to the bucket containing the key.
     */
    inline bucket *get_bucket(const Key &key) const {
        std::size_t hash_created = hash(key);
        return get_bucket(key, hash_created);
    }

    /**
     * @brief Reserves space for the specified number of elements.
     *
     * @param reserve The number of elements to reserve space for.
     */
    inline void reserve(std::size_t reserve) {
        if (reserve > capacity) {
            hashmap map(reserve);
            for (std::size_t i = 0; i < capacity; i++) {
                if (data[i].ocupied()) {
                    map.add_with_hash(data[i].key, std::move(data[i].value), data[i]._hash);
                }
            }
            *this = std::move(map);
        }
    }

    /**
     * @brief Grows the capacity of the hashmap.
     */
    inline void grow() {
        std::size_t new_capacity = static_cast<std::size_t>(static_cast<float>(capacity) * (1.0f + growth_factor));
        reserve(capacity ? new_capacity : 64);
    }

    /**
     * @brief Clears all elements from the hashmap.
     */
    inline void clear() {
        destroy();
        size = 0;
        std::memset((char *)(void *)data, 0, capacity * bucket_size());
    }

    /**
     * @brief Computes the hash value for a given key.
     *
     * @param key The key to hash.
     * @return std::size_t The hash value of the key.
     */
    inline std::size_t hash(const Key &key) const { return HashFunc()(key); };

    /**
     * @brief Computes the probe value for a given hash.
     *
     * @param hash The hash value.
     * @return std::size_t The probe value.
     */
    inline std::size_t probe(std::size_t hash) const { return 1LL + hash % (capacity - 1LL); }

    /**
     * @brief Retrieves an iterator to the element with the specified key.
     *
     * @param key The key to search for.
     * @return iterator An iterator to the element with the specified key.
     */
    inline iterator get(const Key &key) const {
        if (capacity == 0)
            return end();
        std::size_t key_hash = (std::size_t)-1;
        auto bucket = get_bucket(key, key_hash);
        return bucket ? (bucket->ocupied()) ? iterator(bucket, data + capacity) : end() : end();
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

        std::size_t key_probe = probe(key_hash);

        while (probes < capacity) {
            if (bucket->ocupied()) {
                if (bucket->_hash == key_hash && CompFunc()(bucket->key, key)) {
                    return iterator(bucket, data + capacity);
                }
            } else if (!(bucket->tombstone())) {
                // Empty bucket, key not found
                return end();
            }
            // Move to the next bucket
            index = (index + key_probe) % capacity;
            key_probe++;
            bucket = data + index;
            probes++;
        }
        return end();
    }

    /**
     * @brief Adds a key-value pair to the hashmap with a precomputed hash.
     *
     * @param key The key to add.
     * @param value The value to add.
     * @param hash The precomputed hash value of the key.
     * @return iterator An iterator to the added element.
     */
    inline iterator add_with_hash(const Key &key, const T &value, std::size_t hash) {
        if ((size + 1) >= (capacity * load_factor)) {
            grow();
        }
        std::size_t key_hash = (hash == static_cast<std::size_t>(-1)) ? this->hash(key) : hash;
        auto bucket = get_bucket(key, key_hash);

        if (!bucket) {
            // Hashmap is full, cannot insert
            return end();
        }

        if (bucket->ocupied()) {
            // Key already exists, update the value
            bucket->value = value;
            return iterator(bucket, data + capacity);
        } else {
            // Insert new key-value pair
            new (&bucket->key) Key(key);
            new (&bucket->value) T(value);
            bucket->_hash = key_hash;
            bucket->_flags = HASHMAP_BUCKET_OCCUPIED;
            size++;
            return iterator(bucket, data + capacity);
        }
    }

    /**
     * @brief Adds a key-value pair to the hashmap with a precomputed hash.
     *
     * @param key The key to add.
     * @param value The value to add.
     * @param hash The precomputed hash value of the key.
     * @return iterator An iterator to the added element.
     */
    inline iterator add_with_hash(const Key &key, T &&value, std::size_t hash) {
        if ((size + 1) >= (capacity * load_factor)) {
            grow();
        }
        std::size_t key_hash = (hash == static_cast<std::size_t>(-1)) ? this->hash(key) : hash;
        auto bucket = get_bucket(key, key_hash);

        if (!bucket) {
            // Hashmap is full, cannot insert
            return end();
        }

        if (bucket->ocupied()) {
            // Key already exists, update the value
            bucket->value = std::move(value);
            return iterator(bucket, data + capacity);
        } else {
            // Insert new key-value pair
            new (&bucket->key) Key(key);
            new (&bucket->value) T(std::move(value));
            bucket->_hash = key_hash;
            bucket->_flags = HASHMAP_BUCKET_OCCUPIED;
            size++;
            return iterator(bucket, data + capacity);
        }
    }

    /**
     * @brief Adds a key-value pair to the hashmap.
     *
     * @param key The key to add.
     * @param value The value to add.
     * @return iterator An iterator to the added element.
     */
    inline iterator add(const Key &key, const T &value) { return add_with_hash(key, value, (std::size_t)-1); }

    /**
     * @brief Adds a key-value pair to the hashmap.
     *
     * @param key The key to add.
     * @param value The value to add.
     * @return iterator An iterator to the added element.
     */
    inline iterator add(const Key &key, T &&value) { return add_with_hash(key, std::move(value), (std::size_t)-1); }

    /**
     * @brief Removes the element with the specified key from the hashmap.
     *
     * @param key The key to remove.
     * @return iterator An iterator to the removed element.
     */
    inline iterator remove(const Key &key) {
        iterator it = find(key);
        if (it != end()) {
            it.pointer->key.~Key();
            it.pointer->value.~T();
            it.pointer->_flags = HASHMAP_BUCKET_TOMBSTONE;
            size--;
            return it;
        }
        return end();
    }

    /**
     * @brief Checks if the hashmap contains the specified key.
     *
     * @param key The key to check.
     * @return bool True if the key is found, false otherwise.
     */
    inline bool contains(const Key &key) { return find(key) != end(); }

    /**
     * @brief Tries to get the value associated with the specified key.
     *
     * @param key The key to search for.
     * @param Value The reference to store the value if found.
     * @return bool True if the key is found, false otherwise.
     */
    inline bool try_get_value(const Key &key, T &Value) {
        if (auto it = find(key); it != end()) {
            Value = it->value;
            return true;
        }
        return false;
    }

    /**
     * @brief Checks if the hash map will need to regrow on the next insert.
     *
     * This function determines whether the hash map's size after the next insert
     * will exceed the product of its current capacity and load factor, indicating
     * that a regrow (rehashing) operation will be necessary.
     *
     * @return true if the hash map will need to regrow on the next insert, false otherwise.
     */
    inline bool will_regrow_next_insert() const { return (size + 1) >= (capacity * load_factor); }

    /**
     * @brief Destroys all elements in the hashmap.
     */
    inline void destroy() {
        if (data) {
            for (auto &bucket : *this) {
                if (bucket._flags & HASHMAP_BUCKET_OCCUPIED) {
                    bucket.key.~Key();
                    bucket.value.~T();
                }
            }
        }
    }

    /**
     * @brief Accesses the value associated with the specified key.
     *
     * @param key The key to search for.
     * @return T& The value associated with the key.
     */
    inline T &operator[](const Key &key) {
        if (!capacity)
            grow();
        std::size_t key_hash = static_cast<std::size_t>(-1);
        auto bucket = get_bucket(key, key_hash);
        if (bucket->_flags & HASHMAP_BUCKET_OCCUPIED) {
            return bucket->value;
        }

        new (&bucket->key) Key(key);
        new (&bucket->value) T();
        bucket->_hash = key_hash;
        bucket->_flags = HASHMAP_BUCKET_OCCUPIED;
        size++;
        return bucket->value;
    }
};
}; // namespace containers
}; // namespace gtr
#endif // HASHMAP_H
