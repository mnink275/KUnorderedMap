#include "myUnorderedMap.h"
#include <chrono>
#include <string>


using namespace std::chrono;

int main()
{
    myUnorderedMap<double, double> doubleMap;
    myUnorderedMap<string, string> stringMap;
    myUnorderedMap<int, int> intMap;
    
    for (int i = 0; i < 700; i++)
    {
        intMap[i] = i;
        doubleMap[i + 0.5] = i + 0.5;
        stringMap["key" + to_string(i)] = "value" + to_string(i);
    }





    // performance test
    /*time_point<high_resolution_clock> start_point, end_point;
    start_point = high_resolution_clock::now();


    end_point = high_resolution_clock::now();
    auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
    auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
    cout << "Time taken = " << (end - start) << " microseconds" << endl;*/

    return 0;
}
