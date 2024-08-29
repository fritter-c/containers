#ifndef TRACKING_ALLOCATOR_H
#define TRACKING_ALLOCATOR_H
#include <cassert>
#include <cstddef>
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
namespace gtr {
namespace containers {
template <typename T> struct tracking_allocator {
    std::unordered_set<void *> ptrs;

    // GTR standard allocator
    inline T *malloc(std::size_t size) {
        auto ptr = std::malloc(size * sizeof(T) + sizeof(std::size_t));
        ptrs.insert((void *)(((std::size_t *)ptr) + 1));
        *((std::size_t *)ptr) = size;
        return reinterpret_cast<T *>((((std::size_t *)ptr) + 1));
    }
    inline T *realloc(T *ptr, std::size_t size, std::size_t old) {
        if (ptrs.find(ptr) == ptrs.end())
            throw std::runtime_error("relloc called on stray pointer");
        std::size_t old_size = *((std::size_t *)ptr - 1);
        if (old != old_size)
            throw std::runtime_error("relloc invalid size");
        auto new_ptr = std::realloc((std::size_t *)ptr - 1, size * sizeof(T) + sizeof(std::size_t));
        ptrs.erase(ptr);
        ptrs.insert((void *)(((std::size_t *)new_ptr) + 1));
        *((std::size_t *)new_ptr) = size;
        return reinterpret_cast<T *>((((std::size_t *)new_ptr) + 1));
    }
    inline void free(void *ptr, std::size_t size) {
        if (ptrs.find(ptr) == ptrs.end())
            throw std::runtime_error("free called on stray pointer");
        ptrs.erase(ptr);
        std::size_t allocated_size = *((std::size_t *)ptr - 1);
        if (allocated_size != size)
            throw std::runtime_error("bad memory allocation");
        std::free((std::size_t *)ptr - 1);
    }

    ~tracking_allocator() { assert(ptrs.empty() == true); }
    tracking_allocator(){};
    tracking_allocator(tracking_allocator<T> &&other) noexcept : ptrs(std::move(other.ptrs)){};
    tracking_allocator(const tracking_allocator<T> &other){};
    tracking_allocator<T> &operator=(const tracking_allocator<T> &other){};
    tracking_allocator<T> &operator=(tracking_allocator<T> &&other) noexcept {
        if (this != &other) {
            ptrs = std::move(other.ptrs);
        }
        return *this;
    };
};

}; // namespace containers
}; // namespace gtr
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif
