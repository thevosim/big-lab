#include <string>
#include <gtest/gtest.h>
#include <cmath>
#include <algorithm>
#include <random>
#include "avl_tree.hpp"

/*
** AVL-TREE TESTS FIXTURE
*/
class AVLTreeTest : public ::testing::Test {
protected:
    // Вспомогательный метод для проверки базовой структуры после ротаций
    template <typename T>
    void assertStructure(const AVLTree<T>& tree, const T& expectedRoot, const T& expectedLeft, const T& expectedRight, int expectedHeight) {
        auto* r = tree.root;
        ASSERT_NE(r, nullptr);
        EXPECT_EQ(r->key, expectedRoot);
        ASSERT_NE(r->left, nullptr);
        ASSERT_NE(r->right, nullptr);
        EXPECT_EQ(r->left->key, expectedLeft);
        EXPECT_EQ(r->right->key, expectedRight);
        EXPECT_EQ(r->height, expectedHeight);
    }

    // Рекурсивная проверка инвариантов AVL-дерева
    template <typename T>
    int checkAVLPropertyInternal(typename AVLTree<T>::Node* node) {
        if (!node) return 0;

        int leftHeight = checkAVLPropertyInternal<T>(node->left);
        int rightHeight = checkAVLPropertyInternal<T>(node->right);

        EXPECT_TRUE(node->left == nullptr || node->left->key < node->key);
        EXPECT_TRUE(node->right == nullptr || node->right->key > node->key);

        int diff = std::abs(leftHeight - rightHeight);
        EXPECT_LE(diff, 1);

        EXPECT_EQ(node->height, std::max(leftHeight, rightHeight) + 1);

        return node->height;
    }

    template <typename T>
    void verifyTree(const AVLTree<T>& tree) {
        checkAVLPropertyInternal<T>(tree.root);
    }

    template <typename T>
    void verifyTreeAndHeight(const AVLTree<T>& tree, int numElements) {
        int h = checkAVLPropertyInternal<T>(tree.root);
        int maxHeight = static_cast<int>(1.44 * std::log2(numElements + 2));
        EXPECT_LE(h, maxHeight);
    }
};

/*
** AVL-TREE TEST CASES
*/

TEST_F(AVLTreeTest, HandlesLeftLeftCase) {
    AVLTree<int> tree;

    tree.insert(30);
    tree.insert(20);
    tree.insert(10);

    assertStructure(tree, 20, 10, 30, 2);
}

TEST_F(AVLTreeTest, HandlesRightRightCase) {
    AVLTree<int> tree;

    tree.insert(10);
    tree.insert(20);
    tree.insert(30);

    assertStructure(tree, 20, 10, 30, 2);
}

TEST_F(AVLTreeTest, HandlesLeftRightCase) {
    AVLTree<int> tree;

    tree.insert(30);
    tree.insert(10);
    tree.insert(20);

    assertStructure(tree, 20, 10, 30, 2);
}

TEST_F(AVLTreeTest, HandlesRightLeftCase) {
    AVLTree<int> tree;

    tree.insert(10);
    tree.insert(30);
    tree.insert(20);

    assertStructure(tree, 20, 10, 30, 2);
}

TEST_F(AVLTreeTest, UpdatesHeightCorrectlyOnComplexInsertions) {
    AVLTree<int> tree;

    tree.insert(50);
    tree.insert(25);
    tree.insert(75);
    tree.insert(15);
    tree.insert(40);
    tree.insert(60);
    tree.insert(90);
    tree.insert(35);

    verifyTree(tree);
}

TEST_F(AVLTreeTest, SequentialInsertionMaintainsLogNHeight) {
    AVLTree<int> tree;
    const int numElements = 10000;

    for (int i = 1; i <= numElements; ++i) {
        tree.insert(i);
    }

    verifyTreeAndHeight(tree, numElements);
}

TEST_F(AVLTreeTest, RandomInsertionMaintainsBalance) {
    AVLTree<int> tree;
    const int numElements = 50000;
    std::mt19937 gen(42); 
    std::uniform_int_distribution<int> dist(-100000, 100000);

    for (int i = 0; i < numElements; ++i) {
        tree.insert(dist(gen));
    }

    verifyTree(tree);
}

/*
** RB-TREE TESTS (заглушка)
*/