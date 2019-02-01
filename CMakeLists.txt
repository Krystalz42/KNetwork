cmake_minimum_required(VERSION 3.6)

set(NAME network)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${NAME} VERSION 1.0.0 LANGUAGES CXX)

include_directories(KNW)

add_executable(network
        KNW/BaseDataType.cpp
        KNW/BaseDataType.hpp
        KNW/Config.hpp
        KNW/ServerTCP.cpp
        KNW/ServerTCP.hpp
        main.cpp KNW/DataTCP.hpp KNW/IOTCP.cpp KNW/IOTCP.hpp)

find_package(Boost REQUIRED COMPONENTS system program_options thread filesystem)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fsanitize=address -g3")

message(${Boost_LIBRARIES})
target_link_libraries(${NAME}
        ${Boost_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        )