#include "myUnorderedMap.h"
#include <chrono>
#include <string>



using namespace std::chrono;

int main()
{
    myUnorderedMap<double, double> map1;
    myUnorderedMap<string, string> map2;
    // myUnorderedMap<int, int> map1;
    
   
    time_point<high_resolution_clock> start_point, end_point;
    start_point = high_resolution_clock::now();

    /*for (double i = 1.1; i < 50.5; i++)
    {
        map1[i] = i + 100;
    }*/
    for (double i = 1.1; i < 50.5; i++)
    {
        /*map1[i] = i;*/
        map2["string key"] = "string value";
    }

    //myUnorderedMap<double, double> map2 = move(map1);
    //myUnorderedMap<int, int> map2 = map1;

    end_point = high_resolution_clock::now();
    auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
    auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
    cout << "Time taken = " << (end - start) << " microseconds" << endl;

    /*string str = "Hello";
    cout << std::hash<int>{}(5);*/
    
  /*  map1.print();
    auto ptr = map1.find(2.1);
    ptr->value_type.second = 1.1;
    map1.print();
    cout << map1.mapSize();*/
    /*map_reciver.print();
    map.print();*/

    return 0;
}
