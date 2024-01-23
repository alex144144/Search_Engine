cmake_minimum_required(VERSION 3.27)
project(search_engine)

set(CMAKE_CXX_STANDARD 23)

add_executable(search_engine
        converterjson.cpp
        converterjson.h
        invertedIndex.cpp
        invertedIndex.h
        searchServer.cpp
        searchServer.h
        search_engine_test.cc
)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(nlohmann_json)
add_subdirectory(resources)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL
        https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(search_engine PRIVATE gtest_main)

include(GoogleTest)

gtest_discover_tests(search_engine)