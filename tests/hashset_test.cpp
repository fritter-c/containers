#include <gtest/gtest.h>
#include "hashset.h"
using namespace gtr::containers;

TEST(HashSetTest, DefaultConstructor) {
    hashset<int> set;
    EXPECT_EQ(set.size, 0);
    EXPECT_EQ(set.capacity, 0);
}

TEST(HashSetTest, Add) {
    hashset<int> set;
    set.add(10);
    EXPECT_EQ(set.size, 1);
    EXPECT_TRUE(set.contains(10));
    EXPECT_FALSE(set.contains(11));

    set.add(1829673);
    set.add(12312);
    set.add(12334);
    EXPECT_EQ(set.size, 4);
    EXPECT_TRUE(set.contains(1829673));
    EXPECT_TRUE(set.contains(12334));
}

TEST(HashSetTest, Remove) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    set.remove(10);
    EXPECT_EQ(set.size, 3);
    EXPECT_FALSE(set.contains(10));
    EXPECT_TRUE(set.contains(1829673));
    EXPECT_TRUE(set.contains(12334));
}

TEST(HashSetTest, Clear) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    set.clear();
    EXPECT_EQ(set.size, 0);
    EXPECT_FALSE(set.contains(10));
    EXPECT_FALSE(set.contains(1829673));
    EXPECT_FALSE(set.contains(12312));
    EXPECT_FALSE(set.contains(12334));
}

TEST(HashSetTest, MoveConstructor) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    hashset<int> set2(std::move(set));
    EXPECT_EQ(set2.size, 4);
    EXPECT_TRUE(set2.contains(10));
    EXPECT_TRUE(set2.contains(1829673));
    EXPECT_TRUE(set2.contains(12312));
    EXPECT_TRUE(set2.contains(12334));
}

TEST(HashSetTest, MoveAssignment) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    hashset<int> set2;
    set2 = std::move(set);
    EXPECT_EQ(set2.size, 4);
    EXPECT_TRUE(set2.contains(10));
    EXPECT_TRUE(set2.contains(1829673));
    EXPECT_TRUE(set2.contains(12312));
    EXPECT_TRUE(set2.contains(12334));
}

TEST(HashSetTest, Contains) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    EXPECT_TRUE(set.contains(10));
    EXPECT_TRUE(set.contains(1829673));
    EXPECT_TRUE(set.contains(12312));
    EXPECT_TRUE(set.contains(12334));
    EXPECT_FALSE(set.contains(123));
    EXPECT_FALSE(set.contains(1234));
    EXPECT_FALSE(set.contains(12345));
}

TEST(HashSetTest, Reserve) {
    hashset<int> set;
    set.reserve(10);
    EXPECT_EQ(set.capacity, 10);
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    set.add(123);
    set.add(1234);
    set.add(12345);
    set.add(123456);
    set.add(1234567);
    set.add(12345678);
    set.add(123456789);
    EXPECT_EQ(set.size, 11);
    EXPECT_EQ(set.capacity, 20);
}

TEST(HashSetTest, Size) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    EXPECT_EQ(set.size, 4);
}

TEST(HashSetTest, BeginEnd) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    int count = 0;
    for (auto it = set.begin(); it != set.end(); ++it) {
        count++;
    }
    EXPECT_EQ(count, 4);

    for(const auto &i : set) {
        count--;
    }
    EXPECT_EQ(count, 0);
}

TEST(HashSetTest, Get) {
    hashset<int> set;
    set.add(10);
    set.add(1829673);
    set.add(12312);
    set.add(12334);
    auto it = set.find(10);
    EXPECT_TRUE(it != set.end());
    EXPECT_EQ(*it, 10);
    it = set.find(12312);
    EXPECT_TRUE(it != set.end());
    EXPECT_EQ(*it, 12312);
    it = set.find(123);
    EXPECT_TRUE(it == set.end());
}

TEST(HashMapTest, FullSet) {
    hashset<int> map(64);
    for (int j = 0; j < 2; j++) {
        for (int i = j; i < j + 47; i++) {
            map.add(i);
        }
        for (int i = j; i < j + 47; i++) {
            EXPECT_TRUE(map.contains(i));
        }
        for (int i = j; i < j +  47; i++) {
            map.remove(i);
        }
    }
    EXPECT_EQ(map.capacity, 64);
}
