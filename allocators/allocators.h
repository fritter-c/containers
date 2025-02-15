#ifndef ALLOCATORS_H
#define ALLOCATORS_H
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include "../redblack_tree.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

/*
 * Allocators that will be used by gtr containers.
 * A valid allocator for a single buffer gtr container must implement
 * malloc, realloc and free (NOTE: realloc must support shrinking)
 * IMPORTANT: free on NULL pointer can be called
 */
namespace gtr {
namespace containers {

/*
 * C-allocator (deafult allocator for most gtr containers)
 * The C++ standard allocator interface is also implemented.
 */
template <typename T> struct c_allocator {
    // C++ standard allocator
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }

    template <class U> bool operator==(const c_allocator<U> &) const noexcept { return true; }
    template <class U> bool operator!=(const c_allocator<U> &) const noexcept { return false; }

    // GTR standard allocator
    inline T *malloc(std::size_t size) {

        return reinterpret_cast<T *>(std::malloc(size * sizeof(T)));
    }
    inline T *realloc(T *ptr, std::size_t size, std::size_t old_size) {
        (void)old_size;

        return reinterpret_cast<T *>(std::realloc(ptr, size * sizeof(T)));
    }
    inline void free(void *ptr, std::size_t size) {
        (void)size, std::free(ptr);
    }

    constexpr c_allocator() noexcept = default;
    constexpr ~c_allocator() = default;

    template <class U> c_allocator(c_allocator<U> &&) noexcept {}

    template <class U> c_allocator(const c_allocator<U> &) noexcept {}
};

template <typename T> struct tracking_allocator {
    // C++ standard allocator
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    struct allocation {
        T *ptr;
        std::size_t size;

        bool operator==(const allocation &other) const { return ptr == other.ptr; }
        bool operator!=(const allocation &other) const { return ptr != other.ptr; }
        bool operator>(const allocation &other) const { return ptr > other.ptr; }
        bool operator<(const allocation &other) const { return ptr < other.ptr; }
    };

    redblack_tree<allocation> allocations;

    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }

    // GTR standard allocator
    inline T *malloc(std::size_t size) {
        auto ptr =
            reinterpret_cast<redblack_tree<allocation>::node *>(std::malloc(size * sizeof(T) + sizeof(typename redblack_tree<allocation>::node)));
        ptr->value.size = size;
        ptr->value.ptr = reinterpret_cast<T *>(ptr + 1);
        allocations.insert(ptr);
        return ptr->value.ptr;
    }
    inline T *realloc(T *ptr, std::size_t size, std::size_t old_size) {
        if (ptr == nullptr) {
            return malloc(size);
        }

        auto alloc = allocations.find(*(reinterpret_cast<redblack_tree<allocation>::node *>(ptr) - 1));
        if (alloc) {
            // check if the size matches old_size
            if (alloc->value.size != old_size) {
                throw std::runtime_error("Size mismatch in realloc");
            }
        } else {
            throw std::runtime_error("Allocation not found in realloc");
        }

        allocations.erase(alloc->value);
        auto new_ptr = reinterpret_cast<redblack_tree<allocation>::node *>(
            std::realloc(reinterpret_cast<redblack_tree<allocation>::node *>(ptr) - 1, size * sizeof(T) + sizeof(allocation)));
        new_ptr->value.size = size;
        new_ptr->value.ptr = reinterpret_cast<T *>(new_ptr + 1);
        allocations.insert(new_ptr);
        return new_ptr->value.ptr;
    }
    inline void free(void *ptr, std::size_t size) {
        auto alloc = allocations.find((reinterpret_cast<redblack_tree<allocation>::node *>(ptr) - 1)->value);
        if (alloc) {
            if (alloc->value.size != size) {
                throw std::runtime_error("Size mismatch in free");
            }
            allocations.erase(alloc);
            std::free(alloc);
        }
        else {
            throw std::runtime_error("Allocation not found in free");
        }
    }

    constexpr tracking_allocator() noexcept = default;
    constexpr ~tracking_allocator() = default;
    void assert_no_leaks() {
        if (allocations.root) {
            throw std::runtime_error("Memory leak detected");
        }
    }
};

/*
 * Meta-allocator (used for containers that require additional metadata)
 * NOTE: No metadata is allocated if the container buffer is not allocated
 * NOTE2: Metadata must be POD type
 * The C++ standard allocator interface is also implemented.
 */
