#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

template<class Key, class T, class Hash = std::hash<Key>>
class myUnorderedMap
{
private:
    // bidirectional list
    struct ListNode {
        pair<const Key, T> data_pair;
        size_t hash_val;
        shared_ptr<ListNode> prev;
        shared_ptr<ListNode> next;

        ListNode()
            : data_pair({}, {}), hash_val(0), prev(nullptr), next(nullptr) {}
        ListNode(const Key& _key, size_t _hash_val)
            : hash_val(_hash_val), prev(nullptr), next(nullptr), data_pair(_key, {}) {}
        ListNode(const Key& _key, size_t _hash_val, ListNode* _next)
            : hash_val(_hash_val), data_pair(_key, {}), next(_next) {}
        explicit ListNode(ListNode& _node)
            : data_pair(_node.data_pair.first, _node.data_pair.second),
            hash_val(_node.hash_val), prev(_node.prev), next(_node.next) {};
    };

public:
    // container's iterator
    template<bool IsConst>
    class CommonIterator
    {
        // iterator tags
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = ListNode;
        using pointer = ListNode*;
        using reference = ListNode&;

    public:
        explicit CommonIterator(pointer _ptr) : ptr(_ptr) {};

        std::conditional_t<IsConst, const pair<const Key, T>&, pair<const Key, T>&> operator*()
        {
            return ptr->data_pair;
        }

        CommonIterator& operator++()
        {
            ptr = (ptr->next).get();
            return *this;
        }

        CommonIterator& operator--()
        {
            ptr = (ptr->prev).get();
            return *this;
        }

        std::conditional_t<IsConst, const pair<const Key, T>*, pair<const Key, T>*> operator->()
        {
            return &(ptr->data_pair);
        }

        friend bool operator!=(const CommonIterator& left, const CommonIterator& right)
        {
            return left.ptr != right.ptr;
        }

        friend bool operator==(const CommonIterator& left, const CommonIterator& right)
        {
            return left.ptr == right.ptr;
        }

    private:
        std::conditional_t<IsConst, const pointer, pointer> ptr;
    };
public:
    using iterator = CommonIterator<false>;
    using const_iterator = CommonIterator<true>;

public:
    // constructor and destructor
    myUnorderedMap();

    ~myUnorderedMap();

    // copy constructors
    myUnorderedMap(const myUnorderedMap& other_map);

    myUnorderedMap<Key, T>& operator=(const myUnorderedMap& other_map);

    // move constructors
    myUnorderedMap(myUnorderedMap&& other_map) noexcept;

    myUnorderedMap<Key, T>& operator=(myUnorderedMap&& other_map) noexcept;

    // assign operator []
    T& operator[](const Key& key);

    void print();

    size_t Size();

    bool isEmpty();

    pair<const Key, T>* brute_force_find(const Key& key);

    pair<const Key, T>* find(const Key& key);

    size_t max_bucket_count();

    size_t bucket_count();

    float load_factor();

    void rehash(size_t new_capacity);

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

private:
    size_t hash_func(const Key& key);

    void copy_handler(const myUnorderedMap& other_map);

    void move_handler(myUnorderedMap&& other_map);

    void create_end_fake_node();

    void nodes_unbinding();

private:
    vector<shared_ptr<ListNode>> hash_set;
    size_t capacity;
    size_t size;
    // Unjustified usage of shared_ptr? It seems to me,
    // that raw pointers are better (lower memory costs and maybe performance).
    shared_ptr<ListNode> m_begin;
    shared_ptr<ListNode> m_end;
    shared_ptr<ListNode> m_rbegin;
    shared_ptr<ListNode> m_rend;
    float max_load_factor;
    size_t bucket_count_val;
};


template<class Key, class T, class Hash>
myUnorderedMap<Key, T, Hash>::myUnorderedMap()
    : capacity(239), m_begin(nullptr), m_rbegin(nullptr), m_end(nullptr),
    m_rend(nullptr), max_load_factor(2.0f), size(0), bucket_count_val(0)
{
    //cout << "Constructor!" << "\n";
    hash_set.resize(capacity, nullptr);
}

template<class Key, class T, class Hash>
myUnorderedMap<Key, T, Hash>::~myUnorderedMap()
{
    if (size > 0)
    {
        // Elimination of loop pointers (unbinding of each node)
        nodes_unbinding();
    }
}

template<class Key, class T, class Hash>
myUnorderedMap<Key, T, Hash>::myUnorderedMap(const myUnorderedMap& other_map)
    : capacity(other_map.capacity), m_begin(nullptr), m_rbegin(nullptr), m_end(nullptr),
    m_rend(nullptr), max_load_factor(2.0f), size(0), bucket_count_val(0)
{
    cout << "Copy operator()" << "\n";
    if (this == &other_map) return;

    copy_handler(other_map);
}

template<class Key, class T, class Hash>
myUnorderedMap<Key, T>& myUnorderedMap<Key, T, Hash>::operator=(const myUnorderedMap& other_map)
{
    cout << "Copy operator=" << "\n";

    if (this == &other_map) return *this;

    copy_handler(other_map);

    return *this;
}

