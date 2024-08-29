#ifndef HASHMAP_H
#define HASHMAP_H
#include "allocator_base.h"
#include "allocators/allocators.h"
namespace gtr {
namespace containers {

// FNV-1a
template <typename Key> struct hashmap_internal_hash {
    std::size_t operator()(const Key &key) {
        const char *s;
        if constexpr (std::is_pointer<Key>::value) {
            s = (const char *)key;
        } else {
            s = (const char *)&key;
        }
        std::size_t hash = 14695981039346656037ULL;
        std::size_t fnv_prime = 1099511628211ULL;
        for (std::size_t i = 0; i < sizeof(Key); ++i) {
            hash = hash ^ s[i];
            hash = hash * fnv_prime;
        }
        return hash;
    }
};

template <typename Key> struct hashmap_internal_comp {
    bool operator()(const Key &k1, const Key &k2) { return k1 == k2; }
};

template <> struct hashmap_internal_comp<const char *> {
    bool operator()(const char *k1, const char *k2) { return std::strcmp(k1, k2) == 0; }
};

enum hashmap_bucket_flags : uint8_t { HASHMAP_BUCKET_OCCUPIED = 1 << 0, HASHMAP_BUCKET_TOMBSTONE = 1 << 1 };

template <typename Key, typename T> struct hashmap_bucket {
    Key key;
    T value;
    uint8_t _flags;
    uint64_t _hash;
};

template <typename Key, typename T, class HashFunc = hashmap_internal_hash<Key>, class CompFunc = hashmap_internal_comp<Key>,
          class Allocator = c_allocator<hashmap_bucket<Key, T>>>
struct hashmap : private _allocator_ebo<hashmap_bucket<Key, T>, Allocator> {
    struct iterator {
        hashmap_bucket<Key, T> *pointer;
        hashmap_bucket<Key, T> *end_ptr;

        inline constexpr iterator(hashmap_bucket<Key, T> *ptr, hashmap_bucket<Key, T> *end) : pointer(ptr), end_ptr(end) {
            if (pointer != end_ptr && !(pointer->_flags & (HASHMAP_BUCKET_OCCUPIED | HASHMAP_BUCKET_TOMBSTONE)))
                operator++();
        };

        inline hashmap_bucket<Key, T> &operator*() const { return *pointer; }

        inline hashmap_bucket<Key, T> *operator->() { return pointer; }

