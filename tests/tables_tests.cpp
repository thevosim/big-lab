#include <string>
#include <map>
#include <random>
#include <gtest/gtest.h>
#include "../src/tables/ordered_table.hpp"
#include "../src/tables/unordered_table.hpp"
/*
 * ORDERED TABLE TESTS
 */
TEST(TemplateOrderedTable, EmptyInit) {
    OrderedTable<int, int> t;
    EXPECT_TRUE(t.empty());
    EXPECT_EQ(t.size(), 0);
}

TEST(TemplateOrderedTable, InsertWorks) {
    OrderedTable<int, int> t;
    EXPECT_TRUE(t.insert(1, 10));
    EXPECT_FALSE(t.insert(1, 20));
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t.find(1)->second, 10);
}

TEST(TemplateOrderedTable, FindExisting) {
    OrderedTable<int, int> t;
    t.insert(2, 200);
    auto it = t.find(2);
    ASSERT_NE(it, t.end());
    EXPECT_EQ(it->second, 200);
}

TEST(TemplateOrderedTable, FindMissing) {
    OrderedTable<int, int> t;
    EXPECT_EQ(t.find(5), t.end());
}

TEST(TemplateOrderedTable, BracketInsert) {
    OrderedTable<int, int> t;
    t[5] = 50;
    EXPECT_EQ(t.size(), 1);
    EXPECT_EQ(t[5], 50);
}

TEST(TemplateOrderedTable, BracketDefault) {
    OrderedTable<int, int> t;
    int val = t[10];
    EXPECT_EQ(val, 0);
    EXPECT_EQ(t.size(), 1);
}

TEST(TemplateOrderedTable, EraseKey) {
    OrderedTable<int, int> t;
    t.insert(1, 10);
    t.insert(2, 20);
    EXPECT_TRUE(t.erase(1));
    EXPECT_FALSE(t.contains(1));
}

TEST(TemplateOrderedTable, EraseIterator) {
    OrderedTable<int, int> t;
    t.insert(1, 10);
    t.insert(2, 20);
    auto it = t.find(1);
    t.erase(it);
    EXPECT_FALSE(t.contains(1));
}

TEST(TemplateOrderedTable, CompareWithStdMap) {
    OrderedTable<int, int> custom;
    std::map<int, int> ref;
    std::mt19937 gen(123);
    for (int i = 0; i < 3000; ++i) {
        int key = gen() % 500;
        if (gen() % 2) {
            int val = gen() % 1000;
            custom.insert(key, val);
            ref.insert({key, val});
        } else {
            custom.erase(key);
            ref.erase(key);
        }
    }
    EXPECT_EQ(custom.size(), ref.size());
    auto it1 = custom.begin();
    auto it2 = ref.begin();
    while (it1 != custom.end() && it2 != ref.end()) {
        EXPECT_EQ(it1->first, it2->first);
        EXPECT_EQ(it1->second, it2->second);
        ++it1;
        ++it2;
    }
    EXPECT_EQ(it1, custom.end());
    EXPECT_EQ(it2, ref.end());
}

TEST(TemplateOrderedTableExtra, SingleElementLifecycle) {
    OrderedTable<int, int> t;

    t.insert(1, 10);
    EXPECT_EQ(t.size(), 1);

    t.erase(1);
    EXPECT_TRUE(t.empty());
}

TEST(TemplateOrderedTableExtra, EraseAllSequentially) {
    OrderedTable<int, int> t;
    for (int i = 0; i < 10; ++i)
        t.insert(i, i);
    for (int i = 0; i < 10; ++i)
        EXPECT_TRUE(t.erase(i));
    EXPECT_TRUE(t.empty());
}

TEST(TemplateOrderedTableExtra, IteratorTraversalCount) {
    OrderedTable<int, int> t;
    for (int i = 0; i < 50; ++i)
        t.insert(i, i);
    int count = 0;
    for (auto it = t.begin(); it != t.end(); ++it)
        count++;
    EXPECT_EQ(count, t.size());
}

TEST(TemplateOrderedTableExtra, IteratorEraseChain) {
    OrderedTable<int, int> t;
    for (int i = 0; i < 5; ++i)
        t.insert(i, i);
    auto it = t.begin();
    while (it != t.end()) {
        it = t.erase(it);
    }
    EXPECT_TRUE(t.empty());
}

TEST(TemplateOrderedTableExtra, ClearResetsState) {
    OrderedTable<int, int> t;
    for (int i = 0; i < 10; ++i)
        t.insert(i, i);
    t.clear();
    EXPECT_EQ(t.size(), 0);
    EXPECT_TRUE(t.empty());
}

TEST(TemplateOrderedTableExtra, ItemsReturnsCopy) {
    OrderedTable<int, int> t;
    t.insert(1, 10);
    t.insert(2, 20);
    auto items = t.items();
    EXPECT_EQ(items.size(), 2);
    items[0].second = 999;
    EXPECT_NE(t.find(1)->second, 999);
}

TEST(TemplateOrderedTableExtra, InsertReverseOrder) {
    OrderedTable<int, int> t;
    for (int i = 100; i >= 0; --i)
        t.insert(i, i);
    int prev = -1;
    for (auto& p : t) {
        EXPECT_GT(p.first, prev);
        prev = p.first;
    }
}