template<class Key, class T, class Hash>
myUnorderedMap<Key, T, Hash>::myUnorderedMap(myUnorderedMap&& other_map) noexcept
    : capacity(other_map.capacity), m_begin(nullptr), m_rbegin(nullptr), m_end(nullptr),
    m_rend(nullptr), max_load_factor(0.0f), size(0), bucket_count_val(0)
{
    cout << "Move operator()" << "\n";
    if (this == &other_map) return;

    move_handler(move(other_map));
}

template<class Key, class T, class Hash>
myUnorderedMap<Key, T>& myUnorderedMap<Key, T, Hash>::operator=(myUnorderedMap&& other_map) noexcept
{
    cout << "Move operator=" << "\n";
    if (this == &other_map) return *this;

    move_handler(move(other_map));

    return *this;
}

template<class Key, class T, class Hash>
T& myUnorderedMap<Key, T, Hash>::operator[](const Key& key)
{
    // check if it's time to rehash
    if (size > 0 && load_factor() > max_load_factor)
    {
        capacity = 2 * capacity + 1;
        rehash(capacity);
    }

    size_t hash_val = hash_func(key);
    size++;

    if (hash_set[hash_val] == nullptr)
    {
        // if node with corresponding hash_val doesn't exist,
        // create new node, connect it to the main linked list and
        // put it's pointer to the hash_set.
        auto node = make_shared<ListNode>(key, hash_val);
        bucket_count_val++;
        if (size == 1)
        {
            hash_set[hash_val] = node;
            m_begin = node;
            m_rbegin = node;

            create_end_fake_node();
        }
        else
        {
            hash_set[hash_val] = node;
            m_rbegin->next = node;
            node->prev = m_rbegin;
            m_rbegin = node;
            node->next = m_end;
        }

        return node->data_pair.second;
    }
    else
    {
        // if node with corresponding hash_val exists,
        // just change value of ListNode with corresponding key.
        shared_ptr<ListNode> it = hash_set[hash_val];
        shared_ptr<ListNode> prev_it;
        while (it != m_end && it->hash_val == hash_val)
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
        auto node = make_shared<ListNode>(key, hash_val);
        prev_it->next = node;
        node->prev = prev_it;
        if (it != m_end)
        {
            node->next = it;
            it->prev = node;
        }
        else
        {
            node->next = m_end;
        }

        return node->data_pair.second;
    }
}

template<class Key, class T, class Hash>
void myUnorderedMap<Key, T, Hash>::print()
{
    if (m_begin == nullptr) cout << "Called map is empty!" << endl;
    shared_ptr<ListNode> it = m_begin;
    while (it != m_end)
    {
        cout << it->data_pair.second << " ";
        it = it->next;
    }
    cout << endl;
}

template<class Key, class T, class Hash>
size_t myUnorderedMap<Key, T, Hash>::Size()
{
    return size;
}

template<class Key, class T, class Hash>
bool myUnorderedMap<Key, T, Hash>::isEmpty()
{
    bool isEmpty = 1;
    for (const auto& pListNode : hash_set)
    {
        if (pListNode != nullptr)
        {
            isEmpty = 0;
            return isEmpty;
        }
    }
    return isEmpty;
}

template<class Key, class T, class Hash>
pair<const Key, T>* myUnorderedMap<Key, T, Hash>::brute_force_find(const Key& key)
{
    if (m_begin == nullptr) cout << "Find error! Map is empty!" << endl;
    auto it = m_begin;
    Key target = it->data_pair.first;
    while (target != key)
    {
        it = it->next;
        if (it == m_end)
        {
            cout << "Element with key " << key
                << " wasn't found." << "\n";
            return nullptr;
        }
        target = it->data_pair.first;
    }
    return &(it->data_pair);
}

