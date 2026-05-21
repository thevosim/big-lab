#include <gtest/gtest.h>
#include <string>
#include "chain_hash_table.hpp"

/*
 * CHAIN HASH TABLE TESTS
 */

TEST(HashTableBasic, InsertAndFind) {
    ChainHashTable<int, std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");
    ASSERT_NE(table.find(1), nullptr);
    EXPECT_EQ(*table.find(1), "one");
    ASSERT_NE(table.find(2), nullptr);
    EXPECT_EQ(*table.find(2), "two");
    EXPECT_EQ(table.find(3), nullptr);
}

TEST(HashTableBasic, OverwriteValue) {
    ChainHashTable<int, int> table;
    table.insert(1, 10);
    table.insert(1, 20);
    ASSERT_NE(table.find(1), nullptr);
    EXPECT_EQ(*table.find(1), 20);
    EXPECT_EQ(table.size(), 1);
}

TEST(HashTableBasic, OperatorAccess) {
    ChainHashTable<int, int> table;
    table[5] = 42;
    EXPECT_EQ(table[5], 42);
    table[5] = 100;
    EXPECT_EQ(table[5], 100);
    EXPECT_EQ(table.size(), 1);
}

TEST(HashTableBasic, Contains) {
    ChainHashTable<int, int> table;
    table.insert(1, 1);
    EXPECT_TRUE(table.contains(1));
    EXPECT_FALSE(table.contains(2));
}

TEST(HashTableBasic, Erase) {
    ChainHashTable<int, int> table;
    table.insert(1, 10);
    table.insert(2, 20);
    EXPECT_TRUE(table.erase(1));
    EXPECT_FALSE(table.contains(1));
    EXPECT_EQ(table.size(), 1);
    EXPECT_FALSE(table.erase(1));
}

TEST(HashTableEdge, EmptyTable) {
    ChainHashTable<int, int> table;
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    EXPECT_EQ(table.find(1), nullptr);
}

TEST(HashTableEdge, Clear) {
    ChainHashTable<int, int> table;
    for (int i = 0; i < 100; ++i)
        table.insert(i, i*i);
    table.clear();
    EXPECT_TRUE(table.empty());
    EXPECT_EQ(table.size(), 0);
    for (int i = 0; i < 100; ++i)
        EXPECT_EQ(table.find(i), nullptr);
}

TEST(HashTableEdge, Reserve) {
    ChainHashTable<int, int> table;
    table.reserve(1000);
    for (int i = 0; i < 1000; ++i)
        table.insert(i, i);
    EXPECT_EQ(table.size(), 1000);
    for (int i = 0; i < 1000; ++i)
        EXPECT_EQ(*table.find(i), i);
}

TEST(HashTableIteration, TESTForEach) {
    ChainHashTable<int, int> table;
    for (int i = 0; i < 100; ++i)
        table.insert(i, i);
    int sum = 0;
    auto func = [&](int k, int v) { sum += v; };
    table.for_each(func);
    EXPECT_EQ(sum, 4950);
}

TEST(HashTableStress, RandomOperations) {
    ChainHashTable<int, int> table;
    std::unordered_map<int, int> ref;

    std::mt19937 rng(123);
    std::uniform_int_distribution<int> key_dist(0, 1000);
    std::uniform_int_distribution<int> val_dist(0, 100000);
    std::uniform_int_distribution<int> op_dist(0, 2);

    for (int i = 0; i < 100000; ++i) {
        int op = op_dist(rng);
        int key = key_dist(rng);
        if (op == 0) {
            int val = val_dist(rng);
            table.insert(key, val);
            ref[key] = val;
        } 
        else if (op == 1) {
            bool r1 = table.erase(key);
            bool r2 = ref.erase(key) > 0;
            EXPECT_EQ(r1, r2);
        } 
        else {
            auto p1 = table.find(key);
            auto it = ref.find(key);
            if (it == ref.end()) {
                EXPECT_EQ(p1, nullptr);
            } else {
                ASSERT_NE(p1, nullptr);
                EXPECT_EQ(*p1, it->second);
            }
        }
        EXPECT_EQ(table.size(), ref.size());
    }
}

TEST(HashTableStress, ManyInserts) {
    ChainHashTable<int, int> table;
    for (int i = 0; i < 200000; ++i)
        table.insert(i, i);
    EXPECT_EQ(table.size(), 200000);
    for (int i = 0; i < 200000; ++i) {
        ASSERT_NE(table.find(i), nullptr);
        EXPECT_EQ(*table.find(i), i);
    }
}

TEST(HashTableStress, InsertEraseMix) {
    ChainHashTable<int, int> table;
    for (int i = 0; i < 100000; ++i)
        table.insert(i, i);
    for (int i = 0; i < 100000; i += 2)
        table.erase(i);
    for (int i = 0; i < 100000; ++i) {
        if (i % 2 == 0)
            EXPECT_EQ(table.find(i), nullptr);
        else
            EXPECT_EQ(*table.find(i), i);
    }
}

/*
 * OPEN ADDRESSING HASH TABLE TESTS
 */
