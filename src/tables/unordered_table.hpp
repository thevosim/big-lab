#include <iostream>
#include <vector>
#include <utility>

template <typename K, typename V>
class UnorderedTable
{
public:
    using Pair = std::pair<K, V>;
    using iterator = typename std::vector<Pair>::iterator;
    using const_iterator = typename std::vector<Pair>::const_iterator;

private:
    std::vector<Pair> data;

    iterator find(const K& key)
    {
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            if (it->first == key)
                return it;
        }
        return data.end();
    }

public:
    iterator begin() { return data.begin(); }

    iterator end() { return data.end(); }

    const_iterator begin() const { return data.begin(); }

    const_iterator end() const { return data.end(); }

    iterator find(const K& key) { return internal_find(key); }

    std::pair<iterator, bool> insert(const Pair& value)
    {
        iterator it = find(value.first);
        if (it != data.end())
        {
            return { it, false };
        }
        data.push_back(value);
        return { std::prev(data.end()), true };
    }

    void push_back_unsafe(const Pair& value)
    {
        data.push_back(value);
    }

    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        for (; first != last; ++first)
        {
            insert(*first);
        }
    }

    V& operator[](const K& key)
    {
        iterator it = internal_find(key);
        if (it != data.end())
        {
            return it->second;
        }
        data.push_back({ key, V() });
        return data.back().second;
    }

    bool erase(const K& key)
    {
        iterator it = internal_find(key);
        if (it == data.end())
        {
            return false;
        }
        if (it != std::prev(data.end()))
        {
            *it = std::move(data.back());
        }
        data.pop_back();
        return true;
    }

    iterator erase(iterator it)
    {
        if (it == data.end())
        {
            return data.end();
        }
        size_t pos = it - data.begin();
        if (it != std::prev(data.end()))
        {
            *it = std::move(data.back());
        }
        data.pop_back();
        return (pos < data.size()) ? (data.begin() + pos) : data.end();
    }

    size_t size() const
    {
        return data.size();
    }

    bool empty() const
    {
        return data.empty();
    }

    void clear()
    {
        data.clear();
    }

    void reserve(size_t n)
    {
        data.reserve(n);
    }
};