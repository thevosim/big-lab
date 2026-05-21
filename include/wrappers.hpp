#ifndef TABLE_WRAPPERS_HPP
#define TABLE_WRAPPERS_HPP

#include <vector>
#include <string>
#include <utility>
#include <iterator>
#include <optional>

#include "../src/trees/avl_tree.hpp"
#include "../src/trees/red_black_tree.hpp"
#include "../src/hash_tables/chain_hash_table.hpp"
#include "../src/hash_tables/open_address_hash_table.hpp"
#include "../src/tables/unordered_table.hpp"
#include "../src/tables/ordered_table.hpp"
template <typename K, typename V>
class ITableWrapper
{
public:
    virtual ~ITableWrapper() = default;
    virtual bool insert(const K& key, const V& value) = 0;
    virtual bool erase(const K& key) = 0;
    virtual V* find(const K& key) = 0;
    virtual const V* find(const K& key) const = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual void collectAll(std::vector<std::pair<K, V>>& vec) = 0;
};

template <typename K, typename V>
class AVLTreeWrapper : public ITableWrapper<K, V>
{
private:
    struct Element
    {
        K key;
        mutable V value;

        bool operator<(const Element& o) const
        {
            return key < o.key;
        }
        bool operator>(const Element& o) const
        {
            return key > o.key;
        }
        bool operator==(const Element& o) const
        {
            return key == o.key;
        }
    };

    AVLTree<Element>* tree;
    size_t m_size;

public:
    AVLTreeWrapper() : tree(new AVLTree<Element>()), m_size(0)
    {
    }

    ~AVLTreeWrapper() override
    {
        delete tree;
    }

    bool insert(const K& key, const V& value) override
    {
        Element dummy{ key, value };
        const Element* existing = tree->find(dummy);
        if (existing)
        {
            existing->value = value;
            return false;
        }
        tree->insert(dummy);
        m_size++;
        return true;
    }

    bool erase(const K& key) override
    {
        Element dummy{ key, V() };
        if (!tree->find(dummy))
        {
            return false;
        }
        tree->remove(dummy);
        m_size--;
        return true;
    }

    V* find(const K& key) override
    {
        Element dummy{ key, V() };
        const Element* res = tree->find(dummy);
        if (res)
        {
            return const_cast<V*>(&res->value);
        }
        return nullptr;
    }

    const V* find(const K& key) const override
    {
        Element dummy{ key, V() };
        const Element* res = tree->find(dummy);
        if (res)
        {
            return &res->value;
        }
        return nullptr;
    }

    void clear() override
    {
        delete tree;
        tree = new AVLTree<Element>();
        m_size = 0;
    }

    size_t size() const override
    {
        return m_size;
    }

    bool empty() const override
    {
        return m_size == 0;
    }

    void collectAll(std::vector<std::pair<K, V>>& vec) override
    {
        std::vector<Element> tmp;
        tree->collectAll(tmp);
        
        for (const auto& item : tmp)
        {
            vec.emplace_back(item.key, item.value);
        }
    }
};

template <typename K, typename V>
class RBTreeWrapper : public ITableWrapper<K, V>
{
private:
    RBTree<K, V>* tree;

public:
    RBTreeWrapper() : tree(new RBTree<K, V>())
    {
    }

    ~RBTreeWrapper() override
    {
        delete tree;
    }

    bool insert(const K& key, const V& value) override
    {
        V* existing = tree->find(key);
        if (existing)
        {
            *existing = value;
            return false;
        }
        tree->insert(key, value);
        return true;
    }

    bool erase(const K& key) override
    {
        return tree->erase(key);
    }

    V* find(const K& key) override
    {
        return tree->find(key);
    }

    const V* find(const K& key) const override
    {
        return tree->find(key);
    }

    void clear() override
    {
        tree->clear();
    }

    size_t size() const override
    {
        return tree->size();
    }

    bool empty() const override
    {
        return tree->empty();
    }

    void collectAll(std::vector<std::pair<K, V>>& vec) override
    {
        tree->inorder([&](const K& k, const V& v)
        {
            vec.push_back({ k, v });
        });
    }
};

template <typename K, typename V>
class ChainHashTableWrapper : public ITableWrapper<K, V>
{
private:
    ChainHashTable<K, V>* table;

public:
    ChainHashTableWrapper() : table(new ChainHashTable<K, V>())
    {
    }

    ~ChainHashTableWrapper() override
    {
        delete table;
    }

    bool insert(const K& key, const V& value) override
    {
        if (table->contains(key))
        {
            (*table)[key] = value;
            return false;
        }
        (*table)[key] = value;
        return true;
    }

    bool erase(const K& key) override
    {
        return table->erase(key);
    }

    V* find(const K& key) override
    {
        if (!table->contains(key))
        {
            return nullptr;
        }
        return &((*table)[key]);
    }

