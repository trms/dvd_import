/*
 *
 *  output_winwave.c (formerly output_winmci.c)
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
 *  AC3DEC_WAVE
 *  -----------
 *  As of v0.03, output_winmci.c is now output_winwave.c
 *     The old name caused some confusion, since "MCI" implies 
 *     media control interface, which is a completely separate API
 *     altogether.  AC3DEC_WAVE does not use any MCI function calls.
 *
 *
 *  this file implements WAVE audio-playback through Windows' waveOut library
 *  output_winmci.c requires eventbox.h
 * v0.01 first release
 * v0.02 now prints the string waveoutcaps.szPname (name of wave device)
 *       not very interesting, since output_open() uses WAVE_MAPPER
 * v0.03 downmixing support, modified output_play() to accept 2 parameters
 *       2nd parameter is bsi_t *bsi
 * v0.05 moved the QUIT feature to decode.c ( main() )
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>

/* stuff for Windows */
#include <windows.h>
#include <mmsystem.h> /* waveOut library functions */
#include <process.h> /* events & other Windows messaging junk */
#include <conio.h> /* keyboard input _kbhit() , _getch() */

/*#include <unistd.h>*/

#include "ac3.h"
#include "decode.h"
#include "debug.h"
#include "output.h"
#include "ring_buffer.h"
#include "eventbox.h"
#include "downmix.h"

#define BUFFER_SIZE 32768
#define KEY_PAUSE_TOGGLE 0x20  /* <SPACEBAR> */

/* private functions/structures */

void parse_keyboard( void );
void CALLBACK MCICallBack( HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, 
	DWORD dwParam1, DWORD dwParam2);
void init_wavehdr( LPWAVEHDR lphdr );
void parse_keyboard_error( MMRESULT mmresult );
void parse_keyboard( void  );

static struct tEventBox EventBox;	/* a Windows event, see eventbox.h*/

static HWAVEOUT m_hwo = 0 ; /* handle to a waveOut device */
static WAVEHDR m_wavehdrs[ NUM_BUFFERS ]; /* one waveheader for each buffer segment */

static WAVEFORMATEX m_waveformatex = { 0, 0, 0, 0, 0, 0, 0 }; /* clear variable */
static LPWAVEFORMATEX m_lpwfx = &m_waveformatex; /* pointer to m_waveformatex */


/* MCICallBack is a callback procedure.  The waveOut device sends all status
   messages to this designated callback.  The callback performs a very 
   *trivial* operation, when it receives the message WOM_DONE from the waveOut 
   device, MCICallBack triggers our user-defined *event* called 
	 -> EventBox.status.EventPBDone.

   output_flush() polls this event, and interprets its occurence as a
   released buffer segment.  note that callback procedures cannot call any of 
   the waveOut functions.  So it's up to our main application thread to
   manage the housekeeping chores ( unpreparing the released WAVEHDRs, 
   preparing new WAVEHDRs, writing sound-data to waveOut, etc.)

   Although dwParam1 and dwParam2 can carry user-defined data, none is
   placed here at this time.  The deleted source-code originates from 
   a prior application.
*/

void CALLBACK MCICallBack( HWAVEOUT hwo, UINT uMsg,
		DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	BOOL rc;
	LPWAVEHDR lpWaveHdr = (LPWAVEHDR) dwParam1;

	switch( uMsg )
	{
	  case WOM_DONE:	
		// capture the parameters, a WAVEHDR 
		EventBox.status.EventPBDone.lpParam = (LPWAVEHDR) dwParam1;
		// now signal the event that a PLAYBACK buffer is done
		rc = SetEvent( EventBox.status.EventPBDone.hE );

		assert( rc != 0 );	// successful return code != 0

		//buffer_count = ( lpWaveHdr->dwUser );	// extract buffer #
		/* can't call waveoutprepare or unprepare from a callback funtion! */
		/*waveOutUnprepareHeader( m_hwo, m_lpoutwavehdr, sizeof(WAVEHDR));*/
		break;

	  case WOM_CLOSE:	
		printf("\nMCICallBack() : Received Message WOM_CLOSE!");
		/* trigger stop PlayBUFFER event, this is a notification system */
		rc = SetEvent( EventBox.status.EventPBDone.hE );
		assert( rc != 0 );	// successful return code != 0
		break;

	  case WOM_OPEN:	
		printf("\nMCICallBack() : Received Message WOM_OPEN!");
		break;

	  default:
		printf("\nMCICallBack() : Received UNKNOWN message (%u)!", uMsg );
	} /* endswitch (uMsg ) */

} /* endfunction void CALLBACK MCICallBack() */



