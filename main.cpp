#include <iostream>
#include <type_traits>
#include <vector>
#include <unordered_map>


int main() {
    std::cout << "Empty main()\n";
    struct NotIterator {
        int first{ 0 };
        int second{ 0 };
    };
    NotIterator not_iterator;
    auto* ptr = &not_iterator;
    std::unordered_map<int, int> m(ptr, ptr);
    std::unordered_map<int, int> init{ {1, 10} , {2, 20}, {3, 30} };

    return 0;
}
