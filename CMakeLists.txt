cmake_minimum_required(VERSION 3.30)
project(Netsim_simulationen)

set(CMAKE_CXX_STANDARD 20)

add_executable(Netsim_simulationen main.cpp
        helpers.cpp
        main.cpp
        nodes.cpp
        package.cpp
        storage_types.cpp
        )
