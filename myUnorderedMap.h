#pragma once

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

template<class Key, class T, class Hash = std::hash<Key>>
class myUnorderedMap
{
public:
    template<class Key, class T>
    class ListNode;

    // container's iterator
    template<class Key, class T>
    class Iterator
    {
        // iterator tags
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = ListNode<Key, T>;
        using pointer = ListNode<Key, T>*;
        using reference = ListNode<Key, T>&;

    public:
        explicit Iterator(pointer _ptr) : ptr(_ptr) {};

        pair<const Key, T>& operator*()
        {
            return ptr->data_pair;
        }

        Iterator& operator++()
        {
            ptr = (ptr->next).get();
            return *this;
        }

        pair<const Key, T>* operator->()
        {
            return &(ptr->data_pair);
        }

    private:
        pointer ptr;
    };

public:
    // constructor and destructor
    myUnorderedMap()
        : capacity(701), m_begin(nullptr), rbegin(nullptr), m_end(nullptr),
        rend(nullptr), max_hash_value(0), size(0), bucket_count_val(0)
    {
        //cout << "Constructor!" << "\n";
        hash_set.resize(capacity, nullptr);
    }

    ~myUnorderedMap()
    {
        //cout << "Destructor!" << "\n";
    }

    // copy constructors
    myUnorderedMap(const myUnorderedMap& other_map)
        : capacity(701), m_begin(nullptr), rbegin(nullptr), m_end(nullptr),
        rend(nullptr), max_hash_value(0), size(0), bucket_count_val(0)
    {
        cout << "Copy operator()!" << "\n";
        if (this == &other_map) return;

        copy_handler(other_map);
    }

    myUnorderedMap<Key, T>& operator=(const myUnorderedMap& other_map)
    {
        cout << "Copy operator=!" << "\n";

        if (this == &other_map) return *this;

        copy_handler(other_map);

        return *this;
    }

    // move constructors
    myUnorderedMap(myUnorderedMap&& other_map) noexcept
        : capacity(701), m_begin(nullptr), rbegin(nullptr), m_end(nullptr),
        rend(nullptr), max_hash_value(0), size(0), bucket_count_val(0)
    {
        cout << "Move operator()!" << "\n";
        if (this == &other_map) return;

        hash_set = move(other_map.hash_set);
        size = other_map.size;
        bucket_count_val = other_map.bucket_count_val;
        m_begin = other_map.m_begin;
        rbegin = other_map.rbegin;

        other_map.m_begin = nullptr;
        other_map.rbegin = nullptr;
    }

    myUnorderedMap<Key, T>& operator=(myUnorderedMap&& other_map) noexcept
    {
        cout << "Move operator=!" << "\n";
        if (this == &other_map) return *this;

        hash_set = move(other_map.hash_set);
        size = other_map.size;
        bucket_count_val = other_map.bucket_count_val;
        m_begin = other_map.m_begin;
        rbegin = other_map.rbegin;

        other_map.m_begin = nullptr;
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
            bucket_count_val++;
            if (isEmpty())
            {
                hash_set[hash_val] = node;
                m_begin = node;
                rbegin = node;
            }
            else
            {
                hash_set[hash_val] = node;
                rbegin->next = node;
                node->prev = rbegin;
                rbegin = node;
            }
            
            return node->data_pair.second;
        }
        else
        {
            // if node with corresponding hash_val exists,
            // just change value of ListNode with corresponding key.
            shared_ptr<ListNode<Key, T>> it = hash_set[hash_val];
            shared_ptr<ListNode<Key, T>> prev_it;
            while (it != nullptr && it->hash_val == hash_val)
            {
                if (it->data_pair.first == key)
                {
                    return it->data_pair.second;
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

            return node->data_pair.second;
        }
    }

    void print()
    {
        if (m_begin == nullptr) cout << "Called map is empty!" << endl;
        shared_ptr<ListNode<Key, T>> it = m_begin;
        while (it != nullptr)
        {
            cout << it->data_pair.second << " ";
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
        auto it = m_begin;
        Key target = it->data_pair.first;
        while (target != key)
        {
            it = it->next;
            if (it == nullptr)
            {
                cout << "Element with key " << key
                    << " wasn't found." << "\n";
                return nullptr;
            }
            target = it->data_pair.first;
        }
        return &(it->data_pair);
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
            Key target = it->data_pair.first;
            while (key != target)
            {
                it = it->next;
                if (it == nullptr)
                {
                    cout << "Element with the key " << key
                        << " wasn't found." << "\n";
                    return nullptr;
                }
                target = it->data_pair.first;
            }
            return &(it->data_pair);
        }
    }

    size_t max_bucket_count()
    {
        return capacity;
    }

    size_t bucket_count()
    {
        return bucket_count_val;
    }

    float load_factor()
    {
        return static_cast<float>(Size()) / bucket_count();
    }

    Iterator<Key, T> begin()
    {
        return Iterator<Key, T>(m_begin.get());
    }


private:
    size_t hash_func(const Key& key)
    {
        /*size_t hash_val = static_cast<int>(key) % capacity;*/
        size_t hash_val = Hash{}(key) % capacity;
        return hash_val;
    }

    void copy_handler(const myUnorderedMap& other_map)
    {
        hash_set = other_map.hash_set;
        size = other_map.size;
        bucket_count_val = other_map.bucket_count_val;
        auto new_node = make_shared<ListNode<Key, T>>(*other_map.m_begin);
        m_begin = new_node;
        hash_set[new_node->hash_val] = new_node;

        auto prev_it = new_node;
        auto it = new_node->next;
        // Complexity: O(n)
        while (it != nullptr)
        {
            new_node = make_shared<ListNode<Key, T>>(*it);
            prev_it->next = new_node;
            new_node->prev = prev_it;
            if (new_node->hash_val != prev_it->hash_val)
            {
                hash_set[new_node->hash_val] = new_node;
            }
            it = it->next;
            prev_it = prev_it->next;
        }
        rbegin = prev_it;
    }
  
private:
    // bidirectional list
    template<class Key, class T>
    struct ListNode {
        pair<const Key, T> data_pair;
        size_t hash_val;
        shared_ptr<ListNode<Key, T>> prev;
        shared_ptr<ListNode<Key, T>> next;
        /*ListNode() : hash_val(0), next(nullptr), data_pair(0, 0) {}*/
        ListNode(const Key& _key, size_t _hash_val)
            : hash_val(_hash_val), prev(nullptr), next(nullptr), data_pair(_key, {}) {}
        ListNode(const Key& _key, size_t _hash_val, ListNode* _next)
            : hash_val(_hash_val), data_pair(_key, {}), next(_next) {}
        explicit ListNode(ListNode<Key, T>& _node)
            : data_pair(_node.data_pair.first, _node.data_pair.second),
            hash_val(_node.hash_val), prev(_node.prev), next(_node.next) {};
    };

private:
    vector<shared_ptr<ListNode<Key, T>>> hash_set;
    size_t capacity;
    size_t size;
    shared_ptr<ListNode<Key, T>> m_begin;
    shared_ptr<ListNode<Key, T>> m_end;
    shared_ptr<ListNode<Key, T>> rbegin;
    shared_ptr<ListNode<Key, T>> rend;
    size_t max_hash_value; // Candidate for deleting?
    size_t bucket_count_val;
};