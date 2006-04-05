/* 
 *  bitstream.c
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
 *  AC3DEC_MCI
 *  ----------
 *  v0.02 modified bitstream_load() to pull-data from g_filedemux_queue.
 *        replaced instances of fread() with the wrapper function "my_fread()"
 *
 *        class demux_buffer g_filedemux_queue;
 *        changed fread(xxx, 1, 4, file ) -> fread( xxx, 4, 1, file )
 *        tells fread() to fetch "1 4-byte chunk" instead of "4 1-byte chunks"
 *           This apparently solves the end of stream crashing problems

 */

#include <stdlib.h>
#include <stdio.h>
#include "ac3.h"
#include "decode.h"
#include "crc.h"
#include "bitstream.h"

//My vego-matic endian swapping routine
#define SWAP_ENDIAN32(x)  ((((uint_8*)&x)[0] << 24) |  \
                         (((uint_8*)&x)[1] << 16) |  \
                         (((uint_8*)&x)[2] << 8) |   \
                         ((uint_8*)&x)[3])           


static long bitstream_load(bitstream_t *bs);


/* wrapper function for fread() */
/* the wrapper redirects fread() calls to the filedemux_queue.
   The demux-queue is in turn connected to a VOB-stream demuxer.*/
size_t
my_fread( void *buffer, size_t size, size_t count, FILE *stream );
/* function definition is in my_fread.cpp */


/* Fetches 1-32 bits from the file opened in bitstream_open */
uint_32
bitstream_get(bitstream_t *bs,uint_32 num_bits)
{
	uint_32 result;
	uint_32 bits_read;
	uint_32 bits_to_go;


	if(num_bits == 0)
		return 0;

	bits_read = num_bits > bs->bits_left ? bs->bits_left : num_bits; 

	result = bs->current_word  >> (32 - bits_read);
	bs->current_word <<= bits_read;
	bs->bits_left -= bits_read;

	if(bs->bits_left == 0){
		long loaded = bitstream_load(bs);
	}


	if (bits_read < num_bits)
	{
		bits_to_go = num_bits - bits_read;
		result <<= bits_to_go;
		result |= bs->current_word  >> (32 - bits_to_go);
		bs->current_word <<= bits_to_go;
		bs->bits_left -= bits_to_go;
	}
	
	bs->total_bits_read += num_bits;
	crc_process(result,num_bits);
	return result;
}

extern FILE* g_pInfile; /* declared in decode.c */

static long
bitstream_load(bitstream_t *bs)
{
	int bytes_read = 0;
	static long total_read = 0;

	/* fread changed to my_fread */
	/* v0.02, seems to eliminate crashes at the end of "chopped" streams */
	if ( bs->file ){
		bytes_read = my_fread(&bs->current_word,4,1,bs->file);
		bs->file = g_pInfile;
	}

	bytes_read = ( bytes_read << 2 ); /* multiply by 4 */

	bs->current_word = SWAP_ENDIAN32(bs->current_word);
	bs->bits_left = bytes_read * 8;

	//FIXME finishing up the stream isn't done too gracefully

	/*
	printf("\ntotal_read = %ld", total_read ); */
	total_read += bytes_read;
	
	if (bytes_read < 4){
		bs->done = 1;
		bs->file = 0;		// kill the file pointer since its the end
		//printf( "bitstream_load() End of stream\n");
	}

	return bytes_read;
}

/* Opens a bitstream for use in bitstream_get */
bitstream_t*
bitstream_open(FILE *file)
{
	bitstream_t *bs;
	
	if(!file)
		return 0;

	bs = malloc(sizeof(bitstream_t));
	if(!bs)
		return 0;

	bs->done = 0;

	/* Read in the first 32 bit word and initialize the structure */
	bs->file = file;

	reset_autofillhandler();

	bitstream_load(bs);


	return bs;
}


int bitstream_setfile(bitstream_t *bs, FILE *file )
{
	(bs->file = file);
	return (bs->done);
}

int bitstream_done(bitstream_t *bs)
{
	return (bs->done);
}

void 
bitstream_close(bitstream_t *bs)
{
	if( bs->file ){
		fclose(bs->file);
		bs->file = 0;
	}
	free(bs);
}
