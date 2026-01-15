cmake_minimum_required(VERSION 3.30)
project(Netsim_simulationen)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Netsim_simulationen main.cpp
        package.cpp
        storage_types.cpp)
