#include <string>
#include <gtest/gtest.h>
#include "chain_hash_table.hpp"
#include "open_address_hash_table.hpp"
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

TEST(HashTableTest, InitialStateIsCorrect) 
{
    OpenAddressHashTable<int, std::string> table;

    EXPECT_EQ(table.get_size(), 0);
    EXPECT_EQ(table.get_capacity(), 8);
}

TEST(HashTableTest, InsertAndGetSingleElement) 
{
    OpenAddressHashTable<int, std::string> table;

    table.insert(1, "One");
    auto result = table.get(1);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "One");
    EXPECT_EQ(table.get_size(), 1);
}

TEST(HashTableTest, GetReturnsNulloptForMissingKey) 
{
    OpenAddressHashTable<int, std::string> table;

    table.insert(1, "One");
    auto result = table.get(999);

    EXPECT_FALSE(result.has_value());
}

TEST(HashTableTest, UpdateExistingKeyOverwritesValue) 
{
    OpenAddressHashTable<int, std::string> table;

    table.insert(1, "OldValue");
    table.insert(1, "NewValue");
    auto result = table.get(1);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), "NewValue");
    EXPECT_EQ(table.get_size(), 1);
}

TEST(HashTableTest, RemoveDeletesElementAndDecreasesSize) 
{
    OpenAddressHashTable<int, std::string> table;

    table.insert(1, "One");
    table.insert(2, "Two");
    table.remove(1);
    auto result = table.get(1);

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(table.get_size(), 1);
}

TEST(HashTableTest, OperatorBracketCreatesAndReturnsReference) 
{
    OpenAddressHashTable<std::string, int> table;

    table["apples"] = 5;
    table["apples"] += 10;
    auto result = table.get("apples");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 15);
    EXPECT_EQ(table.get_size(), 1);
}

TEST(HashTableTest, RehashIncreasesCapacityAndKeepsData) 
{
    OpenAddressHashTable<int, int> table(8);

    for (int i = 0; i < 6; ++i) 
    {
        table.insert(i, i * 10);
    }
    size_t old_capacity = table.get_capacity();
    
    table.insert(6, 60); 

    EXPECT_GT(table.get_capacity(), old_capacity);
    EXPECT_EQ(table.get_capacity(), 16);
    EXPECT_EQ(table.get_size(), 7);
    for (int i = 0; i <= 6; ++i) 
    {
        auto result = table.get(i);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), i * 10);
    }
}

TEST(HashTableTest, IteratorSkipsEmptyAndDeletedNodes) 
{
    OpenAddressHashTable<int, std::string> table;

    table.insert(1, "A");
    table.insert(2, "B");
    table.insert(3, "C");
    table.remove(2); 

    int count = 0;
    bool found_one = false;
    bool found_three = false;

    for (const auto& pair : table) 
    {
        count++;
        if (pair.first == 1 && pair.second == "A") found_one = true;
        if (pair.first == 3 && pair.second == "C") found_three = true;
    }

    EXPECT_EQ(count, 2);
    EXPECT_TRUE(found_one);
    EXPECT_TRUE(found_three);
}


TEST(HashTableStressTest, HandlesLargeAmountOfDataAndRehashes) 
{
    OpenAddressHashTable<int, int> table;
    const int num_elements = 100000;

    for (int i = 0; i < num_elements; ++i) 
    {
        table.insert(i, i * 2);
    }

    EXPECT_EQ(table.get_size(), num_elements);
    EXPECT_GE(table.get_capacity(), num_elements); 

    auto result_first = table.get(0);
    auto result_mid = table.get(num_elements / 2);
    auto result_last = table.get(num_elements - 1);

    ASSERT_TRUE(result_first.has_value());
    EXPECT_EQ(result_first.value(), 0);

    ASSERT_TRUE(result_mid.has_value());
    EXPECT_EQ(result_mid.value(), (num_elements / 2) * 2);

    ASSERT_TRUE(result_last.has_value());
    EXPECT_EQ(result_last.value(), (num_elements - 1) * 2);
}

TEST(HashTableStressTest, HeavyOperatorBracketUsage) 
{
    OpenAddressHashTable<std::string, int> table;
    const int operations = 50000;

    for (int i = 0; i < operations; ++i) 
    {
        std::string key = "key_" + std::to_string(i % 1000); 
        table[key]++;
    }

    EXPECT_EQ(table.get_size(), 1000);
    EXPECT_EQ(table["key_0"], operations / 1000);
    EXPECT_EQ(table["key_999"], operations / 1000);
}