/* clear a variable WAVEHDR (*lphdr) */
void
init_wavehdr( LPWAVEHDR lphdr )
{
	lphdr->dwBufferLength = BUFFER_SIZE;
	lphdr->dwBytesRecorded = BUFFER_SIZE;
	lphdr->dwFlags = 0;
	lphdr->dwLoops = 0;
	lphdr->dwUser = 0;	/* AC3DEC doesn't use this field */
	lphdr->lpData = NULL;  /* calling function must set this member! */
	lphdr->lpNext = NULL;
	lphdr->reserved = 0; /* reserved, must be set to 0! */
}

//FIXME uncomment all the matlab calls in this module
//      and you'll get a handy matlab output file
//      of the signal.
//#include "matlab.h"
//static matlab_file_t *foo;
//


/*
 * open the audio device for writing to
 */
int output_open(int bits, int rate, int channels)
{
  int tmp;
  DWORD rc;
  MMRESULT mmresult; /* return code from MM functions */
  WAVEOUTCAPS waveoutcaps; /* dev capabilities, status */


  /* initialize the Event Box 
     The events must be registered with windows before they can be used! */
	rc = CreateEvent( NULL, FALSE, FALSE, "EventPBDone" );
	assert( rc != 0 );
	EventBox.status.EventPBDone.hE = rc;	// our "event" now exists
	
	rc = CreateEvent( NULL, FALSE, FALSE,  "EventPBReady" );
	assert( rc != 0 );
	EventBox.status.EventPBReady.hE = rc;	// our "event" now exists
	
	
	tmp = ResetEvent( EventBox.status.EventPBDone.hE );
	assert ( tmp != 0 );
/*	tmp = SetEvent( EventBox.status.EventPBDone.hE ); */
	assert ( tmp != 0 );
	tmp = ResetEvent( EventBox.status.EventPBReady.hE );
	assert ( tmp != 0 );
/*	tmp = SetEvent( EventBox.status.EventPBReady.hE ); */
	assert ( tmp != 0 );

  /*
   * Open the device driver
   */

	/*printf("\nwaveOutGetNumDevs() returned %u", waveOutGetNumDevs() );*/

	if ( waveOutGetNumDevs() == 0 )
	{
		fprintf( stderr, "\n*** ERROR *** No usable waveOut devices!");
		exit( EXIT_FAILURE );
	}

  /* setup WAVEFORMATEX structure */

	m_lpwfx->cbSize = sizeof( m_waveformatex ); /* must be set! */
	m_lpwfx->wBitsPerSample = bits;
	m_lpwfx->nChannels = channels;
	m_lpwfx->nSamplesPerSec = rate;
	m_lpwfx->nAvgBytesPerSec = m_lpwfx->nSamplesPerSec * m_lpwfx->nChannels;

	if ( bits == 16 )
		m_lpwfx->nAvgBytesPerSec *= 2;
	else if ( bits != 8 )
	{
		fprintf( stderr, "\n*** ERROR *** AC3-samplesize is not 8 or 16-bit!");
		exit( EXIT_FAILURE );
	}
		/* minimum atomic sample unit */
	m_lpwfx->nBlockAlign = (m_lpwfx->wBitsPerSample * m_lpwfx->nChannels )/8;
	m_lpwfx->wFormatTag = WAVE_FORMAT_PCM; /* linear PCM */

	printf(" \nrate = %ld, bits = %d, channels = %d",rate, bits, channels );
	
  /* open waveOut device, supply a user-defined CALLBACK function, 
	 MCICallBack().  Windows will send all waveOut messages to this function.*/

	mmresult = waveOutOpen( &m_hwo, WAVE_MAPPER , m_lpwfx, (DWORD) MCICallBack,
	  NULL, CALLBACK_FUNCTION | WAVE_ALLOWSYNC);

	if ( mmresult != MMSYSERR_NOERROR )
	{
	  fprintf( stderr, "\noutput_open() : waveOutOpen() ERROR " );
	  switch ( mmresult )	{
		case MMSYSERR_ALLOCATED : 
			fprintf( stderr, "Specified resource is already allocated." );
			break;
		case MMSYSERR_BADDEVICEID : 
			fprintf( stderr, "Specified device identifier is out of range." );
			break;
		case MMSYSERR_NODRIVER: 
			fprintf( stderr, "No device driver is present. ");
			break; 
		case MMSYSERR_NOMEM : 
			fprintf( stderr, "Unable to allocate or lock memory." );
			break;
		case WAVERR_BADFORMAT : 
			fprintf( stderr, "Attempted to open with an unsupported waveform-audio format. ");
			break;
		case WAVERR_SYNC : 
			fprintf( stderr, "Wave device is synchronous!");
			break;
		default:
			fprintf( stderr, "unknown return code %u!", mmresult );
	  }
	  fprintf( stderr, "\n*** ERROR *** waveOutOpen(%d) failed!", m_hwo );
	  exit( ERROR );
	  /* return 0;  error */
	}

	/* Initialize the ring buffer */
	rb_init();

	//FIXME remove
	//	foo = matlab_open("foo.m");
	//


	mmresult = waveOutGetDevCaps( m_hwo, &waveoutcaps, 
		sizeof( waveoutcaps ) );

	if ( mmresult == MMSYSERR_NOERROR )
		printf("\nwaveOut device name : %s ", waveoutcaps.szPname );

	/* display keyboard help */
	printf("\n<SPACEBAR> to pause, <Q> to quit\n");
	
	return 1;

	/* ERROR : return 0 */
}



