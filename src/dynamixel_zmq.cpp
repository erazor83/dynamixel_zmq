/**
 * Dynamixel ZeroMQ service
 * __author__		= Alexander Krause <alexander.krause@ed-solutions.de>
 * __version__	= 0.0.1
 * __date__			= 2013-09-17
 */

#include <zmq.hpp>
#include <unistd.h>

#include "boost/program_options.hpp"
#include <iostream>
#include <string>
#include <fstream>

#include "dynamixel.h"

#define DESCRIPTION "dyn_zmq - Dynamixel ZeroMQ service"
namespace { 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
 
}
//using namespace std;
//using namespace dynapi;

int main(int argc, char** argv) {
	// === program parameters ===
	std::string zmq_uri="tcp://*:5555";
	std::string serial_port="/dev/ttyUSB0";
	std::string interface_type="cm5";
	uint32_t serial_speed=1000000;
	
	namespace po = boost::program_options;

  // Setup options.
  po::options_description desc("Options");
	desc.add_options()
		("help", "produce help message")
		("uri", po::value< std::string >( &zmq_uri ),					"ZeroMQ server uri | default: tcp://*:5555" )
		("port", po::value< std::string >( &serial_port ),		"serial port       | default: /dev/ttyUSB0" )
		("speed", po::value< uint32_t >( &serial_speed ),			"serial speed      | default: 1000000" )
		("type", po::value< std::string >( &interface_type ),	"interface type    | default: cm5" )
		("dynamixel-scan", "scan for dynamixel servos")
	;

  po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 

		po::notify(vm);  
			
		if (vm.count("help")) {
			std::cout << DESCRIPTION << std::endl << desc << std::endl; 
			return SUCCESS; 
		}

	} catch(po::error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
		std::cerr << desc << std::endl; 
		return ERROR_IN_COMMAND_LINE; 
	} 

#if 1
	std::cout << "uri   = " << zmq_uri << std::endl; 
	std::cout << "port  = " << serial_port << std::endl;
	std::cout << "speed = " << serial_speed << std::endl;
#endif

	// === dynamixel part ===
	dynamixel_t *dyn;
	dyn = dynamixel_new_rtu(serial_port.c_str(), (uint32_t)serial_speed, _DYNAMIXEL_SERIAL_DEFAULTS);

	int8_t dyn_connected;
	dynamixel_set_debug(dyn,true);
	dyn_connected=dynamixel_connect(dyn);
	
	if (vm.count("dynamixel-scan")) {
		if (dyn_connected==0) {
			uint8_t *found_ids;
			uint8_t id_count;
			id_count=dynamixel_search(dyn, 1,30,&found_ids);
			printf("%i Dynamixels found\n",id_count);
			while (id_count--) {
				printf(
					"  * Dynamixels #% 3i found @ % 2i\n",
					id_count,
					found_ids[id_count]
				);
			}
			dynamixel_close(dyn);
			dynamixel_free(dyn);
			return SUCCESS; 
		} else {
			dynamixel_free(dyn);
			return 0;
		}
	}
		
		
	// === ZMQ part ===
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_REP);
	socket.bind (zmq_uri.c_str());

	std::cout << "Server started (uri="<<zmq_uri<<")" << std::endl;
	while (true) {
		zmq::message_t request;

		//  Wait for next request from client
		socket.recv (&request);
		std::cout << "Received Hello" << std::endl;

		//  Do some 'work'
		sleep (1);

		//  Send reply back to client
		zmq::message_t reply (5);
		memcpy ((void *) reply.data (), "World", 5);
		socket.send (reply);
	}
	return 0;
}

