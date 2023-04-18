#include "KUnorderedMap.h"
#include <string>




int main() {

    // iterator and const_iterator
    ink::KUnorderedMap<int, int> iterator_test_map;
    for (int i = 0; i < 10; i++) {
        iterator_test_map[i] = i + 50;
    }
    ink::KUnorderedMap<int, int>::iterator it = iterator_test_map.begin();
    for (size_t i = 0; i < 3; i++) {
        ++it;
    }
    assert(it->second == 53);

    for (size_t i = 0; i < 3; i++) {
        --it;
    }
    assert(it->second == 50);

    for (const auto& pr : iterator_test_map) {
        std::cout << pr.second << " ";
    }
    std::cout << "\n";
    auto const_it = iterator_test_map.cbegin();
    for (const auto& pr : iterator_test_map) {
        std::cout << pr.second << " ";
    }
    std::cout << "\n";

    // performance test
    // with the current rehash implementation, the write speed
    // of 7000 elements has increased by about 2 times
    // (compared to assignment without rehash() calls)
    ink::KUnorderedMap<int, int> PerformanceTest;

    for (int i = 0; i < 7000; i++) {
        PerformanceTest[i] = i + 100;
    }

    //
    //// custom data
    //ink::KUnorderedMap<KekStruct, int, KekHash> CustomStructMap1;
    //ink::KUnorderedMap<int, KekStruct> CustomStructMap2;
    //for (int i = 0; i < 3; i++) {
    //    KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
    //    CustomStructMap1[kek] = i;
    //}
    //std::cout << "\n";
    //for (int i = 0; i < 3; i++) {
    //    KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
    //    CustomStructMap1[std::move(kek)] = i;
    //}
    //std::cout << "\n";
    //for (int i = 0; i < 3; i++) {
    //    KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
    //    CustomStructMap2[i] = kek;
    //}
    //std::cout << "\n";
    //for (int i = 0; i < 3; i++) {
    //    KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
    //    CustomStructMap2[i] = std::move(kek);
    //}

    return 0;
}
