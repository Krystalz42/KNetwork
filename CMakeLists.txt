cmake_minimum_required(VERSION 3.13)
project(KNetwork)

set(CMAKE_CXX_STANDARD 14)

include_directories(network)

add_executable(KNetwork
        network/BaseDataType.cpp
        network/BaseDataType.hpp
        network/ClientTCP.cpp
        network/ClientTCP.hpp
        network/Config.hpp
        network/DataTCP.cpp
        network/DataTCP.hpp
        network/IOManager.cpp
        network/IOManager.hpp
        network/IOTCP.cpp
        network/IOTCP.hpp
        network/ServerTCP.cpp
        network/ServerTCP.hpp
        main.cpp)
