#include <iostream>
#include "myUnorderedMap.h"


int main()
{
    myUnorderedMap<int, int> map;
    map.print();
    map[2] = 3;
    map[12] = 4;
    map.print();

    return 0;
}
