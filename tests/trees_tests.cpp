#include <string>
#include <gtest/gtest.h>
#include <cmath>
#include <algorithm>
#include <random>
#include "avl_tree.hpp"
/*
** AVL-TREE TESTS
*/
int checkAVLProperty(Node* node) {
    if (!node) return 0;

    int leftHeight = checkAVLProperty(node->left);
    int rightHeight = checkAVLProperty(node->right);

    EXPECT_TRUE(node->left == nullptr || node->left->key < node->key);
    EXPECT_TRUE(node->right == nullptr || node->right->key > node->key);

    int diff = std::abs(leftHeight - rightHeight);
    EXPECT_LE(diff, 1);

    EXPECT_EQ(node->height, std::max(leftHeight, rightHeight) + 1);

    return node->height;
}


TEST(AVLTreeTest, HandlesLeftLeftCase) {
    Node* root = nullptr;

    root = insert(root, 30);
    root = insert(root, 20);
    root = insert(root, 10);

    EXPECT_EQ(root->key, 20);
    ASSERT_NE(root->left, nullptr);
    ASSERT_NE(root->right, nullptr);
    EXPECT_EQ(root->left->key, 10);
    EXPECT_EQ(root->right->key, 30);
    EXPECT_EQ(root->height, 2);
}

TEST(AVLTreeTest, HandlesRightRightCase) {
    Node* root = nullptr;

    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30);

    EXPECT_EQ(root->key, 20);
    ASSERT_NE(root->left, nullptr);
    ASSERT_NE(root->right, nullptr);
    EXPECT_EQ(root->left->key, 10);
    EXPECT_EQ(root->right->key, 30);
    EXPECT_EQ(root->height, 2);
}

TEST(AVLTreeTest, HandlesLeftRightCase) {
    Node* root = nullptr;

    root = insert(root, 30);
    root = insert(root, 10);
    root = insert(root, 20);

    EXPECT_EQ(root->key, 20);
    ASSERT_NE(root->left, nullptr);
    ASSERT_NE(root->right, nullptr);
    EXPECT_EQ(root->left->key, 10);
    EXPECT_EQ(root->right->key, 30);
    EXPECT_EQ(root->height, 2);
}

TEST(AVLTreeTest, HandlesRightLeftCase) {
    Node* root = nullptr;

    root = insert(root, 10);
    root = insert(root, 30);
    root = insert(root, 20);

    EXPECT_EQ(root->key, 20);
    ASSERT_NE(root->left, nullptr);
    ASSERT_NE(root->right, nullptr);
    EXPECT_EQ(root->left->key, 10);
    EXPECT_EQ(root->right->key, 30);
    EXPECT_EQ(root->height, 2);
}

TEST(AVLTreeTest, UpdatesHeightCorrectlyOnComplexInsertions) {
    Node* root = nullptr;

    root = insert(root, 50);
    root = insert(root, 25);
    root = insert(root, 75);
    root = insert(root, 15);
    root = insert(root, 40);
    root = insert(root, 60);
    root = insert(root, 90);
    root = insert(root, 35);

    EXPECT_EQ(root->key, 50);
    EXPECT_EQ(root->height, 4);
    EXPECT_EQ(root->left->height, 3);
    EXPECT_EQ(root->right->height, 2);
}


TEST(AVLTreeStressTest, SequentialInsertionMaintainsLogNHeight) {
    Node* root = nullptr;
    const int numElements = 10000;

    for (int i = 1; i <= numElements; ++i) {
        root = insert(root, i);
    }

    checkAVLProperty(root);
    int maxHeight = static_cast<int>(1.44 * std::log2(numElements + 2));
    EXPECT_LE(root->height, maxHeight);
}

TEST(AVLTreeStressTest, RandomInsertionMaintainsBalance) {
    Node* root = nullptr;
    const int numElements = 50000;
    std::mt19937 gen(42); 
    std::uniform_int_distribution<int> dist(-100000, 100000);

    for (int i = 0; i < numElements; ++i) {
        root = insert(root, dist(gen));
    }

    checkAVLProperty(root);
}

/*
** RB-TREE TESTS
*/