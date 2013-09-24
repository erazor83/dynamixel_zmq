SET(CORELIBS ${Boost_SYSTEM_LIBRARY})

IF (${CMAKE_SYSTEM} MATCHES ".*gentoo.*")
	SET (GENTOO ON)
	SET (LINUX)
	MESSAGE("=== Detected system is Gentoo ===")
ELSE()
	MESSAGE("=== Detected system is ${CMAKE_SYSTEM} ===") 
ENDIF()

INCLUDE (CheckIncludeFiles)
CHECK_INCLUDE_FILES("zmq.hpp" HAS_ZMQ)

IF (NOT DEFINED HAS_ZMQ)
	MESSAGE(SEND_ERROR "ZeroMQ CPP headers not found!")
ENDIF()

CHECK_INCLUDE_FILES("msgpack.hpp" HAS_MSGPACK)
IF (NOT DEFINED HAS_MSGPACK)
	MESSAGE(SEND_ERROR "MessagePack CPP headers not found!")
ENDIF()

MESSAGE("LIBS: ${HAS_ZMQ} ${HAS_MSGPACK}")
IF (GENTOO)
	IF (NOT DEFINED HAS_MSGPACK)
		MESSAGE("    install dev-libs/msgpack")
	ENDIF()
	IF (NOT DEFINED HAS_ZMQ)
		MESSAGE("    install net-libs/cppzmq")
	ENDIF()
ENDIF()


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/lib/libdynamixel/src)

MESSAGE(STATUS "dynamixel_zmq CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

ADD_SUBDIRECTORY(lib)

ADD_EXECUTABLE(dynamixel_zmq dynamixel_zmq.cpp)
TARGET_LINK_LIBRARIES(dynamixel_zmq ${Boost_LIBRARIES} zmq msgpack dynamixel)


