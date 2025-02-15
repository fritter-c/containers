#include <gtest/gtest.h>
#include "deque.h"

TEST(DequeTest, PushBack) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d[i], i);
    }
}

TEST(DequeTest, PushFront) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_front(i);
    }
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d[i], 999 - i);
    }
}

TEST(DequeTest, PopBack) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    for (int i = 0; i < 1000; ++i) {
        d.pop_back();
        ASSERT_EQ(d.size(), 1000 - i - 1);
    }
}

TEST(DequeTest, PopFront) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    for (int i = 0; i < 1000; ++i) {
        d.pop_front();
        ASSERT_EQ(d.size(), 1000 - i - 1);
    }
}

TEST(DequeTest, Clear) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    d.clear();
    ASSERT_EQ(d.size(), 0);
}

TEST(DequeTest, CopyConstructor) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    gtr::containers::deque<int> d2(d);
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d[i], d2[i]);
    }
}

TEST(DequeTest, MoveConstructor) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    gtr::containers::deque<int> d2(std::move(d));
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d2[i], i);
    }
}

TEST(DequeTest, CopyAssignment) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    gtr::containers::deque<int> d2;
    d2 = d;
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d[i], d2[i]);
    }
}

TEST(DequeTest, MoveAssignment) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    gtr::containers::deque<int> d2;
    d2 = std::move(d);
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d2[i], i);
    }
}

TEST(DequeTest, Iterator) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    int i = 0;
    for (int value : d) {
        ASSERT_EQ(value, i++);
    }
}

TEST(DequeTest, ConstIterator) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    int i = 0;
    for (const int &value : d) {
        ASSERT_EQ(value, i++);
    }
}

TEST(DequeTest, At) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d.at(i), i);
    }
    ASSERT_THROW(d.at(1000), std::out_of_range);
}

TEST(DequeTest, BracketOperator) {
    gtr::containers::deque<int> d;
    for (int i = 0; i < 1000; ++i) {
        d.push_back(i);
    }
    for (int i = 0; i < 1000; ++i) {
        ASSERT_EQ(d[i], i);
    }
}

TEST(DequeTest, FrontBack) {
    gtr::containers::deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);
    ASSERT_EQ(d.front(), 1);
    ASSERT_EQ(d.back(), 3);
    d.pop_front();
    ASSERT_EQ(d.front(), 2);
    ASSERT_EQ(d.back(), 3);
    d.pop_back();
    ASSERT_EQ(d.front(), 2);
    ASSERT_EQ(d.back(), 2);
}

TEST(DequeTest, HeavyUsage){
      gtr::containers::deque<int> d;
        for (int i = 0; i < 1000; ++i) {
            d.push_back(i);
        }
        for (int i = 0; i < 1000; ++i) {
            ASSERT_EQ(d.front(), i);
            d.pop_front();
        }

        for (int i = 0; i < 1000; ++i) {
            d.push_front(i);
        }

        for (int i = 0; i < 1000; ++i) {
            ASSERT_EQ(d.back(), i);
            d.pop_back();
        }

        // Mixed usage
        for (int i = 0; i < 10000; ++i) {
            d.push_back(i);
        }
        for (int i = 0; i < 5000; ++i) {
            d.pop_front();
        }
        for (int i = 0; i < 5000; ++i) {
            d.push_front(i);
        }
        for (int i = 0; i < 5000; ++i) {
            d.pop_back();
        }
        for (int i = 0; i < 5000; ++i) {
            d.push_back(i);
        }
        for (int i = 0; i < 5000; ++i) {
            d.pop_front();
        }
        for (int i = 0; i < 5000; ++i) {
            d.push_front(i);
        }
        for (int i = 0; i < 5000; ++i) {
            d.pop_back();
        }

        for (int i = 0; i < 10000; ++i) {
            d.push_back(i);
        }        

        ASSERT_EQ(d.size(), 15000);

        for (int i = 5000 - 1; i >= 0; --i) {
            ASSERT_EQ(d.front(), i);
            d.pop_front();
        }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
