#include <gtest/gtest.h>
#include "hashmap.h"
#include "gtr_string.h"
using namespace gtr::containers;
using namespace gtr;

TEST(HashMapTest, DefaultConstructor){
    hashmap<int, int> map;
    EXPECT_EQ(map.capacity, 0);
}

TEST(HashMapTest, Add){
    hashmap<int, int> map;
    map.add(10, 69);
    EXPECT_EQ(map.size, 1);
    EXPECT_TRUE(map.contains(10));
    EXPECT_FALSE(map.contains(11));

    int value = 0;
    EXPECT_TRUE(map.try_get_value(10, value));
    EXPECT_EQ(value, 69);

    map.add(1829673, 23);
    map.add(12312, 43);
    map.add(12334, 12);
    EXPECT_EQ(map.size, 4);
	EXPECT_TRUE(map.contains(1829673));
    EXPECT_TRUE(map.contains(12334));
}

TEST(HashMapTest, Colision){
    hashmap<int, int> map(10);
    
    map.add(10, 1);
    map.add(100, 2);
    map.add(1000, 3);
    EXPECT_EQ(map.size, 3);
    
    int value = 0;
    EXPECT_TRUE(map.try_get_value(10, value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(map.try_get_value(100, value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(map.try_get_value(1000, value));
    EXPECT_EQ(value, 3);
}

TEST(HashMapTest, StringAndMove){
    static_string<> ff = "Fernando";
    hashmap<static_string<>, static_string<>> map;
    map.add(ff, "Ritter");
    map.add("Lahude", "Xibata");
    
    auto a = map.find("Fernando");
    EXPECT_TRUE(a != map.end());
    EXPECT_STREQ(a.value(), "Ritter");
    
    auto b = map.find("Lahude");
    EXPECT_TRUE(b != map.end());
    EXPECT_STREQ(b.value(), "Xibata");
    
    auto c = map.remove("Lahude");
    EXPECT_EQ(c.pointer, b.pointer);
    
    hashmap<const char*, const char*> map2(10);
    
    map2.add("Fernando", "Ritter");  
    EXPECT_TRUE(map2.contains("Fernando"));
    EXPECT_FALSE(map2.contains("Ritter"));
    const char* d;
    if (map2.try_get_value("Fernando", d)){
        EXPECT_STREQ(d, "Ritter");
    }
    auto map3 = std::move(map);
    EXPECT_EQ(map3.size, 1);
    
    auto a2 = map3.find("Fernando");
    EXPECT_TRUE(a2 != map3.end());
    EXPECT_STREQ(a2.value(), "Ritter");

    auto map4 = std::move(map2);
    EXPECT_EQ(map4.size, 1);
    
    auto a3 = map4.find("Fernando");
    EXPECT_TRUE(a3 != map4.end());
    EXPECT_STREQ(a3.value(), "Ritter");
}

TEST(HashMapTest, TombStone){
    hashmap<int, int> map(10);

    map.add(10, 1);
    map.add(100, 2);
    map.add(1000, 3);
    EXPECT_EQ(map.size, 3);
    
    int value = 0;
    EXPECT_TRUE(map.try_get_value(10, value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(map.try_get_value(100, value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(map.try_get_value(1000, value));
    EXPECT_EQ(value, 3);

    auto it = map.remove(100);
    EXPECT_TRUE(it != map.end());
    EXPECT_EQ(map.size, 2);
    EXPECT_FALSE(map.try_get_value(100, value));


    map.reserve(100);
    EXPECT_TRUE(map.try_get_value(10, value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(map.try_get_value(1000, value));
    EXPECT_EQ(value, 3);
}

TEST(HashMapTest, Grow){
    hashmap<int, int> map(5);
    map.add(10, 1);
    map.add(100, 2);
    map.add(1000, 3);
    map.add(10000, 4);
    for(int i = 10; i <= 10000; i *=10){
        EXPECT_TRUE(map.contains(i));
    }
}

TEST(HashMapTest, SubscribeOperator) {
    hashmap<int, int> map;
	map[1] = 10;
	map[2] = 100;
	map[3] = 1000;
	EXPECT_EQ(map.size, 3);
	EXPECT_EQ(map[1], 10);
	EXPECT_EQ(map[2], 100);
	EXPECT_EQ(map[3], 1000);
}

TEST(HashMapTest, Initializer){
    hashmap<const char *, int> map = {
        {"Fernando", static_cast<int>(strlen("Fernando"))},
        {"Alice", static_cast<int>(strlen("Alice"))},
        {"Bob", static_cast<int>(strlen("Bob"))},
        {"Charlie", static_cast<int>(strlen("Charlie"))},
        {"David", static_cast<int>(strlen("David"))},
        {"Eve", static_cast<int>(strlen("Eve"))},
        {"Frank", static_cast<int>(strlen("Frank"))},
        {"Grace", static_cast<int>(strlen("Grace"))},
        {"Ivy", static_cast<int>(strlen("Ivy"))},
        {"Jack", static_cast<int>(strlen("Jack"))},
        {"Tessa", static_cast<int>(strlen("Tessa"))},
        {"Uma", static_cast<int>(strlen("Uma"))},
        {"Victor", static_cast<int>(strlen("Victor"))},
        {"Courtney", static_cast<int>(strlen("Courtney"))},
        {"Dennis", static_cast<int>(strlen("Dennis"))},
        {"Kristen", static_cast<int>(strlen("Kristen"))},
        {"Henry", static_cast<int>(strlen("Henry"))},
        {"Jose", static_cast<int>(strlen("Jose"))},
        {"Douglas", static_cast<int>(strlen("Douglas"))},
        {"Melissa", static_cast<int>(strlen("Melissa"))},
        {"Keith", static_cast<int>(strlen("Keith"))},
        {"Alyssa", static_cast<int>(strlen("Alyssa"))},
        {"Zachary", static_cast<int>(strlen("Zachary"))},
        {"Kristin", static_cast<int>(strlen("Kristin"))},
        {"Kyle", static_cast<int>(strlen("Kyle"))},
        {"Natasha", static_cast<int>(strlen("Natasha"))},
        {"Katie", static_cast<int>(strlen("Katie"))},
        {"Billy", static_cast<int>(strlen("Billy"))},
        {"Monica", static_cast<int>(strlen("Monica"))},
        {"Lawrence", static_cast<int>(strlen("Lawrence"))},
    };
    EXPECT_EQ(map.size, 30);

    auto it = map.find("Jose");
    EXPECT_TRUE(it != map.end());
    EXPECT_EQ(it->value, 4);

    map.remove("Jose");
    auto it2 = map.find("Jose");
    EXPECT_TRUE(it2 == map.end());

    for (auto &item : map) {
        static int i = 0;
        EXPECT_EQ(item.value, static_cast<int>(strlen(item.key)));
        i++;
    }

    EXPECT_TRUE(map.contains("Billy"));
    EXPECT_FALSE(map.contains("Marcelo"));
    EXPECT_TRUE(map.contains("Victor"));
    EXPECT_FALSE(map.contains("ALyssa")); // Typo
}

struct Point {
    short id;
    float x, y;

    const bool operator==(const Point &other) const {
		return id == other.id && x == other.x && y == other.y;
	}
    //hash function
    std::size_t operator()(const Point &p) const { return std::hash<short>()(p.id) ^ std::hash<float>()(p.x) ^ std::hash<float>()(p.y); }
};

TEST(HashMapTest, CustomKey) {
    hashmap<Point, int, Point> map;
    Point p1 = {1, 1.0f, 2.0f};
    Point p2 = {2, 3.0f, 4.0f};
    Point p3 = {3, 5.0f, 6.0f};
    map.add(p1, 10);
    map.add(p2, 20);
    map.add(p3, 30);
    EXPECT_EQ(map.size, 3);
    EXPECT_EQ(map[p1], 10);
    EXPECT_EQ(map[p2], 20);
    EXPECT_EQ(map[p3], 30);
}

#include <gtest/gtest.h>
#include <unordered_map>
#include "hashmap.h"
#include "static_string.h"
#include <random>

using namespace gtr::containers;

TEST(HashMapTest, HeavyPopulationInsertion) {
    hashmap<int, int> map;
    const int num_elements = 1000000; // One million elements

    // Insert elements
    for (int i = 0; i < num_elements; ++i) {
        map.add(i, i * 2);
    }

    // Verify size
    EXPECT_EQ(map.size, num_elements);

    // Verify that all elements are correctly inserted
    for (int i = 0; i < num_elements; ++i) {
        int value = 0;
        EXPECT_TRUE(map.try_get_value(i, value));
        EXPECT_EQ(value, i * 2);
    }
}

TEST(HashMapTest, HeavyPopulationInsertionAndRemoval) {
    hashmap<int, int> map;
    const int num_elements = 500000; // Half a million elements

    // Insert elements
    for (int i = 0; i < num_elements; ++i) {
        map.add(i, i * 3);
    }

    // Remove half of the elements
    for (int i = 0; i < num_elements; i += 2) {
        map.remove(i);
    }

    // Verify size
    EXPECT_EQ(map.size, num_elements / 2);

    // Verify that the correct elements are present
    for (int i = 0; i < num_elements; ++i) {
        int value = 0;
        if (i % 2 == 0) {
            EXPECT_FALSE(map.try_get_value(i, value));
        }
        else {
            EXPECT_TRUE(map.try_get_value(i, value));
            EXPECT_EQ(value, i * 3);
        }
    }
}

TEST(HashMapTest, RandomizedOperations) {
    hashmap<int, int> map;
    std::unordered_map<int, int> reference_map;
    const int num_operations = 100000;
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> dist_key(1, 1000000);
    std::uniform_int_distribution<int> dist_value(1, 1000000);
    std::uniform_int_distribution<int> dist_op(1, 100);

    for (int i = 0; i < num_operations; ++i) {
        int op = dist_op(rng);
        int key = dist_key(rng);
        int value = dist_value(rng);

        if (op <= 60) {
            // 60% chance to add or update
            map.add(key, value);
            reference_map[key] = value;
        }
        else if (op <= 80) {
            // 20% chance to remove
            map.remove(key);
            reference_map.erase(key);
        }
        else {
            // 20% chance to check existence
            bool map_contains = map.contains(key);
            bool ref_contains = reference_map.find(key) != reference_map.end();
            EXPECT_EQ(map_contains, ref_contains);
            if (map_contains) {
                int map_value = 0;
                EXPECT_TRUE(map.try_get_value(key, map_value));
                EXPECT_EQ(map_value, reference_map[key]);
            }
        }
    }

    // Final verification
    EXPECT_EQ(map.size, reference_map.size());
    for (const auto& pair : reference_map) {
        int value = 0;
        EXPECT_TRUE(map.try_get_value(pair.first, value));
        EXPECT_EQ(value, pair.second);
    }
}

TEST(HashMapTest, IteratorValidityUnderHeavyLoad) {
    hashmap<int, int> map;
    const int num_elements = 100000;

    // Insert elements
    for (int i = 0; i < num_elements; ++i) {
        map.add(i, i + 100);
    }

    // Verify iterator can traverse all elements
    int count = 0;
    for (const auto& bucket : map) {
        EXPECT_EQ(bucket.value, bucket.key + 100);
        ++count;
    }
    EXPECT_EQ(count, num_elements);
}

TEST(HashMapTest, StressTestWithStrings) {
    hashmap<string, int> map;
    const int num_elements = 50;

    // Insert elements
    for (int i = 0; i < num_elements; ++i) {
        map.add(string("Key_") + string::to_string(i), i);
    }

    // Verify size
    EXPECT_EQ(map.size, num_elements);

    // Remove some elements
    for (int i = 0; i < num_elements; i += 3) {
        map.remove(string("Key_") + string::to_string(i));
    }

    // Verify elements
    for (int i = 0; i < num_elements; ++i) {
        int value = 0;
        if (i % 3 == 0) {
            EXPECT_FALSE(map.try_get_value(string("Key_") + string::to_string(i), value));
        }
        else {
            EXPECT_TRUE(map.try_get_value(string("Key_") + string::to_string(i), value));
            EXPECT_EQ(value, i);
        }
    }
}

TEST(HashMapTest, RehashingStressTest) {
    hashmap<int, int> map;

    // Insert elements to trigger multiple rehashes
    for (int i = 0; i < 200000; ++i) {
        map.add(i, i * i);
        // Occasionally check size and capacity
        if (i % 50000 == 0 && i != 0) {
            EXPECT_GE(map.capacity, i);
        }
    }

    // Verify all elements
    for (int i = 0; i < 200000; ++i) {
        int value = 0;
        EXPECT_TRUE(map.try_get_value(i, value));
        EXPECT_EQ(value, i * i);
    }
}

TEST(HashMapTest, HeavyPopulationWithCustomObjects) {
    struct LargeObject {
        int data[100];
        LargeObject() {
            for (int i = 0; i < 100; ++i) data[i] = i;
        }
        bool operator==(const LargeObject& other) const {
            for (int i = 0; i < 100; ++i) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }
    };

    struct LargeObjectHash {
        std::size_t operator()(const LargeObject& obj) const {
            std::size_t hash = 0;
            for (int i = 0; i < 100; ++i) {
                hash ^= std::hash<int>()(obj.data[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

    hashmap<LargeObject, int, LargeObjectHash> map;
    const int num_elements = 10000;

    // Insert elements
    for (int i = 0; i < num_elements; ++i) {
        LargeObject obj;
        obj.data[0] = i; // Make each object unique
        map.add(obj, i);
    }

    // Verify elements
    for (int i = 0; i < num_elements; ++i) {
        LargeObject obj;
        obj.data[0] = i;
        int value = 0;
        EXPECT_TRUE(map.try_get_value(obj, value));
        EXPECT_EQ(value, i);
    }
}   

TEST(HashMapTest, Iterators)
{
    hashmap<int, int> map;
    map.add(1, 10);
    map.add(2, 20);
    map.add(3, 30);
    map.add(4, 40);
    map.add(5, 50);

    // Test begin and end

    auto it = map.begin();
    EXPECT_EQ(it->key, 1);
}


TEST(HashMapTestDictionary, DefaultConstructor) {
    dictionary<int, int> dict;
    EXPECT_EQ(dict.capacity, 0);

}

TEST(HashMapTestDictionary, Add) {
    dictionary<int, int> dict;
    dict.add(10, 69);
    EXPECT_EQ(dict.size, 1);
    EXPECT_TRUE(dict.contains(10));
    EXPECT_FALSE(dict.contains(11));
    int value = 0;
    EXPECT_TRUE(dict.try_get_value(10, value));
    EXPECT_EQ(value, 69);
    dict.add(1829673, 23);
    dict.add(12312, 43);
    dict.add(12334, 12);
    EXPECT_EQ(dict.size, 4);
    EXPECT_TRUE(dict.contains(1829673));
    EXPECT_TRUE(dict.contains(12334));
}

TEST(HashMapTestDictionary, Colision) {
    dictionary<int, int> dict(10);
    dict.add(10, 1);
    dict.add(100, 2);
    dict.add(1000, 3);
    EXPECT_EQ(dict.size, 3);
    int value = 0;
    EXPECT_TRUE(dict.try_get_value(10, value));
    EXPECT_EQ(value, 1);
    EXPECT_TRUE(dict.try_get_value(100, value));
    EXPECT_EQ(value, 2);
    EXPECT_TRUE(dict.try_get_value(1000, value));
    EXPECT_EQ(value, 3);
}

TEST(HashMapTestDictionary, StringAndMove) {
    string ff = "Fernando";
    dictionary<string, string> dict;
    dict.add(ff, "Ritter");
    dict.add("Lahude", "Xibata");
    auto a = dict.find("Fernando");
    EXPECT_TRUE(a != dict.end());
    EXPECT_STREQ(a.value(), "Ritter");
    auto b = dict.find("Lahude");
    EXPECT_TRUE(b != dict.end());
    EXPECT_STREQ(b.value(), "Xibata");
    auto c = dict.remove("Lahude");
    EXPECT_EQ(c.pointer, b.pointer);
    dictionary<const char *, const char *> dict2(10);
    dict2.add("Fernando", "Ritter");
    EXPECT_TRUE(dict2.contains("Fernando"));
    EXPECT_FALSE(dict2.contains("Ritter"));
    const char *d;
    if (dict2.try_get_value("Fernando", d)) {
        EXPECT_STREQ(d, "Ritter");
    }
    auto dict3 = std::move(dict);
    EXPECT_EQ(dict3.size, 1);
    auto a2 = dict3.find("Fernando");
    EXPECT_TRUE(a2 != dict3.end());
    EXPECT_STREQ(a2.value(), "Ritter");
    auto dict4 = std::move(dict2);
    EXPECT_EQ(dict4.size, 1);
    auto a3 = dict4.find("Fernando");
    EXPECT_TRUE(a3 != dict4.end());
    EXPECT_STREQ(a3.value(), "Ritter");
}

TEST(HashMapTestDictionary, Keys) {
    dictionary<int, int> dict;
    dict.add(1, 10);
    dict.add(2, 20);
    dict.add(3, 30);
    dict.add(4, 40);
    dict.add(5, 50);
    auto keys = dict.keys();
    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 3);
    EXPECT_EQ(keys[3], 4);
    EXPECT_EQ(keys[4], 5);
    dict.remove(3);
    keys = dict.keys();
    EXPECT_EQ(keys[0], 1);
    EXPECT_EQ(keys[1], 2);
    EXPECT_EQ(keys[2], 4);
    EXPECT_EQ(keys[3], 5);
}



int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
