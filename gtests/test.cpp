#include <gtest/gtest.h>
#include <string>

#include "KUnorderedMap.h"

struct KekStruct {
    KekStruct() : ival(0), dval(0), sval("") {}
    KekStruct(int _ival, double _dval, const std::string& _sval)
        : ival(_ival), dval(_dval), sval(_sval) {}
    KekStruct(KekStruct&& other) noexcept :
        ival(other.ival), dval(other.dval), sval(other.sval) {

        std::cout << "KekStruct MOVE\n";
    }
    KekStruct& operator=(KekStruct&& other) noexcept {
        std::cout << "KekStruct MOVE operator=\n";
        ival = std::move(other.ival);
        dval = std::move(other.dval);
        sval = std::move(other.sval);
        return *this;
    }
    KekStruct(const KekStruct& other) :
        ival(other.ival), dval(other.dval), sval(other.sval) {

        std::cout << "KekStruct COPY\n";
    }
    KekStruct& operator=(const KekStruct& other) {
        std::cout << "KekStruct COPY operator=\n";
        ival = other.ival;
        dval = other.dval;
        sval = other.sval;
        return *this;
    }

    friend bool operator==(const KekStruct& lhs, const KekStruct& rhs) {
        return (lhs.ival == rhs.ival)
            || (lhs.dval == rhs.dval)
            || (lhs.sval == rhs.sval);
    }

    int ival;
    double dval;
    std::string sval;
};

struct KekHash {
    size_t operator()(const KekStruct& some_kek_struct) const {
        return std::hash<int>{}(some_kek_struct.ival);
    }
};

ink::KUnorderedMap<int, int> intMap;
ink::KUnorderedMap<double, double> doubleMap;
ink::KUnorderedMap<std::string, std::string> stringMap;

// operator[]
TEST(UMapInitialization, SquareBracketsOperator) {
    for (int i = -700; i < 700; ++i) {
        intMap[i] = i + 100;
        doubleMap[i + 0.5] = i + 100.5;
        stringMap["key: " + std::to_string(i)] = "value: " + std::to_string(i);
    }
}

// find()
TEST(UMapInitialization, Finding) {
    EXPECT_EQ(intMap.find(555)->second, 655);
    EXPECT_EQ(intMap.find(10000), intMap.end());

    EXPECT_EQ(doubleMap.find(13.5)->second, 113.5);
    EXPECT_EQ(doubleMap.find(-10000.2523), doubleMap.end());

    EXPECT_EQ(stringMap.find("key: 600")->second, "value: 600");
    EXPECT_EQ(stringMap.find("RandomKey"), stringMap.end());
}

// erase()
TEST(UMapInitialization, Erasing) {
    EXPECT_EQ(doubleMap.find(13.5)->second, 113.5);
    auto iterator = doubleMap.find(13.5);
    doubleMap.erase(iterator);
    EXPECT_EQ(doubleMap.find(13.5), doubleMap.end());
    EXPECT_EQ(doubleMap.erase(13.5), 0);
    EXPECT_EQ(doubleMap.erase(14.5), 1);
}

// isEmpty()
TEST(UMapInitialization, isEmpty) {
    EXPECT_TRUE(!intMap.isEmpty());
    intMap.clear();
    EXPECT_TRUE(intMap.isEmpty());
    EXPECT_EQ(intMap.size(), 0);
}

// copy and move operator=()
TEST(UMapInitialization, CopyAndMoveAssignOperator) {
    ink::KUnorderedMap<int, int> intMapOther;
    intMap[5] = 500;
    intMapOther = intMap;
    EXPECT_EQ(intMapOther[5], 500);

    intMapOther.clear();
    intMapOther = std::move(intMap);
    EXPECT_EQ(intMapOther[5], 500);
}

// maxLoadFactor()
TEST(UMapInitialization, HashPolicy) {
    EXPECT_EQ(doubleMap.maxLoadFactor(), 2.0f);
    doubleMap.maxLoadFactor(5.0f);
    EXPECT_EQ(doubleMap.maxLoadFactor(), 5.0f);
}

// iterator and const_iterator
TEST(UMapInitialization, Iterators) {
    ink::KUnorderedMap<int, int> iterator_test_map;
    for (int i = 0; i < 10; i++) {
        iterator_test_map[i] = i + 50;
    }
    auto it = iterator_test_map.begin();
    auto const_it = iterator_test_map.cbegin();
    for (size_t i = 0; i < 3; i++) {
        ++it;
        ++const_it;
    }
    EXPECT_EQ(it->second, 53);
    EXPECT_EQ(const_it->second, 53);

    auto begin_it = iterator_test_map.begin();
    (*begin_it).second = 125;
    EXPECT_EQ(iterator_test_map.begin()->second, 125);
    // (*const_it).second = 125 <--- CE

    for (size_t i = 0; i < 3; i++) {
        --it;
        --const_it;
    }
    EXPECT_EQ(it->second, 125);
    EXPECT_EQ(const_it->second, 125);
}