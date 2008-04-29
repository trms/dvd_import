/*  demuxbuff.cpp -  implementation of class <demux_buffer>
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
 *
 *  AC3DEC_WAVE
 *  -----------
 *
 *  0.02 initial release
 *       the class <demux_buffer> serves as a queue between BBDMUX and 
 *       bitsteran_load() [bitstream.c]
 *
 
 *  The fill_queue() routine is pretty sloppy.
 *  The flush_queue() mechanism has not yet been implemented (not needed)
 */

#include <windows.h>
#include <stdio.h>
#include "demuxbuf.h"

// constructor, initialize variables and allocate queue-memory
demux_buffer::demux_buffer( void )
{
	// allocate buffer memory
	buffer_memory = (LPBYTE) malloc( DMX_BUFFER_SIZE );
	public_buffer = (LPBYTE) malloc( DMX_BUFFER_SIZE );
	in_pos = 0;
	out_pos = 0;
	cbstored = 0;
	af_mode = DMX_MODE_NOAUTOFILL;
	ae_mode = DMX_MODE_NOAUTOEMPTY;
	autofill_handler = NULL;	// function-call is NULL
	autoempty_handler = NULL;	// function-call is NULL

	if ( buffer_memory == NULL || public_buffer == NULL )
	{	// fatal error
		fprintf( stderr, 
			"\ndemux_buffer::demux_buffer() could not allocate memory");
		exit(1 );
	}
};

// destructor, free memory (if necessary)
demux_buffer::~demux_buffer(  )
{
	/* flush the buffer's contents, if DMX_MODE_AUTOEMPTY */
	if ( ae_mode == DMX_MODE_AUTOEMPTY )
	if ( buffer_memory )
		free( buffer_memory );
	if ( public_buffer )
		free( public_buffer );
};


/*
 * pull (remove) data from queue
 * if successful, returns #bytes pulled
 */
long
demux_buffer::pull_data( LPBYTE outdata, long cbrequest )
{
	long bytes_copied = 0, bytes_copied2 = 0, total_bytes_copied = 0 ;
	long bytes_remaining = cbrequest;
	long cbRight; // number of data bytes to the right of out_pos
	BOOL fendofsrc = FALSE;
	int getrc;	// return code from fill_queue*(

	cbRight = get_capacity() - out_pos ;  

	// does the request exceed the queue's current occupancy?
	// if so, refill the queue until it's full or there's no
	// more data
	if ( cbrequest > get_qsize() && af_mode == DMX_MODE_AUTOFILL ) 
	// yes, queue needs to be refilled
		getrc = fill_queue( cbrequest );  // push data into the queue
	else
	if ( cbrequest > get_qsize() )
		printf("\ndemux_buffer::pull_data() : no more data, no autofill ");

	// at this point, the buffer is either full, or there is no
	// more data.  
  
	// check if the desired data is "wrapped around" the
	// end and beginning of our buffer_memory space
	cbRight = get_capacity() - out_pos;
	bytes_remaining = cbrequest;
	bytes_copied = 0;
	if ( cbrequest > cbRight )
	{	// yes, must make two passes, 
		// this if-clause makes the "1st copy pass"

		bytes_copied = cbRight;  // bytes copied so far
		// copy the first chunk
		if ( memcpy( &outdata[ 0 ], &buffer_memory[ out_pos ], 
			bytes_copied ) != ( &outdata[ 0 ] ) )
		{
			fprintf( stderr, "demux_buffer::pull_data() copy failed!");
			exit( 1 );
		}

		bytes_remaining = cbrequest - bytes_copied;  
		cbstored -= bytes_copied;	// update byte counter
		out_pos = 0;	// we just emptied the "right side" of the queue,
						// thus queue's output index-pointer = 0
	}	// endif (end of "first copy pass")

	// Now, copy (bytes_remaining) bytes.  At this point, there is
	// no chance of the operation requiring a "wraparound" copy, because
	// the "1st copy pass" handled that special case.

	// If this is the 2nd pass, bytes_remaining was updated by the 1st pass,
	// otherwise (if this is the first pass) bytes_remaining = cbrequest
	bytes_copied2 = min( bytes_remaining, get_qsize() );
	if ( memcpy( &outdata[ bytes_copied ], &buffer_memory[ out_pos ],
		bytes_copied2 ) != ( &outdata[ bytes_copied ] ) )
	{
		fprintf( stderr, "demux_buffer::pull_data() copy failed!");
		exit( 1 );
	}
		
	cbstored -= bytes_copied2;
	out_pos += bytes_copied2; // out_pos will not "fall off the edge" of
		// the buffer, because we initially checked for this condition,
		// and took steps to make a "1st copy pass" 
	total_bytes_copied = bytes_copied + bytes_copied2;
		// total_bytes_copied can be < cbrequest

	if ( total_bytes_copied != cbrequest )
		printf("\ndemux_buffer::pull_data() request was %ld, could only supply %ld\n", 
			cbrequest, total_bytes_copied );

	return total_bytes_copied;	// return #valid bytes 
}; /* end of demux_buffer::pull_data() */


