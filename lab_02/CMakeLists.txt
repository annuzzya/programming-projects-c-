cmake_minimum_required(VERSION 3.16)
project(ReplaceIfResearch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()

add_executable(ReplaceIfResearch
        main.cpp
        replace_experiments.cpp
        replace_experiments.tpp
)