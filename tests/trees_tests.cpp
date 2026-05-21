#include <algorithm>
#include <gtest/gtest.h>
#include <map>
#include <random>
#include "red_black_tree.hpp"
#include "avl_tree.hpp"

/*
** RB-TREE TESTS
*/

TEST(RBTreeTest, EmptyTree) {
    RBTree<int, int> tree;
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
    EXPECT_EQ(tree.find(10), nullptr);
}

TEST(RBTreeTest, InsertOne) {
    RBTree<int, int> tree;
    EXPECT_TRUE(tree.insert(5, 100));
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 1);
    auto* val = tree.find(5);
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 100);
}

TEST(RBTreeTest, DuplicateInsert) {
    RBTree<int, int> tree;

    EXPECT_TRUE(tree.insert(1, 10));
    EXPECT_FALSE(tree.insert(1, 20));

    EXPECT_EQ(tree.size(), 1);

    auto* val = tree.find(1);
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 10);
}

TEST(RBTreeTest, OrderedTraversal) {
    RBTree<int, int> tree;

    tree.insert(5, 0);
    tree.insert(1, 0);
    tree.insert(10, 0);
    tree.insert(7, 0);

    auto vec = tree.toVector();

    ASSERT_EQ(vec.size(), 4);

    EXPECT_EQ(vec[0].first, 1);
    EXPECT_EQ(vec[1].first, 5);
    EXPECT_EQ(vec[2].first, 7);
    EXPECT_EQ(vec[3].first, 10);
}

TEST(RBTreeTest, EraseLeaf) {
    RBTree<int, int> tree;

    tree.insert(1, 10);
    tree.insert(2, 20);
    tree.insert(3, 30);

    EXPECT_TRUE(tree.erase(3));
    EXPECT_FALSE(tree.contains(3));
    EXPECT_EQ(tree.size(), 2);
}

TEST(RBTreeTest, EraseRoot) {
    RBTree<int, int> tree;

    tree.insert(10, 1);

    EXPECT_TRUE(tree.erase(10));
    EXPECT_TRUE(tree.empty());
}

TEST(RBTreeTest, OperatorAccess) {
    RBTree<int, std::string> tree;

    tree[5] = "hello";

    EXPECT_TRUE(tree.contains(5));
    EXPECT_EQ(tree[5], "hello");
}

TEST(RBTreeTest, Clear) {
    RBTree<int, int> tree;

    for (int i = 0; i < 100; ++i)
        tree.insert(i, i);

    tree.clear();

    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
}

TEST(RBTreeTest, EraseNonExisting) {
    RBTree<int, int> tree;

    tree.insert(1, 10);

    EXPECT_FALSE(tree.erase(999));
    EXPECT_EQ(tree.size(), 1);
    EXPECT_TRUE(tree.contains(1));
}

TEST(RBTreeTest, SequentialInsert) {
    RBTree<int, int> tree;

    for (int i = 0; i < 1000; ++i)
        EXPECT_TRUE(tree.insert(i, i * 10));

    EXPECT_EQ(tree.size(), 1000);

    for (int i = 0; i < 1000; ++i) {
        auto* val = tree.find(i);
        ASSERT_NE(val, nullptr);
        EXPECT_EQ(*val, i * 10);
    }
}

TEST(RBTreeTest, SequentialErase) {
    RBTree<int, int> tree;

    for (int i = 0; i < 1000; ++i)
        tree.insert(i, i);

    for (int i = 0; i < 1000; ++i) {
        EXPECT_TRUE(tree.erase(i));
        EXPECT_FALSE(tree.contains(i));
    }

    EXPECT_TRUE(tree.empty());
}

TEST(RBTreeTest, ReverseInsert) {
    RBTree<int, int> tree;

    for (int i = 1000; i >= 0; --i)
        tree.insert(i, i);

    auto vec = tree.toVector();

    ASSERT_EQ(vec.size(), 1001);

    for (int i = 0; i <= 1000; ++i)
        EXPECT_EQ(vec[i].first, i);
}

TEST(RBTreeTest, RandomInsertErase) {
    RBTree<int, int> tree;

    std::vector<int> vals = {
        50, 20, 70, 10, 30, 60, 80, 25, 35, 65, 75, 5, 15
    };

    for (int x : vals)
        tree.insert(x, x);

    for (int x : vals) {
        EXPECT_TRUE(tree.contains(x));
    }

    std::shuffle(vals.begin(), vals.end(), std::mt19937(123));

    for (int x : vals) {
        EXPECT_TRUE(tree.erase(x));
        EXPECT_FALSE(tree.contains(x));
    }

    EXPECT_TRUE(tree.empty());
}

