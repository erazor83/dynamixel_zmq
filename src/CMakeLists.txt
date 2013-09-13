set(CORELIBS ${Boost_SYSTEM_LIBRARY})


include_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

if (DYNIF_SUPPORT_CM5)
	set(HAVE_POSIXSERIAL							ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDYNIF_SUPPORT_CM5")
endif()
message(STATUS "dyn_zmq CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

add_subdirectory(lib)

add_executable(dyn_zmq dyn_zmq.cpp)
target_link_libraries(dyn_zmq ${Boost_LIBRARIES} zmq dynapi)


