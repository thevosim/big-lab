#pragma once
#include <functional>
#include <vector>

enum class RbColor { RED, BLACK };

template <typename K, typename V>
class RBTree {
private:
    struct RBNode {
        K key;
        V value;
        RbColor color = RbColor::RED;
        RBNode* left = nullptr;
        RBNode* right = nullptr;
        RBNode* parent = nullptr;
        bool isNil = false;

        RBNode() : isNil(true), color(RbColor::BLACK) {}
        RBNode(const K& k, const V& v) : key(k), value(v), isNil(false) {}
        RBNode(K&& k, V&& v) : key(std::move(k)), value(std::move(v)), isNil(false) {}
    };

    RBNode* m_nil;
    RBNode* m_root;
    size_t m_count;

    bool isRed(RBNode* node) const {
        return node && node->color == RbColor::RED;
    }

    bool isBlack(RBNode* node) const {
        return !node || node->color == RbColor::BLACK;
    }
    void setBlack(RBNode* node) {
        if (node) node->color = RbColor::BLACK;
    }

    void setRed(RBNode* node) {
        if (node && !node->isNil) node->color = RbColor::RED;
    }

    void rotateLeft(RBNode* x) {
        RBNode* y = x->right;
        x->right = y->left;
        if (y->left != m_nil) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == m_nil)
            m_root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rotateRight(RBNode* y) {
        RBNode* x = y->left;
        y->left = x->right;
        if (x->right != m_nil) x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == m_nil)
            m_root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
        x->right = y;
        y->parent = x;
    }

    void balanceAfterInsert(RBNode* node) {
        while (node != m_root && isRed(node->parent)) {
            RBNode* parent = node->parent;
            RBNode* grand = parent->parent;
            if (parent == grand->left) {
                RBNode* uncle = grand->right;
                if (isRed(uncle)) {
                    setBlack(parent);
                    setBlack(uncle);
                    setRed(grand);
                    node = grand;
                } else {
                    if (node == parent->right) {
                        node = parent;
                        rotateLeft(node);
                        parent = node->parent;
                    }
                    setBlack(parent);
                    setRed(grand);
                    rotateRight(grand);
                }
            } else {
                RBNode* uncle = grand->left;
                if (isRed(uncle)) {
                    setBlack(parent);
                    setBlack(uncle);
                    setRed(grand);
                    node = grand;
                } else {
                    if (node == parent->left) {
                        node = parent;
                        rotateRight(node);
                        parent = node->parent;
                    }
                    setBlack(parent);
                    setRed(grand);
                    rotateLeft(grand);
                }
            }
        }
        setBlack(m_root);
    }

    RBNode* findMinimum(RBNode* node) const {
        while (node->left != m_nil) node = node->left;
        return node;
    }

    RBNode* findMaximum(RBNode* node) const {
        while (node->right != m_nil) node = node->right;
        return node;
    }

    void replaceNode(RBNode* oldNode, RBNode* newNode) {
        if (oldNode->parent == m_nil)
            m_root = newNode;
        else if (oldNode == oldNode->parent->left)
            oldNode->parent->left = newNode;
        else
            oldNode->parent->right = newNode;
        newNode->parent = oldNode->parent;
    }

    void fixAfterDelete(RBNode* node, RBNode* parent) {
        while (node != m_root && isBlack(node)) {
            if (node == parent->left) {
                RBNode* brother = parent->right;
                if (isRed(brother)) {
                    setBlack(brother);
                    setRed(parent);
                    rotateLeft(parent);
                    brother = parent->right;
                }
                if (isBlack(brother->left) &&
                    isBlack(brother->right)) {
                    setRed(brother);
                    node = parent;
                    parent = node->parent;
                    }
                else {
                    if (isBlack(brother->right)) {
                        setBlack(brother->left);
                        setRed(brother);
                        rotateRight(brother);
                        brother = parent->right;
                    }
                    brother->color = parent->color;
                    setBlack(parent);
                    setBlack(brother->right);
                    rotateLeft(parent);
                    node = m_root;
                }
            } else {
                RBNode* brother = parent->left;
                if (isRed(brother)) {
                    setBlack(brother);
                    setRed(parent);
                    rotateRight(parent);
                    brother = parent->left;
                }
                if (isBlack(brother->left) &&
                    isBlack(brother->right)) {
                    setRed(brother);
                    node = parent;
                    parent = node->parent;
                    }
                else {
                    if (isBlack(brother->left)) {
                        setBlack(brother->right);
                        setRed(brother);
                        rotateLeft(brother);
                        brother = parent->left;
                    }
                    brother->color = parent->color;
                    setBlack(parent);
                    setBlack(brother->left);
                    rotateRight(parent);
                    node = m_root;
                }
            }
        }
        setBlack(node);
    }

    void eraseNode(RBNode* victim) {
        RBNode* y = victim;
        RBNode* x = m_nil;
        RBNode* x_parent = m_nil;
        RbColor y_original_color = y->color;

        if (victim->left == m_nil) {
            x = victim->right;
            x_parent = victim->parent;
            replaceNode(victim, victim->right);
        } else if (victim->right == m_nil) {

            x = victim->left;
            x_parent = victim->parent;

            replaceNode(victim, victim->left);

        } else {
            y = findMinimum(victim->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == victim) {
                x_parent = y;
            } else {
                x_parent = y->parent;
                replaceNode(y, y->right);
                y->right = victim->right;
                y->right->parent = y;
            }
            replaceNode(victim, y);
            y->left = victim->left;
            y->left->parent = y;
            y->color = victim->color;
        }
        delete victim;
        --m_count;
        if (y_original_color == RbColor::BLACK) {
            fixAfterDelete(x, x_parent);
        }
    }

    RBNode* findNode(const K& key) const {
        RBNode* cur = m_root;
        while (cur != m_nil) {
            if (key < cur->key)
                cur = cur->left;
            else if (cur->key < key)
                cur = cur->right;
            else
                return cur;
        }
        return m_nil;
    }

    void deleteSubtree(RBNode* node) {
        if (node == m_nil) return;
        deleteSubtree(node->left);
        deleteSubtree(node->right);
        delete node;
    }

