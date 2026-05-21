#pragma once
#include <vector>
#include <string>
#include "polynomus.hpp"

template<typename Key, typename Value>
class OrderedTable {
public:
    using value_type = std::pair<Key, Value>;
    using container_type = std::vector<value_type>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    bool insert(const Key& key, const Value& value) {
        int idx = findIndex(key);
        if (idx < data_.size() && data_[idx].first == key)
            return false;
        data_.insert(data_.begin() + idx, {key, value});
        return true;
    }

    bool erase(const Key& key) {
        int idx = findIndex(key);
        if (idx >= data_.size() || data_[idx].first != key)
            return false;
        data_.erase(data_.begin() + idx);
        return true;
    }

    iterator erase(iterator it) { return data_.erase(it); }

    iterator find(const Key& key) {
        int idx = findIndex(key);
        if (idx < data_.size() && data_[idx].first == key)
            return data_.begin() + idx;
        return data_.end();
    }

    const_iterator find(const Key& key) const {
        int idx = findIndex(key);
        if (idx < data_.size() && data_[idx].first == key)
            return data_.begin() + idx;
        return data_.end();
    }

    bool contains(const Key& key) const {
        return find(key) != end();
    }

    Value& operator[](const Key& key) {
        int idx = findIndex(key);
        if (idx == data_.size() || data_[idx].first != key) {
            data_.insert(data_.begin() + idx, {key, Value()});
        }
        return data_[idx].second;
    }

    size_t size()          const { return data_.size();  }
    bool empty()           const { return data_.empty(); }
    void clear()                 { data_.clear();        }
    iterator begin()             { return data_.begin(); }
    iterator end()               { return data_.end();   }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end()   const { return data_.end();   }
    container_type items() const { return data_;         }

private:
    container_type data_;
    int findIndex(const Key& key) const {
        int left = 0;
        int right = static_cast<int>(data_.size());
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (data_[mid].first < key)
                left = mid + 1;
            else
                right = mid;
        }
        return left;
    }
};