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
        intMap[i] = i + 100;
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


    // performance test
    /*time_point<high_resolution_clock> start_point, end_point;
    start_point = high_resolution_clock::now();


    end_point = high_resolution_clock::now();
    auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
    auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
    cout << "Time taken = " << (end - start) << " microseconds" << endl;*/

    return 0;
}
