#ifndef ALLOCATOR_BASE_H
#define ALLOCATOR_BASE_H
#include <utility>
namespace gtr {
namespace containers {
template <typename T, class Allocator> struct _allocator_ebo : private Allocator {
    _allocator_ebo(const Allocator &alloc = Allocator()) : Allocator(alloc){};
    _allocator_ebo(Allocator &&alloc) noexcept : Allocator(std::move(alloc)){};
    Allocator &get_allocator() { return *this; }
    const Allocator &get_allocator() const { return *this; }
};
}; // namespace containers
}; // namespace gtr
#endif
