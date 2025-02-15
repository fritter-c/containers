#include <gtest/gtest.h>
#include <vector>
#include "redblack_tree.h"
#include <algorithm>
#include <memory>
using namespace gtr::containers;

// Test basic insertion and find functionality.
TEST(RedBlackTreeTest, InsertAndFind) {
    redblack_tree<int> tree;
    
    auto node1 = new redblack_tree<int>::node(10);
    auto node2 = new redblack_tree<int>::node(5);
    auto node3 = new redblack_tree<int>::node(15);
    
    tree.insert(node1);
    tree.insert(node2);
    tree.insert(node3);

    // Ensure the nodes can be found.
    EXPECT_NE(tree.find(10), nullptr);
    EXPECT_NE(tree.find(5), nullptr);
    EXPECT_NE(tree.find(15), nullptr);
    EXPECT_EQ(tree.find(20), nullptr);

    // Verify that the root is black after fixup.
    ASSERT_NE(tree.root, nullptr);
    EXPECT_FALSE(tree.root->is_red);

    //cleanup
    delete node1;
    delete node2;
    delete node3;
}

// Test in-order traversal using the iterator.
TEST(RedBlackTreeTest, InOrderIterator) {
    redblack_tree<int> tree;
    
    tree.insert(new redblack_tree<int>::node(20));
    tree.insert(new redblack_tree<int>::node(10));
    tree.insert(new redblack_tree<int>::node(30));
    tree.insert(new redblack_tree<int>::node(25));
    tree.insert(new redblack_tree<int>::node(35));
    tree.insert(new redblack_tree<int>::node(5));

    // Expected in-order sequence: 5, 10, 20, 25, 30, 35.
    std::vector<int> expected = {5, 10, 20, 25, 30, 35};
    std::vector<int> result;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        result.push_back(*it);
    }
    EXPECT_EQ(result, expected);
}

// Test finding the minimum and maximum elements in the tree.
TEST(RedBlackTreeTest, FindMinAndMax) {
    redblack_tree<int> tree;
    
    tree.insert(new redblack_tree<int>::node(20));
    tree.insert(new redblack_tree<int>::node(10));
    tree.insert(new redblack_tree<int>::node(30));
    tree.insert(new redblack_tree<int>::node(5));
    tree.insert(new redblack_tree<int>::node(40));

    auto minNode = tree.find_min();
    auto maxNode = tree.find_max();

    ASSERT_NE(minNode, nullptr);
    ASSERT_NE(maxNode, nullptr);

    EXPECT_EQ(minNode->value, 5);
    EXPECT_EQ(maxNode->value, 40);
}

// Test the successor and predecessor helper functions.
TEST(RedBlackTreeTest, SuccessorAndPredecessor) {
    redblack_tree<int> tree;
    
    auto node20 = new redblack_tree<int>::node(20);
    auto node10 = new redblack_tree<int>::node(10);
    auto node30 = new redblack_tree<int>::node(30);
    auto node5  = new redblack_tree<int>::node(5);
    auto node15 = new redblack_tree<int>::node(15);
    auto node25 = new redblack_tree<int>::node(25);
    auto node35 = new redblack_tree<int>::node(35);

    tree.insert(node20);
    tree.insert(node10);
    tree.insert(node30);
    tree.insert(node5);
    tree.insert(node15);
    tree.insert(node25);
    tree.insert(node35);

    // Check successor chain.
    EXPECT_EQ(tree.successor(node5)->value, 10);
    EXPECT_EQ(tree.successor(node10)->value, 15);
    EXPECT_EQ(tree.successor(node15)->value, 20);
    EXPECT_EQ(tree.successor(node20)->value, 25);
    EXPECT_EQ(tree.successor(node25)->value, 30);
    EXPECT_EQ(tree.successor(node30)->value, 35);
    EXPECT_EQ(tree.successor(node35), nullptr);

    // Check predecessor chain.
    EXPECT_EQ(tree.predecessor(node35)->value, 30);
    EXPECT_EQ(tree.predecessor(node30)->value, 25);
    EXPECT_EQ(tree.predecessor(node25)->value, 20);
    EXPECT_EQ(tree.predecessor(node20)->value, 15);
    EXPECT_EQ(tree.predecessor(node15)->value, 10);
    EXPECT_EQ(tree.predecessor(node10)->value, 5);
    EXPECT_EQ(tree.predecessor(node5), nullptr);
}

// Test iterator decrement functionality.
TEST(RedBlackTreeTest, IteratorDecrement) {
    redblack_tree<int> tree;
    
    tree.insert(new redblack_tree<int>::node(1));
    tree.insert(new redblack_tree<int>::node(2));
    tree.insert(new redblack_tree<int>::node(3));
    tree.insert(new redblack_tree<int>::node(4));
    tree.insert(new redblack_tree<int>::node(5));

    // Get an iterator to the end and decrement once.
    auto it = tree.end();
    --it;  // now should point to the maximum element, which is 5.
    EXPECT_EQ(*it, 5);

    // Collect all elements in reverse order.
    std::vector<int> reversed;
    for (auto rit = tree.end(); rit != tree.begin(); ) {
        --rit;
        reversed.push_back(*rit);
    }
    std::vector<int> expected = {5, 4, 3, 2, 1};
    EXPECT_EQ(reversed, expected);
}

TEST(RedBlackTreeTest, Bounds) {
    redblack_tree<int> tree;

    tree.insert(new redblack_tree<int>::node(1));
    tree.insert(new redblack_tree<int>::node(2));
    tree.insert(new redblack_tree<int>::node(3));
    tree.insert(new redblack_tree<int>::node(3));
    tree.insert(new redblack_tree<int>::node(3));
    tree.insert(new redblack_tree<int>::node(3));
    tree.insert(new redblack_tree<int>::node(3));
    tree.insert(new redblack_tree<int>::node(4));
    tree.insert(new redblack_tree<int>::node(5));
    // Check lower bound.
    auto it = tree.lower_bound(3);
    EXPECT_EQ(*it, 3);
    // Check upper bound.
    it = tree.upper_bound(3);
    EXPECT_EQ(*it, 4);

    // Check range
    EXPECT_EQ(std::distance(tree.lower_bound(3), tree.upper_bound(3)), 5);

}
