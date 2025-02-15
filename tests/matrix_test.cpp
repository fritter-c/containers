#include "matrix.h"
#include <gtest/gtest.h>

using namespace gtr::containers;

TEST(MatrixTest, DefaultConstructor) {
    matrix<double> matrix(1, 1);
    EXPECT_EQ(matrix.rows, 1);
    EXPECT_EQ(matrix.columns, 1);
}

TEST(MatrixTest, ValueAssignmet) {
    matrix<double> matrix(2000000, 5);
    for (int i = 0; i < 5; i++) {
        auto buffer = matrix[i];
        for (int j = 0; j < 2000000; j++) {
            buffer[j] = i;
        }
    }
    for (int i = 0; i < 5; i++) {
        auto buffer = matrix[i];
        for (int j = 0; j < 2000000; j++) {
            EXPECT_EQ(buffer[j], i);
        }
    }
}

TEST(MatrixTest, StdMatrix) {
    std::vector<std::vector<double>> data(5, std::vector<double>(2000000, 0));
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 2000000; j++) {
            data[i][j] = i;
        }
    }
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 2000000; j++) {
            EXPECT_EQ(data[i][j], i);
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
