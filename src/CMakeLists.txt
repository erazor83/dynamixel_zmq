set(CORELIBS ${Boost_SYSTEM_LIBRARY})

add_subdirectory(lib)

include_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

add_executable(dyn_zmq dyn_zmq.cpp)
target_link_libraries(dyn_zmq ${Boost_LIBRARIES} zmq dynapi)


