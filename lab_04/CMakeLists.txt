cmake_minimum_required(VERSION 3.10)
project(lab_04 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W3 /std:c++20)
else()
    add_compile_options(-Wall -std=c++20 -O3 -fexceptions -pthread -DGCC)
endif()

add_executable(lab_04
        main.cpp
        safe_fields.cpp
        safe_fields.h
)