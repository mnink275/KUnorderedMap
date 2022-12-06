#include "myUnorderedMap.h"

template<class Key, class T>
myUnorderedMap<Key, T>::myUnorderedMap() : size(10), begin(nullptr),
    cbegin(nullptr), max_hash_value(0)
{
    cout << "Map Constructor" << endl;
    hash_set.resize(size, nullptr);
}


template<class Key, class T>
myUnorderedMap<Key, T>::~myUnorderedMap()
{
    cout << "~Map Destructor" << endl;
}


template<class Key, class T>
T& myUnorderedMap<Key, T>::operator[](const Key& key)
{
    size_t hash_val = hash_func(key);

    if (hash_set[hash_val] == nullptr)
    {
        // if node with corresponding hash_val doesn't exists,
        // create new node, connect it to the main linked list and
        // put it's pointer to the hash_set.
        ListNode<Key, T>* node = new ListNode<Key, T>(key, hash_val);
        if (isEmpty())
        {
            begin = node;
            cbegin = node;
        }
        else
        {
            cbegin->next = node;
            cbegin = node;
        }
        hash_set[hash_val] = node;
        
        return node->value_type.second;
    }
    else
    {
        // if node with corresponding hash_val exists,
        // create new node and connect it to the main linked list.
        ListNode<Key, T>* it = hash_set[hash_val];
        while (it->next != nullptr)
        {
            it = it->next;
        }
        ListNode<Key, T>* node = new ListNode<Key, T>(key, hash_val);
        it->next = node;
        
        if (node->hash_val > max_hash_value)
        {
            max_hash_value = hash_val;
            cbegin = node;
        }

        return node->value_type.second;
    }
}


template<class Key, class T>
int myUnorderedMap<Key, T>::hash_func(const Key& key)
{
    int hash_val = static_cast<int>(key) % 10;
    return hash_val;
}


template<class Key, class T>
void myUnorderedMap<Key, T>::print()
{
    ListNode<Key, T>* it = begin;
    while (it != nullptr)
    {
        cout << it->value_type.second << " ";
        it = it->next;
    }
    cout << endl;
}


template<class Key, class T>
bool myUnorderedMap<Key, T>::isEmpty()
{
    bool isEmpty = 1;
    for (const auto& pListNode : hash_set)
    {
        if (pListNode != nullptr) isEmpty = 0;
    }
    return isEmpty;
}

template class myUnorderedMap<int, int>;
template class myUnorderedMap<double, double>;