template<class Key, class T, class Hash>
pair<const Key, T>* myUnorderedMap<Key, T, Hash>::find(const Key& key)
{
    // if hash_func - random, this 'find' won't work!
    // TODO:
    // how to prevent implicit convertion, if I write
    // find_by_hash(int_value) ?
    if (m_begin == nullptr) cout << "Find error! Map is empty!" << endl;
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
            if (it == m_end)
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

template<class Key, class T, class Hash>
size_t myUnorderedMap<Key, T, Hash>::max_bucket_count()
{
    return capacity;
}

template<class Key, class T, class Hash>
size_t myUnorderedMap<Key, T, Hash>::bucket_count()
{
    return bucket_count_val;
}

template<class Key, class T, class Hash>
float myUnorderedMap<Key, T, Hash>::load_factor()
{
    return static_cast<float>(Size()) / bucket_count();
}

template<class Key, class T, class Hash>
void myUnorderedMap<Key, T, Hash>::rehash(size_t new_capacity)
{
    hash_set.clear();
    hash_set.resize(new_capacity, nullptr);

    capacity = new_capacity;
    bucket_count_val = 0;

    // let the "begin node" to be the first one in the expanded hash_set
    size_t curr_hash = hash_func(m_begin->data_pair.first);
    hash_set[curr_hash] = m_begin;
    auto it = m_begin->next;
    auto next_it = it->next;
    //m_begin->next = nullptr;
    m_begin->hash_val = curr_hash;
    m_rbegin = m_begin;

    // loop for other nodes
    while (it != m_end)
    {
        curr_hash = hash_func(it->data_pair.first);
        it->hash_val = curr_hash;
        if (hash_set[curr_hash] == nullptr)
        {
            bucket_count_val++;
            hash_set[curr_hash] = it;
            m_rbegin->next = it;
            it->prev = m_rbegin;
            it->next = m_end;
            m_rbegin = it;
        }
        else
        {
            auto bucket_it = hash_set[curr_hash];
            auto bucket_it_next = bucket_it->next;
            while (bucket_it_next != m_end && bucket_it_next->hash_val == curr_hash)
            {
                bucket_it = bucket_it_next;
                bucket_it_next = bucket_it_next->next;
            }
            bucket_it->next = it;
            it->prev = bucket_it;
            it->next = bucket_it_next;
            if (bucket_it_next != m_end)
            {
                bucket_it_next->prev = it;
            }
            else
            {
                //assert(m_rbegin->next == it);
                m_rbegin = it;
            }  
        }

        it = next_it;
        if (it != m_end) next_it = next_it->next;
    }
}

template<class Key, class T, class Hash>
typename myUnorderedMap<Key, T, Hash>::iterator myUnorderedMap<Key, T, Hash>::begin()
{
    return iterator(m_begin.get());
}

template<class Key, class T, class Hash>
typename myUnorderedMap<Key, T, Hash>::const_iterator myUnorderedMap<Key, T, Hash>::begin() const
{
    return const_iterator(m_begin.get());
}

template<class Key, class T, class Hash>
typename myUnorderedMap<Key, T, Hash>::iterator myUnorderedMap<Key, T, Hash>::end()
{
    return iterator(m_end.get());
}

template<class Key, class T, class Hash>
typename myUnorderedMap<Key, T, Hash>::const_iterator myUnorderedMap<Key, T, Hash>::end() const
{
    return const_iterator(m_end.get());
}

template<class Key, class T, class Hash>
size_t myUnorderedMap<Key, T, Hash>::hash_func(const Key& key)
{
    /*size_t hash_val = static_cast<int>(key) % capacity;*/
    size_t hash_val = Hash{}(key) % capacity;
    return hash_val;
}

template<class Key, class T, class Hash>
void myUnorderedMap<Key, T, Hash>::copy_handler(const myUnorderedMap& other_map)
{
    hash_set = other_map.hash_set;
    size = other_map.size;
    capacity = other_map.capacity;
    max_load_factor = other_map.max_load_factor;
    bucket_count_val = other_map.bucket_count_val;
    auto new_node = make_shared<ListNode>(*other_map.m_begin);
    m_begin = new_node;
    hash_set[new_node->hash_val] = new_node;

    auto prev_it = new_node;
    auto it = new_node->next;
    // Complexity: O(n)
    while (it != other_map.m_end)
    {
        new_node = make_shared<ListNode>(*it);
        prev_it->next = new_node;
        new_node->prev = prev_it;
        if (new_node->hash_val != prev_it->hash_val)
        {
            hash_set[new_node->hash_val] = new_node;
        }
        it = it->next;
        prev_it = prev_it->next;
    }
    m_rbegin = prev_it;
    create_end_fake_node();
    m_rbegin->next = m_end;
}

template<class Key, class T, class Hash>
void myUnorderedMap<Key, T, Hash>::move_handler(myUnorderedMap&& other_map)
{
    nodes_unbinding();
    hash_set = move(other_map.hash_set);
    size = exchange(other_map.size, 0);
    capacity = exchange(other_map.capacity, 0);
    max_load_factor = exchange(other_map.max_load_factor, 0);
    bucket_count_val = exchange(other_map.bucket_count_val, 0);
    m_begin = exchange(other_map.m_begin, nullptr);
    m_end = exchange(other_map.m_end, nullptr);
    m_rbegin = exchange(other_map.m_rbegin, nullptr);
    m_rend = exchange(other_map.m_rend, nullptr);
}

template<class Key, class T, class Hash>
void myUnorderedMap<Key, T, Hash>::create_end_fake_node()
{
    auto fake_shared_ptr = make_shared<ListNode>();
    m_end = fake_shared_ptr;
    m_rend = fake_shared_ptr;
    m_rbegin->next = m_end;
    m_begin->prev = m_rend;
}

template<class Key, class T, class Hash>
void myUnorderedMap<Key, T, Hash>::nodes_unbinding()
{
    // ListNode nodes unbinding, before ~Destructor and MoveCtor
    // to prevent memory leaks due to nodes looped binding. 
    auto prev_it = m_begin;
    auto it = prev_it->next;
    m_begin = nullptr;
    m_rbegin = nullptr;
    m_rend = nullptr;
    while (it != m_end)
    {
        prev_it->prev = nullptr;
        prev_it->next = nullptr;
        prev_it = it;
        it = it->next;
    }
    prev_it->prev = nullptr;
    prev_it->next = nullptr;
}