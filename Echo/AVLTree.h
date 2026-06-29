#pragma once
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

template<typename K, typename V>
class AVLTree {
private:
    struct Node {
        K key;
        V value;
        int height;
        Node* left;
        Node* right;
        Node(const K& k, const V& v)
            : key(k), value(v), height(1), left(nullptr), right(nullptr) {
        }
    };

    Node* root;
    size_t count;

    int getHeight(Node* n) const { return n ? n->height : 0; }
    int getBalance(Node* n) const { return n ? getHeight(n->left) - getHeight(n->right) : 0; }

    void updateHeight(Node* n) {
        n->height = 1 + max(getHeight(n->left), getHeight(n->right));
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* balance(Node* n) {
        updateHeight(n);
        int balance = getBalance(n);

        if (balance > 1) {
            if (getBalance(n->left) < 0)
                n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (balance < -1) {
            if (getBalance(n->right) > 0)
                n->right = rotateRight(n->right);
            return rotateLeft(n);
        }
        return n;
    }

    Node* insertRec(Node* node, const K& key, const V& value) {
        if (!node) {
            count++;
            return new Node(key, value);
        }

        if (key < node->key)
            node->left = insertRec(node->left, key, value);
        else if (node->key < key)
            node->right = insertRec(node->right, key, value);
        else {
            node->value = value;
            return node;
        }

        return balance(node);
    }

    Node* minValueNode(Node* node) const {
        while (node->left) node = node->left;
        return node;
    }

    Node* removeRec(Node* root, const K& key) {
        if (!root) return nullptr;

        if (key < root->key)
            root->left = removeRec(root->left, key);
        else if (root->key < key)
            root->right = removeRec(root->right, key);
        else {
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;
                if (!temp) {
                    temp = root;
                    root = nullptr;
                }
                else {
                    *root = *temp;
                }
                delete temp;
                count--;
            }
            else {
                Node* temp = minValueNode(root->right);
                root->key = temp->key;
                root->value = temp->value;
                root->right = removeRec(root->right, temp->key);
            }
        }

        if (!root) return nullptr;
        return balance(root);
    }

    void inorderRec(Node* node, vector<pair<K, V>>& out) const {
        if (!node) return;
        inorderRec(node->left, out);
        out.emplace_back(node->key, node->value);
        inorderRec(node->right, out);
    }

    void reverseInorderRec(Node* node, vector<pair<K, V>>& out, int& remaining) const {
        if (!node || remaining <= 0) return;
        reverseInorderRec(node->right, out, remaining);
        if (remaining > 0) {
            out.emplace_back(node->key, node->value);
            remaining--;
        }
        reverseInorderRec(node->left, out, remaining);
    }

    void rangeSearchRec(Node* node, const K& low, const K& high, vector<V>& out) const {
        if (!node) return;

        if (node->key < low)
            rangeSearchRec(node->right, low, high, out);
        else if (high < node->key)
            rangeSearchRec(node->left, low, high, out);
        else {
            rangeSearchRec(node->left, low, high, out);
            out.push_back(node->value);
            rangeSearchRec(node->right, low, high, out);
        }
    }

    void clearRec(Node* node) {
        if (!node) return;
        clearRec(node->left);
        clearRec(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr), count(0) {}

    ~AVLTree() {
        clear();
    }

    void insert(const K& key, const V& value) {
        root = insertRec(root, key, value);
    }

    bool search(const K& key, V& value) const {
        Node* curr = root;
        while (curr) {
            if (key < curr->key) curr = curr->left;
            else if (curr->key < key) curr = curr->right;
            else { value = curr->value; return true; }
        }
        return false;
    }

    V* searchPtr(const K& key) {
        Node* curr = root;
        while (curr) {
            if (key < curr->key) curr = curr->left;
            else if (curr->key < key) curr = curr->right;
            else return &(curr->value);
        }
        return nullptr;
    }

    const V* searchPtr(const K& key) const {
        Node* curr = root;
        while (curr) {
            if (key < curr->key) curr = curr->left;
            else if (curr->key < key) curr = curr->right;
            else return &(curr->value);
        }
        return nullptr;
    }

    bool remove(const K& key) {
        if (!searchPtr(key)) return false;
        root = removeRec(root, key);
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

    vector<pair<K, V>> reverseInorder(int limit = -1) const {
        vector<pair<K, V>> result;
        int rem = (limit < 0) ? (int)count : limit;
        reverseInorderRec(root, result, rem);
        return result;
    }

    vector<V> rangeSearch(const K& low, const K& high) const {
        vector<V> result;
        rangeSearchRec(root, low, high, result);
        return result;
    }
};