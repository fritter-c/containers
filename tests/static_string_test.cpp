#include <gtest/gtest.h>
#include "static_string.h"
using namespace gtr::containers;
TEST(StackStringTest, DefaultConstructor) {
    static_string<32> s;
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s, "");
}

TEST(StackStringTest, ConstructorFromString) {
    static_string<32> s("Hello");
    EXPECT_EQ(s.size(), 5);
    EXPECT_STREQ(s, "Hello");
}

TEST(StackStringTest, CopyConstructor) {
    static_string<32> s1("Hello");
    static_string<32> s2(s1);
    EXPECT_STREQ(s2, "Hello");
}

TEST(StackStringTest, MoveConstructor) {
    static_string<32> s1("Hello");
    static_string<32> s2(std::move(s1));
    EXPECT_STREQ(s2, "Hello");
}

TEST(StackStringTest, CopyAssignmentOperator) {
    static_string<32> s1("Hello");
    static_string<32> s2;
    s2 = s1;
    EXPECT_STREQ(s2, "Hello");
}

TEST(StackStringTest, MoveAssignmentOperator) {
    static_string<32> s1("Hello");
    static_string<32> s2;
    s2 = std::move(s1);
    EXPECT_STREQ(s2, "Hello");
}

TEST(StackStringTest, OperatorPlusEqual) {
    static_string<32> s1("Hello");
    static_string<32> s2("World");
    s1 += " ";
    s1 += s2;
    EXPECT_STREQ(s1, "Hello World");
}

TEST(StackStringTest, Find) {
    static_string<32> s("Hello World");
    EXPECT_EQ(s.find("World"), 6);
    EXPECT_EQ(s.find("world"), static_string<32>::npos);
}

TEST(StackStringTest, IndexOperator) {
    static_string<32> s("Hello");
    EXPECT_EQ(s[0], 'H');
    EXPECT_EQ(s[1], 'e');
    EXPECT_EQ(s[4], 'o');
}

TEST(StackStringTest, CrossTypeCopyConstructor) {
    static_string<64> s1("Hello");
    static_string<32> s2(s1);
    EXPECT_STREQ(s2, "Hello");

    static_string<16> s3(s1);
    EXPECT_STREQ(s3, "Hello");

    static_string<8> s4(s1);
    EXPECT_STREQ(s4, "Hello");
}

TEST(StackStringTest, CrossTypeMoveConstructor) {
    static_string<64> s1("Hello");
    static_string<32> s2(std::move(s1));
    EXPECT_STREQ(s2, "Hello");
}

TEST(StackStringTest, CrossTypeCopyAssignment) {
    static_string<64> s1("Hello");
    static_string<32> s2;
    s2 = s1;
    EXPECT_STREQ(s2, "Hello");

    static_string<16> s3;
    s3 = s1;
    EXPECT_STREQ(s3, "Hello");
}

TEST(StackStringTest, CrossTypeMoveAssignment) {
    static_string<64> s1("Hello");
    static_string<32> s2;
    s2 = std::move(s1);
    EXPECT_STREQ(s2, "Hello");

    static_string<64> s3("Worldddddddddddg");
    static_string<16> s4;
    s4 = std::move(s3);
    EXPECT_STREQ(s4, "Worlddddddddddd");
}

TEST(StackStringTest, CrossTypeOperatorPlusEqual) {
    static_string<64> s1("Hello");
    static_string<32> s2("World");
    static_string<16> s3("Hello");
    s1 += " ";
    s1 += s2;
    s2 += " ";
    s2 += s3;
    s3 += " ";
    s3 += s2;

    EXPECT_STREQ(s1, "Hello World");
    EXPECT_STREQ(s2, "World Hello");
    EXPECT_STREQ(s3, "Hello World Hel");
}


TEST(StackStringTest, OstreamOperator) {
    static_string<32> s("Hello, world!");
    std::ostringstream os;
    os << s;
    EXPECT_EQ(os.str(), "Hello, world!");
}

TEST(StackStringTest, IstreamOperator) {
    static_string<32> s;
    std::istringstream is("Hello, world!");
    is >> s;
    EXPECT_STREQ(s, "Hello, world!");
}

TEST(StackStringTest, AppendCString) {
    static_string<32> s("Hello");
    s.append(", world!");
    EXPECT_STREQ(s, "Hello, world!");
}

TEST(StackStringTest, AppendStackString) {
    static_string<32> s1("Hello");
    static_string<16> s2(", world!");
    s1.append(s2);
    EXPECT_STREQ(s1, "Hello, world!");
}

TEST(StackStringTest, AppendChar) {
    static_string<32> s("Hello");
    s.append('!');
    EXPECT_STREQ(s, "Hello!");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
