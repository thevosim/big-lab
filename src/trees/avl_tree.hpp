#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

template <typename T>
class AVLTree
{
    friend class AVLTreeTest;
private:
    struct Node
    {
        T key;
        int height;
        Node* left;
        Node* right;

        Node(T k) : key(k), height(1), left(nullptr), right(nullptr)
        {
        }
    };

    Node* root;

    int height(Node* p)
    {
        return p ? p->height : 0;
    }

    int bfactor(Node* p)
    {
        return p ? height(p->right) - height(p->left) : 0;
    }

    void fixheight(Node* p)
    {
        int hl = height(p->left);
        int hr = height(p->right);
        p->height = std::max(hl, hr) + 1;
    }

    Node* rotateRight(Node* p)
    {
        Node* q = p->left;
        p->left = q->right;
        q->right = p;
        fixheight(p);
        fixheight(q);
        return q;
    }

    Node* rotateLeft(Node* q)
    {
        Node* p = q->right;
        q->right = p->left;
        p->left = q;
        fixheight(q);
        fixheight(p);
        return p;
    }

    Node* balance(Node* p)
    {
        fixheight(p);

        if (bfactor(p) == 2)
        {
            if (bfactor(p->right) < 0)
            {
                p->right = rotateRight(p->right);
            }
            return rotateLeft(p);
        }

        if (bfactor(p) == -2)
        {
            if (bfactor(p->left) > 0)
            {
                p->left = rotateLeft(p->left);
            }
            return rotateRight(p);
        }

        return p;
    }
    Node* find(Node* p, const T& k)
    {
        if (!p)
        {
            return nullptr;
        }

        if (k < p->key)
        {
            return find(p->left, k);
        }
        if (k > p->key)
        {
            return find(p->right, k);
        }

        return p;
    }

    const Node* find(Node* p, const T& k) const
    {
        if (!p)
        {
            return nullptr;
        }

        if (k < p->key)
        {
            return find(p->left, k);
        }
        if (k > p->key)
        {
            return find(p->right, k);
        }

        return p;
    }
    Node* insert(Node* p, T k)
    {
        if (!p)
        {
            return new Node(k);
        }

        if (k < p->key)
        {
            p->left = insert(p->left, k);
        }
        else if (k > p->key)
        {
            p->right = insert(p->right, k);
        }
        else
        {
            p->key = k;
            return p;
        }

        return balance(p);
    }
    Node* findMin(Node* p)
    {
        return p->left ? findMin(p->left) : p;
    }
    Node* removeMin(Node* p)
    {
        if (!p->left) return p->right;
        p->left = removeMin(p->left);
        return balance(p);
    }
    Node* remove(Node* p, T k)
    {
        if (!p) return nullptr;

        if (k < p->key)
        {
            p->left = remove(p->left, k);
        }
        else if (k > p->key)
        {
            p->right = remove(p->right, k);
        }
        else
        {
            Node* q = p->left;
            Node* r = p->right;
            delete p;

            if (!r) return q;

            Node* min = findMin(r);
            min->right = removeMin(r);
            min->left = q;
            return balance(min);
        }
        return balance(p);
    }
    void inorder(Node* p)
    {
        if (!p)
        {
            return;
        }
        inorder(p->left);
        std::cout << p->key << " ";
        inorder(p->right);
    }

    void destroy(Node* p)
    {
        if (p)
        {
            destroy(p->left);
            destroy(p->right);
            delete p;
        }
    }
    void collectAll(Node* p, std::vector<TableItem>& vec) 
    {
        if (!p) return;
        collectAll(p->left, vec);
        vec.push_back(p->key);
        collectAll(p->right, vec);
    }
    

public:
    struct TableItem 
    {
        std::string key;
        Polynomus value;

        bool operator<(const TableItem& o) const { return key < o.key; }
        bool operator>(const TableItem& o) const { return key > o.key; }
        bool operator==(const TableItem& o) const { return key == o.key; }
    };

    void collectAll(std::vector<TableItem>& vec) 
    {
        collectAll(root, vec);
    }

    AVLTree() : root(nullptr)
    {
    }

    ~AVLTree()
    {
        destroy(root);
    }

    void insert(T k)
    {
        root = insert(root, k);
    }
    void remove(T k)
    {
        root = remove(root, k);
    }
    void print()
    {
        inorder(root);
        std::cout << std::endl;
    }
    const T* find(const T& k) const
    {
        const Node* p = find(root, k);
        return p ? &p->key : nullptr;
    }
};

#endif