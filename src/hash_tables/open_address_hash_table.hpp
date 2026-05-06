#ifndef OPEN_ADDRESS_HASHTABLE_HPP
#define OPEN_ADDRESS_HASHTABLE_HPP

#include <vector>
#include <string>
#include <optional>
#include <cstdint>
#include <iterator>
enum State 
{ 
    EMPTY, 
    OCCUPIED, 
    DELETED 
};

template <typename K, typename V>
class OpenAddressHashTable 
{
private:
    static constexpr uint32_t KNUTH_GOLDEN_RATIO = 2654435761U;
    static constexpr size_t DJB2_INIT_HASH = 5381;
    static constexpr float LOAD_FACTOR_THRESHOLD = 0.7f;
    static constexpr size_t INITIAL_CAPACITY = 8;

    struct Node 
    {
        K key;
        V value;
        State state = EMPTY;
    };

    std::vector<Node> table;
    size_t capacity;
    size_t size = 0;

    size_t customHash(int key) const
    {
        return (static_cast<size_t>(key) * KNUTH_GOLDEN_RATIO) % capacity;
    }

    size_t customHash(const std::string& key) const
    {
        size_t h = DJB2_INIT_HASH;
        for (char c : key)
        {
            h = ((h << 5) + h) + c;
        }
        return h % capacity;
    }

    template <typename T>
    size_t customHash(const T& key) const
    {
        return static_cast<size_t>(key) % capacity;
    }

    void rehash() 
    {
        size_t old_capacity = capacity;
        std::vector<Node> old_table = std::move(table);

        capacity *= 2;
        table.assign(capacity, Node());
        size = 0;

        for (size_t i = 0; i < old_capacity; ++i) 
        {
            if (old_table[i].state == OCCUPIED) 
            {
                insert(old_table[i].key, old_table[i].value);
            }
        }
    }

public:
    OpenAddressHashTable(size_t cap = INITIAL_CAPACITY) : capacity(cap) 
    {
        table.resize(capacity);
    }
    class Iterator 
    {
    private:
        typename std::vector<Node>::iterator curr;
        typename std::vector<Node>::iterator end;

        void skipEmpty() 
        {
            while (curr != end && curr->state != OCCUPIED) 
            {
                ++curr;
            }
        }

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<K, V>;

        Iterator(typename std::vector<Node>::iterator start, typename std::vector<Node>::iterator last)
            : curr(start), end(last) 
        {
            skipEmpty();
        }

        std::pair<const K&, V&> operator*() const 
        {
            return { curr->key, curr->value };
        }

        Iterator& operator++() 
        {
            if (curr != end) 
            {
                ++curr;
                skipEmpty();
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const 
        {
            return curr != other.curr;
        }
    };

    Iterator begin() { return Iterator(table.begin(), table.end()); }
    Iterator end()   { return Iterator(table.end(), table.end()); }

    void insert(const K& key, const V& value) 
    {
        if (size >= capacity * LOAD_FACTOR_THRESHOLD) 
        {
            rehash();
        }

        size_t idx = customHash(key);

        while (table[idx].state == OCCUPIED) 
        {
            if (table[idx].key == key) 
            {
                table[idx].value = value;
                return;
            }
            idx = (idx + 1) % capacity;
        }

        table[idx].key = key;
        table[idx].value = value;
        table[idx].state = OCCUPIED;
        size++;
    }

    std::optional<V> get(const K& key) const 
    {
        size_t idx = customHash(key);
        size_t start_idx = idx;

        while (table[idx].state != EMPTY) 
        {
            if (table[idx].state == OCCUPIED && table[idx].key == key) 
            {
                return table[idx].value;
            }
            idx = (idx + 1) % capacity;
            if (idx == start_idx) break;
        }
        return std::nullopt;
    }

    void remove(const K& key) 
    {
        size_t idx = customHash(key);
        size_t start_idx = idx;

        while (table[idx].state != EMPTY) 
        {
            if (table[idx].state == OCCUPIED && table[idx].key == key) 
            {
                table[idx].state = DELETED;
                size--;
                return;
            }
            idx = (idx + 1) % capacity;
            if (idx == start_idx) break;
        }
    }

    V& operator[](const K& key)
    {
        if (size >= capacity * LOAD_FACTOR_THRESHOLD) 
        {
            rehash();
        }

        size_t idx = customHash(key);

        while (table[idx].state != EMPTY) 
        {
            if (table[idx].state == OCCUPIED && table[idx].key == key) 
            {
                return table[idx].value;
            }
            idx = (idx + 1) % capacity;
        }

        table[idx].key = key;
        table[idx].value = V(); 
        table[idx].state = OCCUPIED;
        size++;

        return table[idx].value;
    }
    size_t get_size() const { return size; }
    size_t get_capacity() const { return capacity; }
};

#endif