#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include "ListNode.h"

// Forward declaration
template<class Key, class T, class Hash>
class KUnorderedMap;

// UnorderedMap's iterator
template<class Key, class T, bool IsConst, class Hash>
class CommonIterator {
public:
    using ListNode = ListNodeStructer<Key, T>;

    // iterator tags
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = ListNode;
    using pointer = ListNode*;
    using reference = ListNode&;

public:
    friend class CommonIterator<Key, T, true, Hash>;
    friend class KUnorderedMap<Key, T, Hash>;

    explicit CommonIterator(pointer ptr_) : ptr(ptr_) {};

    CommonIterator(const CommonIterator<Key, T, false, Hash>& non_const_it)
        : ptr(non_const_it.ptr) {}

    std::conditional_t<IsConst, const std::pair<const Key, T>&,
        std::pair<const Key, T>&> operator*() {
        return ptr->data_pair;
    }

    CommonIterator& operator++() {
        ptr = (ptr->next).get();
        return *this;
    }

    CommonIterator& operator--() {
        ptr = (ptr->prev).get();
        return *this;
    }

    std::conditional_t<IsConst, const std::pair<const Key, T>*,
        std::pair<const Key, T>*> operator->() {
        return &(ptr->data_pair);
    }

    friend bool operator!=(const CommonIterator& left, const CommonIterator& right) {
        return left.ptr != right.ptr;
    }

    friend bool operator==(const CommonIterator& left, const CommonIterator& right) {
        return left.ptr == right.ptr;
    }

private:
    std::conditional_t<IsConst, const pointer, pointer> ptr;
};