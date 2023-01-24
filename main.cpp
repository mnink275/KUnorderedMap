#include "myUnorderedMap.h"
#include <chrono>
#include <string>
#include <cassert>


using namespace std::chrono;

int main()
{
    myUnorderedMap<int, int> intMap;
    myUnorderedMap<double, double> doubleMap;
    myUnorderedMap<string, string> stringMap;

    // operator[]
    for (int i = 0; i < 700; i++)
    {
        //cout << "DEBIG" + to_string(i) << "\n";
        intMap[i] = i + 100;
        //cout << intMap.load_factor() << " " << intMap.Size() << " " << intMap.bucket_count() << "\n";
        doubleMap[i + 0.5] = i + 100.5;
        stringMap["key" + to_string(i)] = "value" + to_string(i);
    }

    // find()
    assert(intMap.find(555)->second == 655);
    assert(doubleMap.find(13.5)->second == 113.5);
    assert(stringMap.find("key600")->second == "value600");

    // brute_force_find()
    assert(intMap.brute_force_find(555)->second == 655);
    assert(doubleMap.brute_force_find(13.5)->second == 113.5);
    assert(stringMap.brute_force_find("key600")->second == "value600");

    // isEmpty()
    assert(!intMap.isEmpty());
    myUnorderedMap<int, int> intMapOther;
    assert(intMapOther.isEmpty());
    
    // print()
    intMap.print();
    /*doubleMap.print();
    stringMap.print();*/

    // copy operation
    intMapOther = intMap;
    assert(!intMap.isEmpty());
    assert(!intMapOther.isEmpty());

    // move operation
    intMapOther = move(intMap);
    assert(intMap.isEmpty());
    assert(!intMapOther.isEmpty());

    // other functions
    cout << "max_bucket_count: " << doubleMap.max_bucket_count() << "\n";
    cout << "bucket_count: " << doubleMap.bucket_count() << "\n";
    cout << "load_factor: " << doubleMap.load_factor() << "\n";

    // iterator
    myUnorderedMap<double, double>::iterator it = doubleMap.begin();
    for (size_t i = 0; i < 3; i++) ++it;
    cout << "it_to_begin++ 3 times: " << it->second << "\n";

    for (size_t i = 0; i < 3; i++) --it;
    cout << "it_to_begin-- 3 times: " << it->second << "\n";

    for (auto& pr : doubleMap)
    {
        cout << pr.second << " ";
    }
    cout << "\n";

    // performance test
    // with the current rehash implementation, the write speed
    // of 7000 elements has increased by about 2 times
    myUnorderedMap<int, int> PerformanceTest;

    time_point<high_resolution_clock> start_point, end_point;
    start_point = high_resolution_clock::now();

    for (int i = 0; i < 7000; i++)
    {
        PerformanceTest[i] = i + 100;
    }

    end_point = high_resolution_clock::now();
    auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
    auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
    cout << "Time taken = " << (end - start) << " microseconds\n";
    cout << PerformanceTest.load_factor() << "\n";

    return 0;
}
