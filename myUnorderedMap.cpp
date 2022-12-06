#include "myUnorderedMap.h"

template<class Key, class T>
myUnorderedMap<Key, T>::myUnorderedMap() : size(10)
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
        ListNode<Key, T>* node = new ListNode<Key, T>(key, hash_val);
        hash_set[hash_val] = node;
        return node->value_type.second;
    }
    else
    {
        ListNode<Key, T>* it = hash_set[hash_val];
        while (it->next != nullptr)
        {
            it = it->next;
        }
        ListNode<Key, T>* node = new ListNode<Key, T>(key, hash_val);
        it->next = node;
        return node->value_type.second;
    }
    
}


template<class Key, class T>
int  myUnorderedMap<Key, T>::hash_func(const Key& key)
{
    int hash_val = key % 10;
    return hash_val;
}


template<class Key, class T>
void myUnorderedMap<Key, T>::print()
{
    for (const auto& pListNode : hash_set)
    {
        if (pListNode)
        {
            ListNode<Key, T>* it = pListNode;
            while (it != nullptr)
            {
                cout << it->value_type.second << " ";
                it = it->next;
            }
        }
    }
    cout << endl;
}


template class myUnorderedMap<int, int>;