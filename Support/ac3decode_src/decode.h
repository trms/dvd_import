/* 
 *  decode.h
 *
 *	Copyright (C) Aaron Holtzman - May 1999
 *
 *  This file is part of ac3dec, a free Dolby AC-3 stream decoder.
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
 *   v0.04 decode_sanity_check() now returns a value (int)
 *         helps decode.c decide whether or not to re-sync AC3 frame
 *         #define ERR_RESYNC_THRESHOLD = minimum # errors to trigger frame 
 *          resync inside decode.c
 */
#ifndef _DECODE_H
#define _DECODE_H 1


typedef struct stream_coeffs_s
{
	float fbw[5][256];
	float lfe[256];
} stream_coeffs_t;

typedef struct stream_samples_s
{
	float channel[6][512];
} stream_samples_t;

#define DECODE_MAGIC_NUMBER 0xdeadbeef
/* error thresholds, if # detected errors exceeds the threshold,
   decode.c forces a resync */
#define ERR_RESYNC_THRESHOLD 2
#define ERR_MANT_THRESHOLD 3
#define ERR_EXP_THRESHOLD 3

/*
void decode_sanity_check_init(void);
void decode_sanity_check(void);
*/

void decode_sanity_check_init(void);
long decode_sanity_check(void);

#endif