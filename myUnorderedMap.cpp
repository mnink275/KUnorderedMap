#include "myUnorderedMap.h"

template<class Key, class T>
myUnorderedMap<Key, T>::myUnorderedMap() : size(7369), begin(nullptr),
    cbegin(nullptr), max_hash_value(0)
{
    //cout << "Map Constructor" << endl;
    hash_set.resize(size, nullptr);
}


template<class Key, class T>
myUnorderedMap<Key, T>::~myUnorderedMap()
{
    //cout << "Map ~Destructor" << endl;
    // TODO
    //dataErasing(begin);
    cout << "Destructor!" << endl;
}


template<class Key, class T>
myUnorderedMap<Key, T>::myUnorderedMap(const myUnorderedMap&& other_map) noexcept
{
    // template + T&& = Universal reference
    // „тобы не перегружать функции дл€ lvalue и rvalue, можно
    // использовать std::forward, чтобы прокидывать нужный тип.
    
    cout << "(T&& t)" << "\n";

}


template<class Key, class T>
myUnorderedMap<Key, T>& myUnorderedMap<Key, T>
    ::operator=(const myUnorderedMap& other_map)
{
    cout << "Copy constructor!" << endl;

    if (this == &other_map) return *this;

    dataErasing(begin);

    // Ќужно скопировать каждый узел и восстановить поинтеры!
    hash_set = other_map.hash_set;
    
    ListNode<Key, T>* node = new ListNode<Key, T>(other_map.begin);
    for (const ListNode<Key, T>* pListNode : hash_set)
    {
        if (pListNode == nullptr) continue;
        pListNode = node;
        while (node->next != nullptr
            && node->hash_val == pListNode->hash_val)
        {
            ListNode<Key, T>* tmp = node;
            node = new ListNode<Key, T>(node->next);
            tmp->next = node;
        }
    }
    
    int left = 0;
    while (left++)
    {
        if (hash_set[left] != nullptr)
        {
            begin = hash_set[left];
            break;
        }
    }

    int right = hash_set.size() - 1;
    while (right--)
    {
        if (hash_set[right] != nullptr)
        {
            cbegin = hash_set[right];
            break;
        }
    }

    return *this;
}


template<class Key, class T>
myUnorderedMap<Key, T>& myUnorderedMap<Key, T>
    ::operator=(myUnorderedMap&& other_map) noexcept
{
    cout << "Move constructor!" << endl;

    if (this == &other_map) return *this;

    // deliting the data of the left object
    // TODO
    dataErasing(begin);
    begin = nullptr;
    cbegin = nullptr;

    hash_set = move(other_map.hash_set);
    begin = other_map.begin;
    cbegin = other_map.cbegin;

    // moved map erasing
    other_map.begin = nullptr;
    other_map.cbegin = nullptr;

    return *this;
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
    int hash_val = static_cast<int>(key) % size;
    return hash_val;
}


template<class Key, class T>
void myUnorderedMap<Key, T>::print()
{
    if (begin == nullptr) cout << "Called map is empty!" << endl;
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


template<class Key, class T>
void myUnorderedMap<Key, T>::dataErasing(ListNode<Key, T>* root)
{
    ListNode<Key, T>* it;
    while (root != nullptr)
    {
        it = root->next;
        delete root;
        root = it;
    }
}


template class myUnorderedMap<int, int>;
template class myUnorderedMap<double, double>;