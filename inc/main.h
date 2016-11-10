/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'plan-gui' Graphical User Interface
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] main: FlexSEA Plan project: GUI app to control FlexSEA slaves
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-09 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef INC_MAINH_H
#define INC_MAINH_H

#ifdef __cplusplus
extern "C" {
#endif

//****************************************************************************
// Include(s)
//****************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>  //ToDo useful?
#include <time.h>
#include "trapez.h"
#include "flexsea_board.h"
#include "flexsea_system.h"
#include "flexsea.h"

//****************************************************************************
// Shared variable(s)
//****************************************************************************

//extern char *fake_argv[];
//extern const char *delims;

//****************************************************************************
// Public Function Prototype(s):
//****************************************************************************



//****************************************************************************
// Definition(s):
//****************************************************************************

#ifdef SINGLE_COMMAND
#ifdef MULTIPLE_COMMANDS
#error "Pick one Command option!"
#endif
#endif

//Multiple commands:
#define MAX_COMMAND_LEN 		256
#define MAX_ARGS 				12

//IO functions, Reset:
#define RESET_PORT				9
#define RESET_PIN				12

//#define NULL					0

//Timers:
//======

#define TIM_FREQ_TO_P(f)		(1000/f)	//f in Hz, return in ms

//Stream:
//======

#define STREAM_MIN_FREQ			1
#define STREAM_MAX_FREQ			1000
#define STREAM_DEFAULT_FREQ		35

#ifdef __cplusplus
}
#endif

#endif
