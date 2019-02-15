cmake_minimum_required(VERSION 3.13)
project(test)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(network)

add_executable(test
        network/IOManager.cpp
        network/IOManager.hpp
        network/IOTCP.cpp
        network/IOTCP.hpp
        network/ServerTCP.cpp
        network/ServerTCP.hpp
        main.cpp
        net.hpp
        network/DataTCP.cpp
        network/DataTCP.hpp
        network/BaseDataType.cpp
        network/BaseDataType.hpp
        network/Config.hpp
        network/ClientTCP.cpp network/ClientTCP.hpp)

#set(NAME test)
#
#project(${NAME})
#
#set(CMAKE_CXX_STANDARD 14)
#
#target_link_libraries(${NAME} PRIVATE Boost::filesystem)
#target_link_libraries(${NAME} PRIVATE Boost::thread)
#target_link_libraries(${NAME} PRIVATE Boost::program_options)
#
#add_executable(${NAME}
#        main.cpp net.hpp network/ServerTCP.cpp network/ServerTCP.hpp
#        network/IOTCP.cpp
#        network/IOTCP.hpp network/IOManager.cpp network/IOManager.hpp)
#