#pragma once

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

template<class Key, class T>
struct ListNode {
    pair<const Key, T> value_type;
    size_t hash_val;
    shared_ptr<ListNode<Key, T>> prev;
    shared_ptr<ListNode<Key, T>> next;
    /*ListNode() : hash_val(0), next(nullptr), value_type(0, 0) {}*/
    ListNode(const Key& _key, size_t _hash_val)
        : hash_val(_hash_val), prev(nullptr), next(nullptr), value_type(_key, {}) {}
    ListNode(const Key& _key, size_t _hash_val, ListNode* _next)
        : hash_val(_hash_val), value_type(_key, {}), next(_next) {}
    //explicit ListNode(const shared_ptr<ListNode<Key, T>> _node)
    //    : value_type(_node->value_type.first, _node->value_type.second),
    //    hash_val(_node->hash_val),
    //    next(_node->next) {}
};


template<class Key, class T>
class myUnorderedMap
{
public:
    // constructor and destructor
    myUnorderedMap()
        : capacity(701), begin(nullptr), rbegin(nullptr), end(nullptr),
        rend(nullptr), max_hash_value(0), size(0)
    {
        cout << "Constructor!" << "\n";
        hash_set.resize(capacity, nullptr);
    }

    ~myUnorderedMap()
    {
        cout << "Destructor!" << "\n";
    }

    // copy constructors
    myUnorderedMap(const myUnorderedMap& other_map)
        : capacity(701), begin(nullptr), rbegin(nullptr), end(nullptr),
        rend(nullptr), max_hash_value(0), size(0)
    {
        cout << "Copy operator()!" << endl;
        if (this == &other_map) return;

        copy_handler(other_map);
    }

    myUnorderedMap<Key, T>& operator=(const myUnorderedMap& other_map)
    {
        cout << "Copy operator=!" << endl;

        if (this == &other_map) return *this;

        copy_handler(other_map);

        return *this;
    }

    // move constructors
    myUnorderedMap(myUnorderedMap&& other_map) noexcept
        : capacity(701), begin(nullptr), rbegin(nullptr), end(nullptr),
        rend(nullptr), max_hash_value(0), size(0)
    {
        cout << "Move operator()!" << "\n";
        if (this == &other_map) return;

        hash_set = move(other_map.hash_set);
        size = other_map.size;
        begin = other_map.begin;
        rbegin = other_map.rbegin;

        other_map.begin = nullptr;
        other_map.rbegin = nullptr;
    }

    myUnorderedMap<Key, T>& operator=(myUnorderedMap&& other_map) noexcept
    {
        cout << "Move operator=!" << "\n";
        if (this == &other_map) return *this;

        hash_set = move(other_map.hash_set);
        size = other_map.size;
        begin = other_map.begin;
        rbegin = other_map.rbegin;

        other_map.begin = nullptr;
        other_map.rbegin = nullptr;

        return *this;
    }


    // assign operator []
    T& operator[](const Key& key)
    {
        size_t hash_val = hash_func(key);
        size++;

        if (hash_set[hash_val] == nullptr)
        {
            // if node with corresponding hash_val doesn't exist,
            // create new node, connect it to the main linked list and
            // put it's pointer to the hash_set.
            auto node = make_shared<ListNode<Key, T>>(key, hash_val);
            if (isEmpty())
            {
                hash_set[hash_val] = node;
                begin = node;
                rbegin = node;
            }
            else
            {
                hash_set[hash_val] = node;
                rbegin->next = node;
                node->prev = rbegin;
                rbegin = node;
            }
            
            return node->value_type.second;
        }
        else
        {
            // if node with corresponding hash_val exists,
            // just change value of ListNode with corresponding key.
            shared_ptr<ListNode<Key, T>> it = hash_set[hash_val];
            shared_ptr<ListNode<Key, T>> prev_it;
            while (it != nullptr && it->hash_val == hash_val)
            {
                if (it->value_type.first == key)
                {
                    return it->value_type.second;
                }
                prev_it = it;
                it = it->next;
            }

            // if node with corresponding key doesn't exist,
            // create new node and add it to the ending of 
            // the corresponding bucket.
            auto node = make_shared<ListNode<Key, T>>(key, hash_val);
            prev_it->next = node;
            node->prev = prev_it;
            if (it != nullptr)
            {
                node->next = it;
                it->prev = node;
            }

            return node->value_type.second;
        }
    }

