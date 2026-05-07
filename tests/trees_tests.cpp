#include <algorithm>
#include <gtest/gtest.h>
#include <map>
#include <random>
#include "red_black_tree.hpp"

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