#include "myUnorderedMap.h"
#include <chrono>




int main()
{
    myUnorderedMap<int, int> map;
    

    using namespace std::chrono;
    

    for (int i = 0; i < 5; i++)
    {
        map[i] = i;
    }

    time_point<high_resolution_clock> start_point, end_point;
    start_point = high_resolution_clock::now();

    myUnorderedMap<int, int> map_reciver;
    map_reciver = map;
    map.print();
    map_reciver.print();
    end_point = high_resolution_clock::now();
    auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
    auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
    cout << "Time taken = " << (end - start) << " microseconds" << endl;
    /*map_reciver.print();
    map.print();*/

    return 0;
}
