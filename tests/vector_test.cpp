#include <gtest/gtest.h>
#include "text.h"
#include "vector.h"

using namespace gtr::containers;
using namespace gtr;

TEST(VectorTest, DefaultConstructor) {
    vector<int> v;
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 0);
}

vector<text512> create_vector() {
    vector<text512> v;
    v.push_back("Hello, World!");
    v.push_back("Hello, World!");
    v.push_back("Hello, World!");
    return v;
}

TEST(VectorTest, WithNoTrivialData) {
    text256 t1("Hello, World!");
    text256 t2("Hello, World!");
    text256 t3("Hello, World!");
    vector<text256> v;
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_EQ(v[0], t1);
    EXPECT_EQ(v[1], t2);
    EXPECT_EQ(v[2], t3);

    // Copy constructor
    vector<text256> v2(v);

    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 4);
    EXPECT_EQ(v2[0], t1);
    EXPECT_EQ(v2[1], t2);
    EXPECT_EQ(v2[2], t3);
    v2.push_back(t1);
    EXPECT_EQ(v2.size(), 4);
    EXPECT_EQ(v2.capacity(), 4);

    // Move
    v2 = (std::move(v));
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 4);
}

TEST(VectorTest, GetVector) {
    vector<text512> v = create_vector();
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_STREQ(v[0], "Hello, World!");
    EXPECT_STREQ(v[1], "Hello, World!");
    EXPECT_STREQ(v[2], "Hello, World!");
}

TEST(VectorTest, Erase) {
    vector<text512> v = create_vector();
    v.erase(v.begin());
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_STREQ(v[0], "Hello, World!");
    EXPECT_STREQ(v[1], "Hello, World!");
    v.erase(v.begin() + 1);
    EXPECT_EQ(v.size(), 1);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_STREQ(v[0], "Hello, World!");
    v.erase(v.begin());
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 4);
}

TEST(VectorTest, Mean) {
    vector<double> v = {1.0, 2.0, 3.0, 4.0, 5.0};
    EXPECT_DOUBLE_EQ(v.mean(), 3.0);
}

TEST(VectorTest, Reverse) {
    vector<double> v = {1.0, 2.0, 3.0, 4.0, 5.0};
    vector<double> v2 = {5.0, 4.0, 3.0, 2.0, 1.0};
    v.reverse();
    EXPECT_TRUE(v == v2);
}

TEST(VectorTest, ShrinkToFit) {
    vector<text1024> v;
    v.resize(70);
    EXPECT_EQ(v.size(), 70);
    v.resize(30);
    EXPECT_EQ(v.size(), 30);
    EXPECT_EQ(v.capacity(), 70);
    v.shrink_to_fit();
    EXPECT_EQ(v.size(), 30);
    EXPECT_EQ(v.capacity(), 30);
}

TEST(VectorTest, Clear) {
    vector<text1024> v;
    v.resize(70);
    EXPECT_EQ(v.size(), 70);
    v.clear();
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 70);
}

TEST(VectorTest, Resize) {
    vector<text1024> v;
    v.resize(70);
    EXPECT_EQ(v.size(), 70);
    EXPECT_EQ(v.capacity(), 70);
    v.resize(30);
    EXPECT_EQ(v.size(), 30);
    EXPECT_EQ(v.capacity(), 70);
}

TEST(VectorTest, Reserve) {
    vector<text1024> v;
    v.reserve(70);
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 70);
    v.reserve(30);
    EXPECT_EQ(v.size(), 0);
    EXPECT_EQ(v.capacity(), 70);
}

TEST(VectorTest, PushBack) {
    vector<text1024> v;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    EXPECT_EQ(v.size(), 3);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_EQ(v[0], t1);
    EXPECT_EQ(v[1], t2);
    EXPECT_EQ(v[2], t3);
}

TEST(VectorTest, PopBack) {
    vector<text1024> v;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    v.pop_back();
    EXPECT_EQ(v.size(), 2);
    EXPECT_EQ(v.capacity(), 4);
    EXPECT_EQ(v[0], t1);
    EXPECT_EQ(v[1], t2);
}

TEST(VectorTest, CopyConstructor) {
    vector<text1024> v;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    vector<text1024> v2(v);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 4);
    EXPECT_EQ(v2[0], t1);
    EXPECT_EQ(v2[1], t2);
    EXPECT_EQ(v2[2], t3);
}

TEST(VectorTest, MoveConstructor) {
    vector<text1024> v;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    vector<text1024> v2(std::move(v));
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 4);
    EXPECT_EQ(v2[0], t1);
    EXPECT_EQ(v2[1], t2);
    EXPECT_EQ(v2[2], t3);
}

TEST(VectorTest, CopyAssignment) {
    vector<text1024> v;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    vector<text1024> v2;
    v2 = v;
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 4);
    EXPECT_EQ(v2[0], t1);
    EXPECT_EQ(v2[1], t2);
    EXPECT_EQ(v2[2], t3);
}

