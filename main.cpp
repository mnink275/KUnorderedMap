#include "myUnorderedMap.h"
#include <chrono>
#include <string>

using namespace std::chrono;

struct KekStruct
{
    KekStruct() : ival(0), dval(0), sval("") {}
    KekStruct(int _ival, double _dval, string _sval)
    {
        ival = _ival;
        dval = _dval;
        sval = _sval;
    }
    KekStruct(KekStruct&& kek) noexcept : ival(kek.ival), dval(kek.dval), sval(kek.sval)
    {
        cout << "KekStruct MOVE\n";
    }
    KekStruct& operator=(KekStruct&& kek) noexcept
    {
        cout << "KekStruct MOVE operator=\n";
        ival = move(kek.ival);
        dval = move(kek.dval);
        sval = move(kek.sval);
        return *this;
    }
    KekStruct(const KekStruct& kek) : ival(kek.ival), dval(kek.dval), sval(kek.sval)
    {
        cout << "KekStruct COPY\n";
    }
    KekStruct& operator=(const KekStruct& kek)
    {
        cout << "KekStruct COPY operator=\n";
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
    string sval;
};

struct KekHash
{
    size_t operator()(const KekStruct& struct_type) const
    {
        return hash<int>{}(struct_type.ival);
    }
};


int main()
{
    MyUnorderedMap<int, int> intMap;
    MyUnorderedMap<double, double> doubleMap;
    MyUnorderedMap<string, string> stringMap;

    // operator[]
    for (int i = 0; i < 700; i++)
    {
        //cout << "DEBIG" + to_string(i) << "\n";
        intMap[i] = i + 100;
        //cout << intMap[i] << " " << intMap.loadFactor() << " " << intMap.Size() << " " << intMap.bucket_count() << "\n";
        doubleMap[i + 0.5] = i + 100.5;
        stringMap["key" + to_string(i)] = "value" + to_string(i);
    }

    // find()
    assert(intMap.find(10000) == intMap.end());
    assert(intMap.find(555)->second == 655);
    assert(doubleMap.find(13.5)->second == 113.5);
    assert(stringMap.find("key600")->second == "value600");

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
    cout << "max_bucket_count: " << doubleMap.max_bucket_count() << "\n";
    cout << "bucket_count: " << doubleMap.bucket_count() << "\n";
    cout << "load_factor: " << doubleMap.loadFactor() << "\n";
    assert(doubleMap.maxLoadFactor() == 2.0f);
    doubleMap.maxLoadFactor(5.0f);
    assert(doubleMap.maxLoadFactor() == 5.0f);


    // iterator and const_iterator
    MyUnorderedMap<int, int> iterator_test_map;
    for (int i = 0; i < 10; i++)
    {
        iterator_test_map[i] = i + 50;
    }
    MyUnorderedMap<int, int> ::iterator it = iterator_test_map.begin();
    for (size_t i = 0; i < 3; i++) ++it;
    cout << "it_to_begin++ 3 times: " << it->second << "\n";

    for (size_t i = 0; i < 3; i++) --it;
    cout << "it_to_begin-- 3 times: " << it->second << "\n";

    for (auto& pr : iterator_test_map)
    {
        cout << pr.second << " ";
    }
    cout << "\n";
    auto const_it = iterator_test_map.cbegin();
    for (const auto& pr : iterator_test_map)
    {
        cout << pr.second << " ";
    }

    // performance test
    // with the current rehash implementation, the write speed
    // of 7000 elements has increased by about 2 times
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
    cout << "Time taken = " << (end - start) << " microseconds\n";
    cout << PerformanceTest.loadFactor() << "\n";

    // custom data
    MyUnorderedMap<KekStruct, int, KekHash> CustomStructMap1;
    MyUnorderedMap<int, KekStruct> CustomStructMap2;
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + to_string(i));
        CustomStructMap1[kek] = i;
    }
    cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + to_string(i));
        CustomStructMap1[move(kek)] = i;
    }
    cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + to_string(i));
        CustomStructMap2[i] = kek;
    }
    cout << "\n";
    for (int i = 0; i < 3; i++)
    {
        KekStruct kek(i, i + 0.5, "GLaDOS" + to_string(i));
        CustomStructMap2[i] = move(kek);
    }

    return 0;
}
