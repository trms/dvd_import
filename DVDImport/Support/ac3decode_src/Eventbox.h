/*
 *
 *  eventbox.h
 *
 *
 *  This file has been contributed to ac3dec, a free Dolby AC-3 stream decoder.
 *	
 *  ac3dec is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  ac3dec is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 *  AC3DEC_WAVE
 *  -----------
 *
 *  eventbox collects several related user-declared events in one
 *  convenient struct.  (This file came from another project...)
 *
 *  only output_winmci.c uses these definitions!
 *
 *    08/16/99 - first release
 */

#include<windows.h>
#include<assert.h>

#ifndef __EVENTBOX__H
#define __EVENTBOX__H 1

// eventbox.h
// 
// Eventbox is a struct that organizes windows messages and application
// generated messages into "command" and "status" categories, respectively.
//

//

// When the wave-device (play or record) finishes using a buffer, the
// device generates a WOM_DONE message (sent to our callback function.)
//
// The callback function does 2 things (in this order) :
//		1) stores the pointer to the released buffer (*lpParam)
//		2) signals the event  (sets the hE handle)

struct my_event_struct {
	HANDLE hE;	// the event
	LPVOID lpParam;	// the Parameter
};

typedef struct my_event_struct my_event;

struct command_type {
		my_event EventRecStart;  // EventBox.command.EventRecStart
		my_event EventRecStop;
		my_event EventPlayStart;
		my_event EventPlayStop;
};

struct status_type {
		my_event EventPBDone; // EventBox.sttatus.EventPBDone
		my_event EventRBDone;	// REC buffer done
		my_event EventPBReady;	// PLAY buffer ready
		my_event EventRBReady;	// REC buffer ready
};

struct tEventBox {

	// commands are program/user issued operations...
	struct command_type command;

	// status are callbacks or I/O related events
	struct status_type status;
};

/*
typedef struct { 
    LPSTR  lpData; 
    DWORD  dwBufferLength; 
    DWORD  dwBytesRecorded; 
    DWORD  dwUser; 
    DWORD  dwFlags; 
    DWORD  dwLoops; 
    struct wavehdr_tag * lpNext; 
    DWORD  reserved; 
} WAVEHDR; 
 */


#endif
