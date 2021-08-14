cmake_minimum_required(VERSION 3.1)

set ( CXX_STANDARD 11 ) 

set (CMAKE_BUILD_TYPE Debug FORCE)

project(CPP_HEX_EDITOR CXX)

add_executable( che main.cpp sha256.cpp )
