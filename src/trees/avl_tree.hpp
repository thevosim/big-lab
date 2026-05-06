#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <algorithm>
#include <iostream>

template <typename T>
class AVLTree
{
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
            return p;
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

public:
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

    void print()
    {
        inorder(root);
        std::cout << std::endl;
    }
};

#endif