TEST(VectorTest, MoveAssignment) {
    vector<text1024> v;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    vector<text1024> v2;
    v2 = std::move(v);
    EXPECT_EQ(v2.size(), 3);
    EXPECT_EQ(v2.capacity(), 4);
    EXPECT_EQ(v2[0], t1);
    EXPECT_EQ(v2[1], t2);
    EXPECT_EQ(v2[2], t3);
}

TEST(VectorTest, Equality) {
    vector<text1024> v1;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v1.push_back(t1);
    v1.push_back(t2);
    v1.push_back(t3);
    vector<text1024> v2;
    v2.push_back(t1);
    v2.push_back(t2);
    v2.push_back(t3);
    EXPECT_TRUE(v1 == v2);
    v2.pop_back();
    EXPECT_FALSE(v1 == v2);
}

TEST(VectorTest, Inequality) {
    vector<text1024> v1;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v1.push_back(t1);
    v1.push_back(t2);
    v1.push_back(t3);
    vector<text1024> v2;
    v2.push_back(t1);
    v2.push_back(t2);
    v2.push_back(t3);
    EXPECT_FALSE(v1 != v2);
    v2.pop_back();
    EXPECT_TRUE(v1 != v2);
}

TEST(VectorTest, LessThan) {
    vector<text1024> v1;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v1.push_back(t1);
    v1.push_back(t2);
    v1.push_back(t3);
    vector<text1024> v2;
    v2.push_back(t1);
    v2.push_back(t2);
    v2.push_back(t3);
    EXPECT_FALSE(v1 < v2);
    v2.pop_back();
    EXPECT_FALSE(v1 < v2);
    v2.push_back(t1);
    v2.push_back(t1);
    EXPECT_TRUE(v1 < v2);
}

TEST(VectorTest, LessThanOrEqual) {
    vector<text1024> v1;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v1.push_back(t1);
    v1.push_back(t2);
    v1.push_back(t3);
    vector<text1024> v2;
    v2.push_back(t1);
    v2.push_back(t2);
    v2.push_back(t3);
    EXPECT_TRUE(v1 <= v2);
    v2.pop_back();
    EXPECT_FALSE(v1 <= v2);
    v2.push_back(t1);
    v2.push_back(t1);
    EXPECT_TRUE(v1 <= v2);
}

TEST(VectorTest, GreaterThan) {
    vector<text1024> v1;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v1.push_back(t1);
    v1.push_back(t2);
    v1.push_back(t3);
    vector<text1024> v2;
    v2.push_back(t1);
    v2.push_back(t2);
    v2.push_back(t3);
    EXPECT_FALSE(v1 > v2);
    v2.pop_back();
    EXPECT_TRUE(v1 > v2);
    v2.push_back(t1);
    v2.push_back(t1);
    EXPECT_FALSE(v1 > v2);
}

TEST(VectorTest, GreaterThanOrEqual) {
    vector<text1024> v1;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v1.push_back(t1);
    v1.push_back(t2);
    v1.push_back(t3);
    vector<text1024> v2;
    v2.push_back(t1);
    v2.push_back(t2);
    v2.push_back(t3);
    EXPECT_TRUE(v1 >= v2);
    v2.pop_back();
    EXPECT_TRUE(v1 >= v2);
    v2.push_back(t1);
    v2.push_back(t1);
    EXPECT_FALSE(v1 >= v2);
}

TEST(VectorTest, IndexOperator) {
    vector<text1024> v;
    text1024 t1("Hello, World!");
    text1024 t2("Hello, World!");
    text1024 t3("Hello, World!");
    v.push_back(t1);
    v.push_back(t2);
    v.push_back(t3);
    EXPECT_EQ(v[0], t1);
    EXPECT_EQ(v[1], t2);
    EXPECT_EQ(v[2], t3);
}

TEST(VectorTest, Biggest) {
    vector<int> v = {1, 2, 3, 4, 5};
    EXPECT_EQ(v.biggest(), 4);
    v.clear();
    EXPECT_EQ(v.biggest(), vector<int>::npos);
}

TEST(VectorTest, Smallest) {
    vector<int> v = {1, 2, 3, 4, 5};
    EXPECT_EQ(v.smallest(), 0);
    v.clear();
    EXPECT_EQ(v.smallest(), vector<int>::npos);
}

TEST(VectorTest, Sum) {
    vector<int> v = {1, 2, 3, 4, 5};
    EXPECT_EQ(v.sum(), 15);
    v.clear();
    EXPECT_EQ(v.sum(), 0);
}

TEST(VectorTest, Product) {
    vector<int> v = {1, 2, 3, 4, 5};
    EXPECT_EQ(v.product(), 120);
}

struct dummy_comparable {
    int value;
    bool operator==(const dummy_comparable &rhs) const { return value == rhs.value; }
    bool operator<(const dummy_comparable &rhs) const { return value < rhs.value; }
    bool operator>(const dummy_comparable &rhs) const { return value > rhs.value; }
};

