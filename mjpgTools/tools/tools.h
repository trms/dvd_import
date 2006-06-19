// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// tools.h:		part of the mjpgTools Library																			 //
//				Header File for specific commands																		 //
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

#ifndef __TOOLS_H__
#define __TOOLS_H__

void Message(const char* lpText, ...);								// displays a message onto stderr or in a messagebox
void Message2(const char* lpTitle, const char* lpText, ...);		// displays a message onto stderr or in a messagebox and but uses a user-custom Title too

void infoMessage(const char* lpText, ...);							// displays a message onto stderr or in a messagebox
void warnMessage(const char* lpText, ...);							// displays a message onto stderr or in a messagebox
void errMessage(const char* lpText, ...);							// displays a message onto stderr or in a messagebox

void infoMessage2(const char* lpTitle, const char* lpText, ...);	// displays a message onto stderr or in a messagebox and but uses a user-custom Title too
void warnMessage2(const char* lpTitle, const char* lpText, ...);	// displays a message onto stderr or in a messagebox and but uses a user-custom Title too
void errMessage2(const char* lpTitle, const char* lpText, ...);		// displays a message onto stderr or in a messagebox and but uses a user-custom Title too

#ifdef MJPGTOOLS_INTERNALS // only share from within internal DLL-routines, not in 'old' C-routines
BOOL AskMessage(LPCTSTR lpMessage, LPCTSTR lpTitle);	// asks the question and return 'true' when the YES-button was pressed, 'false' otherwise
#endif // MJPGTOOLS_INTERNALS

// added since 1.5.0.0 -- progress-stuff
typedef enum {
	invalid		= 0,
	init,	// start
	reinit,	// re-initializes
	finish,	// end
	step
} lhk_progress_type_e;

#pragma pack(push, lhkTools, 1)
typedef struct _progress_struct_s { // a progressbar is intended not to use percents (NOT 0%..100%, but 0..initCount)
	unsigned short			version;	// indicates version of struct, curversion = '1' (rel. 1.5.0.0)
	lhk_progress_type_e		type;
	// valid in case of type::init
	const char*				initText;	// what will be processed
	long					initCount;	// 'maxRange' for a progressbar
	// valid for type::step
	const char*				stepText;	// what is currently done (should be a 'short' message)
	long					stepPos;	// new pos for a progressbar
} lhkProgress_t, *lhkProgress_p;
#pragma pack(pop, lhkTools)

#ifdef __cplusplus
extern "C" {
#endif

// these functions are also called from within 'C'-routines, so they need to have C-linkage

// functions used to transmit progress-data to calleer
void lhkProgress_Init(const void* owner, const unsigned msg, const char* initText, const long initCount);
void lhkProgress_ReInit(const void* owner, const unsigned msg, const char* initText, const long initCount);
void lhkProgress_Step(const void* owner, const unsigned msg, const char* stepText, const long stepPos);
void lhkProgress_End(const void* owner, const unsigned msg); // notifies caller that we're done

#ifdef __cplusplus
}
#endif


#endif // __TOOLS_H__