    const V* find(const K& key) const override
    {
        if (!table->contains(key))
        {
            return nullptr;
        }
        return &((*const_cast<ChainHashTable<K, V>*>(table))[key]);
    }

    void clear() override
    {
        table->clear();
    }

    size_t size() const override
    {
        return table->size();
    }

    bool empty() const override
    {
        return table->empty();
    }

    void collectAll(std::vector<std::pair<K, V>>& vec) override
    {
        table->for_each([&](const K& k, const V& v)
        {
            vec.push_back({ k, v });
        });
    }
};

template <typename K, typename V>
class OpenAddressHashTableWrapper : public ITableWrapper<K, V>
{
private:
    OpenAddressHashTable<K, V>* table;
    size_t m_size;

public:
    OpenAddressHashTableWrapper() : table(new OpenAddressHashTable<K, V>()), m_size(0)
    {
    }

    ~OpenAddressHashTableWrapper() override
    {
        delete table;
    }

    bool insert(const K& key, const V& value) override
    {
        if (table->get(key).has_value())
        {
            (*table)[key] = value;
            return false;
        }
        (*table)[key] = value;
        m_size++;
        return true;
    }

    bool erase(const K& key) override
    {
        if (!table->get(key).has_value())
        {
            return false;
        }
        table->remove(key);
        m_size--;
        return true;
    }

    V* find(const K& key) override
    {
        if (!table->get(key).has_value())
        {
            return nullptr;
        }
        return &((*table)[key]);
    }

    const V* find(const K& key) const override
    {
        if (!table->get(key).has_value())
        {
            return nullptr;
        }
        return &((*const_cast<OpenAddressHashTable<K, V>*>(table))[key]);
    }

    void clear() override
    {
        delete table;
        table = new OpenAddressHashTable<K, V>();
        m_size = 0;
    }

    size_t size() const override
    {
        return m_size;
    }

    bool empty() const override
    {
        return m_size == 0;
    }

    void collectAll(std::vector<std::pair<K, V>>& vec) override
    {
        for (auto it = table->begin(); it != table->end(); ++it)
        {
            vec.push_back({ it->first, it->second });
        }
    }
};

template <typename K, typename V>
class UnorderedTableWrapper : public ITableWrapper<K, V>
{
private:
    UnorderedTable<K, V>* table;

public:
    UnorderedTableWrapper() : table(new UnorderedTable<K, V>())
    {
    }

    ~UnorderedTableWrapper() override
    {
        delete table;
    }

    bool insert(const K& key, const V& value) override
    {
        auto res = table->insert({ key, value });
        if (!res.second)
        {
            res.first->second = value;
        }
        return res.second;
    }

    bool erase(const K& key) override
    {
        return table->erase(key);
    }

    V* find(const K& key) override
    {
        auto it = table->find(key);
        if (it != table->end())
        {
            return &it->second;
        }
        return nullptr;
    }

    const V* find(const K& key) const override
    {
        auto* non_const_table = const_cast<UnorderedTable<K, V>*>(table);
        auto it = non_const_table->find(key);
        if (it != non_const_table->end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void clear() override
    {
        delete table;
        table = new UnorderedTable<K, V>();
    }

    size_t size() const override
    {
        return std::distance(table->begin(), table->end());
    }

    bool empty() const override
    {
        return table->begin() == table->end();
    }

    void collectAll(std::vector<std::pair<K, V>>& vec) override
    {
        for (auto it = table->begin(); it != table->end(); ++it)
        {
            vec.push_back({ it->first, it->second });
        }
    }
};

template <typename K, typename V>
class OrderedTableWrapper : public ITableWrapper<K, V>
{
private:
    OrderedTable<K, V>* table;

public:
    OrderedTableWrapper() : table(new OrderedTable<K, V>())
    {
    }

    ~OrderedTableWrapper() override
    {
        delete table;
    }

    bool insert(const K& key, const V& value) override
    {
        bool inserted = table->insert(key, value);
        if (!inserted)
        {
            (*table)[key] = value;
        }
        return inserted;
    }

    bool erase(const K& key) override
    {
        return table->erase(key);
    }

    V* find(const K& key) override
    {
        auto it = table->find(key);
        if (it != table->end())
        {
            return &it->second;
        }
        return nullptr;
    }

    const V* find(const K& key) const override
    {
        const OrderedTable<K, V>* const_table = table;
        auto it = const_table->find(key);
        if (it != const_table->end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void clear() override
    {
        table->clear();
    }

    size_t size() const override
    {
        return table->size();
    }

    bool empty() const override
    {
        return table->empty();
    }

    void collectAll(std::vector<std::pair<K, V>>& vec) override
    {
        for (auto it = table->begin(); it != table->end(); ++it)
        {
            vec.push_back({ it->first, it->second });
        }
    }
};
#endif