public:
    RBTree() {
        m_nil = new RBNode();
        m_nil->left = m_nil;
        m_nil->right = m_nil;
        m_nil->parent = m_nil;
        m_root = m_nil;
        m_count = 0;
    }

    ~RBTree() {
        clear();
        delete m_nil;
    }

    RBTree(RBTree&& other) noexcept
        : m_nil(other.m_nil), m_root(other.m_root), m_count(other.m_count) {
        other.m_nil = nullptr;
        other.m_root = nullptr;
        other.m_count = 0;
    }

    RBTree& operator=(RBTree&& other) noexcept {
        if (this != &other) {
            clear();
            delete m_nil;
            m_nil = other.m_nil;
            m_root = other.m_root;
            m_count = other.m_count;
            other.m_nil = nullptr;
            other.m_root = nullptr;
            other.m_count = 0;
        }
        return *this;
    }

    bool insert(const K& key, const V& value) {
        if (findNode(key) != m_nil) return false;
        RBNode* newNode = new RBNode(key, value);
        newNode->left = m_nil;
        newNode->right = m_nil;
        m_count++;
        if (m_root == m_nil) {
            m_root = newNode;
            newNode->parent = m_nil;
            newNode->left = m_nil;
            newNode->right = m_nil;
            setBlack(m_root);
            return true;
        }

        RBNode* cur = m_root;
        RBNode* parent = m_nil;
        while (cur != m_nil) {
            parent = cur;
            if (key < cur->key)
                cur = cur->left;
            else
                cur = cur->right;
        }

        newNode->parent = parent;
        if (key < parent->key)
            parent->left = newNode;
        else
            parent->right = newNode;
        balanceAfterInsert(newNode);
        return true;
    }

    bool erase(const K& key) {
        RBNode* victim = findNode(key);
        if (victim == m_nil) return false;
        eraseNode(victim);
        return true;
    }

    V* find(const K& key) {
        RBNode* node = findNode(key);
        return (node != m_nil) ? &node->value : nullptr;
    }

    const V* find(const K& key) const {
        RBNode* node = findNode(key);
        return (node != m_nil) ? &node->value : nullptr;
    }

    V& operator[](const K& key) {
        RBNode* node = findNode(key);
        if (node != m_nil) return node->value;
        insert(key, V{});
        return *find(key);
    }

    bool contains(const K& key) const { return findNode(key) != m_nil; }
    size_t size() const { return m_count; }
    bool empty() const { return m_count == 0; }

    void clear() {
        if (m_root != m_nil)
            deleteSubtree(m_root);
        m_root = m_nil;
        m_count = 0;
    }

    template <typename Func>
    void inorder(Func&& callback) const {
        std::function<void(RBNode*)> dfs = [&](RBNode* node) {
            if (node == m_nil) return;
            dfs(node->left);
            callback(node->key, node->value);
            dfs(node->right);
        };
        dfs(m_root);
    }

    std::vector<std::pair<K, V>> toVector() const {
        std::vector<std::pair<K, V>> result;
        auto lambda = [&](const K& k, const V& v) {
            result.emplace_back(k, v);
        };
        inorder(lambda);
        return result;
    }
};