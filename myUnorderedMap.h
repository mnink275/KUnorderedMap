#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <utility>

using namespace std;

template<class Key, class T, class Hash = std::hash<Key>>
class MyUnorderedMap
{
private:
    // bidirectional list
    struct ListNode {
        pair<const Key, T> data_pair;
        size_t hash_val;
        shared_ptr<ListNode> prev;
        shared_ptr<ListNode> next;

        ListNode() {} // doesn't have fields initialization, cos it is used
        explicit ListNode(ListNode& _node)
            : data_pair(_node.data_pair.first, _node.data_pair.second),
            hash_val(_node.hash_val), prev(_node.prev), next(_node.next) {};
        ListNode(std::pair<Key, T>&& pr, size_t _hash_val)
            : hash_val(_hash_val), prev(nullptr), next(nullptr),
            data_pair(std::move(pr)) {}
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
    MyUnorderedMap();
    ~MyUnorderedMap();

    MyUnorderedMap(const MyUnorderedMap& other_map);
    MyUnorderedMap<Key, T>& operator=(const MyUnorderedMap& other_map);

    MyUnorderedMap(MyUnorderedMap&& other_map) noexcept;
    MyUnorderedMap<Key, T>& operator=(MyUnorderedMap&& other_map) noexcept;

    MyUnorderedMap(std::initializer_list<std::pair<const Key, T>> init_list);

    // Capacity:
    size_t Size() const; // O(1)
    bool isEmpty() const; // O(1)
    //max_size() const;

    // Lookup:
    pair<const Key, T>* brute_force_find(const Key& key);
    iterator find(const Key& key);
    const_iterator find(const Key& key) const;
    T& operator[](const Key& key); // Avg: O(1), Worst: O(n)
    T& operator[](Key&& key);
    void print() const; // O(n)

    // Bucket interface:
    size_t bucket_count() const; // O(1)
    size_t max_bucket_count() const; // O(1)

    // Hash policy:
    float loadFactor() const; // O(1)
    float maxLoadFactor() const; // O(1)
    void maxLoadFactor(float new_max_load_factor); // O(1)
    void rehash(size_t new_capacity); // Avg: O(n), words: O(n^2)
    void reserve(size_t count); // Avg: O(n), Worst: O(n^2)

    // Iterators:
    iterator begin(); // O(1)
    iterator end(); // O(1)
    const_iterator begin() const; // O(1)
    const_iterator end() const; // O(1)
    // maybe it's possible to merge above and below const functions
    const_iterator cbegin() const; // O(1)
    const_iterator cend() const; // O(1)

private:
    size_t hash_func(const Key& key) const;
    void copy_handler(const MyUnorderedMap& other_map);
    void move_handler(MyUnorderedMap&& other_map);
    void create_end_fake_node();
    void nodes_unbinding();
    template<class CommonIt>
    CommonIt find_helper(const Key& key) const;
    template <class SomeKey>
    T& emplace(SomeKey&& key);

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
    bool is_empty;
};

template<class Key, class T, class Hash>
MyUnorderedMap<Key, T, Hash>::MyUnorderedMap()
    : capacity(239), size(0), m_begin(nullptr), m_end(nullptr), m_rbegin(nullptr),
    m_rend(nullptr), max_load_factor(2.0f), bucket_count_val(0), is_empty(true)
{
    //cout << "Constructor!" << "\n";
    hash_set.resize(capacity, nullptr);
}

template<class Key, class T, class Hash>
MyUnorderedMap<Key, T, Hash>::~MyUnorderedMap()
{
    if (size > 0)
    {
        // Elimination of loop pointers (unbinding of each node)
        nodes_unbinding();
    }
}

template<class Key, class T, class Hash>
MyUnorderedMap<Key, T, Hash>::MyUnorderedMap(const MyUnorderedMap& other_map)
{
    cout << "Copy operator()" << "\n";
    if (this == &other_map) return;

    copy_handler(other_map);
}

template<class Key, class T, class Hash>
MyUnorderedMap<Key, T>& MyUnorderedMap<Key, T, Hash>::operator=(const MyUnorderedMap& other_map)
{
    cout << "Copy operator=" << "\n";

    if (this == &other_map) return *this;

    copy_handler(other_map);

    return *this;
}

template<class Key, class T, class Hash>
MyUnorderedMap<Key, T, Hash>::MyUnorderedMap(MyUnorderedMap&& other_map) noexcept
{
    cout << "Move operator()" << "\n";
    if (this == &other_map) return;

    move_handler(move(other_map));
}

template<class Key, class T, class Hash>
MyUnorderedMap<Key, T>& MyUnorderedMap<Key, T, Hash>::operator=(MyUnorderedMap&& other_map) noexcept
{
    cout << "Move operator=" << "\n";
    if (this == &other_map) return *this;

    move_handler(move(other_map));

    return *this;
}

template<class Key, class T, class Hash>
MyUnorderedMap<Key, T, Hash>
::MyUnorderedMap(std::initializer_list<std::pair<const Key, T>> init_list)
{
    // move through list
    // ???
    throw std::runtime_error("not implemented");
}

template<class Key, class T, class Hash>
T& MyUnorderedMap<Key, T, Hash>::operator[](const Key& key)
{
    return emplace(key);
}

