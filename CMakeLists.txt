
cmake_minimum_required(VERSION 3.13)
project(KNetwork)

set(CMAKE_CXX_STANDARD 14)

add_library(KNetwork
BaseDataType.cpp
BaseDataType.hpp
CMakeLists.txt
ClientTCP.cpp
ClientTCP.hpp
Config.hpp
DataTCP.cpp
DataTCP.hpp
IOManager.cpp
IOManager.hpp
IOTCP.cpp
IOTCP.hpp
ServerTCP.cpp
ServerTCP.hpp
)