/* output_flush() is responsible for deallocating WAVEHDR structures.
  First, 
  the function waits for the event "EventPBDone" (set by MCICallBack() ).
  Second, output_flush() deallocates as many WAVEHDRs as it can
  from m_wavehdrs[].
  Third, the function exits, returning the # buffers freed.
  output_close() requires this return-value!
  */
int
output_flush(void) /* called by output_play() */
{
	int freed_count = 0;
	DWORD rc;
	LPWAVEHDR lpoutwavehdr = NULL;
	MMRESULT mmresult = 0;
	int windex;

	sint_16 *out_buf = 0;

	/* 1) wait for our "event" to happen, which will be set by MCICallBack() */
	/*    wait up to 1000ms for this event...then timeout */
	rc = WaitForSingleObject( EventBox.status.EventPBDone.hE, 1000 ); 
	if ( rc == WAIT_TIMEOUT )
	{
		fprintf( stderr, "\noutput_flush() : WaitForSingleObject() timed-out!");
		/*time-out interval elapsed, and the object's state is nonsignaled. */
	}
	else /* we got the event, now we must reset it! */
		ResetEvent( EventBox.status.EventPBDone.hE );

	windex = rb_read_get_index(); /* get [index] to the END of ring-buffer  */
	if ( windex == RB_EMPTY )  /* is ring-buffer EMPTY? */
	{
		/* the ring-buffer will never be empty after EventPBDone.
		   However, it could be empty if WaitForSingleObject() timed-out.
		   This condition implies there are *no more* WAVEHDRs in our
		   waveOut device.  */
		fprintf(stderr, "\noutput_flush() : rb_read_get_index() returned RB_EMPTY" );
	}


	/* 2) now UNprepare the WAVEHDR that has just been freed for us */


	/* this while loop unprepares WAVEHDRs until it reaches the current
	   playing position (WAVERR_STILLPLAYING.)  Why do we need to deallocate
	   more than one WAVEHDR?
	   Recall that we defined a user callback, MCICallBack(), to handle
	   waveOut messages.  The callback sets an *EVENT*, EventPBDone.
	   This function output_flush(), waits for EventPBDone.
	   Unfortunately, an *event* is a binary variable (ON/OFF.)  The
	   system might be busy before output_flush() detects the event.

		In other words, the event might have been set once, twice, three
		times, etc. before output_flush() sees it.  There is no way
		for output_flush() to know.  Thus, to prevent from "falling behind",
		output_flush() must implement a "catch up" mechanism to
		reclaim all the freed-WAVEHDRs, since there could be more than 1.

		(Alternatively, this entire output-package could be rewritten to
		 supply a user MESSAGE handler.  Windows messages are queued, so
		 there is no danger of losing a message.)
	*/
	mmresult = MMSYSERR_NOERROR; /* initialize variable for loop's while() */
	while ( windex != RB_EMPTY && mmresult != WAVERR_STILLPLAYING )
	{
		windex = rb_read_get_index();
		if ( windex != RB_EMPTY )
		{ /* startif */
		  lpoutwavehdr = &m_wavehdrs[ windex ];
		  mmresult = waveOutUnprepareHeader( m_hwo, lpoutwavehdr, 
			  sizeof( *lpoutwavehdr ) );

		  if ( mmresult != WAVERR_STILLPLAYING )
		  {
			++freed_count; /* update our #freeBuffers counter */
			rb_end_read(); 

		  } 
/*	  	  switch ( mmresult )	{
		  case MMSYSERR_INVALHANDLE : 
			fprintf( stderr, "\nSpecified device handle is invalid. " );
			break;
		case  MMSYSERR_NODRIVER : 
			fprintf( stderr, "\nSpecified device identifier is out of range." );
			break;
		case MMSYSERR_NOMEM : 
			fprintf( stderr, "\nUnable to allocate or lock memory." );
			break;
		case WAVERR_STILLPLAYING :
			fprintf( stderr, "\nwaveHdr still playing." );
			break;
		case MMSYSERR_NOERROR : 
			printf("\nWaveOutUnprepareHeader() successful.");
			break;
		default:
			fprintf( stderr, "\nERROR: unknown return code %u for waveOutUnprepareHeader()", mmresult );
	}*/


		} /*end if ( windex != RB_EMPTY ) */
	} /* end while ( windex != ... ) */

	return freed_count;
}



