#include <gtest/gtest.h>
#include "text.h"
using namespace gtr::containers;

TEST(TextTest, DefaultConstructor) {
    text<32> s;
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s, "");
}

TEST(TextTest, ConstructorFromCString) {
    text<32> s("Hello, World!");
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, CopyConstructor) {
    text<32> s("Hello, World!");
    text<32> s2(s);
    EXPECT_EQ(s2.size(), 13);
    EXPECT_STREQ(s2, "Hello, World!");
}

TEST(TextTest, MoveConstructor) {
    text<32> s("Hello, World!");
    text<32> s2(std::move(s));
    EXPECT_EQ(s2.size(), 13);
    EXPECT_STREQ(s2, "Hello, World!");
}

TEST(TextTest, CopyAssignment) {
    text<32> s("Hello, World!");
    text<32> s2;
    s2 = s;
    EXPECT_EQ(s2.size(), 13);
    EXPECT_STREQ(s2, "Hello, World!");
}

TEST(TextTest, MoveAssignment) {
    text<32> s("Hello, World!");
    text<32> s2;
    s2 = std::move(s);
    EXPECT_EQ(s2.size(), 13);
    EXPECT_STREQ(s2, "Hello, World!");
}

TEST(TextTest, AppendChar) {
    text<32> s("Hello, World");
    EXPECT_STREQ(s, "Hello, World");
    s.append('!');
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, AppendCString) {
    text<32> s("Hello, World");
    s.append("!");
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, AppendText) {
    text<32> s("Hello, World");
    text<32> s2("!");
    s.append(s2);
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, OperatorPlusChar) {
    text<32> s("Hello, World");
    text<32> s2 = s + '!';
    EXPECT_EQ(s2.size(), 13);
    EXPECT_STREQ(s2, "Hello, World!");
}

TEST(TextTest, OperatorPlusCString) {
    text<32> s("Hello, World");
    text<32> s2 = s + "!";
    EXPECT_EQ(s2.size(), 13);
    EXPECT_STREQ(s2, "Hello, World!");
}

TEST(TextTest, OperatorPlusText) {
    text<32> s("Hello, World");
    text<32> s2("!");
    text<32> s3 = s + s2;
    EXPECT_EQ(s3.size(), 13);
    EXPECT_STREQ(s3, "Hello, World!");
}

TEST(TextTest, OperatorPlusEqualChar) {
    text<32> s("Hello, World");
    s += '!';
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, OperatorPlusEqualCString) {
    text<32> s("Hello, World");
    s += "!";
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, OperatorPlusEqualText) {
    text<32> s("Hello, World");
    text<32> s2("!");
    s += s2;
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, OperatorIndex) {
    text<32> s("Hello, World");
    EXPECT_EQ(s[0], 'H');
    EXPECT_EQ(s[1], 'e');
    EXPECT_EQ(s[2], 'l');
    EXPECT_EQ(s[3], 'l');
    EXPECT_EQ(s[4], 'o');
    EXPECT_EQ(s[5], ',');
    EXPECT_EQ(s[6], ' ');
    EXPECT_EQ(s[7], 'W');
    EXPECT_EQ(s[8], 'o');
    EXPECT_EQ(s[9], 'r');
    EXPECT_EQ(s[10], 'l');
    EXPECT_EQ(s[11], 'd');
    EXPECT_EQ(s[12], '\0');
}

TEST(TextTest, CString) {
    text<32> s("Hello, World");
    EXPECT_STREQ(s.c_str(), "Hello, World");
}

TEST(TextTest, LongString) {
    text<32> s("Fernando Lahude Ritter Fernando");
    EXPECT_EQ(s.size(), 31);
    EXPECT_STREQ(s, "Fernando Lahude Ritter Fernando");
    s.append('!');
    EXPECT_EQ(s.size(), 32);
    EXPECT_STREQ(s, "Fernando Lahude Ritter Fernando!");
}

TEST(TextTest, Reserve) {
    text<32> s("Hello, World");
    s.reserve(64);
    EXPECT_EQ(s.capacity(), 64);
}

TEST(TextTest, Clear) {
    text<32> s("Hello, World");
    s.clear();
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s, "");
}

