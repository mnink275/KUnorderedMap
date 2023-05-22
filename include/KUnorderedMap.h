#pragma once

#include <cassert>
#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <utility>
#include "ListNode.h"
#include "Iterator.h"

namespace ink {

    template<class Key, class T, class Hash = std::hash<Key>>
    class KUnorderedMap {
    private:
        using ListNode = ListNodeStructer<Key, T>;

    public:
        using iterator = CommonIterator<Key, T, false, Hash>;
        using const_iterator = CommonIterator<Key, T, true, Hash>;

        template<class InputIt>
        using iterator_category_t = typename std::iterator_traits<InputIt>::iterator_category;

    public:
        KUnorderedMap();
        ~KUnorderedMap();

        KUnorderedMap(const KUnorderedMap& other_map);
        KUnorderedMap<Key, T>& operator=(const KUnorderedMap& other_map);

        KUnorderedMap(KUnorderedMap&& other_map) noexcept;
        KUnorderedMap<Key, T>& operator=(KUnorderedMap&& other_map) noexcept;

        KUnorderedMap(std::initializer_list<std::pair<const Key, T>> init_list);
        template<class InputIt>
        KUnorderedMap(InputIt left, InputIt last);

        // Capacity:
        size_t size() const; // O(1)
        bool isEmpty() const; // O(1)
        //max_size() const;

        // Modifiers
        void clear();
        iterator erase(const_iterator pos);
        size_t erase(const Key& key);

        // Lookup:
        std::pair<const Key, T>* bruteForceFind(const Key& key);
        iterator find(const Key& key);
        const_iterator find(const Key& key) const;
        T& operator[](const Key& key); // Avg: O(1), Worst: O(n)
        T& operator[](Key&& key);
        void print() const; // O(n)

        // Bucket interface:
        size_t usedBucketCount() const; // O(1)
        size_t totalBucketCount() const; // O(1)

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
        const_iterator cbegin() const; // O(1)
        const_iterator cend() const; // O(1)

    private:
        size_t hash_func(const Key& key) const;
        void copy_handler(const KUnorderedMap& other_map);
        void move_handler(KUnorderedMap&& other_map);
        void create_end_fake_node();
        void nodes_unbinding();
        template<class CommonIt>
        CommonIt find_helper(const Key& key) const;
        template <class SomeKey>
        iterator emplace(SomeKey&& key);

    private:
        std::vector<std::shared_ptr<ListNode>> hash_set;
        size_t m_size;
        size_t m_capacity;
        // Unjustified usage of shared_ptr? It seems to me,
        // that raw pointers are better (lower memory costs and maybe performance).
        std::shared_ptr<ListNode> m_begin;
        std::shared_ptr<ListNode> m_end;
        std::shared_ptr<ListNode> m_rbegin;
        std::shared_ptr<ListNode> m_rend;
        float m_max_load_factor;
        size_t m_used_bucket_count;
        bool m_is_empty;
    };

    template<class Key, class T, class Hash>
    KUnorderedMap<Key, T, Hash>::KUnorderedMap() :
        m_capacity(239), m_size(0), m_begin(nullptr), m_end(nullptr),
        m_rbegin(nullptr), m_rend(nullptr), m_max_load_factor(2.0f),
        m_used_bucket_count(0), m_is_empty(true) {
        //cout << "Constructor!" << "\n";
        hash_set.resize(m_capacity, nullptr);
    }

    template<class Key, class T, class Hash>
    KUnorderedMap<Key, T, Hash>::~KUnorderedMap() {
        if (m_size > 0) {
            // Elimination of loop pointers (unbinding of each node)
            nodes_unbinding();
        }
    }

    template<class Key, class T, class Hash>
    KUnorderedMap<Key, T, Hash>::KUnorderedMap(const KUnorderedMap& other_map) {
        if (this == &other_map) return;

        copy_handler(other_map);
    }

    template<class Key, class T, class Hash>
    KUnorderedMap<Key, T>& KUnorderedMap<Key, T, Hash>::operator=(
        const KUnorderedMap& other_map) {

        if (this == &other_map) return *this;

        copy_handler(other_map);

        return *this;
    }

    template<class Key, class T, class Hash>
    KUnorderedMap<Key, T, Hash>::KUnorderedMap(
        KUnorderedMap&& other_map) noexcept {
        if (this == &other_map) return;

        move_handler(move(other_map));
    }

