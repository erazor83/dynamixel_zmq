cmake_minimum_required(VERSION 2.6)
project(DYN_ZMQ)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      OFF)

find_package(Boost COMPONENTS system REQUIRED)
find_package(Boost COMPONENTS program_options REQUIRED)

set(DYNIF_SUPPORT_CM5  ON)

set(CMAKE_CXX_FLAGS "-g -Wall")
add_subdirectory(src)