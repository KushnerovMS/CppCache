cmake_minimum_required(VERSION 3.16) 

project(Cache)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Cache src/main.cpp src/ARC.hpp src/IdealCache.hpp)

add_executable(TestOfEfficiency src/TestOfEfficiency.cpp src/ARC.hpp src/IdealCache.hpp src/LRUC.hpp)

add_subdirectory(tests)
