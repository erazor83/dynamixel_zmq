/*
 * Copyright (C) 2013 Alexander Krause <alexander.krause@ed-solutions.de>
 * 
 * Dynamixel ZeroMQ service
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef DYNAMIXEL_ZMQ_H

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
	DYNAMIXEL_RQ_ZMQ_ECHO									=0x100,
	
	DYNAMIXEL_RQ_SYNC_WRITE_WORDS					=0x183, 

#ifdef ENABLE_PYPOSE_COMMANDS
	DYNAMIXEL_RQ_READ_PYPOSE_PLAYER_STATE	=0x200, 
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
	ZMQ_ERR_PLAYER_RUNNING					= -1100,
	
} zmq_error_code_t;
#endif
