#pragma once

#include <iostream>
#include <vector>

using namespace std;

template<class Key, class T>
struct ListNode {
    pair<const Key, T> value_type;
    size_t hash_val;
    ListNode* next;
    ListNode() : hash_val(0), next(nullptr), value_type(0, 0) {}
    ListNode(const Key& _key, size_t _hash_val)
        : hash_val(_hash_val), next(nullptr), value_type(_key, 0) {}
    ListNode(const Key& _key, size_t _hash_val, ListNode* _next)
        : hash_val(_hash_val), next(nullptr), value_type(_key, 0),
        next(_next) {}
    ListNode(const ListNode<Key, T>* _node)
        : value_type(_node->value_type.first, _node->value_type.second),
        hash_val(_node->hash_val),
        next(_node->next)
    {

    }
};


template<class Key, class T>
class myUnorderedMap
{
public:
    myUnorderedMap();
    myUnorderedMap(const myUnorderedMap& other_map) = delete;
    myUnorderedMap& operator=(const myUnorderedMap& other_map);
    myUnorderedMap(const myUnorderedMap&& other_map) noexcept;
    myUnorderedMap& operator=(myUnorderedMap&& other_map) noexcept;
    ~myUnorderedMap();

    T& operator[](const Key& key);
    void print();
    bool isEmpty();
private:
    int hash_func(const Key& key);
    void dataErasing(ListNode<Key, T>* root);

    vector<ListNode<Key, T>*> hash_set;
    size_t size;
    ListNode<Key, T>* begin;
    ListNode<Key, T>* cbegin;
    int max_hash_value;
};