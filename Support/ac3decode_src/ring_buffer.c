/*
 *
 *  ring_buffer.c
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
 *
 *  AC3DEC_WAVE
 *  -----------
 *
 *  added functions to report current read-index and write-index
 *  these are the buffer's HEAD and END (if you think of the buffer
 *  as a queue.)
 *
 *  rb_write_get_index( void ) - returns write index (or returns RB_FULL )
 *  rb_read_get_index( void )  - returns read index (or returns RB_EMPTY )
 *  
 */

//FIXME separate the typedefs from ac3.h
typedef signed short sint_16;

/*#include <unistd.h>*/
#include "ring_buffer.h"

#define INC_INDEX(x) (((x) + 1) % NUM_BUFFERS)

// The buffer that was just written into
static volatile int write_index;
// The buffer that was just read from
static volatile int read_index;
static sint_16 ring_buf[NUM_BUFFERS][BUFFER_SIZE];

void
rb_init(void)
{
	write_index = 0;
	read_index = 0;
}

sint_16* rb_begin_read(void)
{
	if(read_index == write_index)
		return 0;
	return ring_buf[INC_INDEX(read_index)];
}

/* returns RB_EMPTY -1, if no buffer in the data */
sint_16 rb_read_get_index( void )
{
	if ( read_index == write_index )
		return RB_EMPTY;
	else
		return INC_INDEX(read_index);
}

void rb_end_read(void)
{
	read_index = INC_INDEX(read_index);	
}

sint_16* rb_begin_write(void)
{
	if(read_index == INC_INDEX(write_index))
		return 0;
	return ring_buf[INC_INDEX(write_index)];
}

/* returns RB_FULL -2, if buffer cannot accept more data */
sint_16 rb_write_get_index( void )
{
	if(read_index == INC_INDEX(write_index))
		return RB_FULL ;
	else
		return INC_INDEX(write_index);
}


void rb_end_write(void)
{
	write_index = INC_INDEX(write_index);	
}