// if successful, returns #bytes copied
long
demux_buffer::push_data( LPBYTE indata, long cbSize )
{
	long bytes_copied = 0, bytes_copied2 = 0, total_bytes_copied = 0 ;
	long bytes_remaining = cbSize;
	long cbRight; // number of data bytes to the right of in_pos

	if ( get_qsize() > DMX_FILLTHRESHOLD )
		return FILL_QUEUEFULL;

	// check if the input-data's size exceeds the "right-side" of the
	// queue.  I.e. must the input-data be "wrapped around" the
	// end and beginning of our buffer_memory space?
	cbRight = get_capacity() - in_pos;
	bytes_remaining = cbSize;
	bytes_copied = 0;
	if ( cbSize > cbRight )
	{	// yes, two passes are required to copy the data. 
		// this if-clause makes the "1st copy pass"

		bytes_copied = cbRight;  // bytes copied so far
		// copy the first chunk
		if ( memcpy( &buffer_memory[ in_pos ], &indata[ 0 ], 
			bytes_copied ) != ( &buffer_memory[ in_pos ] ) )
		{
			fprintf( stderr, "demux_buffer::push_data() copy failed!");
			exit( 1 );
		}

		bytes_remaining = cbSize - bytes_copied;  
		cbstored += bytes_copied;	// update byte counter
		in_pos = 0;	// we just filled up to the "right side" of the queue,
						// thus queue's input index-pointer = 0
	}	// endif (end of "first copy pass")

	// Now, copy (bytes_remaining) bytes.  At this point, there is
	// no chance of the operation requiring a "wraparound" copy, because
	// the "1st copy pass" handled that special case.

	// If this is the 2nd pass, bytes_remaining was updated by the 1st pass,
	// otherwise (if this is the first pass) bytes_remaining = cbrequest
	bytes_copied2 = bytes_remaining ;
	if ( memcpy( &buffer_memory[ in_pos ], &indata[ bytes_copied ], 
			bytes_copied2 ) != ( &buffer_memory[ in_pos ] ) )
	{
		fprintf( stderr, "demux_buffer::push_data() copy failed!");
		exit( 1 );
	}
		
	cbstored += bytes_copied2;
	in_pos += bytes_copied2; // in_pos will not "fall off the edge" of
		// the buffer, because we initially checked for this condition,
		// making a "1st copy pass" as necessary
	total_bytes_copied = bytes_copied + bytes_copied2;

	// total_bytes_copied must == cbSize, otherwise something went wrong
	if ( total_bytes_copied != cbSize )
	{
		fprintf( stderr, "\ndemux_buffer::push_data() transfer-size mismatch!");
		exit( 1 );
	}

	//printf("\n::push_data() returned %ld cbstored=%ld", total_bytes_copied,
	// 	get_qsize() );
	return total_bytes_copied;	// return #valid bytes 

}; /* demux_buffer::push_data() */


