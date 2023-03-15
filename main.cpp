#include "myUnorderedMap.h"
#include <chrono>
#include <string>

using namespace std::chrono;

struct KekStruct
{
    KekStruct() : ival(0), dval(0), sval("") {}
    KekStruct(int _ival, double _dval, const std::string& _sval)
        : ival(_ival), dval(_dval), sval(_sval) {}
    KekStruct(KekStruct&& kek) noexcept : ival(kek.ival), dval(kek.dval), sval(kek.sval)
    {
        std::cout << "KekStruct MOVE\n";
    }
    KekStruct& operator=(KekStruct&& kek) noexcept
    {
        std::cout << "KekStruct MOVE operator=\n";
        ival = std::move(kek.ival);
        dval = std::move(kek.dval);
        sval = std::move(kek.sval);
        return *this;
    }
    KekStruct(const KekStruct& kek) : ival(kek.ival), dval(kek.dval), sval(kek.sval)
    {
        std::cout << "KekStruct COPY\n";
    }
    KekStruct& operator=(const KekStruct& kek)
    {
        std::cout << "KekStruct COPY operator=\n";
        ival = kek.ival;
        dval = kek.dval;
        sval = kek.sval;
        return *this;
    }
    
    friend bool operator==(const KekStruct& left, const KekStruct& right)
    {
        return (left.ival == right.ival)
            || (left.dval == right.dval)
            || (left.sval == right.sval);
    }

    int ival;
    double dval;
    std::string sval;
};

struct KekHash
{
    size_t operator()(const KekStruct& struct_type) const
    {
        return std::hash<int>{}(struct_type.ival);
    }
};


int main()
{
    MyUnorderedMap<int, int> intMap;
    MyUnorderedMap<double, double> doubleMap;
    MyUnorderedMap<std::string, std::string> stringMap;

    // operator[]
    for (int i = 0; i < 700; i++)
    {
        //cout << "DEBIG" + to_string(i) << "\n";
        intMap[i] = i + 100;
        //cout << intMap[i] << " " << intMap.loadFactor() << " " << intMap.Size() << " " << intMap.bucket_count() << "\n";
        doubleMap[i + 0.5] = i + 100.5;
        stringMap["key" + std::to_string(i)] = "value" + std::to_string(i);
    }

    // find()
    assert(intMap.find(10000) == intMap.end());
    assert(intMap.find(555)->second == 655);
    assert(doubleMap.find(13.5)->second == 113.5);
    assert(stringMap.find("key600")->second == "value600");

    // erase()
    auto ptr = doubleMap.find(13.5);
    doubleMap.erase(ptr);
    assert(doubleMap.find(13.5) == doubleMap.end());
    assert(doubleMap.erase(13.5) == 0);
    assert(doubleMap.erase(14.5) == 1);

    // brute_force_find()
    assert(intMap.brute_force_find(555)->second == 655);
    assert(doubleMap.brute_force_find(13.5)->second == 113.5);
    assert(stringMap.brute_force_find("key600")->second == "value600");

    // isEmpty()
    assert(!intMap.isEmpty());
    MyUnorderedMap<int, int> intMapOther;
    assert(intMapOther.isEmpty());
    
    // print()
    // intMap.print();
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
    std::cout << "total_bucket_count: " << doubleMap.total_bucket_count() << "\n";
    std::cout << "used_bucket_count: " << doubleMap.used_bucket_count() << "\n";
    std::cout << "load_factor: " << doubleMap.loadFactor() << "\n";
    assert(doubleMap.maxLoadFactor() == 2.0f);
    doubleMap.maxLoadFactor(5.0f);
    assert(doubleMap.maxLoadFactor() == 5.0f);
    doubleMap.clear();
    assert(doubleMap.isEmpty());


    // iterator and const_iterator
    MyUnorderedMap<int, int> iterator_test_map;
    for (int i = 0; i < 10; i++)
    {
        iterator_test_map[i] = i + 50;
    }
    MyUnorderedMap<int, int>::iterator it = iterator_test_map.begin();
    for (size_t i = 0; i < 3; i++) ++it;
    assert(it->second == 53);

    for (size_t i = 0; i < 3; i++) --it;
    assert(it->second == 50);

    for (const auto& pr : iterator_test_map)
    {
        std::cout << pr.second << " ";
    }
    std::cout << "\n";
    auto const_it = iterator_test_map.cbegin();
    for (const auto& pr : iterator_test_map)
    {
        std::cout << pr.second << " ";
    }
    std::cout << "\n";

    // performance test
    // with the current rehash implementation, the write speed
    // of 7000 elements has increased by about 2 times
    // (compared to assignment without rehash() calls)
    MyUnorderedMap<int, int> PerformanceTest;

    time_point<high_resolution_clock> start_point, end_point;
    start_point = high_resolution_clock::now();

    for (int i = 0; i < 7000; i++)
    {
        PerformanceTest[i] = i + 100;
    }

    end_point = high_resolution_clock::now();
    auto start = time_point_cast<microseconds>(start_point).time_since_epoch().count();
    auto end = time_point_cast<microseconds>(end_point).time_since_epoch().count();
    std::cout << "Time taken = " << (end - start) << " microseconds\n";
    assert(PerformanceTest.loadFactor() <= 2);

    // custom data
    MyUnorderedMap<KekStruct, int, KekHash> CustomStructMap1;
    MyUnorderedMap<int, KekStruct> CustomStructMap2;
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
        CustomStructMap1[kek] = i;
    }
    std::cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
        CustomStructMap1[std::move(kek)] = i;
    }
    std::cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
        CustomStructMap2[i] = kek;
    }
    std::cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + std::to_string(i));
        CustomStructMap2[i] = std::move(kek);
    }

    return 0;
}
