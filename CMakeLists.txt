cmake_minimum_required(VERSION 3.19)

project(MyUnorderedMap LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

#add_library(header_lib STATIC ${PROJECT_SOURCE_DIR}/src/somecpp.cpp)

# target_include_directories(header_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(UMap_make main.cpp)

if (CMAKE_GENERATOR MATCHES "Visual Studio 17 2022")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT UMap_make)
endif()

target_include_directories(UMap_make PRIVATE ${PROJECT_SOURCE_DIR}/include)

# target_link_libraries(UMap_make header_lib)