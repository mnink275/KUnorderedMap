#pragma once

#include <vector>
#include <memory>

// Bidirectional List
template<class Key, class T>
struct ListNodeStructer {
    std::pair<const Key, T> data_pair;
    size_t hash_val;
    std::shared_ptr<ListNodeStructer> prev;
    std::shared_ptr<ListNodeStructer> next;

    ListNodeStructer() {}
    explicit ListNodeStructer(ListNodeStructer& node_)
        : data_pair(node_.data_pair.first, node_.data_pair.second),
        hash_val(node_.hash_val), prev(node_.prev), next(node_.next) {};
    ListNodeStructer(std::pair<Key, T>&& pr, size_t hash_val_)
        : data_pair(std::move(pr)), hash_val(hash_val_), prev(nullptr),
        next(nullptr) {}
};