#pragma once
#include <vector>
#include <string>

using namespace std;

template<typename T, typename V>
class HashTable {
private:
    struct HashNode {
        T key;
        V value;
        HashNode* next;
        HashNode(const T& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    vector<HashNode*> buckets;
    size_t numBuckets;
    size_t numElements;

    size_t djb2Hash(const string& str) const {
        size_t hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c); // hash * 33 + c
        }
        return hash;
    }

    template<typename H>
    size_t djb2Hash(const H& value) const {
        // Para punteros: usar direccion de memoria
        return reinterpret_cast<size_t>(&value) * 2654435761u;
    }

    size_t getHash(const T& key) const {
        return djb2Hash(key) % numBuckets;
    }

    void rehash() {
        size_t oldBuckets = numBuckets;
        numBuckets *= 2;
        vector<HashNode*> newBuckets(numBuckets, nullptr);

        for (size_t i = 0; i < oldBuckets; ++i) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                HashNode* next = curr->next;
                size_t newIndex = djb2Hash(curr->key) % numBuckets;
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

    HashTable(const HashTable& other)
        : numBuckets(other.numBuckets), numElements(other.numElements) {
        buckets.resize(numBuckets, nullptr);
        for (size_t i = 0; i < numBuckets; ++i) {
            HashNode** dst = &buckets[i];
            HashNode* src = other.buckets[i];
            while (src != nullptr) {
                *dst = new HashNode(src->key, src->value);
                dst = &((*dst)->next);
                src = src->next;
            }
        }
    }

    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            clear();
            numBuckets = other.numBuckets;
            numElements = other.numElements;
            buckets.resize(numBuckets, nullptr);
            for (size_t i = 0; i < numBuckets; ++i) {
                HashNode** dst = &buckets[i];
                HashNode* src = other.buckets[i];
                while (src != nullptr) {
                    *dst = new HashNode(src->key, src->value);
                    dst = &((*dst)->next);
                    src = src->next;
                }
            }
        }
        return *this;
    }

    void insert(const T& key, const V& value) {
        if ((double)numElements / numBuckets > 0.75) {
            rehash();
        }

        size_t index = getHash(key);
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

    bool find(const T& key, V& value) const {
        size_t index = getHash(key);
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

    V* findPtr(const T& key) {
        size_t index = getHash(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    const V* findPtr(const T& key) const {
        size_t index = getHash(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool remove(const T& key) {
        size_t index = getHash(key);
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

    bool contains(const T& key) const {
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

    vector<pair<T, V>> getAllItems() const {
        vector<pair<T, V>> items;
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