/*
 * writes the samples to the already opened waveOut device
 */
long output_play(stream_samples_t *samples, bsi_t *pbsi )
{
	int i, ch;
	int windex; /* rb write index */
	static long counter = 0;
	static sint_16 accBuf[ BUFFER_SIZE ]; /* accumulator buffer */
	LPWAVEHDR lpoutwavehdr = NULL; /* pointer to WAVEHDR */
	MMRESULT mmresult;
	float left_sample;
	float right_sample;
	sint_16 *out_buf;
	sint_16 left_pcm;
	sint_16 right_pcm;

  /* AC3DEC generates 1024 bytes (256 stereo samples) per call.
     Windows is not very efficient and procesing numerous, small chunks
	 of wave-data.  So, we'll "collect" ac3dec's output samples until
	 we fill a much larger buffer, accBuf[ BUFFER_SIZE ].

     Each call adds a little bit more to accBuf.  When accBuf becomes full,
	 we finally *WRITE* the entire accBuf (as 1 chunk) to waveOutWrite().
	*/ 

	/* buf_offset = offset in #SAMPLES (not bytes!) */
	long buf_offset = (counter % (BUFFER_SIZE / DECODED_CHUNK_SIZE )) * 512;
	/* note, this formula is not very robust... BUFFER_SIZE must be 
	   EVENLY divisible by DECODED_CHUNK_SIZE, otherwise catastrophe! */

	downmix( samples, &accBuf[ buf_offset ], pbsi, FALSE );

	if ( counter == 0 )
		printf("\rBytes written,           Duration (HH:MM:SS) (seconds)\n");

	++counter; /* keep track of #times this function has been entered */

	/* check if we've accumulated enough buffer space. */
	/* Is our accBuf[] full?  If not, return without doing anything. */
	if ( (counter % (BUFFER_SIZE >> 10 ) )!= 0 )
		return 0;  /* the accumulation buffer accBuf[] is *not* full */

	/* if we make it this far, we have a full accBuf[].  Time to write
	   the data to waveOut device. */

	if ( counter < 100 )
		printf("\r[%ldbytes]", counter*1024 );
	else
	if ( counter < 1024 )
		printf("\r[%ldKb]", counter );
	else
	if ( counter < 1024*1024 )
		printf("\r[%.2fMb]", counter/1024.0 );

	/* Keep trying to dump frames from the ring buffer until we get a 
	 * write slot available */
	out_buf = rb_begin_write();
	while(!out_buf)
	{
		/*usleep(5000); don't need this, output_flush() has an idle timer */
		output_flush();
		out_buf = rb_begin_write();
	} 
	
	windex = rb_write_get_index(); /* get index to HEAD of ring-buffer */
	/* we write data to the HEAD, and waveOut releases the played buffers
	   at the END of the ring-buffer */
	if ( windex == RB_FULL )
	{
		fprintf(stderr, "\noutput_play() : rb_write_get_index() returned RB_FULL" );
		output_close(); /* catastrophic error */
		exit(ERROR);
	}

	/* now that we got a free segment in our ring-buffer (rb), we will
	   copy accBuf[]'s contents to the ring-buffer.  accBuf[] is
	   only a temporary buffer, and will be overwritten next call. */

	memcpy( (LPVOID)out_buf, (LPVOID)accBuf, BUFFER_SIZE );

  
  /* 2) prepare a WAVEHDR corresponding to the wavedata */

  /* Before a WAVEHDR can be prepared, it must be "clear."  I.e. if it has been
	 previously used, we must first UNprepare it.  output_flush() takes care
	 of this overhead. */

	lpoutwavehdr = &m_wavehdrs[ windex ] ; 
	/*printf("\noutput_play() : Calling waveOutPrepareHeader() ");*/
	init_wavehdr( lpoutwavehdr );

	lpoutwavehdr->lpData = (LPVOID) out_buf;
	lpoutwavehdr->dwBufferLength = BUFFER_SIZE;
	mmresult=waveOutPrepareHeader( m_hwo, lpoutwavehdr, sizeof( *lpoutwavehdr ) );

	if ( mmresult != MMSYSERR_NOERROR )
	{
	  fprintf( stderr, "\noutput_play() : waveOutPrepareHeader() ERROR " );
	  switch ( mmresult )	{
		case MMSYSERR_INVALHANDLE : 
			fprintf( stderr, "Specified device handle is invalid. " );
			break;
		case  MMSYSERR_NODRIVER : 
			fprintf( stderr, "Specified device identifier is out of range." );
			break;
		case MMSYSERR_NOMEM : 
			fprintf( stderr, "Unable to allocate or lock memory." );
			break;
		default:
			fprintf( stderr, "unknown return code %u!", mmresult );
	  }

	} /* endif (mmresult != MMSYSERR_NOERROR ) */


  /* 3) write the buffer to the waveOut device.  Play the buffer! */

	/* printf("\nCalling waveOutWrite() "); */

	mmresult=waveOutWrite( m_hwo, lpoutwavehdr, sizeof( *lpoutwavehdr ) );

	if ( mmresult != MMSYSERR_NOERROR )
	{
	  fprintf( stderr, "\noutput_play() : waveOutWrite() ERROR " );
	  switch ( mmresult )	{
		case MMSYSERR_INVALHANDLE : 
			fprintf( stderr, "Specified device handle is invalid. " );
			break;
		case  MMSYSERR_NODRIVER : 
			fprintf( stderr, "Specified device identifier is out of range." );
			break;
		case MMSYSERR_NOMEM : 
			fprintf( stderr, "Unable to allocate or lock memory." );
			break;
		case WAVERR_UNPREPARED : 
			fprintf( stderr, "Data block pointed to by WAVEHDR unprepared." );
			break;
		default:
			printf("unknown return code %u!", mmresult );
	  } /* endswitch */
	} /* endif ( mmresult != MMSYSERR_NOERROR ) */

	rb_end_write();
	parse_keyboard(); /* check keyboard commands */
	return 1;
} /* endfunction void output_play(stream_samples_t *samples) */



