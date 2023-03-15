#include <vector>
#include <memory>

// bidirectional list
template<class Key, class T>
struct ListNodeStructer {
    std::pair<const Key, T> data_pair;
    size_t hash_val;
    std::shared_ptr<ListNodeStructer> prev;
    std::shared_ptr<ListNodeStructer> next;

    ListNodeStructer() {}
    explicit ListNodeStructer(ListNodeStructer& _node)
        : data_pair(_node.data_pair.first, _node.data_pair.second),
        hash_val(_node.hash_val), prev(_node.prev), next(_node.next) {};
    ListNodeStructer(std::pair<Key, T>&& pr, size_t _hash_val)
        : data_pair(std::move(pr)),
        hash_val(_hash_val), prev(nullptr), next(nullptr) {}
};