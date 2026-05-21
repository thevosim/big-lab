#pragma once

#include <vector>
#include <utility>
#include <random>
#include <cstdint>

template <typename K, typename V>
class ChainHashTable {
private:
    using pkv = std::pair<K, V>;
    using Chain = std::vector<pkv>;

    struct HashFamily {
        uint64_t a, b;
        HashFamily() : a(1), b(0) {}
        HashFamily(uint64_t a_val, uint64_t b_val) : a(a_val), b(b_val) {}
        size_t operator()(const K& key, size_t m) const {
            uint64_t h = std::hash<K>{}(key);
            uint64_t res = a*h + b;
            return res % m;
        }
    };

    std::vector<Chain> buckets;
    size_t el_cnt = 0;
    HashFamily hash_func;
    std::mt19937_64 rng;
    
    static constexpr size_t init_cap = 16;
    static constexpr double alpha = 0.75;

    void generate_hash_params() {
        std::uniform_int_distribution<uint64_t> dist(1, UINT64_MAX);
        hash_func = HashFamily(dist(rng)|1, dist(rng));
    }

    void rebuild(size_t new_cap) {
        std::vector<Chain> old = std::move(buckets);
        buckets.assign(new_cap, Chain{});
        el_cnt = 0;
        generate_hash_params();
        for (auto& ch : old) {
            for (auto& [k, v] : ch) {
                insert_internal(k, v);
            }
        }
    }

    void insert_internal(const K& key, const V& value) {
        size_t idx = hash_func(key, buckets.size());
        for (auto& e : buckets[idx]) {
            if (e.first == key) {
                e.second = value;
                return;
            }
        }
        buckets[idx].emplace_back(key, value);
        ++el_cnt;
    }

public:
    ChainHashTable()
        : buckets(init_cap),
          rng(std::random_device{}()) {
        generate_hash_params();
    }

    ChainHashTable(size_t cap)
        : buckets(cap),
          rng(std::random_device{}()) {
        generate_hash_params();
    }

    V& insert(const K& key, const V& value) {
        if ((double)(el_cnt+1)/buckets.size() > alpha)
            rebuild(buckets.size()*2);
        size_t idx = hash_func(key, buckets.size());
        for (auto& e : buckets[idx]) {
            if (e.first == key) {
                e.second = value;
                return e.second;
            }
        }
        buckets[idx].emplace_back(key, value);
        ++el_cnt;
        return buckets[idx].back().second;
    }

    V* find(const K& key) {
        size_t idx = hash_func(key, buckets.size());
        for (auto& e : buckets[idx]) {
            if (e.first == key) {
                return &e.second;
            }
        }
        return nullptr;
    }

    const V* find(const K& key) const {
        size_t idx = hash_func(key, buckets.size());
        for (const auto& e : buckets[idx]) {
            if (e.first == key) {
                return &e.second;
            }
        }
        return nullptr;
    }
    
    V& operator[](const K& key) {
        if ((double)(el_cnt+1)/buckets.size()>alpha) 
            rebuild(buckets.size()*2);
        size_t idx = hash_func(key, buckets.size());
        for (auto& e : buckets[idx]) {
            if (e.first == key) {
                return e.second;
            }
        }
        buckets[idx].emplace_back(key, V{});
        ++el_cnt;
        return buckets[idx].back().second;
    }

    bool erase(const K& key) {
        size_t idx = hash_func(key, buckets.size());
        auto& chain = buckets[idx];
        for (size_t i = 0; i<chain.size(); ++i) {
            if (chain[i].first == key) {
                chain[i] = std::move(chain.back());
                chain.pop_back();
                --el_cnt;
                return true;
            }
        }
        return false;
    }

    size_t size() const { return el_cnt; }
    bool empty() const { return el_cnt == 0; }
    double get_oad_factor() const { return (double)el_cnt/buckets.size(); }
    bool contains(const K& key) const { return find(key) != nullptr; }

    void clear() {
        buckets.assign(buckets.size(), Chain{});
        el_cnt = 0;
        generate_hash_params();
    }

    void reserve(size_t n) {
        size_t needed = (size_t)(n/alpha)+1;
        if (needed > buckets.size())
            rebuild(needed);
    }

    template <typename Func>
    void for_each(Func f) {
        for (auto& chain : buckets)
            for (auto& [k, v] : chain)
                f(k, v);
    }
};