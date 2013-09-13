//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <unistd.h>

#include "boost/program_options.hpp"
#include <iostream>
#include <string>
#include <fstream>

#include "dynapi/dynapi.h"

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
	uint32_t serial_speed=1000000;
	
	namespace po = boost::program_options;

  // Setup options.
  po::options_description desc("Options");
	desc.add_options()
		("help", "produce help message")
		("uri", po::value< std::string >( &zmq_uri ),      "ZeroMQ server uri | default: tcp://*:5555" )
		("port", po::value< std::string >( &serial_port ), "serial port       | default: /dev/ttyUSB0" )
		("speed", po::value< uint32_t >( &serial_speed ),  "serial speed      | default: 1000000" )
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
	dynapi::dyn_if_conn connection = dynapi::DYN_IC_SERIAL;

	try {
		// open a connection to cm-5 interface
		// (shared_ptr provides automatic garbage collection, ptr will be released
		// if last shared_ptr is destructed)
		boost::shared_ptr<dynapi::DynInterface> dyni (dynapi::dynGetIfFromFactory(serial_port, connection));
		
		//dynamixel direct commands
		if (vm.count("dynamixel-scan")) {
			std::vector<dynapi::Ax> devices( dyni->scanBus() );
			std::cout << devices <<  std::endl <<  std::endl;
			return SUCCESS; 
		}
		
	} catch (std::exception &x) { // anything went wrong
		std::cout << x.what() << std::endl;
		return EXIT_FAILURE;
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

