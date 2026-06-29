#pragma once
#include <vector>
#include <string>
#include <utility>
#include <functional>

using namespace std;

template<typename K, typename V>
class HashTable {
private:
    struct HashNode {
        K key;
        V value;
        HashNode* next;
        HashNode(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    vector<HashNode*> buckets;
    size_t numBuckets;
    size_t numElements;

    size_t hashFunc(const K& key) const {
        return hash<K>{}(key) % numBuckets;
    }

    void rehash() {
        size_t oldBuckets = numBuckets;
        numBuckets *= 2;
        vector<HashNode*> newBuckets(numBuckets, nullptr);

        for (size_t i = 0; i < oldBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                HashNode* next = curr->next;
                size_t newIndex = hash<K>{}(curr->key) % numBuckets;
                curr->next = newBuckets[newIndex];
                newBuckets[newIndex] = curr;
                curr = next;
            }
        }
        buckets = move(newBuckets);
    }

public:
    HashTable(size_t initialBuckets = 16)
        : numBuckets(initialBuckets), numElements(0) {
        buckets.resize(numBuckets, nullptr);
    }

    ~HashTable() {
        clear();
    }

    void insert(const K& key, const V& value) {
        if ((double)numElements / numBuckets > 0.75) {
            rehash();
        }

        size_t index = hashFunc(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value;
                return;
            }
            curr = curr->next;
        }

        HashNode* newNode = new HashNode(key, value);
        newNode->next = buckets[index];
        buckets[index] = newNode;
        numElements++;
    }

    bool find(const K& key, V& value) const {
        size_t index = hashFunc(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                value = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    V* findPtr(const K& key) {
        size_t index = hashFunc(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    const V* findPtr(const K& key) const {
        size_t index = hashFunc(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool remove(const K& key) {
        size_t index = hashFunc(key);
        HashNode* curr = buckets[index];
        HashNode* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr) {
                    buckets[index] = curr->next;
                }
                else {
                    prev->next = curr->next;
                }
                delete curr;
                numElements--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    bool contains(const K& key) const {
        V dummy;
        return find(key, dummy);
    }

    void clear() {
        for (size_t i = 0; i < numBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                HashNode* next = curr->next;
                delete curr;
                curr = next;
            }
            buckets[i] = nullptr;
        }
        numElements = 0;
    }

    size_t size() const { return numElements; }
    bool isEmpty() const { return numElements == 0; }

    vector<pair<K, V>> getAllItems() const {
        vector<pair<K, V>> items;
        for (size_t i = 0; i < numBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                items.emplace_back(curr->key, curr->value);
                curr = curr->next;
            }
        }
        return items;
    }
};