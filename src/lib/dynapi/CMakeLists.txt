cmake_minimum_required(VERSION 2.6)
project(DYN_ZMQ)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      OFF)

set(HAVE_POSIXSERIAL							On)

find_package(Boost COMPONENTS system REQUIRED)

add_library (dynapi dynapi.cc ax.cc dyninternal.cc ifinfo.cc dyninterfacecm5posixserial.cc)

set(CMAKE_CXX_FLAGS "-DHAVE_POSIXSERIAL -g -Wall")

if (HAVE_POSIXSERIAL) 
	set(CMAKE_CXX_FLAGS "-DHAVE_POSIXSERIAL")
endif()