 template<class Key, class T, class Hash>
 T& MyUnorderedMap<Key, T, Hash>::operator[](Key&& key)
 {
     return emplace(move(key));
 }

template<class Key, class T, class Hash>
void MyUnorderedMap<Key, T, Hash>::print() const
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
size_t MyUnorderedMap<Key, T, Hash>::Size() const
{
    return size;
}

template<class Key, class T, class Hash>
bool MyUnorderedMap<Key, T, Hash>::isEmpty() const
{
    return is_empty;
}

template<class Key, class T, class Hash>
pair<const Key, T>* MyUnorderedMap<Key, T, Hash>::brute_force_find(const Key& key)
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
typename MyUnorderedMap<Key, T, Hash>
::iterator MyUnorderedMap<Key, T, Hash>::find(const Key& key)
{
    return find_helper<iterator>(key);
}

template<class Key, class T, class Hash>
typename MyUnorderedMap<Key, T, Hash>
::const_iterator MyUnorderedMap<Key, T, Hash>::find(const Key& key) const
{
    return find_helper<const_iterator>(key);
}

template<class Key, class T, class Hash>
size_t MyUnorderedMap<Key, T, Hash>::max_bucket_count() const
{
    return capacity;
}

template<class Key, class T, class Hash>
size_t MyUnorderedMap<Key, T, Hash>::bucket_count() const
{
    return bucket_count_val;
}

template<class Key, class T, class Hash>
float MyUnorderedMap<Key, T, Hash>::loadFactor() const
{
    return static_cast<float>(Size()) / bucket_count();
}

template<class Key, class T, class Hash>
float MyUnorderedMap<Key, T, Hash>::maxLoadFactor() const
{
    return max_load_factor;
}

template<class Key, class T, class Hash>
void MyUnorderedMap<Key, T, Hash>::maxLoadFactor(float new_max_load_factor)
{
    max_load_factor = new_max_load_factor;
}

template<class Key, class T, class Hash>
void MyUnorderedMap<Key, T, Hash>::rehash(size_t new_capacity)
{
    hash_set.clear(); // O(n)
    hash_set.resize(new_capacity, nullptr); // O(new_capacity - old_capacity)

    capacity = new_capacity;
    bucket_count_val = 0;

    // let the "begin node" to be the first one in the expanded hash_set
    size_t curr_hash = hash_func(m_begin->data_pair.first); // O(1) ??
    hash_set[curr_hash] = m_begin;
    auto it = m_begin->next;
    auto next_it = it->next;
    m_begin->hash_val = curr_hash;
    m_rbegin = m_begin;

    // loop for the rest nodes
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
                m_rbegin = it;
            }
        }

        it = next_it;
        if (it != m_end) next_it = next_it->next;
    }
}

template<class Key, class T, class Hash>
void MyUnorderedMap<Key, T, Hash>::reserve(size_t count)
{
    rehash(std::ceil(count / max_load_factor));
}

template<class Key, class T, class Hash>
typename MyUnorderedMap<Key, T, Hash>
::iterator MyUnorderedMap<Key, T, Hash>::begin()
{
    return iterator(m_begin.get());
}

template<class Key, class T, class Hash>
typename MyUnorderedMap<Key, T, Hash>
::const_iterator MyUnorderedMap<Key, T, Hash>::begin() const
{
    return const_iterator(m_begin.get());
}

template<class Key, class T, class Hash>
typename MyUnorderedMap<Key, T, Hash>
::iterator MyUnorderedMap<Key, T, Hash>::end()
{
    return iterator(m_end.get());
}

template<class Key, class T, class Hash>
typename MyUnorderedMap<Key, T, Hash>
::const_iterator MyUnorderedMap<Key, T, Hash>::end() const
{
    return const_iterator(m_end.get());
}

