/*
 *
 *  output.h
 *
 *  Based on original code by Angus Mackay (amackay@gus.ml.org)
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
 * AC3DEC_WAVE
 * -----------
 * v0.03 - modified output_play() to have 2 parameters, so it can check
 *         the # of channels
 */

#ifndef _OUTPUT_H
#define _OUTPUT_H 1

#define DECODED_CHUNK_SIZE 1024 /* size of a decoded audio chunk from ac3dec */

int output_open(int bits, int rate, int channels);
long output_play(stream_samples_t *samples, bsi_t *pbsi );
void output_close(void);

/* wavefile writing */
int output_wavefile_open(int bits, int rate, int channels, 
						 const char outname[] );
long output_wavefile_write(stream_samples_t *samples, bsi_t *pbsi );
long output_wavefile_close(const char outname[] );

#endif