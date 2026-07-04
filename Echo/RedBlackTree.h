#pragma once
#include <vector>
#include <string>
<<<<<<< HEAD
=======
#include <algorithm>
>>>>>>> 0c6f778 (AvanceNew)
using namespace std;

template<typename K, typename V>
class RedBlackTree {
private:
    enum class Color { RED, BLACK };

    struct Node {
        K key;
        V value;
        Color color;
        Node* left;
        Node* right;
        Node* parent;
        Node(const K& k, const V& v)
            : key(k), value(v), color(Color::RED),
            left(nullptr), right(nullptr), parent(nullptr) {
        }
    };

    Node* root;
    size_t count;

    bool isRed(Node* node) const {
        return node != nullptr && node->color == Color::RED;
    }

    Node* rotateLeft(Node* h) {
        Node* x = h->right;
        h->right = x->left;
        if (x->left) x->left->parent = h;
        x->parent = h->parent;
        if (h->parent == nullptr) root = x;
        else if (h == h->parent->left) h->parent->left = x;
        else h->parent->right = x;
        x->left = h;
        h->parent = x;
        x->color = h->color;
        h->color = Color::RED;
        return x;
    }

    Node* rotateRight(Node* h) {
        Node* x = h->left;
        h->left = x->right;
        if (x->right) x->right->parent = h;
        x->parent = h->parent;
        if (h->parent == nullptr) root = x;
        else if (h == h->parent->right) h->parent->right = x;
        else h->parent->left = x;
        x->right = h;
        h->parent = x;
        x->color = h->color;
        h->color = Color::RED;
        return x;
    }

    void flipColors(Node* h) {
        h->color = Color::RED;
        if (h->left) h->left->color = Color::BLACK;
        if (h->right) h->right->color = Color::BLACK;
    }

    Node* fixUp(Node* h) {
        if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
        if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
        if (isRed(h->left) && isRed(h->right)) flipColors(h);
        return h;
    }

    Node* insertRec(Node* h, const K& key, const V& value) {
        if (h == nullptr) {
            count++;
            return new Node(key, value);
        }

        if (key < h->key) {
            h->left = insertRec(h->left, key, value);
            h->left->parent = h;
        }
        else if (h->key < key) {
            h->right = insertRec(h->right, key, value);
            h->right->parent = h;
        }
        else {
            h->value = value;
        }

        if (isRed(h->right) && !isRed(h->left)) h = rotateLeft(h);
        if (isRed(h->left) && isRed(h->left->left)) h = rotateRight(h);
        if (isRed(h->left) && isRed(h->right)) flipColors(h);

        return h;
    }

    Node* findMin(Node* h) const {
        while (h->left != nullptr) h = h->left;
        return h;
    }

    Node* removeMin(Node* h) {
        if (h->left == nullptr) {
            delete h;
            return nullptr;
        }
        if (!isRed(h->left) && !isRed(h->left->left))
            h = moveRedLeft(h);
        h->left = removeMin(h->left);
        return fixUp(h);
    }

    Node* moveRedLeft(Node* h) {
        flipColors(h);
        if (isRed(h->right->left)) {
            h->right = rotateRight(h->right);
            h = rotateLeft(h);
            flipColors(h);
        }
        return h;
    }

    Node* moveRedRight(Node* h) {
        flipColors(h);
        if (isRed(h->left->left)) {
            h = rotateRight(h);
            flipColors(h);
        }
        return h;
    }

    Node* removeRec(Node* h, const K& key) {
        if (key < h->key) {
            if (!isRed(h->left) && !isRed(h->left->left))
                h = moveRedLeft(h);
            h->left = removeRec(h->left, key);
        }
        else {
            if (isRed(h->left))
                h = rotateRight(h);
            if (!(h->key < key) && !(key < h->key) && h->right == nullptr) {
                delete h;
                count--;
                return nullptr;
            }
            if (!isRed(h->right) && !isRed(h->right->left))
                h = moveRedRight(h);
            if (!(h->key < key) && !(key < h->key)) {
                Node* x = findMin(h->right);
                h->key = x->key;
                h->value = x->value;
                h->right = removeMin(h->right);
                count--;
            }
            else {
                h->right = removeRec(h->right, key);
            }
        }
        return fixUp(h);
    }

    void inorderRec(Node* h, vector<pair<K, V>>& out) const {
        if (h == nullptr) return;
        inorderRec(h->left, out);
        out.emplace_back(h->key, h->value);
        inorderRec(h->right, out);
    }

    void prefixSearchRec(Node* node, const string& prefix, vector<pair<K, V>>& out) const {
        if (!node) return;

        string nodePrefix = node->key.substr(0, min(prefix.size(), node->key.size()));

        if (prefix <= node->key) {
            prefixSearchRec(node->left, prefix, out);
        }

        if (node->key.size() >= prefix.size()) {
            if (nodePrefix == prefix) {
                out.emplace_back(node->key, node->value);
            }
        }

        if (nodePrefix == prefix || node->key < prefix) {
            prefixSearchRec(node->right, prefix, out);
        }
    }

    void clearRec(Node* h) {
        if (h == nullptr) return;
        clearRec(h->left);
        clearRec(h->right);
        delete h;
    }

public:
    RedBlackTree() : root(nullptr), count(0) {}

    ~RedBlackTree() {
        clear();
    }

    void insert(const K& key, const V& value) {
        root = insertRec(root, key, value);
        if (root) root->color = Color::BLACK;
    }

    bool search(const K& key, V& value) const {
        Node* curr = root;
        while (curr != nullptr) {
            if (key < curr->key) curr = curr->left;
            else if (curr->key < key) curr = curr->right;
            else {
                value = curr->value;
                return true;
            }
        }
        return false;
    }

    V* searchPtr(const K& key) {
        Node* curr = root;
        while (curr != nullptr) {
            if (key < curr->key) curr = curr->left;
            else if (curr->key < key) curr = curr->right;
            else return &(curr->value);
        }
        return nullptr;
    }

    const V* searchPtr(const K& key) const {
        Node* curr = root;
        while (curr != nullptr) {
            if (key < curr->key) curr = curr->left;
            else if (curr->key < key) curr = curr->right;
            else return &(curr->value);
        }
        return nullptr;
    }

    bool remove(const K& key) {
        if (!searchPtr(key)) return false;
        if (!isRed(root->left) && !isRed(root->right))
            root->color = Color::RED;
        root = removeRec(root, key);
        if (root) root->color = Color::BLACK;
        return true;
    }

    void clear() {
        clearRec(root);
        root = nullptr;
        count = 0;
    }

    bool isEmpty() const { return root == nullptr; }
    size_t size() const { return count; }

    vector<pair<K, V>> inorder() const {
        vector<pair<K, V>> result;
        inorderRec(root, result);
        return result;
    }

    vector<pair<K, V>> prefixSearch(const string& prefix) const {
        vector<pair<K, V>> result;
        if (prefix.empty()) {
            return inorder();
        }
        prefixSearchRec(root, prefix, result);
        return result;
    }
};
<<<<<<< HEAD
=======

>>>>>>> 0c6f778 (AvanceNew)
