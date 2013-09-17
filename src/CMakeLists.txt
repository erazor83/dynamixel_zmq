SET(CORELIBS ${Boost_SYSTEM_LIBRARY})


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/lib/libdynamixel/src)

MESSAGE(STATUS "dynamixel_zmq CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

ADD_SUBDIRECTORY(lib)

ADD_EXECUTABLE(dynamixel_zmq dynamixel_zmq.cpp)
TARGET_LINK_LIBRARIES(dynamixel_zmq ${Boost_LIBRARIES} zmq libdynapi)


