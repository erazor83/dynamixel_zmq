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
#ifndef PYPOSE_H


#define PYPOSE_ID                 253
#define PYPOSE_MAX_POSE_SIZE       32
#define PYPOSE_MAX_POSE_COUNT     255
#define PYPOSE_MAX_SEQUENCE_COUNT 255

typedef struct {
	uint8_t len;
	uint16_t *values;
} pypose_pose_t;

typedef struct {
	uint8_t pose_id;
	uint16_t delay;
} pypose_seq_part_t;

typedef struct {
	uint8_t len;
	pypose_seq_part_t* parts;
} pypose_sequence_t;

void pypose_pose_init(void);
void pypose_seq_init(void);

#endif