TEST(TemplateOrderedTableExtra, RandomAccessConsistency) {
    OrderedTable<int, int> t;
    std::map<int, int> ref;
    std::mt19937 gen(999);
    for (int i = 0; i < 2000; ++i) {
        int key = gen() % 300;
        if (gen() % 3 == 0) {
            t[key] = key * 10;
            ref[key] = key * 10;
        } else {
            t.erase(key);
            ref.erase(key);
        }
        EXPECT_EQ(t.size(), ref.size());
    }
}

TEST(TemplateOrderedTableStress, InsertEraseALot) {
    OrderedTable<int, int> t;
    std::map<int, int> ref;
    std::mt19937 gen(42);
    const int n = 20000;
    for (int i = 0; i < n; ++i) {
        int key = gen() % 1000;
        if (gen() % 2) {
            int val = gen() % 10000;
            t.insert(key, val);
            ref.insert({key, val});
        } else {
            t.erase(key);
            ref.erase(key);
        }
        if (i % 1000 == 0) {
            EXPECT_EQ(t.size(), ref.size());
        }
    }
    auto it1 = t.begin();
    auto it2 = ref.begin();
    while (it1 != t.end() && it2 != ref.end()) {
        EXPECT_EQ(it1->first, it2->first);
        EXPECT_EQ(it1->second, it2->second);
        ++it1; ++it2;
    }
    EXPECT_EQ(it1, t.end());
    EXPECT_EQ(it2, ref.end());
}

TEST(TemplateOrderedTableStress, FindALot) {
    OrderedTable<int, int> t;
    const int n = 100000;
    const int q = 300000;

    for (int i = 0; i < n; ++i)
        t.insert(i, i);

    std::mt19937 gen(1337);

    for (int i = 0; i < q; ++i) {
        int key = gen() % n;
        auto it = t.find(key);
        ASSERT_NE(it, t.end());
    }
}

TEST(TemplateOrderedTableStress, BracketsALot) {
    OrderedTable<int, int> t;
    std::mt19937 gen(228);
    for (int i = 0; i < 50000; ++i) {
        int key = gen() % 2000;
        t[key] += 1;
    }
    int total = 0;
    for (auto& p : t)
        total += p.second;
    EXPECT_GT(total, 0);
}

/*
 * UNORDERED TABLE TESTS
 */

class UnorderedTableTests : public ::testing::Test 
{
};

TEST_F(UnorderedTableTests, BasicInsertAndFind)
{
    UnorderedTable<int, std::string> map;
    
    auto result = map.insert({1, "one"});
    auto it = map.find(1);

    EXPECT_TRUE(result.second);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->second, "one");
    EXPECT_EQ(map.size(), 1);
}

TEST_F(UnorderedTableTests, DuplicateInsert)
{
    UnorderedTable<int, std::string> map;
    
    map.insert({1, "one"});
    auto result = map.insert({1, "duplicate"});

    EXPECT_FALSE(result.second);
    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map.find(1)->second, "one");
}

TEST_F(UnorderedTableTests, OperatorSquareBrackets)
{
    UnorderedTable<int, int> map;
    
    map[10] = 100;
    map[10] = 200;
    int value = map[10];
    int newValue = map[20];

    EXPECT_EQ(value, 200);
    EXPECT_EQ(newValue, 0);
    EXPECT_EQ(map.size(), 2);
}

TEST_F(UnorderedTableTests, EraseByKeyWithSwapLogic)
{
    UnorderedTable<int, std::string> map;
    map.insert({1, "first"});
    map.insert({2, "second"});
    map.insert({3, "third"});

    bool removed = map.erase(1);
    auto it2 = map.find(2);
    auto it3 = map.find(3);

    EXPECT_TRUE(removed);
    EXPECT_EQ(map.size(), 2);
    EXPECT_NE(it2, map.end());
    EXPECT_NE(it3, map.end());
}

TEST_F(UnorderedTableTests, EraseByIterator)
{
    UnorderedTable<int, int> map;
    map.insert({1, 10});
    map.insert({2, 20});
    auto it = map.find(1);

    auto nextIt = map.erase(it);

    EXPECT_EQ(map.size(), 1);
    EXPECT_EQ(map.begin()->first, 2);
    EXPECT_EQ(nextIt, map.begin());
}

TEST_F(UnorderedTableTests, ClearAndEmpty)
{
    UnorderedTable<int, int> map;
    map.insert({1, 10});

    map.clear();

    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.find(1), map.end());
}

TEST_F(UnorderedTableTests, StressTestLinearGrowth)
{
    UnorderedTable<int, int> map;
    const int count = 10000;
    map.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        map.push_back_unsafe({i, i * 2});
    }

    bool allFound = true;
    for (int i = 0; i < count; ++i)
    {
        if (map.find(i) == map.end())
        {
            allFound = false;
            break;
        }
    }

    EXPECT_EQ(map.size(), count);
    EXPECT_TRUE(allFound);
}

TEST_F(UnorderedTableTests, StressTestMassiveErase)
{
    UnorderedTable<int, int> map;
    const int count = 5000;
    for (int i = 0; i < count; ++i)
    {
        map.push_back_unsafe({i, i});
    }

    for (int i = 0; i < count; i += 2)
    {
        map.erase(i);
    }

    EXPECT_EQ(map.size(), count / 2);
    EXPECT_EQ(map.find(0), map.end());
    EXPECT_NE(map.find(1), map.end());
}