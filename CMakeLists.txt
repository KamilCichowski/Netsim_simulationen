cmake_minimum_required(VERSION 3.20)
project(Netsim_simulationen)

set(CMAKE_CXX_STANDARD 20)

add_executable(Netsim_simulationen
        main.cpp
        factory.cpp
        helpers.cpp
        nodes.cpp
        package.cpp
        reports.cpp
        storage_types.cpp
)
