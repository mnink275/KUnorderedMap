#pragma once

#include <iostream>
#include <vector>
#include <memory>

using namespace std;

template<class Key, class T>
struct ListNode {
    pair<const Key, T> value_type;
    size_t hash_val;
    shared_ptr<ListNode<Key, T>> next;
    ListNode() : hash_val(0), next(nullptr), value_type(0, 0) {}
    ListNode(const Key& _key, size_t _hash_val)
        : hash_val(_hash_val), next(nullptr), value_type(_key, 0) {}
    ListNode(const Key& _key, size_t _hash_val, ListNode* _next)
        : hash_val(_hash_val), next(nullptr), value_type(_key, 0),
        next(_next) {}
    explicit ListNode(const shared_ptr<ListNode<Key, T>> _node)
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
    myUnorderedMap(const myUnorderedMap& other_map);
    myUnorderedMap& operator=(const myUnorderedMap& other_map);
    myUnorderedMap(myUnorderedMap&& other_map) noexcept;
    myUnorderedMap& operator=(myUnorderedMap&& other_map) noexcept;
    ~myUnorderedMap() = default;

    T& operator[](const Key& key);
    void print();
    size_t mapSize();
    bool isEmpty();
    shared_ptr<ListNode<Key, T>> find_by_hash(const Key&);
    shared_ptr<ListNode<Key, T>> find(const Key&);

private:
    int hash_func(const Key& key);
    void copy_handler(const myUnorderedMap& other_map);

    vector<shared_ptr<ListNode<Key, T>>> hash_set;
    size_t capacity;
    size_t size;
    shared_ptr<ListNode<Key, T>> begin;
    shared_ptr<ListNode<Key, T>> end;
    shared_ptr<ListNode<Key, T>> rbegin;
    shared_ptr<ListNode<Key, T>> rend;
    int max_hash_value;
};