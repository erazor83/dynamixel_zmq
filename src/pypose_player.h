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
#ifndef PYPOSE_PLAYER_H

/* stuff for sequence playing */
#include <pthread.h>
#include <time.h>

typedef enum {
	PP_STATE_STOPPED,
	PP_STATE_RUNNING,
} pypose_player_state_t;

typedef struct {
	bool											debug;
	pypose_player_state_t			state;
	bool											loop;
	uint8_t										pose_id;
	uint8_t										sequence_id;
	dynamixel_t*							dynamixel_ctx;
	pypose_pose_t**						poses;
	pypose_sequence_t** 			sequences;

	pthread_mutex_t						lock;
	pthread_cond_t						cond;
} pypose_player_ctx_t;


void *pyPose_SequencePlayer(void* arg);

void pypose_player_init(pypose_player_ctx_t*);
void pyPose_SequencePlayer_cmd(uint8_t command);
#endif
