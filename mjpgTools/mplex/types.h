// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// types.h : part of the mjpgTools Library																				 //
// --------------------------------------------------------------------------------------------------------------------- //
// Copyleft(c)2002 by Laurens KOEHOORN <https://sourceforge.net/sendmessage.php?touser=240832>							 //
//																														 //
// This library is free software; you can redistribute it and/or modify it under the terms of the						 //
// GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License,	 //
// or (at your option) any later version.																				 //
//																														 //
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied	 //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.														 //
// See the GNU Lesser General Public License for more details.															 //
//																														 //
// You should have received a copy of the GNU Lesser General Public License along with this library; if not,			 //
// write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __TYPES_H__
#define __TYPES_H__

#ifdef _DEBUG
#define DEBUG_AND_INFO_2CONSOLE	// undef this one, if you're debugging without a console
#endif

typedef unsigned __int8		uint8_t;
typedef unsigned __int16	uint16_t;
typedef unsigned __int32	uint32_t;
typedef unsigned __int64	uint64_t;
typedef signed __int8		int8_t;
typedef signed __int16		int16_t;
typedef signed __int32		int32_t;
typedef signed __int64		int64_t;

#define MPEG_FORMAT_MPEG1		0
#define MPEG_FORMAT_VCD			1
#define MPEG_FORMAT_VCD_NSR		2
#define MPEG_FORMAT_MPEG2		3
#define MPEG_FORMAT_SVCD		4
#define MPEG_FORMAT_SVCD_NSR	5
#define MPEG_FORMAT_VCD_STILL	6
#define MPEG_FORMAT_SVCD_STILL	7
#define MPEG_FORMAT_DVD			8

#define MPEG_FORMAT_FIRST		0
#define MPEG_FORMAT_LAST		8

#define MPEG_STILLS_FORMAT(x) (x==MPEG_FORMAT_VCD_STILL||x==MPEG_FORMAT_SVCD_STILL)

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code -- should be set at 11 at all time (to let the exception-handler work properly)
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//          00 = EXIT_SUCCESS
//          01 = EXIT_FAILURE
//          10 = EXIT_WARNING
//
//  In this version of the lib, Exceptions will be raised at abnormal termination instead of using
//  the 'exit()' and '_endthread()' functions
//
#define USER_EXCEPTION_SUCCESS				0xE0000000
#define USER_EXCEPTION_FAILURE				0xE0000001
#define USER_EXCEPTION_WARNING				0xE0000002

extern uint8_t				verbose;		// 0=silent, 1=info, 2=debug
extern const char*			msgTitle;		// NULL or a custom title, set by a routine
extern const char*			defTitle;		// "" : usually a name that identifies the class



void	err_Message(const char* lpFilter, ...);						// ERROR - STOP - HAND
void	warn_Message(const char* lpFilter, ...);					// EXCLAMATIOM - WARNING
void	info_Message(const char* lpFilter, ...);					// INFO
void	debug_Message(const char* lpFilter, ...);					// INFO -- debug
int		ask_Message(uint32_t buttons, const char* lpFilter, ...);	// QUESTION

void	error_exit(void); // will raise an exception, so we realy break out operation
void	error_exit_msg(const char* lpFilter, ...);	// Raises an exception after an error-message has been showed

#endif