TEST(VectorTest, Find) {
    vector<int> v = {1, 2, 3, 4, 5};
    EXPECT_EQ(v.find(3), 2);
    EXPECT_EQ(v.find(6), vector<int>::npos);

    vector<dummy_comparable> v2 = {{1}, {2}, {3}, {4}, {5}};

    EXPECT_EQ(v2.find(dummy_comparable{3}), 2);
    EXPECT_EQ(v2.find(dummy_comparable{6}), vector<dummy_comparable>::npos);

    EXPECT_EQ(v2.find(dummy_comparable{3}, true), 2);
    EXPECT_EQ(v2.find(dummy_comparable{6}, true), vector<dummy_comparable>::npos);
}

TEST(VectorTest, Assign) {
    vector<int> v = {1, 2, 3, 4, 5};
    vector<int> v2 = {6, 7, 8, 9, 10};
    v.assign(v2);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v.capacity(), 5);
    EXPECT_EQ(v[0], 6);
    EXPECT_EQ(v[1], 7);
    EXPECT_EQ(v[2], 8);
    EXPECT_EQ(v[3], 9);
    EXPECT_EQ(v[4], 10);

    int *arr = (int *)malloc(5 * sizeof(int));
    arr[0] = 11;
    arr[1] = 12;
    arr[2] = 13;
    arr[3] = 14;
    arr[4] = 15;
    v.assign_buffer(arr, 5);
    EXPECT_EQ(v.size(), 5);
    EXPECT_EQ(v.capacity(), 5);
    EXPECT_EQ(v[0], 11);
    EXPECT_EQ(v[1], 12);
    EXPECT_EQ(v[2], 13);
    EXPECT_EQ(v[3], 14);
    EXPECT_EQ(v[4], 15);
}

struct complex_struct {
    void *data;
    int size;
    inline static int allocs = 0;

    // Copy constructor
    complex_struct(const complex_struct &other) {
        size = other.size;
        data = malloc(size);
        memcpy(data, other.data, size);
        allocs += size;
    };

    // Move constructor
    complex_struct(complex_struct &&other) noexcept {
        size = other.size;
        data = other.data;
        other.data = nullptr;
        other.size = 0;
    }

    // Copy assignment
    complex_struct &operator=(const complex_struct &other) {
        if (this != &other) {
            size = other.size;
            data = malloc(size);
            memcpy(data, other.data, size);
            allocs += size;
        }
        return *this;
    }

    // Move assignment
    complex_struct &operator=(complex_struct &&other) noexcept  {
        if (this != &other) {
            size = other.size;
            data = other.data;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }

    // Destructor
    ~complex_struct() {
        if (data) {
            free(data);
            allocs -= size;
        }
    }

    complex_struct() {
        data = malloc(10);
        size = 10;
        allocs += 10;
    }
};


TEST(VectorTest, ComplexStruct) {
    vector<double, object_allocator<double, complex_struct>>* v = new vector<double, object_allocator<double, complex_struct>>();
    v->push_back(1.0);
    v->push_back(2.0);

    vector<double, object_allocator<double, complex_struct>>* v4 = new vector<double, object_allocator<double, complex_struct>>((std::move(*v)));
    EXPECT_EQ(v4->size(), 2);

    delete v;
    delete v4;

    EXPECT_EQ(complex_struct::allocs, 0);

    vector<double, object_allocator<double, string>> string_vector;
    string_vector.push_back(1.0);
    string_vector.push_back(2.0);
    string_vector.push_back(3.0);
    string_vector.push_back(4.0);

    string* my_string = string_vector.get_object();
    *my_string = "Fernando";

    EXPECT_EQ(string_vector.size(), 4);
    vector<double, object_allocator<double, string>> string_vector2(string_vector);

    EXPECT_EQ(string_vector2.size(), 4);
    EXPECT_STREQ(string_vector2.get_object()->c_str(), "Fernando");

    // test with string longer than 64 bytes
    vector<double, object_allocator<double, string>> string_vector_long;
    string_vector_long.push_back(1.0);
    string_vector_long.push_back(2.0);
    string_vector_long.push_back(3.0);
    string_vector_long.push_back(4.0);

    string *my_string_long = string_vector_long.get_object();
    *my_string_long = "A very long string that is longer than 64 bytes, to test if moving or copying the vector will break this";

    EXPECT_EQ(string_vector_long.size(), 4);
    vector<double, object_allocator<double, string>> string_vector_long2(string_vector_long);

    EXPECT_EQ(string_vector_long2.size(), 4);
    EXPECT_STREQ(string_vector_long2.get_object()->c_str(),
                 "A very long string that is longer than 64 bytes, to test if moving or copying the vector will break this");

    
    vector<double, object_allocator<double, string>> string_vector_long3(std::move(string_vector_long2));

    EXPECT_EQ(string_vector_long3.size(), 4);
    EXPECT_STREQ(string_vector_long3.get_object()->c_str(),
                 "A very long string that is longer than 64 bytes, to test if moving or copying the vector will break this");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}