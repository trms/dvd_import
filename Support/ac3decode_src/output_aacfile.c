/* output_wavefile.c
 *
 * AC3DEC_WAVE
 * -----------
 *
 *  Output to AAC format.
 *  
 *


/*#include <unistd.h>*/

#include "ac3.h"
#include "decode.h"
#include "debug.h"
#include "output.h"
#include "ring_buffer.h"
#include "eventbox.h"
#include "downmix.h"

#include "aacenc.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>



static DWORD m_DecoderIndex;
	/* the "index" variables correspond to starting locations within
	   the input and output ACM buffers.  output_wavefile_write() updates them
	   after each call to acmStreamConvert() */
static DWORD m_AACIndex;

static FILE *m_fpout; /* output wavefile */

static sint_16 m_lpdecbuffer[ DECODED_CHUNK_SIZE * 4 * 6]; /* decoder ouput samples */
static sint_16 m_44100decbuffer[ DECODED_CHUNK_SIZE * 4 * 6]; /* decoder ouput samples */

static LPBYTE bitBuffer = NULL; /* ACM output buffer, BYTE buffer */
static DWORD m_cbWritten = 0; /* counter, # bytes of compressed wav-data */

static mbAACStream *acstream;
static time_t start_time;	/* WAV encoding start time */

static int samples_read, header_size, output_channels;







extern int m_overwrite;
int
output_aacfile_open( int bits, int rate, int channels, const char outname[] )
{
	mbAACConfig		ac;
	int	bitBufferSize;

	output_channels =
	ac.channels = channels;
	ac.sampling_rate = rate;
	ac.bit_rate = 192000;
	ac.profile = 1;
	ac.write_header = 1;
	ac.use_VBR = 1;
	ac.VBR_setting = 1;

#ifdef DEBUG
	acstream = mbEncodeInit( &ac, &samples_read, &bitBufferSize, &header_size );
#endif

	printf( "Audio is %d channel %dbit %dhz\n", channels, bits, rate );
	
	/* allocate ACM's output buffer, allocate 2X as much memory */
	bitBuffer = ( sint_16 * ) malloc( 1028*2*channels );
	if ( bitBuffer == NULL ){
		fprintf( stderr, "\noutput_wavefile_open() : could not allocate buffer.");
		exit( ERROR );
	}


  /* create the output file */
	m_fpout = fopen( outname, "rb"); /* write binary, modify mode */
	if ( m_fpout != NULL && !m_overwrite ){
		char ch;
		printf("\n%s already exists, overwrite? ", outname );
		ch = getchar();
		if ( ch != 'Y' && ch != 'y' ){
			fprintf( stderr, "\noutput_wavefile_open() : Aborted." );
			exit( ERROR );
		}
		fclose( m_fpout );
	}

	m_fpout = fopen( outname, "wb"); /* write binary, overwrite previous file*/
	if ( m_fpout == NULL ){
		fprintf( stderr, "\noutput_wavefile_open() : Could not create %s", outname );
		exit( ERROR );
	}

	m_cbWritten = SaveData( m_fpout, m_lpdecbuffer, header_size );

	printf("\n");

	m_DecoderIndex = 0;

	time( &start_time ); /* record the starting-time */
	
	return 1;
}	/* endfunction int output_wavefile_open() */










extern int	m_force44100;

static long m_AACIndex = 0;

long output_aacfile_write(stream_samples_t *samples, bsi_t *pbsi )
{
	DWORD cbLeft = 0; /* #bytes still need to be converted, in m_lpdecbuffer[] */
	DWORD cbStored; /* return code from fwrite() */
	MMRESULT mmresult;
	sint_16 *out_samples;
	static DWORD StoredMOD64K = 0; /* granularity counter */

	out_samples = &m_lpdecbuffer[ m_DecoderIndex*(2*output_channels) ];

	downmix( samples, out_samples, pbsi, (output_channels==5) );
	m_DecoderIndex += 256;

	if ( m_DecoderIndex >= 480 ){
		conv480to441( m_lpdecbuffer, 480, 5 );
		memcpy( &m_44100decbuffer[ m_AACIndex*(2*output_channels) ], m_lpdecbuffer, (441)*(2*output_channels) );
		m_AACIndex += 441;
		memcpy( m_lpdecbuffer, &m_lpdecbuffer[ (480)*(2*output_channels) ], (m_DecoderIndex-480)*(2*output_channels) );
		m_DecoderIndex -= 480;
	}


	if ( m_AACIndex >= 1024 ){
		int bitBufSize;
#ifdef DEBUG
		mbEncodeFrame( acstream, m_44100decbuffer, 1024, bitBuffer, &bitBufSize );
#endif
		memcpy( m_44100decbuffer, &m_44100decbuffer[ (1024)*(2*output_channels) ], (m_AACIndex-1024)*(2*output_channels) );
		m_AACIndex -= 1024;

		cbStored = SaveData( m_fpout, bitBuffer, bitBufSize );
		m_cbWritten += cbStored;
		m_DecoderIndex = 0;
	}


	/* v0.05, only update the counter only every 64KB, to reduce overhead */
	if ( ( m_cbWritten >> 16 ) != StoredMOD64K )
	{
		if ( m_cbWritten < 1024*100 )
			printf("\r[%ldbytes]", m_cbWritten );
		else
		if ( m_cbWritten < 1024*1024 )
			printf("\r[%ldKb]", m_cbWritten/1024 );
		else
		if ( m_cbWritten < 1024*1024*1024 )
			printf("\r[%.2fMb]", m_cbWritten/1048576.0 );
		StoredMOD64K = ( m_cbWritten >> 16 );
		return 1;
	}
	return 0;

} /* endfunction output_wavefile_write() */


long
output_aacfile_close( const char outname[]  )
{
	time_t finish_time, el_time; /* ending time, elapsed time */
	MMRESULT mmresult;
	int bitBufSize;

#ifdef DEBUG
	mbEncodeFinish( acstream, bitBuffer, &bitBufSize );
	SaveData( m_fpout, bitBuffer, bitBufSize );
	SaveData( m_fpout, NULL, NULL );

	mbEncodeFree(acstream, bitBuffer );
#endif
	fseek( m_fpout, 0, SEEK_SET );
	fwrite( bitBuffer, 1, header_size, m_fpout );

	/* close the output wavefile, so we can re-open in MODIFY mode */
	if ( fclose( m_fpout ) != 0 ){
		fprintf( stderr, "\nnoutput_wavefile_close() : ERROR Could not close %s!", outname );
	}


	free( bitBuffer); /* deallocate memory */
	
	time( &finish_time );	/* Record the finish time */

	/* time code copied from John Schlichther's AVI2MPG1 */
	el_time = finish_time - start_time;


	return el_time;
} /* void output_wavefile_close( const char outname[]  ) */



long get_aacwrite_time( void )
{
	time_t finish_time, el_time; /* ending time, elapsed time */
	time( &finish_time );	/* Record the finish time */

	/* time code copied from John Schlichther's AVI2MPG1 */
	el_time = finish_time - start_time;
	return el_time;
}


