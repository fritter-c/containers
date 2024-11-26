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
    inline T *malloc(std::size_t size) { return reinterpret_cast<T *>(std::malloc(size * sizeof(T))); }
    inline T *realloc(T *ptr, std::size_t size, std::size_t old_size) {
        (void)old_size;
        return reinterpret_cast<T *>(std::realloc(ptr, size * sizeof(T)));
    }
    inline void free(void *ptr, std::size_t size) { (void)size, std::free(ptr); }

    constexpr c_allocator() noexcept = default;
    constexpr ~c_allocator() = default;

    template <class U> c_allocator(c_allocator<U> &&) noexcept {}

    template <class U> c_allocator(const c_allocator<U> &) noexcept {}
};
}; // namespace containers
}; // namespace gtr
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