    template<class Key, class T, class Hash>
    KUnorderedMap<Key, T>& KUnorderedMap<Key, T, Hash>::operator=(
        KUnorderedMap&& other_map) noexcept {

        if (this == &other_map) return *this;

        move_handler(std::move(other_map));

        return *this;
    }

    template<class Key, class T, class Hash>
    KUnorderedMap<Key, T, Hash>::KUnorderedMap(
        std::initializer_list<std::pair<const Key, T>> init_list) {
        // move through list
        // ???
        throw std::runtime_error("Not implemented");
    }

    template<class Key, class T, class Hash>
    template<class InputIt>
    KUnorderedMap<Key, T, Hash>::KUnorderedMap(InputIt left, InputIt last) : KUnorderedMap() {
        for (; left != last; ++left) {
            emplace(left->first)->second = left->second;
        }
    }

    template<class Key, class T, class Hash>
    T& KUnorderedMap<Key, T, Hash>::operator[](const Key& key) {
        return emplace(key)->second;
    }

    template<class Key, class T, class Hash>
    T& KUnorderedMap<Key, T, Hash>::operator[](Key&& key) {
        return emplace(std::move(key))->second;
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::print() const {
        if (m_begin == nullptr) std::cout << "Called map is empty!" << "\n";

        std::shared_ptr<ListNode> it = m_begin;
        while (it != m_end) {
            std::cout << it->data_pair.second << " ";
            it = it->next;
        }
        std::cout << "\n";
    }

    template<class Key, class T, class Hash>
    size_t KUnorderedMap<Key, T, Hash>::size() const {
        return m_size;
    }

    template<class Key, class T, class Hash>
    bool KUnorderedMap<Key, T, Hash>::isEmpty() const {
        return m_is_empty;
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::clear() {
        if (m_size == 0) return;

        nodes_unbinding(); // O(n)
        m_end = nullptr;
        m_is_empty = true;
        m_size = 0;
        m_used_bucket_count = 0;
        fill(hash_set.begin(), hash_set.end(), nullptr); // O(n)
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::iterator KUnorderedMap<Key, T, Hash>::erase(const const_iterator pos) {
        if (pos == cend()) throw std::runtime_error("Read access violation");

        const auto& pos_ptr = pos.ptr;
        size_t hash = pos_ptr->hash_val;
        if (hash_set[hash] != nullptr) {
            if (pos_ptr->next->hash_val == hash) hash_set[hash] = pos_ptr->next;

            hash_set[hash] = nullptr;
        }

        auto prev = pos_ptr->prev;
        auto next = pos_ptr->next;
        if (prev != m_end) prev->next = prev->next;
        if (next != m_end) next->prev = pos_ptr->prev;

        return iterator(next.get());
    }

    template<class Key, class T, class Hash>
    size_t KUnorderedMap<Key, T, Hash>::erase(const Key& key) {
        auto target_it = find(key);
        if (target_it == end()) {
            return 0;
        }
        else {
            erase(target_it);
            return 1;
        }
    }

    template<class Key, class T, class Hash>
    std::pair<const Key, T>* KUnorderedMap<Key, T, Hash>
        ::bruteForceFind(const Key& key) {
        if (m_begin == nullptr) std::cout << "Find error! Map is empty!" << "\n";
        auto it = m_begin;
        Key target = it->data_pair.first;
        while (target != key) {
            it = it->next;
            if (it == m_end) {
                std::cout << "Element with key " << key
                    << " wasn't found." << "\n";
                return nullptr;
            }
            target = it->data_pair.first;
        }

        return &(it->data_pair);
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::iterator KUnorderedMap<Key, T, Hash>::find(const Key& key) {
        return find_helper<iterator>(key);
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::const_iterator KUnorderedMap<Key, T, Hash>::find(const Key& key) const {
        return find_helper<const_iterator>(key);
    }

    template<class Key, class T, class Hash>
    size_t KUnorderedMap<Key, T, Hash>::totalBucketCount() const {
        return m_capacity;
    }

    template<class Key, class T, class Hash>
    size_t KUnorderedMap<Key, T, Hash>::usedBucketCount() const {
        return m_used_bucket_count;
    }

    template<class Key, class T, class Hash>
    float KUnorderedMap<Key, T, Hash>::loadFactor() const {
        return static_cast<float>(size()) / usedBucketCount();
    }

    template<class Key, class T, class Hash>
    float KUnorderedMap<Key, T, Hash>::maxLoadFactor() const {
        return m_max_load_factor;
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::maxLoadFactor(float new_max_load_factor) {
        m_max_load_factor = new_max_load_factor;
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::rehash(size_t new_capacity) {
        hash_set.clear(); // O(n)
        hash_set.resize(new_capacity, nullptr); // O(new_capacity - old_capacity)

        m_capacity = new_capacity;
        m_used_bucket_count = 0;

        // let the "begin node" to be the first one in the expanded hash_set
        size_t curr_hash = hash_func(m_begin->data_pair.first); // O(1) ??
        hash_set[curr_hash] = m_begin;
        auto it = m_begin->next;
        auto next_it = it->next;
        m_begin->hash_val = curr_hash;
        m_rbegin = m_begin;

        // loop for the rest nodes
        while (it != m_end) {
            curr_hash = hash_func(it->data_pair.first);
            it->hash_val = curr_hash;
            if (hash_set[curr_hash] == nullptr) {
                m_used_bucket_count++;
                hash_set[curr_hash] = it;
                m_rbegin->next = it;
                it->prev = m_rbegin;
                it->next = m_end;
                m_rbegin = it;
            }
            else {
                auto bucket_it = hash_set[curr_hash];
                auto bucket_it_next = bucket_it->next;
                while (bucket_it_next != m_end &&
                    bucket_it_next->hash_val == curr_hash) {
                    bucket_it = bucket_it_next;
                    bucket_it_next = bucket_it_next->next;
                }
                bucket_it->next = it;
                it->prev = bucket_it;
                it->next = bucket_it_next;
                if (bucket_it_next != m_end) {
                    bucket_it_next->prev = it;
                }
                else {
                    m_rbegin = it;
                }
            }

            it = next_it;
            if (it != m_end) next_it = next_it->next;
        }
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::reserve(size_t count) {
        rehash(std::ceil(count / m_max_load_factor));
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::iterator KUnorderedMap<Key, T, Hash>::begin() {
        return iterator(m_begin.get());
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::const_iterator KUnorderedMap<Key, T, Hash>::begin() const {
        return const_iterator(m_begin.get());
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::iterator KUnorderedMap<Key, T, Hash>::end() {
        return iterator(m_end.get());
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::const_iterator KUnorderedMap<Key, T, Hash>::end() const {
        return const_iterator(m_end.get());
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::const_iterator KUnorderedMap<Key, T, Hash>::cbegin() const {
        return const_iterator(m_begin.get());
    }

    template<class Key, class T, class Hash>
    typename KUnorderedMap<Key, T, Hash>
        ::const_iterator KUnorderedMap<Key, T, Hash>::cend() const {
        return const_iterator(m_end.get());
    }

    template<class Key, class T, class Hash>
    size_t KUnorderedMap<Key, T, Hash>::hash_func(const Key& key) const {
        size_t hash_val = Hash{}(key) % m_capacity;
        return hash_val;
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::copy_handler(
        const KUnorderedMap& other_map) {

        hash_set = other_map.hash_set;
        m_size = other_map.m_size;
        m_capacity = other_map.m_capacity;
        m_is_empty = other_map.m_is_empty;
        m_max_load_factor = other_map.m_max_load_factor;
        m_used_bucket_count = other_map.m_used_bucket_count;
        auto new_node = std::make_shared<ListNode>(*other_map.m_begin);
        m_begin = new_node;
        hash_set[new_node->hash_val] = new_node;

        auto prev_it = new_node;
        auto it = new_node->next;
        while (it != other_map.m_end) {
            new_node = std::make_shared<ListNode>(*it);
            prev_it->next = new_node;
            new_node->prev = prev_it;
            if (new_node->hash_val != prev_it->hash_val) {
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
    void KUnorderedMap<Key, T, Hash>::move_handler(KUnorderedMap&& other_map) {
        if (size() != 0) {
            nodes_unbinding();
        }
        hash_set = std::move(other_map.hash_set);
        m_size = std::exchange(other_map.m_size, 0);
        m_capacity = std::exchange(other_map.m_capacity, 0);
        m_is_empty = std::exchange(other_map.m_is_empty, true);
        m_max_load_factor = std::exchange(other_map.m_max_load_factor, 0);
        m_used_bucket_count = std::exchange(other_map.m_used_bucket_count, 0);
        m_begin = std::exchange(other_map.m_begin, nullptr);
        m_end = std::exchange(other_map.m_end, nullptr);
        m_rbegin = std::exchange(other_map.m_rbegin, nullptr);
        m_rend = std::exchange(other_map.m_rend, nullptr);
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::create_end_fake_node() {
        auto fake_shared_ptr = std::make_shared<ListNode>();
        m_end = fake_shared_ptr;
        m_rend = fake_shared_ptr;
        m_rbegin->next = m_end;
        m_begin->prev = m_rend;
    }

    template<class Key, class T, class Hash>
    void KUnorderedMap<Key, T, Hash>::nodes_unbinding() {
        // ListNode nodes unbinding before ~Destructor, MoveCtor and clear()
        // to prevent memory leaks due to nodes looped binding. 
        auto prev_it = m_begin;
        m_begin = nullptr;
        m_rbegin = nullptr;
        m_rend = nullptr;
        auto it = prev_it->next;
        while (it != m_end) {
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
    CommonIt KUnorderedMap<Key, T, Hash>::find_helper(const Key& key) const {
        if (m_begin == nullptr) return CommonIt(m_end.get());

        size_t hash_val = hash_func(key);
        auto it = hash_set[hash_val];
        if (it == nullptr) {
            return CommonIt(m_end.get());
        }
        else {
            // TODO: bad if the Key is a huge class cos
            // copy ctor is invoked here (below)
            Key target = it->data_pair.first;
            while (key != target) {
                it = it->next;
                if (it == m_end) return CommonIt(m_end.get());
                target = it->data_pair.first;
            }
            return CommonIt(it.get());
        }
    }

    template<class Key, class T, class Hash>
    template <class SomeKey>
    typename KUnorderedMap<Key, T, Hash>::iterator KUnorderedMap<Key, T, Hash>::emplace(SomeKey&& key) {
        // check if it's time to rehash
        if (m_size > 0 && loadFactor() > m_max_load_factor) {
            m_capacity = 2 * m_capacity + 1;
            rehash(m_capacity);
        }

        const auto& key_val = key;
        size_t hash_val = hash_func(key_val);
        m_size++;

        if (hash_set[hash_val] == nullptr) {
            // if node with corresponding hash_val doesn't exist,
            // create new node, connect it to the main linked list and
            // put it's pointer to the hash_set.
            auto node = std::make_shared<ListNode>(
                std::make_pair<SomeKey, T>(std::forward<SomeKey>(key), {}),
                hash_val);
            m_used_bucket_count++;
            if (m_size == 1) {
                hash_set[hash_val] = node;
                m_begin = node;
                m_rbegin = node;
                m_is_empty = false;

                create_end_fake_node();
            }
            else {
                hash_set[hash_val] = node;
                m_rbegin->next = node;
                node->prev = m_rbegin;
                m_rbegin = node;
                node->next = m_end;
            }

            return KUnorderedMap<Key, T, Hash>::iterator(node.get());
        }
        else {
            // if node with corresponding hash_val exists,
            // just change value of ListNode with corresponding key.
            std::shared_ptr<ListNode> it = hash_set[hash_val];
            std::shared_ptr<ListNode> prev_it;
            while (it != m_end && it->hash_val == hash_val) {
                if (it->data_pair.first == key) return KUnorderedMap<Key, T, Hash>::iterator(it.get());
                prev_it = it;
                it = it->next;
            }

            // if node with corresponding key doesn't exist,
            // create new node and add it to the ending of 
            // the corresponding bucket.
            auto node = std::make_shared<ListNode>(
                std::make_pair<SomeKey, T>(std::forward<SomeKey>(key), {}),
                hash_val);
            prev_it->next = node;
            node->prev = prev_it;
            if (it != m_end) {
                node->next = it;
                it->prev = node;
            }
            else {
                node->next = m_end;
            }

            return KUnorderedMap<Key, T, Hash>::iterator(node.get());
        }
    }

}  // ink namespace