        inline iterator &operator++() {
            do {
                pointer++;
            } while (pointer != end_ptr && !(pointer->_flags & (HASHMAP_BUCKET_OCCUPIED | HASHMAP_BUCKET_TOMBSTONE)));
            return *this;
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
    float load_factor;
    float growth_factor;

    Allocator &allocator() { return _allocator_ebo<hashmap_bucket<Key, T>, Allocator>::get_allocator(); }

    const Allocator &allocator() const { return _allocator_ebo<hashmap_bucket<Key, T>, Allocator>::get_allocator(); }

    constexpr inline std::size_t bucket_size() { return sizeof(bucket); }

    constexpr inline void _free_all() { this->free(data, capacity); }

    inline hashmap() : _allocator_ebo<hashmap_bucket<Key, T>, Allocator>(), size(0), capacity(64), load_factor(0.6f), growth_factor(1.0f) {
        data = this->malloc(capacity);
        std::memset((char *)(void *)data, 0, capacity * bucket_size());
    };

    /**
     * @brief Constructor for the hashmap with reserved capacity.
     *
     * Initializes the hashmap with the specified reserved capacity.
     *
     * @param _Reserved The reserved capacity for the hashmap.
     */
    inline explicit hashmap(std::size_t _Reserved)
        : _allocator_ebo<hashmap_bucket<Key, T>, Allocator>(), size(0), capacity(_Reserved), load_factor(0.6f), growth_factor(1.0f) {
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
    inline hashmap(std::initializer_list<std::pair<Key, T>> init_list)
        : _allocator_ebo<hashmap_bucket<Key, T>, Allocator>(), load_factor(0.6f), growth_factor(1.0f) {
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
        load_factor = _Src.load_factor;
        growth_factor = _Src.growth_factor;
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
            if (data)
                _free_all();
            data = _Src.data;
            size = _Src.size;
            capacity = _Src.capacity;
            load_factor = _Src.load_factor;
            growth_factor = _Src.growth_factor;
            allocator() = std::move(_Src.get_allocator());
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
        if (data)
            this->free(data, capacity);
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
        if (bucket->_flags & (HASHMAP_BUCKET_OCCUPIED | HASHMAP_BUCKET_TOMBSTONE)) {
            std::size_t key_probe = probe(hash_created);
            do {
                if (bucket->_hash == hash_created) {
                    if (CompFunc()(bucket->key, key)) {
                        return bucket;
                    }
                }
                index = (index + key_probe) % capacity;
                key_probe++;
                bucket = data + index;
            } while (bucket->_flags & HASHMAP_BUCKET_OCCUPIED);
        }
        bucket->_hash = hash_created;
        return bucket;
    }

    /**
     * @brief Retrieves the bucket for a given key.
     *
     * @param key The key to search for.
     * @return bucket* A pointer to the bucket containing the key.
     */
    inline bucket *get_bucket(const Key &key) {
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
                if (data[i]._flags & HASHMAP_BUCKET_OCCUPIED) {
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
        reserve(new_capacity);
    }

    /**
     * @brief Clears all elements from the hashmap.
     */
    inline void clear() {
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
        std::size_t key_hash = (std::size_t)-1;
        auto bucket = get_bucket(key, key_hash);
        return bucket ? (bucket->_flags & HASHMAP_BUCKET_OCCUPIED) ? iterator(bucket, data + capacity) : end() : end();
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
        std::size_t key_hash = hash;
        auto bucket = get_bucket(key, key_hash);
        if (bucket && bucket->_flags == 0) {
            bucket->value = value;
            bucket->key = key;
            bucket->_hash = key_hash;
            bucket->_flags = HASHMAP_BUCKET_OCCUPIED;
            size++;
            return iterator(bucket, data + capacity);
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
    inline iterator add_with_hash(const Key &key, T &&value, std::size_t hash) {
        if ((size + 1) >= (capacity * load_factor)) {
            grow();
        }
        std::size_t key_hash = hash;
        auto bucket = get_bucket(key, key_hash);
        if (bucket && !(bucket->_flags & HASHMAP_BUCKET_OCCUPIED)) {
            bucket->value = std::move(value);
            bucket->key = key;
            bucket->_hash = key_hash;
            bucket->_flags = HASHMAP_BUCKET_OCCUPIED;
            size++;
            return iterator(bucket, data + capacity);
        }
        return end();
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
        iterator it = get(key);
        if (it != end()) {
            it->_flags = HASHMAP_BUCKET_TOMBSTONE;
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
    inline bool contains(const Key &key) { return get(key) != end(); }

    /**
     * @brief Tries to get the value associated with the specified key.
     *
     * @param key The key to search for.
     * @param Value The reference to store the value if found.
     * @return bool True if the key is found, false otherwise.
     */
    inline bool try_get_value(const Key &key, T &Value) {
        if (auto it = get(key); it != end()) {
            Value = it->value;
            return true;
        }
        return false;
    }

    /**
     * @brief Tries to get the value associated with the specified key.
     *
     * @param key The key to search for.
     * @param Value The pointer to store the value if found.
     * @return bool True if the key is found, false otherwise.
     */
    inline bool try_get_value(const Key &key, T *Value) {
        if (auto it = get(key); it != end()) {
            Value = &it->value;
            (void)Value;
            return true;
        }
        return false;
    }

    /**
     * @brief Destroys all elements in the hashmap.
     */
    inline void destroy() {
        if (data) {
            for (auto &bucket : *this) {
                if (bucket._flags & HASHMAP_BUCKET_OCCUPIED) {
                    bucket.value.~T();
                }
            }
            clear();
        }
    }

    /**
     * @brief Accesses the value associated with the specified key.
     *
     * @param key The key to search for.
     * @return T& The value associated with the key.
     */
    inline T &operator[](const Key &key) {
        std::size_t key_hash = static_cast<std::size_t>(-1);
        auto bucket = get_bucket(key, key_hash);
        if (bucket->_flags & HASHMAP_BUCKET_OCCUPIED) {
            return bucket->value;
        }
        bucket->key = key;
        bucket->_hash = key_hash;
        bucket->_flags = HASHMAP_BUCKET_OCCUPIED;
        size++;
        return bucket->value;
    }
};
};     // namespace containers
};     // namespace gtr
#endif // HASHMAP_H
