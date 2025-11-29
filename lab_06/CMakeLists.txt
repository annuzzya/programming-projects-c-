cmake_minimum_required(VERSION 3.10.0)
project(lab_06 VERSION 1.0 LANGUAGES C CXX)

if (MSVC)
    add_compile_options(/W3)
    add_compile_options(/std:c++23preview)
    add_compile_options(/EHsc)
    add_compile_options(/utf-8)
else()
    add_compile_options(-Wall)
    add_compile_options(-std=c++23)
    add_compile_options(-fexceptions)
    add_compile_options(-O3)
    add_compile_options(-DGCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

add_executable(lab_06
        main.cpp
        guess_server.cpp
        guess_server.h
)