// originally fill_queue() assumed that the user-supplied (*fill_handler)()
// would store data in *buffer, and return the #bytes written.
//
// Later on, I realized that BBDMUX demux_mpeg2() calls fwrite() many
// individual times.  So I rerouted the fwrite() calls to push_data().
// This causes a problem, though.  During a single execution of demux_mpeg2(),
// push_data() is called many times [1 byte transferred each time.]
//
// As the queue approahces its FILLTHRESHOLD, some of these fwrite()
// will fail, resulting in catastrophic data-loss.  At a later point, I
// may reroute fwrite() to a 2nd "temp" queue, which collects data 
// for the duration of demux_mpeg2()'s execution.  At the end of the
// call, this temp-queue would be flushed into the primary demux_queue.
//
// For now, to guard against the FILLTHRESHOLD problem, I have
// programmed fill_queue() to stop filling after it meets cbrequest.
// This makes the queue rather inefficient, because fill_queue() is called
// much more often than necessary, but it ensures proper BBDMUX operation.
//
long
demux_buffer::fill_queue( long cbrequest )
{
	// cbrequest, minimum requested data
	long pushrc = 0;
	long St = -1;
	long cbData = 0;

	if ( autofill_handler == NULL )
	{
		fprintf( stderr, "\ndemux_buffer::fill_queue() : " );
		fprintf( stderr, "(*autofill_handler)() not valid!" );
		exit(1);
	}

	while ( (get_qsize() < cbrequest) && pushrc != FILL_QUEUEFULL 
		&& pushrc != FILL_NODATA && get_qsize() != St ) 
	{	// yes, it does not exceed
		
		// call user-supplied function to fill buffer
			//printf("\ndemux_buffer::fill_queue() : calling autofill");
			St = get_qsize();
			pushrc = (*autofill_handler)( public_buffer, &cbData ) ;
			// pushrc == #bytes returned
			if ( pushrc > (get_capacity() - get_qsize()) )
			{
				fprintf( stderr, "\ndemux_buffer::fill_queue() : " );
				fprintf( stderr, "too much data! (pushrc=%d)", pushrc );
				return FILL_OK;
			}
//			printf( " (*)autofill_handler() returned %ld (c=%ld)", pushrc,
//				St );
			
			if ( cbData > 0 )  // any data to push into our queue?
				pushrc = push_data( public_buffer, cbData );
			else
			if ( pushrc < 0 ){
				pushrc = FILL_NODATA;	// no, out of data
			}
	}

//	printf("\nfill_queue() qsize =%ld", get_qsize() );
	return FILL_OK;
}; /* endof fill_queue( long cbrequest ) */


/* get the user's handler-function.  The user's handlerFunc() is
   called whenever demux_buffer calls fill_data() */
int
demux_buffer::set_autofill_handler( handlerFunc user_handler )
{
	if ( user_handler )
		autofill_handler = user_handler;
	else
		fprintf( stderr, "\ndemux_buffer::set_autofill_handler() : NULL !");

	return 0;
}


/* get the user's handler-function.  The user's handlerFunc() is
   called whenever demux_buffer calls flush_data() */
int
demux_buffer::set_autoempty_handler( handlerFunc user_handler )
{
	if ( user_handler )
		autoempty_handler = user_handler;
	else
		fprintf( stderr, "\ndemux_buffer::set_autoempty_handler() : NULL !");

	return 0;
}


int
demux_buffer::set_autofill_mode( int new_mode )
{
	switch( new_mode )
	{
		case DMX_MODE_NOAUTOFILL :
			af_mode = new_mode;
			break;
		case DMX_MODE_AUTOFILL :
			af_mode = new_mode;
			break;
		default:
			fprintf(stderr, "\ndemux_buffer::set_autofill_mode(%d) invalid mode!",
				new_mode );
			return 0;
	}

	return new_mode;

}


int
demux_buffer::set_autoempty_mode( int new_mode )
{
	switch( new_mode )
	{
		case DMX_MODE_NOAUTOEMPTY :
			ae_mode = new_mode;
			break;
		case DMX_MODE_AUTOEMPTY:
			//ae_mode = new_mode; not implemented yet
			fprintf(stderr, "\ndemux_buffer::set_autoempty_mode(%d) not implemented!",
				new_mode );

			break;
		default:
			fprintf(stderr, "\ndemux_buffer::set_autoempty_mode(%d) invalid mode!",
				new_mode );
			return 0;
	}

	return new_mode;

}


// wipe the queue's state to clean
void
demux_buffer::reset( void )
{
	cbstored = 0;
	in_pos = 0;
	out_pos = 0;
	autofill_handler = NULL;
	af_mode = DMX_MODE_NOAUTOFILL;
	ae_mode = DMX_MODE_NOAUTOEMPTY;
}
