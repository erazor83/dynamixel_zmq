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
#ifndef PYPOSE_C

#include "pypose.h"

pypose_pose_t* pyPose_Poses[PYPOSE_MAX_POSE_COUNT];

pypose_sequence_t* pyPose_Sequences[PYPOSE_MAX_SEQUENCE_COUNT];

uint8_t pyPose_PoseSize=0;


void pypose_pose_init(void) {
	uint16_t _pose_idx;
	for (_pose_idx=0; _pose_idx<PYPOSE_MAX_POSE_COUNT; _pose_idx++) {
		pyPose_Poses[_pose_idx]=NULL;
	}
}

void pypose_seq_init(void) {
	uint16_t _seq_idx;
	for (_seq_idx=0; _seq_idx<PYPOSE_MAX_SEQUENCE_COUNT; _seq_idx++) {
		pyPose_Sequences[_seq_idx]=NULL;
	}
}

#endif