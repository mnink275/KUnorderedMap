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
    /*ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next) : val(x), next(next) {}*/
};


template<class Key, class T>
class myUnorderedMap
{
public:
    myUnorderedMap();
    T& operator[](const Key& key);
    void print();
    ~myUnorderedMap();
    bool isEmpty();
private:
    vector<ListNode<Key, T>*> hash_set;
    int hash_func(const Key& key);
    size_t size;

};