#include<stdio.h>
#include "my_fread.h"
#include "demuxbuf.h"
#include "bbdmux.h"
#include "parse.h"


/* my_fread.cpp
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
 * AC3DEC_WAVE
 * -----------
 *  v0.04 - added rudimentary AC3 frame-length checking/syncing
 *          to reject excessively-short frames *BEFORE* they are passed
 *          to ac3dec
 *          also, changed my_fread()'s mode of operation for g_fUseDemuxer == 0,
 *          The demuxQ is *always* used now, even when reading a normal *.AC3
 *          file. (The new frame-checker requires the demuxQ.)
 *  v0.02 - first release
 *          wrapper function for fread()
 *
 *  This file provides functions which "mediate" read-requests from
 *  ac3dec's bitstream_load(), and write-operations from demux_ac3(). 
 *
 *  The demux-buffer (g_ac3demuxQ) is a C++ object, the *only* C++ object
 *  in this entire package.  
 *
 */ 

demux_buffer g_ac3demuxQ;  /* demux-queue, output is AC3-stream */


/* both these variables are declared in *.c source-files.  Since
   *this* source-file is C++, we have to tell the compiler that
   the externals reference C-declared vars, to avoid name-mangling. */
extern "C" FILE* g_pInfile; /* declared in decode.c */
extern "C" int g_fUseDemuxer; /* declared in decode.c */


/* When the demuxer is in use, this handler is called when 
   the demux-queue runs out of data.
   This autofillHandler then calls the demux_ac3() function,
   which *buffer, 
   and returns the #bytes it read */
long
my_autofillHandlerDemuxAC3( unsigned char buffer[], long *cbData )
{
	long bytes_pushed=0, pulled, i;
	unsigned char syncword[2];
	int fSyncFound;

	(*cbData) = 0; /* demux_ac3() calls push_data() directly */

	if ( g_pInfile )
		bytes_pushed = demux_ac3( g_pInfile );

	if ( bytes_pushed<=0 ){
		bytes_pushed = 0;
		g_pInfile = 0;
		//printf( "THE END\n");		// no more data, we are at the end of file
	}

	return bytes_pushed;
}


/* when the demuxer is NOT in use, this handler is called when
   the demux-queue runs out of data.  This autofillHandler then
   calls the fread() function.
*/
long
my_autofillHandlerNoDemux( unsigned char buffer[], long *cbData )
{
	/* unlike the DemuxAC3 handler, this handler reads data into
	   buffer[].  Then it sets (*cbData) = #bytes read.  This
	   informs the demuxQ to manually copy buffer[] into the
	   queue. */
	if ( buffer && buffer )
		(*cbData ) = fread( buffer, 1, 2048, g_pInfile );
	return 0; /* Our handler didn't actually push any data
				 into the queue.  Rather, we fread() data
				 into the buffer[].  demuxQ will take care
				 of this data. */
}


extern void ResetFastRead( void );

void reset_autofillhandler( void )
{
	if ( g_fUseDemuxer ) /* g_fUseDemuxer declared in decode.c */
		g_ac3demuxQ.set_autofill_handler( my_autofillHandlerDemuxAC3 );
	else
		g_ac3demuxQ.set_autofill_handler( my_autofillHandlerNoDemux );
}



/* ac3dec's bitstream_load() function uses fread() to read the
   ac3 bitstream from a file. my_fread() is a "wrapper" function;
   it redirects those reads to ac3demuxQ.  The ac3demuxQ
   fulfills the read requests.  (When ac3demuxQ underruns, it
   automatically calls its refill-handler, which in turn calls
   demux_ac3().)
   (The demux_ac3() [bbdmux.cpp] puts data in ac3demuxQ.)
 */ 
size_t
my_fread( void *buffer, size_t size, size_t count, FILE *stream )
{
	static int first_iteration = 1;
	long cbData = 0; /* #valid bytes returned by demuxer */

	if ( first_iteration )
	{
		first_iteration = 0;
		if ( g_fUseDemuxer ) /* g_fUseDemuxer declared in decode.c */
			g_ac3demuxQ.set_autofill_handler( my_autofillHandlerDemuxAC3 );
		else
			g_ac3demuxQ.set_autofill_handler( my_autofillHandlerNoDemux );

		/* Turn on autofill function.  When the ac3demuxQ underruns,
		   it will automatically call my_autofillHandler to refill. */
		g_ac3demuxQ.set_autofill_mode( DMX_MODE_AUTOFILL );
	}


	/* The queue is always in use, When the demuxQueue runs out of data, it
	   automatically calls the user-specified fillHandler.  But we don't need
	   to worry about that. We just pull_data() from the queue as if its 
	   bottomless.
	*/
	cbData  = g_ac3demuxQ.pull_data( (unsigned char *) buffer, size*count );

	if ( cbData < 0 ){  /* error conditions are <0 */
		cbData = 0;
	} else
	{
		/* enforce size & count granularity */
		// round cbData to nearest (size)
		cbData = cbData - ( cbData % size ); 
		// now, cbData = # of returned "data-units" of (size)
		cbData = cbData / size;  
	}

	return cbData;
}



/* this function belongs in bbdbits.cpp, I have moved it here because of
   its C++ object usage (g_demuxbuffer)
   s_fwrite() is a "wrapper" for fwrite().  the functions in bbdbits.cpp
   call fwrite() to write demuxed-data to a file.  Instead,
   s_fwrite() places the demuxed-data into our ac3DemuxQ object. */
size_t
s_fwrite( const void *buffer, size_t size, size_t count, FILE *stream )
{
	long rc = 0;

	rc = g_ac3demuxQ.push_data( (unsigned char *) buffer, size * count );

	if ( rc < 0 )
		rc = 0;
	else
	{
		/* enforce size & count granularity */
		rc = rc - ( rc % size );  // round cbData to nearest (size)
		rc = rc / size;  // cbData = # of returned "data-units" of (size)
	}

	return rc;
}