template<class Key, class T, class Hash>
typename MyUnorderedMap<Key, T, Hash>
::const_iterator MyUnorderedMap<Key, T, Hash>::cbegin() const
{
    return const_iterator(m_begin.get());
}

template<class Key, class T, class Hash>
typename MyUnorderedMap<Key, T, Hash>
::const_iterator MyUnorderedMap<Key, T, Hash>::cend() const
{
    return const_iterator(m_end.get());
}

template<class Key, class T, class Hash>
size_t MyUnorderedMap<Key, T, Hash>::hash_func(const Key& key) const
{
    size_t hash_val = Hash{}(key) % capacity;
    return hash_val;
}

template<class Key, class T, class Hash>
void MyUnorderedMap<Key, T, Hash>::copy_handler(const MyUnorderedMap& other_map)
{
    hash_set = other_map.hash_set;
    size = other_map.size;
    capacity = other_map.capacity;
    is_empty = other_map.is_empty;
    max_load_factor = other_map.max_load_factor;
    bucket_count_val = other_map.bucket_count_val;
    auto new_node = make_shared<ListNode>(*other_map.m_begin);
    m_begin = new_node;
    hash_set[new_node->hash_val] = new_node;

    auto prev_it = new_node;
    auto it = new_node->next;
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
void MyUnorderedMap<Key, T, Hash>::move_handler(MyUnorderedMap&& other_map)
{
    nodes_unbinding();
    hash_set = move(other_map.hash_set);
    size = exchange(other_map.size, 0);
    capacity = exchange(other_map.capacity, 0);
    is_empty = exchange(other_map.is_empty, true);
    max_load_factor = exchange(other_map.max_load_factor, 0);
    bucket_count_val = exchange(other_map.bucket_count_val, 0);
    m_begin = exchange(other_map.m_begin, nullptr);
    m_end = exchange(other_map.m_end, nullptr);
    m_rbegin = exchange(other_map.m_rbegin, nullptr);
    m_rend = exchange(other_map.m_rend, nullptr);
}

template<class Key, class T, class Hash>
void MyUnorderedMap<Key, T, Hash>::create_end_fake_node()
{
    auto fake_shared_ptr = make_shared<ListNode>();
    m_end = fake_shared_ptr;
    m_rend = fake_shared_ptr;
    m_rbegin->next = m_end;
    m_begin->prev = m_rend;
}

template<class Key, class T, class Hash>
void MyUnorderedMap<Key, T, Hash>::nodes_unbinding()
{
    // ListNode nodes unbinding before ~Destructor and MoveCtor
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

template<class Key, class T, class Hash>
template<class CommonIt>
CommonIt MyUnorderedMap<Key, T, Hash>::find_helper(const Key& key) const
{
    if (m_begin == nullptr) return CommonIt(m_end.get());
    size_t hash_val = hash_func(key);
    auto it = hash_set[hash_val];
    if (it == nullptr)
    {
        return CommonIt(m_end.get());
    }
    else
    {
        // TODO: bad if the Key is huge class cos
        // copy ctor is invoked here (below)
        Key target = it->data_pair.first;
        while (key != target)
        {
            it = it->next;
            if (it == m_end)
            {
                return CommonIt(m_end.get());
            }
            target = it->data_pair.first;
        }
        return CommonIt(it.get());
    }
}

template<class Key, class T, class Hash>
template <class SomeKey>
T& MyUnorderedMap<Key, T, Hash>::emplace(SomeKey&& key)
{
    // check if it's time to rehash
    if (size > 0 && loadFactor() > max_load_factor)
    {
        capacity = 2 * capacity + 1;
        rehash(capacity);
    }

    const auto& key_val = key;
    size_t hash_val = hash_func(key_val);
    size++;

    if (hash_set[hash_val] == nullptr)
    {
        // if node with corresponding hash_val doesn't exist,
        // create new node, connect it to the main linked list and
        // put it's pointer to the hash_set.
        auto node = make_shared<ListNode>(
            make_pair<SomeKey, T>(std::forward<SomeKey>(key), {}),
            hash_val);
        bucket_count_val++;
        if (size == 1)
        {
            hash_set[hash_val] = node;
            m_begin = node;
            m_rbegin = node;
            is_empty = false;

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
        auto node = make_shared<ListNode>(
            make_pair<SomeKey, T>(std::forward<SomeKey>(key), {}),
            hash_val);
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