#include <iostream>
#include "myUnorderedMap.h"
#include <chrono>


int main()
{
    myUnorderedMap<double, double> map;

    using namespace std::chrono;
    time_point<high_resolution_clock> start_point, end_point;
    start_point = high_resolution_clock::now();

    for (int i = 0; i < 1000; i++)
    {
        map[i] = i;
    }
    
    end_point = high_resolution_clock::now();
    auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
    auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
    cout << "Time taken = " << (end - start) << " microseconds" << endl;

    //map.print();


   
    return 0;
}
