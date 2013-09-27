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


#ifdef ENABLE_PYPOSE_COMMANDS
#define PYPOSE_ID             253
#define PYPOSE_MAX_POSE_SIZE  32
#define PYPOSE_MAX_POSE_COUNT 255
#define PYPOSE_MAX_SEQUENCE_COUNT 255
typedef struct {
	uint8_t len;
	uint16_t *values;
} pypose_pose_t;

pypose_pose_t* pyPose_Poses[PYPOSE_MAX_POSE_COUNT];

typedef struct {
	uint8_t pose_id;
	uint16_t delay;
} pypose_seq_part_t;

typedef struct {
	uint8_t len;
	pypose_seq_part_t* parts;
} pypose_sequence_t;

pypose_sequence_t* pyPose_Sequences[PYPOSE_MAX_SEQUENCE_COUNT];

uint8_t pyPose_PoseSize=0;

#endif

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
#ifdef ENABLE_PYPOSE_COMMANDS
	/*0x07, <pose-size>*/
	PYPOSE_SET_POSESIZE				=0x07,
	/*0x08, <index>. <pos1_L>, <pos1_H> */
	PYPOSE_LOAD_POSE					=0x08,
	/*0x08, <index>. <pos1_L>, <pos1_H> */
	PYPOSE_LOAD_SEQUENCE			=0x09,
	
	PYPOSE_PLAY_SEQUENCE			=0x0A,
	PYPOSE_LOOP_SEQUENCE			=0x0B,
	PYPOSE_TEST								=0x19,
#endif
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
	ZMQ_ERR_INVALID_ID							= -1004,
	ZMQ_ERR_BUS_OFFLINE							= -1010,
	
} zmq_error_code_t;
//using namespace std;
//using namespace dynapi;


#ifdef ENABLE_PYPOSE_COMMANDS
/* stuff for sequence playing */
#include <pthread.h>
#include <time.h>
pthread_cond_t pyPose_SequencePlayer_active = PTHREAD_COND_INITIALIZER;

typedef struct {
	uint8_t state;
	bool								enabled;
	dynamixel_t*				dynamixel_ctx;
	pypose_pose_t*			poses;
	pypose_sequence_t* 	sequences;

	pthread_mutex_t lock;
	pthread_cond_t cond;
} pypose_player_ctx_t;


void *pyPose_SequencePlayer(void* arg){
	pypose_player_ctx_t* player_ctx = (pypose_player_ctx_t*)arg;
	int i=0;
	while(true) {
		pthread_mutex_lock(&player_ctx->lock);
		while (player_ctx->enabled == false) {
			i++;
			std::cout << "Sleeping: " << i << std::endl;
			pthread_cond_wait(&player_ctx->cond, &player_ctx->lock);
		}
		pthread_mutex_unlock(&player_ctx->lock);
		i++;
		sleep(1);
		std::cout << "Threading: " << i << std::endl;
	}
	//pthread_cond_broadcast(&player_ctx->cond);
	//
	return NULL;
}

void pyPose_SequencePlayer_cmd(uint8_t command);
#endif