TEST(TextTest, LocalToHeap) {
    text<32> s("Hello, WorldDDDD");
    EXPECT_EQ(s.size(), 16);
    EXPECT_STREQ(s, "Hello, WorldDDDD");
    s.append('!');
    EXPECT_EQ(s.size(), 17);
    EXPECT_STREQ(s, "Hello, WorldDDDD!");
}

TEST(TextTest, Format) {
    text<32> s;
    s.format("Hello, %s!", "World");
    EXPECT_EQ(s.size(), 13);
    EXPECT_STREQ(s, "Hello, World!");
}

TEST(TextTest, Reversed) {
    text<32> s("Hello, World");
    text<32> s2 = s.reversed();
    EXPECT_EQ(s2.size(), 12);
    EXPECT_STREQ(s2, "dlroW ,olleH");
}

TEST(TextTest, Substring) {
    text<32> s("Hello, World");
    text<32> s2 = s.substr(7, 12);
    EXPECT_EQ(s2.size(), 5);
    EXPECT_STREQ(s2, "World");
}

TEST(TextTest, Find) {
    text<32> s("Hello, World");
    EXPECT_EQ(s.find("World"), 7);
    EXPECT_EQ(s.find("Hello"), 0);
    EXPECT_EQ(s.find("ello"), 1);
    EXPECT_EQ(s.find("World!"), -1);
}

TEST(TextTest, FindFirstOf) {
    text<32> s("Hello, World");
    EXPECT_EQ(s.find_first_of('W'), 7);
    EXPECT_EQ(s.find_first_of('H'), 0);
    EXPECT_EQ(s.find_first_of('o'), 4);
    EXPECT_EQ(s.find_first_of('!'), -1);
}

TEST(TextTest, FindLastOf) {
    text<32> s("Hello, World");
    EXPECT_EQ(s.find_last_of('W'), 7);
    EXPECT_EQ(s.find_last_of('H'), 0);
    EXPECT_EQ(s.find_last_of('o'), 8);
    EXPECT_EQ(s.find_last_of('!'), s.npos);
}

TEST(TextTest, Erase) {
    text<32> s("Hello, World");
    s.erase(7, 6);
    EXPECT_EQ(s.size(), 7);
    EXPECT_STREQ(s, "Hello, ");
}

TEST(TextTest, BeginEnd) {
    text<32> s("Hello, World");
    EXPECT_EQ(*s.begin(), 'H');
    EXPECT_EQ(*(s.end() - 1), 'd');
}

// Operators
TEST(TextTest, OperatorEqual) {
    text<32> s("Hello, World");
    text<32> s2("Hello, World");
    EXPECT_TRUE(s == s2);
}

TEST(TextTest, OperatorNotEqual) {
    text<32> s("Hello, World");
    text<32> s2("Hello, World!");
    EXPECT_TRUE(s != s2);
}

TEST(TextTest, OperatorLessThan) {
    text<32> s("Hello, World");
    text<32> s2("Hello, World!");
    EXPECT_TRUE(s < s2);
}

TEST(TextTest, OperatorGreaterThan) {
    text<32> s("Hello, World!");
    text<32> s2("Hello, World");
    EXPECT_TRUE(s > s2);
}

TEST(TextTest, OperatorLessThanEqual) {
    text<32> s("Hello, World");
    text<32> s2("Hello, World");
    text<32> s3("Hello, World!");
    EXPECT_TRUE(s <= s2);
    EXPECT_TRUE(s <= s3);
}

TEST(TextTest, OperatorGreaterThanEqual) {
    text<32> s("Hello, World");
    text<32> s2("Hello, World");
    text<32> s3("Hello, World!");
    EXPECT_TRUE(s >= s2);
    EXPECT_FALSE(s >= s3);
}

TEST(TextTest, OperatorStream) {
    text<32> s("Hello, World");
    std::stringstream ss;
    ss << s;
    EXPECT_STREQ(ss.str().c_str(), "Hello, World");
}

TEST(TextTest, Split) {
    text<32> s("Hello, World");
    auto parts = s.split(' ');
    EXPECT_STREQ(parts[0], "Hello,");
    EXPECT_STREQ(parts[1], "World");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}