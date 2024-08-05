#ifndef HASHMAP_H
#define HASHMAP_H
#include "allocator_base.h"
#include "allocators/allocators.h"

namespace gtr {
namespace containers {
// FNV-1a
template <typename Key> struct hashmap_internal_hash {
    std::size_t operator()(const Key &key) {
        const char *s = (const char *)&key;
        std::size_t hash = 14695981039346656037ULL;
        std::size_t fnv_prime = 1099511628211ULL;
        for (std::size_t i = 0; i < sizeof(Key); ++i) {
            hash = hash ^ s[i];
            hash = hash * fnv_prime;
        }
        return hash;
    }
};

template <> struct hashmap_internal_hash<const char *> {
    std::size_t operator()(const char *key) {
        std::size_t hash = 14695981039346656037ULL;
        std::size_t fnv_prime = 1099511628211ULL;
        for (std::size_t i = 0; key[i] != '\0'; ++i) {
            hash = hash ^ key[i];
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
struct hashmap : private _allocator_ebo<T, Allocator> {
    struct iterator {
        using value_type = hashmap_bucket<Key, T>;
        value_type *pointer;
        value_type *end_ptr;

        inline iterator(value_type *ptr, value_type *end) : pointer(ptr), end_ptr(end) {
            if (pointer != end_ptr && !(pointer->_flags & (HASHMAP_BUCKET_OCCUPIED | HASHMAP_BUCKET_TOMBSTONE)))
                operator++();
        };
        inline value_type &operator*() const { return *pointer; }
        inline value_type *operator->() { return pointer; }
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

    Allocator &allocator() { return _allocator_ebo<T, Allocator>::get_allocator(); }
    const Allocator &allocator() const { return _allocator_ebo<T, Allocator>::get_allocator(); }

    constexpr inline std::size_t bucket_size() { return sizeof(bucket); }
    constexpr inline void _free_all() { allocator().free(data, capacity); }

    inline hashmap() : _allocator_ebo<T, Allocator>(), size(0), capacity(64), load_factor(0.6f), growth_factor(1.0f) {
        data = allocator().malloc(capacity);
        std::memset(data, 0, capacity * bucket_size());
    };

    inline explicit hashmap(std::size_t _Reserved)
        : _allocator_ebo<T, Allocator>(), size(0), capacity(_Reserved), load_factor(0.6f), growth_factor(1.0f) {
        data = allocator().malloc(capacity);
        std::memset(data, 0, capacity * bucket_size());
    };

    inline hashmap(std::initializer_list<std::pair<Key, T>> init_list) : _allocator_ebo<T, Allocator>(), load_factor(0.6f), growth_factor(1.0f) {
        std::size_t init_size = init_list.size();
        data = allocator().malloc(init_size * 4);
        capacity = init_size * 4;
        std::memset(data, 0, capacity * bucket_size());
        size = 0;
        for (const auto &pair : init_list)
            add(pair.first, pair.second);
    }

    inline hashmap(hashmap &&_Src) noexcept : _allocator_ebo<T, Allocator>(std::move(_Src.get_allocator())) {
        data = _Src.data;
        size = _Src.size;
        capacity = _Src.capacity;
        load_factor = _Src.load_factor;
        growth_factor = _Src.growth_factor;
        _Src.data = nullptr;
    }

    inline hashmap(const hashmap &other) = delete;
    inline hashmap &operator=(const hashmap &other) = delete;

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
        }
        return *this;
    }

    inline ~hashmap() {
        if (data)
            allocator().free(data, capacity);
        data = nullptr;
    }

    inline iterator begin() { return iterator(data, data + capacity); }
    inline iterator end() { return iterator(data + capacity, data + capacity); }
    inline const iterator begin() const { return iterator(data, data + capacity); }
    inline const iterator end() const { return iterator(data + capacity, data + capacity); }

    inline bucket *get_bucket(const Key &key, std::size_t &hash_created) {
        if ((size + 1) > (capacity * load_factor)) {
            return nullptr;
        }
        if (hash_created == -1)
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
        return bucket;
    }

    inline void reserve(std::size_t reserve) {
        if (reserve > capacity) {
            hashmap map(reserve);
            for (std::size_t i = 0; i < capacity; i++) {
                if (data[i]._flags & HASHMAP_BUCKET_OCCUPIED) {
                    map.add_with_hash(data[i].key, data[i].value, data[i]._hash);
                }
            }
            *this = std::move(map);
        }
    }
    inline void grow() {
        std::size_t new_capacity = static_cast<std::size_t>(static_cast<float>(capacity) * (1.0f + growth_factor));
        reserve(new_capacity);
    }

    inline void clear() {
        size = 0;
        std::memset(data, 0, capacity * bucket_size());
    }

    inline std::size_t hash(const Key &key) const { return HashFunc()(key); };
    inline std::size_t probe(std::size_t hash) const { return 1LL + hash % (capacity - 1LL); }

    inline iterator get(const Key &key) {
        std::size_t key_hash = -1;
        auto bucket = get_bucket(key, key_hash);
        return bucket ? (bucket->_flags & HASHMAP_BUCKET_OCCUPIED) ? iterator(bucket, data + capacity) : end() : end();
    }

    inline iterator add_with_hash(const Key &key, const T &value, std::size_t hash) {
        if ((size + 1) > (capacity * load_factor)) {
            grow();
        }
        std::size_t key_hash = hash;
        auto bucket = get_bucket(key, key_hash);
        if (bucket) {
            bucket->value = value;
            bucket->key = key;
            bucket->_hash = key_hash;
            bucket->_flags = HASHMAP_BUCKET_OCCUPIED;
            size++;
            return iterator(bucket, data + capacity);
        }
        return end();
    }

    inline iterator add(const Key &key, const T &value) { return add_with_hash(key, value, -1); }

    inline iterator remove(const Key &key) {
        iterator it = get(key);
        if (it != end()) {
            it->_flags = HASHMAP_BUCKET_TOMBSTONE;
            size--;
            return it;
        }
        return end();
    }

    inline bool contains(const Key &key) { return get(key) != end(); }

    inline bool try_get_value(const Key &key, T &Value) {
        if (auto it = get(key); it != end()) {
            Value = it->value;
            return true;
        }
        return false;
    }

    inline T &operator[](const Key &key) {
        std::size_t key_hash = -1;
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
}; // namespace containers
} // namespace gtr
#endif