template <typename T> struct meta_allocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    std::size_t metadata_size;

    template <class U> bool operator==(const meta_allocator<U> &other) const noexcept { return metadata_size == other.metadata_size; }
    template <class U> bool operator!=(const meta_allocator<U> &other) const noexcept { return !(*this == other); }

    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }

    // GTR standard allocator
    inline T *malloc(std::size_t size) {
        return reinterpret_cast<T *>(reinterpret_cast<char *>(std::malloc(size * sizeof(T) + metadata_size)) + metadata_size);
    }
    inline T *realloc(T *ptr, std::size_t size, std::size_t old_size) {
        (void)old_size;
        if (ptr == nullptr) {
            return malloc(size);
        }
        return reinterpret_cast<T *>(
            reinterpret_cast<char *>(std::realloc(reinterpret_cast<char *>(ptr) - metadata_size, size * sizeof(T) + metadata_size)) +
            metadata_size);
    }
    inline void free(void *ptr, std::size_t size) {
        (void)size;
        if (ptr)
            std::free(reinterpret_cast<char *>(ptr) - metadata_size);
    }

    inline char *get_metadata(T *ptr) { return reinterpret_cast<char *>(ptr) - metadata_size; }
    inline const char *get_metadata(T *ptr) const { return reinterpret_cast<const char *>(ptr) - metadata_size; }

    constexpr meta_allocator() noexcept = default;
    constexpr ~meta_allocator() = default;

    template <class U> meta_allocator(c_allocator<U> &&other) noexcept { metadata_size = other.metadata_size; }

    template <class U> meta_allocator(const c_allocator<U> &other) noexcept { metadata_size = other.metadata_size; }
};

/*
 * Allocator for dictionary containers
 * The C++ standard allocator interface is also implemented.
 * This allocates buckets in addition to the key buffer in the same memory block
 */
template <typename T> struct dictionary_allocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using key_type = T::key_type;

    template <class U> bool operator==(const dictionary_allocator<U> &) const noexcept { return true; }
    template <class U> bool operator!=(const dictionary_allocator<U> &) const noexcept { return false; }

    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }

    // GTR standard allocator
    inline T *malloc(std::size_t size) {
        std::size_t total_size = size * sizeof(T) + size * sizeof(key_type);
        auto ptr = static_cast<char *>(std::malloc(total_size));
        ptr += size * sizeof(key_type);
        return reinterpret_cast<T *>(ptr);
    }

    inline void free(void *ptr, std::size_t size) {
        if (ptr) {
            auto p = static_cast<char *>(ptr);
            p -= size * sizeof(key_type);
            std::free(p);
        }
    }

    inline key_type *get_keys(T *ptr, std::size_t size) {
        return reinterpret_cast<key_type *>(reinterpret_cast<char *>(ptr) - size * sizeof(key_type));
    }

    constexpr dictionary_allocator() noexcept = default;
    constexpr ~dictionary_allocator() = default;

    template <class U> dictionary_allocator(dictionary_allocator<U> &&) noexcept {}
};

#if defined(_MSC_VER) || defined(_WIN64) || defined(_WIN32)
#define ALIGNED_ALLOC(size, alignment) _aligned_malloc(size, alignment)
#else
#define ALIGNED_ALLOC(size, alignment) aligned_alloc(alignment, size)
#endif

template <typename T, auto Alignment = alignof(std::max_align_t)> struct aligned_allocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    template <class U> bool operator==(const aligned_allocator<U> &) const noexcept { return true; }
    template <class U> bool operator!=(const aligned_allocator<U> &) const noexcept { return false; }
    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }
    // GTR standard allocator
    inline T *malloc(std::size_t size) { return reinterpret_cast<T *>(ALIGNED_ALLOC(Alignment, size * sizeof(T))); }
    inline void free(void *ptr, std::size_t size) { (void)size, std::free(ptr); }
    constexpr aligned_allocator() noexcept = default;
    constexpr ~aligned_allocator() = default;
    template <class U> aligned_allocator(aligned_allocator<U> &&) noexcept {}
    template <class U> aligned_allocator(const aligned_allocator<U> &) noexcept {}
};

template <typename T, class Object> struct object_allocator {
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::false_type;
    using object_type = Object;

    template <class U> bool operator==(const object_allocator<U, Object> &) const noexcept { return true; }
    template <class U> bool operator!=(const object_allocator<U, Object> &) const noexcept { return false; }

    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }

    // GTR standard allocator
    inline T *malloc(std::size_t size) {
        auto *ptr = (char *)std::malloc(size * sizeof(T) + sizeof(Object));
        new (ptr) Object();
        return reinterpret_cast<T *>(ptr + sizeof(Object));
    }

    inline void free(void *ptr, std::size_t size) {
        if (ptr) {
            auto *p = (char *)ptr - sizeof(Object);
            ((Object *)p)->~Object();
            std::free(p);
        }
    }

    const inline Object *get_object(T *ptr) const { return reinterpret_cast<Object *>(reinterpret_cast<char *>(ptr) - sizeof(Object)); }
    inline Object *get_object(T *ptr) { return reinterpret_cast<Object *>(reinterpret_cast<char *>(ptr) - sizeof(Object)); }
};

template <typename T> struct is_object_allocator : std::false_type {};
template <typename T, typename Object> struct is_object_allocator<object_allocator<T, Object>> : std::true_type {};
template <typename T> inline constexpr bool is_object_allocator_v = is_object_allocator<T>::value;

}; // namespace containers
}; // namespace gtr
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
