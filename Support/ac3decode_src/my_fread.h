#ifndef _MY_FREAD
#define _MY_FREAD 1
// my_fread.h
// 
// AC3DEC_WAVE
// -----------
/*  This file has been contributed to ac3dec, a free Dolby AC-3 stream decoder.
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
 */

extern "C" size_t my_fread( void *buffer, size_t size, size_t count, FILE *stream );

extern "C" void reset_autofillhandler( void );

/* explicitly declare a C-function.  This is necessary because
   my_fread.cpp is C++ source, but functions which call my_fread.cpp
   may be plain C. */

typedef struct ac3_syncinfo_t {
	unsigned syncword : 16;
	unsigned crc1 : 16;
	unsigned fscod : 2;
	unsigned frmsizecod : 6;
} ac3_syncinfo;

typedef union si_t {
	ac3_syncinfo_t syncinfo;
	unsigned char byte[5];
} si_union;


#endif
