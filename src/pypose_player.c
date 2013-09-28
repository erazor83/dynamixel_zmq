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
#ifndef PYPOSE_PLAYER_C

#include "pypose_player.h"

pthread_cond_t pyPose_SequencePlayer_active = PTHREAD_COND_INITIALIZER;

void *pyPose_SequencePlayer(void* arg){
	pypose_player_ctx_t* player_ctx = (pypose_player_ctx_t*)arg;
	pypose_sequence_t* cSequence=NULL;
	uint8_t cSequence_id;
	uint8_t cpart_idx;
	uint8_t pose_id;
	uint16_t data[100];
	
	pypose_pose_t* cPose=NULL;
	
	int16_t dynamixel_ret;
	while(true) {
		//pthread_mutex_lock(&player_ctx->lock);
		while (player_ctx->state == PP_STATE_STOPPED) {
			if (player_ctx->debug) {
				std::cout << "Player stopped..." << std::endl;
			}
			pthread_cond_wait(&player_ctx->cond, &player_ctx->lock);
		}
		//pthread_mutex_unlock(&player_ctx->lock);

		std::cout << "Player started..." << std::endl;

		cSequence_id=player_ctx->sequence_id;
		if ((cSequence_id<PYPOSE_MAX_SEQUENCE_COUNT) && 
				((player_ctx->sequences[cSequence_id])!=NULL)) {
			cSequence=player_ctx->sequences[cSequence_id];
		} else {
			std::cout << "Sequence not defined!" << std::endl;
			cSequence=NULL;
		}
		
		if (cSequence!=NULL) {
			do {
				if (player_ctx->debug) {
					std::cout << "Playing sequence " << (int)player_ctx->sequence_id << std::endl;
				}
				for (cpart_idx=0; cpart_idx<cSequence->len; cpart_idx++) {
					pose_id=cSequence->parts[cpart_idx].pose_id;
					if (player_ctx->debug) {
						std::cout << "Current pose: " << (int)pose_id << std::endl;
						std::cout << "delay: " << (int)cSequence->parts[cpart_idx].delay << std::endl;
					}
					cPose=player_ctx->poses[pose_id];
					
					if ((cPose!=NULL) && (cPose->values!=NULL)) {
						for (uint8_t i=0;i<(cPose->len);i++) {
							data[i*2]=i+1;
							data[i*2+1]=cPose->values[i];
						}

						//write pose
						dynamixel_ret=dynamixel_sync_write_words(
							player_ctx->dynamixel_ctx,
							DYNAMIXEL_R_GOAL_POSITION_L,				/*register*/
							player_ctx->poses[pose_id]->len,		/*id-count*/
							1,																	/*word_count*/
							data
						);
					} else {
						std::cout << "No values found for pose " << (int)pose_id << std::endl;
					}
					usleep(1000*cSequence->parts[cpart_idx].delay);
					if (player_ctx->state == PP_STATE_STOPPED) {
						break;
					}
				}
			} while (player_ctx->loop);
		}
		if (!player_ctx->loop) {
			player_ctx->state=PP_STATE_STOPPED;
		}
	}
	//pthread_cond_broadcast(&player_ctx->cond);
	//
	return NULL;
}

void pypose_player_init(pthread_t *player_thread,pypose_player_ctx_t* pyPose_Player_Context) {

	pyPose_Player_Context->state=PP_STATE_STOPPED;
	pyPose_Player_Context->loop=false;
	pyPose_Player_Context->pose_id=0;
	pyPose_Player_Context->sequence_id=0;
	
	pthread_mutex_init(&pyPose_Player_Context->lock, NULL);
	pthread_cond_init(&pyPose_Player_Context->cond, NULL);
	
	pthread_create(player_thread, NULL, &pyPose_SequencePlayer, (void*)pyPose_Player_Context);
	
}
#endif
