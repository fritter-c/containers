#include <gtest/gtest.h>
#include "allocators/allocators.h"
#include <vector>

using namespace gtr::containers;

void create_allocator_leak() {
    tracking_allocator<int> alloc;
    alloc.malloc(10);
    alloc.assert_no_leaks();
}

void create_allocator_no_leak() {
    tracking_allocator<int> alloc;
    alloc.free(alloc.malloc(10), 10);
    alloc.assert_no_leaks();
}
TEST(TrackingAllocatorTest, Basic) {

	EXPECT_THROW(create_allocator_leak(), std::runtime_error);
    EXPECT_NO_THROW(create_allocator_no_leak());
}

TEST(TrackingAllocatorTest, MultipleAllocs) {
    std::vector<int *> ptrs;
    tracking_allocator<int> alloc;
    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW(ptrs.push_back(alloc.malloc(10)));
    }
    for (int i = 0; i < 10; ++i) {
        EXPECT_NO_THROW(alloc.free(ptrs[i], 10));
    }
    EXPECT_NO_THROW(alloc.assert_no_leaks());
    
}