cmake_minimum_required(VERSION 3.10)
project(Lab5 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W3 /std:c++20 /EHsc /O2 /utf-8)
else()
    add_compile_options(-Wall)
    add_compile_options(-std=c++20)
    add_compile_options(-fexceptions)
    add_compile_options(-O3)
    add_compile_options(-pthread)
    add_compile_options(-DGCC)
endif()

add_executable(Lab5
        main.cpp
)