#ifndef _DEMUXBUF
#define _DEMUXBUF 1
#include <windows.h>

/*
 * Simple queue to integrate AC3DEC with BBDMUX 
 *
 ******************************************************
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
 *******************************************************
 * This DEMUXqueue holds data of type (unsigned char[]).  It sits
 * between the MPEG2V12 getbits() package and BBDMUX's stream
 * output ( fwrite() ) system.
 *
 * QUEUE operation ( class demux_buffer )
 * --------------------------------------------
 *
 * This is a very rudimentary (FIFO) queue for bytedata.  
 * 
 * The queue has two ports :  input and output
 *  They are accessed through two public functions:
 *
 *     push_data() - put data into queue, if queue is full then
               returns QUEUE_FULL, otherwise if the queue's storage overfills
 *     pull_data() - report FILL_NODATA if the queue underruns
 *
 * otherwise, either function will report the #bytes actually stored/retrieved
 *
 * There is one "twist" to this queue.  In addition to push/pull
 * operation, the queue can be set to "DMX_MODE_AUTOFILL" mode.  The user 
 * supplies a "fill-handler" (a function-call), and the queue automatically 
 * calls the fill-handler (to refill the queue) as necessary.
 *
 *
 * BBDMUX and QUEUEing programming issues 
 * --------------------------------------------
 *
 * A standard queue works in complimentary "push/pull" fashion : the
 * user puts data into the queue for storage, then extracts data as necessary.
 * BBDMUX is loop-driven.  A while() loop executes until all desired data
 * from the input-file has been written to an output file.  This implies
 * one call to BBDMUX will flood the queue with data.  Clearly, flow-control 
 * in necessary.
 *
 * I changed the queue's operation from "complimentary push/pull"
 * to "demand (pull) driven."  It is still a queue, except that an
 * automatic fill-handler prevents in-stream buffer underruns (by assumption,
 * AC3DEC hammers the queue's pull_data() function.)
 *
 * When the queue empties (or runs low), pull_data() automatically calls
 * a fill_queue() to refill the queue.  The fill_queue() function is
 * responsible for refilling the queue.  No part of the application (AC3DEC)
 * directly calls push_data().  Thus, the queue is essentially "demand driven."
 *
 * To support this queuing mechanism, I modified BBDMUX so that it executes
 * the while() loop once, then exits.  (Actually, the while() loop runs until
 * it returns >0 bytes of data.  Under normal conditions the loop could execute
 * anywhere from 0 to 4-5 times before fulfilling this requirement and 
 * returning to the caller.)
 *
 *
 * As AC3DEC's bitstream_load() pulls data from DEMUXqueue, the queue
 * gradually empties, until a refill is necessary. The "refill" mechanism 
 * determines the queue's mode of operation.
 *
 * Revision History
 * ----------------------------
 * v0.02 08/17/99 added functions for an automatic flushing mechanism.
 *       when fully implemented, DMX_MODE_AUTOEMPTY will automatically
 *       call a user-supplied autoempty_handler, if the queue overflows.
 *       This allows the queue to be operated in "supply-driven" mode.
 *
 *
 */

#define DMX_BUFFER_SIZE	32768
#define DMX_FILLTHRESHOLD 16384
#define FILL_NODATA -1
#define FILL_QUEUEFULL -2			// fatal error
#define FILL_QUEUEOVERTHRESHOLD -3	// warning
#define FILL_OK 0


/* flags for demux_buffer's operating mode */
#define DMX_MODE_AUTOFILL 1
#define DMX_MODE_NOAUTOFILL 2
#define DMX_MODE_AUTOEMPTY 3
#define DMX_MODE_NOAUTOEMPTY 4

/* autofill handler declaration */
typedef long (*handlerFunc)( unsigned char[], long *);	// user handler function
	// "handlerFunc" is now of type (*)(...)



/* class declaration */

class demux_buffer
{
public: 
	// constructor & destructor
	demux_buffer( void );
	~demux_buffer();

	// public member functions (public interface)
	long get_qsize( void )
		{ return cbstored; };

	long get_capacity( void )
	{ return DMX_BUFFER_SIZE; };

	long push_data( LPBYTE indata, long cbSize );
	long pull_data( LPBYTE outdata, long cbrequest );
	void reset( void );

	int set_autofill_handler( handlerFunc );
	int set_autofill_mode( int new_mode );

	int set_autoempty_handler( handlerFunc );
	int set_autoempty_mode( int new_mode );
	

	LPBYTE public_buffer;	// public buffer, returned as parameter

private :	
	LPBYTE buffer_memory;	// internal storage
	
	long in_pos;	// index to *TOP* of queue (incoming data written
					// to top of queue 
	long out_pos;	// index to *BOTTOM* of queue (data extracted from bottom)
	long cbstored;	// byte-count of total #bytes-stored in this queue

	long fill_queue( long cbrequest ); // "pull" data from the source, 
				// calls push_data() if (af_mode == AUTOFILL), fill_queue() 
				// is called after an underflow condition

	long empty_queue( long cbrequest ); // "push" data to autoempty handler,
				// calls push_data() if (ae_mode == AUTOEMPTY), empty_queue()
				// is called after an overflow condition

	long (*autofill_handler)( unsigned char *buffer, long *cbData );
	long (*autoempty_handler)( unsigned char *buffer, long *cbData );

	long af_mode;// AUTOFILL policy, specifies queue's policy when a pull-
				// operation empties the queue.  The queue can be setup to
				// call an external function when pull_data() causes a
				// queue_underflow condition.  
	long ae_mode;// AUTOEMPTY policy, specifies queue's policy when a push-
				// operation overflows the queue.  The queue can be setup to
				// call an external function when push_data() causes a
				// queue_overflow condition.  
};
#endif

/*the autofill handler should do these things :
 
   1) place data into *buffer
   2) set *cbData to the #valid bytes in *buffer
   3) return a value == to the total# bytes supplied

   cbData and the return-value need not be equal.  For example, some
   fill_handlers may directly call push_data() [and ignore the *buffer
   pointer], in which case cbData should be set to 0.
*/