/* parses keyboard commands, QUIT and PAUSE are currently supported */
void parse_keyboard( void )
{
	MMRESULT mmresult = MMSYSERR_NOERROR;
	int ch = 0; /* user keyboard char */

	if ( _kbhit() ) /* keyboard press? */
	{

		ch = _getch();
		/* v0.05 KEY-QUIT detection has been moved to main() */
/*		if ( toupper( ch ) == toupper( KEY_QUIT ) )
		{
			 we must exit nicely, or the waveOut device is "stuck" open!
			   Then other apps can't use it until the user reboots! yuck 
			printf("\nUSER ABORTED DECODING OPERATION.");
			output_close();  
			exit( ERROR );
		}*/

		if ( toupper( ch ) == toupper( KEY_PAUSE_TOGGLE ) )
		{
			mmresult = waveOutPause( m_hwo );
			printf( " <PAUSED>");
			parse_keyboard_error( mmresult );
			_getch(); /* wait for 2nd keypress to unpause */
			mmresult = waveOutRestart( m_hwo );
			parse_keyboard_error( mmresult );
			printf( "\b\b\b\b\b\b\b\b\b              ");
		}
		else
			_ungetch( ch ); /* put keypress back on key buffer */
	} /* endif ( _kbhit() ) */
} /* endfunction void parse_keyboard( void ) */


