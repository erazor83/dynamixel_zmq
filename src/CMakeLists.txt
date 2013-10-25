SET(CORELIBS ${Boost_SYSTEM_LIBRARY})

IF (${CMAKE_SYSTEM} MATCHES ".*gentoo.*")
	SET (GENTOO ON)
	SET (LINUX)
	MESSAGE("=== Detected system is Gentoo ===")
ELSE()
	MESSAGE("=== Detected system is ${CMAKE_SYSTEM} ===") 
ENDIF()

INCLUDE (CheckIncludeFiles)
INCLUDE (CheckSymbolExists)

#TODO: check for hpp files
#somehow this does not work

MESSAGE("=== Libraries ===") 
CHECK_INCLUDE_FILES("zmq.h" HAS_ZMQ)

IF (NOT HAS_ZMQ)
	MESSAGE(SEND_ERROR "ZeroMQ CPP headers not found!")
ENDIF()

CHECK_INCLUDE_FILES("msgpack.h" HAS_MSGPACK)
IF (NOT HAS_MSGPACK)
	MESSAGE(SEND_ERROR "MessagePack CPP headers not found!")
ENDIF()

IF (USE_DYNAMIXEL_BUNDLED)
	MESSAGE("Will use bundled libdynamixel.")
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/lib/libdynamixel/src)
	ADD_SUBDIRECTORY(lib)
ELSE()
	CHECK_INCLUDE_FILES("dynamixel.h" HAS_DYNAMIXEL)
	IF (NOT HAS_DYNAMIXEL)
		MESSAGE(SEND_ERROR "Dynamixel headers not found!")
	ENDIF()
	CHECK_SYMBOL_EXISTS(TROSSEN_CMD_SUPPORT "dynamixel_config.h" DYNAMIXEL_HAVE_TROSSEN_CMD)
	IF (NOT DYNAMIXEL_HAVE_TROSSEN_CMD)
		message( FATAL_ERROR "TROSSEN_CMD_SUPPORT required in libdynamixel!")
	ENDIF()
ENDIF()

IF (GENTOO)
	IF (NOT HAS_MSGPACK)
		MESSAGE("    install dev-libs/msgpack")
	ENDIF()
	IF (NOT HAS_ZMQ)
		MESSAGE("    install net-libs/cppzmq")
	ENDIF()
	IF (NOT HAS_DYNAMIXEL)
		MESSAGE("    install dev-libs/libdynamixel")
	ENDIF()
ENDIF()

MESSAGE("=== Features ===") 
MESSAGE("  * ENABLE_PYPOSE_COMMANDS     ${ENABLE_PYPOSE_COMMANDS}")
MESSAGE("  * ENABLE_TROSSEN_COMMANDER   ${ENABLE_TROSSEN_COMMANDER}")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

ADD_EXECUTABLE(dynamixel_zmq dynamixel_zmq.cpp)
TARGET_LINK_LIBRARIES(dynamixel_zmq ${Boost_LIBRARIES} zmq msgpack dynamixel pthread)