TEST(RBTreeTest, ReinsertAfterErase) {
    RBTree<int, int> tree;

    tree.insert(42, 1);

    EXPECT_TRUE(tree.erase(42));

    EXPECT_FALSE(tree.contains(42));

    EXPECT_TRUE(tree.insert(42, 999));

    auto* val = tree.find(42);

    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 999);
}



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

    auto* val = tree.find(42);

    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 999);
}

TEST(RBTreeTest, OperatorInsertDefaultValue) {
    RBTree<int, std::string> tree;

    EXPECT_FALSE(tree.contains(100));

    std::string& ref = tree[100];

    EXPECT_TRUE(tree.contains(100));
    EXPECT_TRUE(ref.empty());

    ref = "abc";

    EXPECT_EQ(tree[100], "abc");
}

TEST(RBTreeTest, MultipleClearCalls) {
    RBTree<int, int> tree;

    for (int i = 0; i < 50; ++i)
        tree.insert(i, i);

    tree.clear();

    EXPECT_TRUE(tree.empty());

    tree.clear();

    EXPECT_TRUE(tree.empty());

    EXPECT_EQ(tree.size(), 0);
}

TEST(RBTreeTest, ClearThenReuse) {
    RBTree<int, int> tree;

    for (int i = 0; i < 100; ++i)
        tree.insert(i, i);

    tree.clear();

    for (int i = 100; i < 200; ++i)
        tree.insert(i, i);

    EXPECT_EQ(tree.size(), 100);

    for (int i = 100; i < 200; ++i)
        EXPECT_TRUE(tree.contains(i));
}

TEST(RBTreeTest, MoveConstructor) {
    RBTree<int, int> tree;

    for (int i = 0; i < 100; ++i)
        tree.insert(i, i * 2);

    RBTree<int, int> moved(std::move(tree));

    EXPECT_EQ(moved.size(), 100);

    for (int i = 0; i < 100; ++i) {
        auto* val = moved.find(i);
        ASSERT_NE(val, nullptr);
        EXPECT_EQ(*val, i * 2);
    }
}

TEST(RBTreeTest, MoveAssignment) {
    RBTree<int, int> a;
    RBTree<int, int> b;

    for (int i = 0; i < 100; ++i)
        a.insert(i, i);

    b = std::move(a);

    EXPECT_EQ(b.size(), 100);

    for (int i = 0; i < 100; ++i)
        EXPECT_TRUE(b.contains(i));
}

TEST(RBTreeTest, StressInsertEraseSameKey) {
    RBTree<int, int> tree;

    for (int i = 0; i < 10000; ++i) {
        EXPECT_TRUE(tree.insert(1, i));
        EXPECT_FALSE(tree.insert(1, i));

        auto* val = tree.find(1);

        ASSERT_NE(val, nullptr);

        EXPECT_TRUE(tree.erase(1));
        EXPECT_FALSE(tree.contains(1));
    }

    EXPECT_TRUE(tree.empty());
}

TEST(RBTreeStress, CompareWithStdMap) {
    RBTree<int, int> tree;
    std::map<int, int> ref;

    std::mt19937 rng(123456);

    for (int iter = 0; iter < 100000; ++iter) {
        int op = rng() % 3;
        int key = rng() % 5000;
        int value = rng();

        if (op == 0) {
            bool a = tree.insert(key, value);
            bool b = ref.insert({key, value}).second;

            EXPECT_EQ(a, b);
        }
        else if (op == 1) {
            bool a = tree.erase(key);
            bool b = ref.erase(key) > 0;

            EXPECT_EQ(a, b);
        }
        else {
            auto* val = tree.find(key);
            auto it = ref.find(key);

            if (it == ref.end()) {
                EXPECT_EQ(val, nullptr);
            } else {
                ASSERT_NE(val, nullptr);
                EXPECT_EQ(*val, it->second);
            }
        }

        auto vec = tree.toVector();

        ASSERT_EQ(vec.size(), ref.size());

        size_t idx = 0;
        for (auto& [k, v] : ref) {
            EXPECT_EQ(vec[idx].first, k);
            EXPECT_EQ(vec[idx].second, v);
            ++idx;
        }
    }
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