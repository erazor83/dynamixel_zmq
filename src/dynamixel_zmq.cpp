/**
 * Dynamixel ZeroMQ service
 * __author__		= Alexander Krause <alexander.krause@ed-solutions.de>
 * __version__	= 0.0.1
 * __date__			= 2013-09-17
 */

#include <unistd.h>

#include "boost/program_options.hpp"
#include <iostream>
#include <string>
#include <fstream>

#include <msgpack.hpp>
#include <vector>
#include <zmq.hpp>


#include "dynamixel.h"
typedef enum {
	/* default dynamixel commands */
	DYNAMIXEL_RQ_PING					=0x01,
	DYNAMIXEL_RQ_READ_DATA		=0x02,
	DYNAMIXEL_RQ_WRITE_DATA		=0x03,
	DYNAMIXEL_RQ_REG_WRITE		=0x04,
	DYNAMIXEL_RQ_REG_ACTION		=0x05,
	DYNAMIXEL_RQ_RESET				=0x06,
	DYNAMIXEL_RQ_SYNC_WRITE		=0x83,
	
	/* custom commands */
	DYNAMIXEL_RQ_ZMQ_ECHO					=0x100,
	
	DYNAMIXEL_RQ_SYNC_WRITE_WORDS	=0x183, 
	
} dynamixel_request_t;

#define DESCRIPTION "dyn_zmq - Dynamixel ZeroMQ service"
namespace { 
  const size_t ERROR_IN_COMMAND_LINE = 1; 
  const size_t SUCCESS = 0; 
  const size_t ERROR_UNHANDLED_EXCEPTION = 2; 
 
}

typedef enum {
	ZMQ_ERR_NO_ERROR								= 0,
	ZMQ_ERR_INVALID_COMMAND					= -1000,
	ZMQ_ERR_INVALID_FORMAT					= -1001,
	ZMQ_ERR_INVALID_PARAMETERS			= -1002,
	ZMQ_ERR_INVALID_PARAMETER_COUNT	= -1003,
	ZMQ_ERR_BUS_OFFLINE							= -1010,
	
} zmq_error_code_t;
//using namespace std;
//using namespace dynapi;