    void print()
    {
        if (begin == nullptr) cout << "Called map is empty!" << endl;
        shared_ptr<ListNode<Key, T>> it = begin;
        while (it != nullptr)
        {
            cout << it->value_type.second << " ";
            it = it->next;
        }
        cout << endl;
    }

    size_t Size()
    {
        return size;
    }

    bool isEmpty()
    {
        bool isEmpty = 1;
        for (const auto& pListNode : hash_set)
        {
            if (pListNode != nullptr)
            {
                isEmpty = 0;
                return isEmpty;
                // maybe 'return isEmpty' better?
            }
        }
        return isEmpty;
    }

    pair<const Key, T>* brute_force_find(const Key& key)
    {
        auto it = begin;
        Key target = it->value_type.first;
        while (target != key)
        {
            it = it->next;
            if (it == nullptr)
            {
                cout << "Element with key " << key
                    << " wasn't found." << "\n";
                return nullptr;
            }
            target = it->value_type.first;
        }
        return &(it->value_type);
    }

    pair<const Key, T>* find(const Key& key)
    {
        // if hash_func - random, this 'find' won't work!
        // TODO:
        // how to prevent implicit convertion, if I write
        // find_by_hash(int_value) ?
        size_t hash_val = hash_func(key);
        auto it = hash_set[hash_val];
        if (it == nullptr)
        {
            cout << "Element with the key " << key
                << " wasn't found." << "\n";
            return nullptr;
        }
        else
        {
            Key target = it->value_type.first;
            while (key != target)
            {
                it = it->next;
                if (it == nullptr)
                {
                    cout << "Element with the key " << key
                        << " wasn't found." << "\n";
                    return nullptr;
                }
                target = it->value_type.first;
            }
            return &(it->value_type);
        }
    }

private:
    size_t hash_func(const Key& key)
    {
        /*size_t hash_val = static_cast<int>(key) % capacity;*/
        size_t hash_val = std::hash<Key>{}(key) % capacity;
        return hash_val;
    }

    void copy_handler(const myUnorderedMap& other_map)
    {
        hash_set = other_map.hash_set;
        size = other_map.size;
        auto node = make_shared<ListNode<Key, T>>(other_map.begin);
        // Complexity: O(n)
        for (shared_ptr<ListNode<Key, T>> pListNode : hash_set)
        {
            if (pListNode == nullptr) continue;
            pListNode = node;
            while (node->next != nullptr
                && node->hash_val == pListNode->hash_val)
            {
                shared_ptr<ListNode<Key, T>> tmp = node;
                node = make_shared<ListNode<Key, T>>(node->next);
                tmp->next = node;
                node->prev = tmp;
            }
        }

        for (int i = 0; i < capacity; i++)
        {
            if (hash_set[i] != nullptr)
            {
                begin = hash_set[i];
                break;
            }
        }

        for (int i = capacity - 1; i >= 0; i--)
        {
            if (hash_set[i] != nullptr)
            {
                rbegin = hash_set[i];
                break;
            }
        }
    }

private:
    vector<shared_ptr<ListNode<Key, T>>> hash_set;
    size_t capacity;
    size_t size;
    shared_ptr<ListNode<Key, T>> begin;
    shared_ptr<ListNode<Key, T>> end;
    shared_ptr<ListNode<Key, T>> rbegin;
    shared_ptr<ListNode<Key, T>> rend;
    size_t max_hash_value;
};