/* parse_keyboard_error */
void 
parse_keyboard_error( MMRESULT mmresult )
{
	if ( mmresult != MMSYSERR_NOERROR )
	{
	  fprintf( stderr, "\nparse_keyboard() : ERROR " );
	  switch ( mmresult )	{
		case MMSYSERR_INVALHANDLE : 
			fprintf( stderr, "Specified device handle is invalid. " );
			break;
		case  MMSYSERR_NODRIVER : 
			fprintf( stderr, "Specified device identifier is out of range." );
			break;
		case MMSYSERR_NOMEM : 
			fprintf( stderr, "Unable to allocate or lock memory." );
			break;
		case MMSYSERR_NOTSUPPORTED:
			fprintf( stderr, "Specified device is synchronous and does not support pausing." );
			break;
		default:
			fprintf( stderr, "unknown return code %u!", mmresult );
	  } /*endswitch */
	} /* endif ( mmresult != MMSYSERR_NOERROR ) */
} /* endfunction void parse_keyboard_error( MMRESULT mmresult ) */


void
output_close(void)
{
	MMRESULT mmresult;
	BOOL brc; /* boolean return code */
	int freed_count = 0, freed;

	/* before we can shut down the wave system, we have to *WAIT* for
	   the waveOut device to finish playing its current buffers.
	   This might can take awhile, because we allocated rather *large*
	   buffers! */
	printf( "\n" );
	do {
		freed = output_flush();
		freed_count += freed;
		printf("\routput_close() : waiting for waveOut, buffer #%d",
			freed_count+1 );
	}   /* exit loop if output_flush() is "stuck" (i.e. time-out) */
	while( rb_read_get_index() != RB_EMPTY && (freed > 0) );
	

	/* Now, reset waveOut, if it's not already stopped, it is now! */
	mmresult = waveOutReset( m_hwo ); 
	if ( mmresult != MMSYSERR_NOERROR )
	{
	  fprintf( stderr, "\noutput_close() : waveOutReset() ERROR, ");
	  switch ( mmresult )	{
		case MMSYSERR_INVALHANDLE : 
			fprintf( stderr, "Specified device handle is invalid. " );
			break;
		case  MMSYSERR_NODRIVER : 
			fprintf( stderr, "Specified device identifier is out of range." );
			break;
		case MMSYSERR_NOMEM : 
			fprintf( stderr, "Unable to allocate or lock memory." );
			break;
		case  MMSYSERR_NOTSUPPORTED: 
			fprintf( stderr, "Device is synchronous, not supported");
			break;
		default:
			printf("unknown return code %u!", mmresult );
	  } /* endswitch */
	}  /* endif ( mmresult != MMSYSERR_NOERROR ) */
	

	/* Release the hardware-resource back to Windows */
	mmresult=waveOutClose( m_hwo ); 
	if ( mmresult != MMSYSERR_NOERROR )
	{
	  fprintf( stderr, "\noutput_close() : waveOutClose() ERROR : " );
	  switch ( mmresult )	{
		case MMSYSERR_INVALHANDLE : 
			fprintf( stderr, "Specified device handle is invalid. " );
			break;
		case  MMSYSERR_NODRIVER : 
			fprintf( stderr, "Specified device identifier is out of range." );
			break;
		case MMSYSERR_NOMEM : 
			fprintf( stderr, "Unable to allocate or lock memory." );
			break;
		case  WAVERR_STILLPLAYING: 
			fprintf( stderr, "Audio still playing");
			break;
		default:
			printf("unknown return code %u for waveOutClose()", mmresult );
	  } /* endswitch */
	} /* endif ( mmresult != MMSYSERR_NOERROR ) */
	
	/* shutdown the events that we declared */
	brc = ResetEvent( EventBox.status.EventPBDone.hE );
	brc = CloseHandle( EventBox.status.EventPBDone.hE );
	brc = ResetEvent( EventBox.status.EventPBReady.hE );
	brc = CloseHandle( EventBox.status.EventPBReady.hE);
} /* endfunction void output_close(void) */