int main(int argc, char** argv) {
	// === program parameters ===
	std::string zmq_uri="tcp://*:5555";
	std::string serial_port="/dev/ttyUSB0";
	std::string interface_type="cm5";
	uint32_t serial_speed=1000000;
	
	bool debug=false;
	
	int tx_error_code=0;
	
	int dynamixel_ret=0;
	
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
		("debug", "print out debugging info")
	;

  po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 

		po::notify(vm);  
			
		if (vm.count("help")) {
			std::cout << DESCRIPTION << std::endl << desc << std::endl; 
			return SUCCESS; 
		}
		
		if (vm.count("debug")) {
			debug=true;
		}
	} catch(po::error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
		std::cerr << desc << std::endl; 
		return ERROR_IN_COMMAND_LINE; 
	} 

	if (debug) {
		std::cout << "uri   = " << zmq_uri << std::endl; 
		std::cout << "port  = " << serial_port << std::endl;
		std::cout << "speed = " << serial_speed << std::endl;
	}
	
	// === dynamixel part ===
	dynamixel_t *dyn;
	dyn = dynamixel_new_rtu(serial_port.c_str(), (uint32_t)serial_speed, _DYNAMIXEL_SERIAL_DEFAULTS);

	int8_t dyn_connected;
	dynamixel_set_debug(dyn,debug);
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

	if (debug) {
		std::cout << "Server started (uri="<<zmq_uri<<")" << std::endl;
	}

		
	/* i decided to use a static buffer instead of malloc on every call */
	uint8_t		tmp_uint8[DYNAMIXEL_MAX_PARAMETER_COUNT];
	uint16_t	tmp_uint16[DYNAMIXEL_MAX_PARAMETER_COUNT/2];
	
	while (true) {
		zmq::message_t rx_zmq;
		msgpack::object rx_obj;
		msgpack::object tx_obj;
		msgpack::sbuffer tx_msg;
		msgpack::unpacked rx_msg;
		std::vector<int16_t> rx_vect;
		std::vector<int16_t> tx_vect;

		//  Wait for next request from client
		if (socket.recv (&rx_zmq)) {
			// Deserialize the serialized data.

			msgpack::unpack(&rx_msg, static_cast<char*>(rx_zmq.data()), rx_zmq.size());
			rx_obj = rx_msg.get();

			// Print the deserialized object to stdout.
			if (debug) {
				std::cout << "== rx ==" << std::endl;
				std::cout << "data: " << rx_obj << std::endl;
			}
			//std::cout << rx_obj.get() <<std::endl;
			
			//uint8_t command = rx_obj.get();
			
			tx_error_code=ZMQ_ERR_INVALID_FORMAT;
			if (rx_obj.type==msgpack::type::ARRAY) {
				try {
					rx_obj.convert(&rx_vect);
					tx_error_code=0;
				} catch(std::bad_cast& e) {
					std::cerr << "MESSAGE-ERROR: " << e.what() << std::endl << std::endl; 
				}
			}
			
			if (tx_error_code==0) {
				if (debug) {
					std::cout << "command: " << (int16_t)rx_vect.at(0) << std::endl;
				}
				

				switch (rx_vect.at(0)) {
					case DYNAMIXEL_RQ_PING:
						//zmq-message: <cmd>,<id>
						if (rx_vect.size()!=2) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							dynamixel_ret=dynamixel_ping(dyn,(uint8_t)rx_vect.at(1));
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(dynamixel_ret);
						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;

					case DYNAMIXEL_RQ_READ_DATA:
						//zmq-message: <cmd>,<id>,<register>,<count>
						if (rx_vect.size()!=4) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							uint8_t *pdata;
							dynamixel_ret=dynamixel_read_data(
								dyn,
								(uint8_t)rx_vect.at(1),							/*id*/
								(dynamixel_register_t)rx_vect.at(2),/*address*/
								(uint8_t)rx_vect.at(3),							/*count*/
								&pdata
							);
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							if (dynamixel_ret) {
								for (uint8_t i=0; i<dynamixel_ret;i++) {
									tx_vect.push_back(pdata[i]);
								}
							}
						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;
					case DYNAMIXEL_RQ_WRITE_DATA:
						//zmq-message: <cmd>,<id>,<register>,<count>,<data>,<data+1>
						if ((rx_vect.size()<5) or ((uint16_t)(rx_vect.at(2)*(rx_vect.at(3)+1))>rx_vect.size())) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							/* copy into uint8_t*/
							for (uint8_t cP=0;cP<rx_vect.size()-3;cP++) {
								tmp_uint8[cP]=rx_vect.at(cP+3);
							}
							dynamixel_ret=dynamixel_write_data(
								dyn,
								(uint8_t)rx_vect.at(1),
								(dynamixel_register_t)rx_vect.at(2),
								rx_vect.size()-3,
								tmp_uint8
							);
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(dynamixel_ret);
						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;
					case DYNAMIXEL_RQ_REG_WRITE:
						//zmq-message: <cmd>,<id>,<register>,<count>,<data>,<data+1>
						/* this will cut higher bytes from parameters */
						if (dyn_connected==0) {
							/* copy into uint8_t*/
							for (uint8_t cP=0;cP<rx_vect.size()-3;cP++) {
								tmp_uint8[cP]=rx_vect.at(cP+3);
							}
							dynamixel_ret=dynamixel_reg_write(
								dyn,
								(uint8_t)rx_vect.at(1),
								(dynamixel_register_t)rx_vect.at(2),
								rx_vect.size()-3,
								tmp_uint8
							);
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(dynamixel_ret);

						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;
					case DYNAMIXEL_RQ_REG_ACTION:
						//zmq-message: <cmd>,<id>
						if (rx_vect.size()!=2) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							dynamixel_ret=dynamixel_action(dyn,(uint8_t)rx_vect.at(1));
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(dynamixel_ret);
						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;
						
					case DYNAMIXEL_RQ_RESET:
						//zmq-message: <cmd>,<id>
						if (rx_vect.size()!=2) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							dynamixel_ret=dynamixel_reset(dyn,(uint8_t)rx_vect.at(1));
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(dynamixel_ret);
						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;
						
					case DYNAMIXEL_RQ_SYNC_WRITE:
						//zmq-message: <cmd>,<register>,<id_count>,<parameter_count>,<servo-id>,<data>,<data+n>
						if ((rx_vect.size()<6) or ((uint16_t)(rx_vect.at(2)*(rx_vect.at(3)+1))>rx_vect.size())) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							/* copy into uint8_t*/
							for (uint8_t i=0;i<(rx_vect.at(2)*(rx_vect.at(3)+1));i++) {
								tmp_uint8[i]=rx_vect.at(i+4);
							}
							dynamixel_ret=dynamixel_sync_write(
								dyn,
								(dynamixel_register_t)rx_vect.at(1),	/*register*/
								(uint8_t)rx_vect.at(2),								/*id-count*/
								(uint8_t)rx_vect.at(3),								/*parameter_count*/
								tmp_uint8
							);
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(dynamixel_ret);
						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;
						
					case DYNAMIXEL_RQ_SYNC_WRITE_WORDS:
						//zmq-message: <cmd>,<register>,<id_count>,<parameter_count>,<servo-id>,<data>,<data+n>
						if ((rx_vect.size()<6) or ((uint16_t)(rx_vect.at(2)*(rx_vect.at(3)+1))>rx_vect.size())) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							/* copy into uint8_t*/
							for (uint8_t i=0;i<(rx_vect.at(2)*(rx_vect.at(3)+1));i++) {
								tmp_uint16[i]=rx_vect.at(i+4);
							}
							dynamixel_ret=dynamixel_sync_write_words(
								dyn,
								(dynamixel_register_t)rx_vect.at(1),	/*register*/
								(uint8_t)rx_vect.at(2),								/*id-count*/
								(uint8_t)rx_vect.at(3),								/*word_count*/
								tmp_uint16
							);
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(dynamixel_ret);
						} else {
							tx_error_code=ZMQ_ERR_BUS_OFFLINE;
						}
						break;

					case DYNAMIXEL_RQ_ZMQ_ECHO:
						//zmq-message: <cmd>,<data>,<data+n>
						tx_obj=rx_obj;
						tx_error_code=ZMQ_ERR_NO_ERROR;
						msgpack::pack(&tx_msg, tx_obj);
						break;
						
					default:
						tx_error_code=ZMQ_ERR_INVALID_COMMAND;
				}
				if (tx_error_code) {
					tx_vect.push_back(tx_error_code);
				} else if (tx_vect.size()) {
					msgpack::pack(&tx_msg, tx_vect);
				}
			} else {
				/* invalid incomming type, has to be list */
				std::vector<int> tx_vect;
				tx_vect.push_back(ZMQ_ERR_INVALID_FORMAT);
				msgpack::pack(&tx_msg, tx_vect);
			}
			
			zmq::message_t tx_zmq(tx_msg.size());
			memcpy(static_cast<char*>(tx_zmq.data()), tx_msg.data(), tx_msg.size());

			if (debug) {
				std::cout << "== tx ==" << std::endl;
				//std::cout << tx_vect << std::endl;
			}
			socket.send(tx_zmq);
		}
	}
	return 0;
}