int main(int argc, char** argv) {
	// === program parameters ===
	std::string zmq_uri="tcp://*:5555";
	std::string serial_port="/dev/ttyUSB0";
	std::string interface_type="rs232";
	uint32_t serial_speed=1000000;
	
	bool debug=false;
	
	int16_t tx_error_code=0;
	
	int16_t dynamixel_ret=0;
	
#ifdef ENABLE_PYPOSE_COMMANDS
	//void* print_message(void*);
	
	pypose_player_ctx_t pyPose_Player_Context;
	pyPose_Player_Context.enabled=true;
	pthread_mutex_init(&pyPose_Player_Context.lock, NULL);
	pthread_cond_init(&pyPose_Player_Context.cond, NULL);
	
	pthread_t pyPose_Player_Thread;
	pthread_create(&pyPose_Player_Thread, NULL, &pyPose_SequencePlayer, (void*)&pyPose_Player_Context);

	//sleep(10);
	pyPose_Player_Context.enabled=false;
	
	uint16_t pose_idx;
	for (pose_idx=0; pose_idx<PYPOSE_MAX_POSE_COUNT; pose_idx++) {
		pyPose_Poses[pose_idx]=NULL;
	}
	uint16_t seq_idx;
	for (seq_idx=0; seq_idx<PYPOSE_MAX_SEQUENCE_COUNT; seq_idx++) {
		pyPose_Sequences[seq_idx]=NULL;
	}
#endif
	namespace po = boost::program_options;

  // Setup options.
  po::options_description desc("Options");
	desc.add_options()
		("help", "produce help message")
		("uri", po::value< std::string >( &zmq_uri ),					"ZeroMQ server uri | default: tcp://*:5555" )
		("port", po::value< std::string >( &serial_port ),		"serial port       | default: /dev/ttyUSB0" )
		("speed", po::value< uint32_t >( &serial_speed ),			"serial speed      | default: 1000000" )
		("type", po::value< std::string >( &interface_type ),	"interface type    | default: rs232" )
		("dynamixel-scan", "scan for dynamixel servos")
		("debug", "print out debugging info")
	;

  po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm); // can throw 

		po::notify(vm);  
			
		if (vm.count("help")) {
			std::cout << DESCRIPTION << std::endl << desc << std::endl;
#ifdef ENABLE_PYPOSE_COMMANDS
			std::cout << "  * PyPose support enabled" << std::endl;
#endif
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
						if ((rx_vect.size()<5) or ((uint16_t)(rx_vect.at(3)+4)>rx_vect.size())) {
							tx_error_code=ZMQ_ERR_INVALID_PARAMETER_COUNT;
						} else if (dyn_connected==0) {
							/* copy into uint8_t*/
							for (uint8_t cP=0;cP<(rx_vect.size()-4);cP++) {
								tmp_uint8[cP]=rx_vect.at(cP+4);
							}
							dynamixel_ret=dynamixel_write_data(
								dyn,
								(uint8_t)rx_vect.at(1),
								(dynamixel_register_t)rx_vect.at(2),
								rx_vect.size()-4,
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
							for (uint8_t cP=0;cP<rx_vect.size()-4;cP++) {
								tmp_uint8[cP]=rx_vect.at(cP+4);
							}
							dynamixel_ret=dynamixel_reg_write(
								dyn,
								(uint8_t)rx_vect.at(1),
								(dynamixel_register_t)rx_vect.at(2),
								rx_vect.size()-4,
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
						break;
#ifdef ENABLE_PYPOSE_COMMANDS
					case PYPOSE_SET_POSESIZE:
						//zmq-message: <cmd>,<id>,<size>
						if (rx_vect.at(1)==PYPOSE_ID) {
							if (rx_vect.at(2)<PYPOSE_MAX_POSE_SIZE) {
								pyPose_PoseSize=(uint8_t)rx_vect.at(2);
								if (debug) {
									std::cout << "New pose size received: "<< (int)pyPose_PoseSize << std::endl;
								}
								tx_vect.push_back(ZMQ_ERR_NO_ERROR);
								tx_vect.push_back(pyPose_PoseSize);
							} else {
								tx_error_code=ZMQ_ERR_INVALID_PARAMETERS;
							}
						} else {
							tx_error_code=ZMQ_ERR_INVALID_ID;
						}
						break;
					case PYPOSE_LOAD_POSE:
						//zmq-message: <cmd>,<id>,<index>,<pos1_L>, <pos1_H>
						if (rx_vect.at(1)==PYPOSE_ID) {
							pose_idx=rx_vect.at(2);
							if ((pyPose_PoseSize<PYPOSE_MAX_POSE_SIZE) && ((uint16_t)rx_vect.size()==(pyPose_PoseSize*2+3))) {
								if (pose_idx<PYPOSE_MAX_POSE_COUNT) {
									if (not pyPose_Poses[pose_idx]) {
										//pose does not exist already
										pyPose_Poses[pose_idx]=(pypose_pose_t*)malloc(sizeof(pypose_pose_t));
										pyPose_Poses[pose_idx]->len=0;
										pyPose_Poses[pose_idx]->values=NULL;
									}
									if ((pyPose_Poses[pose_idx]->values) && (pyPose_Poses[pose_idx]->len != pyPose_PoseSize)) {
										//pose size not corret
										free(pyPose_Poses[pose_idx]->values);
										pyPose_Sequences[seq_idx]->parts=NULL;
										pyPose_Poses[pose_idx]->len=0;
									}
									if (not pyPose_Poses[pose_idx]->len) {
										//create new memory
										pyPose_Poses[pose_idx]->values=(uint16_t*)malloc(sizeof(uint16_t)*pyPose_PoseSize);
										pyPose_Poses[pose_idx]->len=pyPose_PoseSize;
									}
									//now do a copy
									uint16_t* uint16p=pyPose_Poses[pose_idx]->values;
									for (uint8_t i=0;i<pyPose_PoseSize;i++) {
										uint16p[i]=rx_vect.at(3+i*2)|(rx_vect.at(4+i*2)<<8);
									}
									if (debug) {
										std::cout << "New pose received." << std::endl;
										std::cout << "  * length: "<< (int)pyPose_PoseSize  << std::endl;
										std::cout << "  * id    : "<< pose_idx  << std::endl;
										std::cout << "  * data  : "<<  std::endl;
										std::cout << "          : ";
										for (uint8_t i=0;i<pyPose_PoseSize;i++) {
											std::cout << uint16p[i] << ",";
										}
										std::cout <<  std::endl;
									}
									tx_vect.push_back(ZMQ_ERR_NO_ERROR);
									tx_vect.push_back(pose_idx);
									tx_vect.push_back(pyPose_PoseSize);
								} else {
									tx_error_code=ZMQ_ERR_INVALID_PARAMETERS;
								}
							} else {
								tx_error_code=ZMQ_ERR_INVALID_PARAMETERS;
							}
						} else {
							tx_error_code=ZMQ_ERR_INVALID_ID;
						}
						break;
					case PYPOSE_LOAD_SEQUENCE:
						//zmq-message: <cmd>,<id>,<pose_id>,<delay_L>,<delay_H>,<???>,<???>,<???>
						if (rx_vect.at(1)==PYPOSE_ID) {
							uint8_t no_elements=((uint16_t)rx_vect.size()-2)/3;
							seq_idx=0;
							if (!pyPose_Sequences[seq_idx]) {
								pyPose_Sequences[seq_idx]=(pypose_sequence_t*)malloc(sizeof(pypose_sequence_t));
								pyPose_Sequences[seq_idx]->len=0;
								pyPose_Sequences[seq_idx]->parts=NULL;
							}
							pypose_sequence_t* cSeq=pyPose_Sequences[seq_idx];
							if ((cSeq->parts) && (cSeq->len != no_elements)) {
								//sequence size not corret
								free(cSeq->parts);
								cSeq->parts=NULL;
								cSeq->len=0;
							}
							if (not cSeq->len) {
								cSeq->len=no_elements;
								//create new memory
								cSeq->parts=(pypose_seq_part_t*)malloc(sizeof(pypose_seq_part_t)*no_elements);
							}
							for (uint8_t i=0;i<no_elements;i++) {
								cSeq->parts[i].pose_id = rx_vect.at(2+3*i);
								cSeq->parts[i].delay = rx_vect.at(3+3*i)|(rx_vect.at(4+3*i)<<8);
							}
							if (debug) {
								std::cout << "New sequence received." << std::endl;
								std::cout << "  * length: "<< (int)no_elements  << std::endl;
								std::cout << "  * id    : "<< seq_idx  << std::endl;
								std::cout << "  * data  : "<<  std::endl;
								std::cout << "          : ";
								for (uint8_t i=0;i<no_elements;i++) {
									std::cout << (int)(cSeq->parts[i].pose_id) << ":" << (int)(cSeq->parts[i].delay) << " | ";
								}
								std::cout <<  std::endl;
							}
									
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
							tx_vect.push_back(seq_idx);
							tx_vect.push_back(no_elements);
						} else {
							tx_error_code=ZMQ_ERR_INVALID_ID;
						}
						break;
					case PYPOSE_PLAY_SEQUENCE:
						//zmq-message: <cmd>,<id>
						if (rx_vect.at(1)==PYPOSE_ID) {
							if (debug) {
								std::cout << "Nothing to do for load sequence." << std::endl;
							}
							tx_vect.push_back(ZMQ_ERR_NO_ERROR);
						} else {
							tx_error_code=ZMQ_ERR_INVALID_ID;
						}
						break;
					case PYPOSE_LOOP_SEQUENCE:
						if (rx_vect.at(1)==PYPOSE_ID) {
						} else {
							tx_error_code=ZMQ_ERR_INVALID_ID;
						}
						break;
					case PYPOSE_TEST:
						if (rx_vect.at(1)==PYPOSE_ID) {
						} else {
							tx_error_code=ZMQ_ERR_INVALID_ID;
						}
						break;
#endif
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
#ifdef ENABLE_PYPOSE_COMMANDS
	//TODO: destroy threads
#endif
	return 0;
}

