#ifndef ALLOCATORS_H
#define ALLOCATORS_H
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <memory>
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
/*
 * Allocators that will be used by gtr containers.
 * A valid allocator for a single buffer gtr container must implement
 * malloc, realloc and free (NOTE: realloc must support shrinking)
 * The C++ standard allocator interface is also implemented.
 */
namespace gtr {
namespace containers {

/*
 * C-allocator (deafult allocator for most gtr containers)
 */
template <typename T> struct c_allocator {
    // C++ standard allocator
    using value_type = T;
    template <typename U> struct rebind {
        typedef c_allocator<U> other;
    };
    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }
    template <class U> c_allocator(const c_allocator<U> &) noexcept {}
    template <class U> bool operator==(const c_allocator<U> &) const noexcept { return true; }
    template <class U> bool operator!=(const c_allocator<U> &) const noexcept { return false; }

    // GTR standard allocator
    inline T *malloc(std::size_t size) { return reinterpret_cast<T *>(std::malloc(size * sizeof(T))); }
    inline T *realloc(T *ptr, std::size_t size, std::size_t old_size) {
        (void)old_size;
        return reinterpret_cast<T *>(std::realloc(ptr, size * sizeof(T)));
    }
    inline constexpr void free(void *ptr, std::size_t size) { (void)size, std::free(ptr); }

    constexpr c_allocator(){};
    c_allocator(c_allocator<T> &&other) noexcept = default;
    c_allocator(const c_allocator<T> &other) = default;
    c_allocator<T> &operator=(const c_allocator<T> &other) noexcept = default;
    c_allocator<T> &operator=(c_allocator<T> &&other) = default;
};

/*
 * Static allocator
 */
template <typename T, int N = 256 * 1024> struct static_allocator {
    alignas(std::max_align_t) std::byte buffer[N];
    std::size_t used = 0;

    // C++ standard allocator
    using value_type = T;
    template <typename U> struct rebind {
        typedef static_allocator<U, N> other;
    };
    T *allocate(std::size_t n) { return malloc(n); }
    void deallocate(T *p, std::size_t n) { return free(p, n); }
    template <class U> static_allocator(const static_allocator<U, N> &) noexcept {}
    template <class U> bool operator==(const static_allocator<U, N> &) const noexcept { return true; }
    template <class U> bool operator!=(const static_allocator<U, N> &) const noexcept { return false; }

    // GTR standard allocator
    inline T *malloc(std::size_t size) {
        size_t off = get_offset();
        if (used + off + size * sizeof(T) >= N)
            return 0;
        T *result = reinterpret_cast<T *>(buffer + used + off);
        used += off + size * sizeof(T);
        return result;
    }
    inline std::size_t get_offset() const {
        if (used % alignof(T) == 0)
            return 0;
        return alignof(T) - (used % alignof(T));
    }
    inline T *realloc(T *ptr, std::size_t size, std::size_t old_size) {
        if (old_size < size)
            return ptr;
        // Trick to avoid memcpy
        if (old_size * sizeof(T) == used - get_offset()) {
            used = 0;
            return malloc(size);
        } else {
            T *new_ptr = malloc(size);
            if (new_ptr) {
                std::memcpy(new_ptr, ptr, old_size * sizeof(T));
            }
            return new_ptr;
        }
    }
    inline void free(void *ptr, std::size_t size) {
        (void)ptr;
        (void)size;
    }

    constexpr static_allocator(){};
    // No copies or moves allowed
    static_allocator(static_allocator<T, N> &&other) = default;
    static_allocator(const static_allocator<T, N> &other) = default;
    static_allocator<T, N> &operator=(const static_allocator<T, N> &other) = default;
    static_allocator<T, N> &operator=(static_allocator<T, N> &&other) noexcept = default;
};
}; // namespace containers
}; // namespace gtr
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
