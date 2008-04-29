/* output_wavefile.c
 *
 * AC3DEC_WAVE
 * -----------
 *
 *  Wave-file writer (uses ACM - audio compression manager)
 *  Client PC's operating system must have "Audio Compression" installed.
 *
 * v0.05 minor printf() fix for "acmStreamOpen()" error-reporting
 *       corrected minor bug when user picks "PCM" format,
 *        waveoformat.excbSize is now set to 0, as suggested by the Win32 SDK
 *       Attempted workaround for using Microsoft Audio codec, failed...
 *       increased WFX size to 64 bytes (from 50)
 *       output_wavefile_open() and output_wavefile_close() now record the
 *        start & stop time(s) for an AC3DEC job.  The elapsed time is
 *        reported.
 *
 * v0.03 initial release
 *   known problems - does not work with MS Audio Codec, nor
 *   ADPCM (IMA or Microsft.)

/*#include <unistd.h>*/

#include "ac3.h"
#include "decode.h"
#include "debug.h"
#include "output.h"
#include "ring_buffer.h"
#include "eventbox.h"
#include "downmix.h"

/* stuff for Windows */
#include <windows.h>
#include <mmreg.h>
#include <process.h> /* events & other Windows messaging junk */
#include <conio.h> /* keyboard input _kbhit() , _getch() */
#include <msacm.h> /* ACM routines, include msacm32.lib */
#include <aviriff.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/*#define WFXEX_SIZE 50*/
#define WFXEX_SIZE 59
#define ACM_WMA_DECOMP_KEY "1A0F78F0-EC8A-11d2-BBBE-006008320064"
#define ACM_WMA_COMP_KEY   "F6DC9830-BC79-11d2-A9D0-006097926036"
#define WAVE_FORMAT_WMA1 0x0160
#define WAVE_FORMAT_WMA2 0x0161

const static BYTE acm_wma_decomp_key[] = ACM_WMA_DECOMP_KEY;

const static BYTE acm_wma_comp_key[] = ACM_WMA_COMP_KEY;

extern int m_insertms;
extern int m_output_pcm;
/* private definitions, variables */

void write_wavheader( void );
static WAVEFORMATEX m_decwfx = { 0, 0, 0, 0, 0, 0, 0 };
static BYTE WaveFormatEx[1024]={0x01,0x00,0x02,0x00,0x80,0xbb,0x00,0x00,0x00,0xee,0x02,0x00,0x04,0x00,0x10,0x00};

/*
typedef struct { 
    WORD  wFormatTag; 
    WORD  nChannels; 
    DWORD nSamplesPerSec; 
    DWORD nAvgBytesPerSec; 
    WORD  nBlockAlign; 
    WORD  wBitsPerSample; 
    WORD  cbSize; 
} WAVEFORMATEX; 

wFormatTag 
Waveform-audio format type. Format tags are registered with Microsoft Corporation for many compression algorithms. A complete list of format tags can be found in the MMREG.H header file. 
nChannels 
Number of channels in the waveform-audio data. Monaural data uses one channel and stereo data uses two channels. 
nSamplesPerSec 
Sample rate, in samples per second (hertz), that each channel should be played or recorded. If wFormatTag is WAVE_FORMAT_PCM, then common values for nSamplesPerSec are 8.0 kHz, 11.025 kHz, 22.05 kHz, and 44.1 kHz. For non-PCM formats, this member must be computed according to the manufacturer's specification of the format tag. 
nAvgBytesPerSec 
Required average data-transfer rate, in bytes per second, for the format tag. If wFormatTag is WAVE_FORMAT_PCM, nAvgBytesPerSec should be equal to the product of nSamplesPerSec and nBlockAlign. For non-PCM formats, this member must be computed according to the manufacturer's specification of the format tag. 
Playback and record software can estimate buffer sizes by using the nAvgBytesPerSec member. 

nBlockAlign 
Block alignment, in bytes. The block alignment is the minimum atomic unit of data for the wFormatTag format type. If wFormatTag is WAVE_FORMAT_PCM, nBlockAlign should be equal to the product of nChannels and wBitsPerSample divided by 8 (bits per byte). For non-PCM formats, this member must be computed according to the manufacturer's specification of the format tag. 
Playback and record software must process a multiple of nBlockAlign bytes of data at a time. Data written and read from a device must always start at the beginning of a block. For example, it is illegal to start playback of PCM data in the middle of a sample (that is, on a non-block-aligned boundary). 

wBitsPerSample 
Bits per sample for the wFormatTag format type. If wFormatTag is WAVE_FORMAT_PCM, then wBitsPerSample should be equal to 8 or 16. For non-PCM formats, this member must be set according to the manufacturer's specification of the format tag. Note that some compression schemes cannot define a value for wBitsPerSample, so this member can be zero. 
cbSize 
Size, in bytes, of extra format information appended to the end of the WAVEFORMATEX structure. This information can be used by non-PCM formats to store extra attributes for the wFormatTag. If no extra information is required by the wFormatTag, this member must be set to zero. Note that for WAVE_FORMAT_PCM formats (and only WAVE_FORMAT_PCM formats), this member is ignored. 



 */





	/* m_decwfx = decoder's output WAVEFORMATEX */
static LPWAVEFORMATEX m_lpoutwfx = NULL;
	/* m_outwfx = POINTER TO output file's WAVEFORMATEX, depending on codec
	   selection, may not be the same as m_decwfx */
static HACMSTREAM m_hacmstream; /* ACM stream */
static LPWAVEFORMATEX m_lpwfx = NULL ; /* pointer to m_waveformatex */
static DWORD m_cbOutputBuffer; /* size of ACM output buffer...determined
							      by acmStreamSize() */
static DWORD m_DecoderIndex;
	/* the "index" variables correspond to starting locations within
	   the input and output ACM buffers.  output_wavefile_write() updates them
	   after each call to acmStreamConvert() */

static FILE *m_fpout; /* output wavefile */
static ACMSTREAMHEADER m_acmstreamheader;

static sint_16 m_lpdecbuffer[ DECODED_CHUNK_SIZE * 4]; /* decoder ouput samples */
static LPBYTE m_lpacmbuffer = NULL; /* ACM output buffer, BYTE buffer */
static DWORD m_cbWritten = 0; /* counter, # bytes of compressed wav-data */

static time_t start_time;	/* WAV encoding start time */






extern int m_overwrite;
int
output_wavefile_open( int bits, int rate, int channels, const char outname[] )
{
	ACMFORMATCHOOSE acmformatchoose;
	char dialog_title[] = "Hehe, choose something, prease";
	DWORD cbWritten = 0, m_cbHeader = 0;
	MMRESULT mmresult;
	int ch;
	char ch_zero =0;
	LPBYTE lpByte = NULL; /* byte pointer */

/*typedef struct {
    DWORD                   cbStruct;
    DWORD                   fdwStyle;
    HWND                    hwndOwner;
    LPWAVEFORMATEX          pwfx;
    DWORD                   cbwfx;
    LPCSTR                  pszTitle;
    char szFormatTag[ACMFORMATTAGDETAILS_FORMATTAG_CHARS];
    char szFormat[ACMFORMATDETAILS_FORMAT_CHARS];
    LPSTR                   pszName;
    DWORD                   cchName;
    DWORD                   fdwEnum;
    LPWAVEFORMATEX          pwfxEnum;
    HINSTANCE               hInstance;
    LPCSTR                  pszTemplateName;
    LPARAM                  lCustData;
    ACMFORMATCHOOSEHOOKPROC pfnHook;
} ACMFORMATCHOOSE; */

	/* allocate memory for WAVEFORMATEX...we can't just declare a WAVEFORMATEX
	   variable, because this doesn't won't allocate the extra RAM for the
	   supplementary format data.
	   */

	m_lpoutwfx = (LPWAVEFORMATEX) malloc( WFXEX_SIZE );
	if ( m_lpoutwfx == NULL )
	{
		fprintf( stderr, "\noutput_wavefile_open() : could not allocate buffer (%d bytes).", WFXEX_SIZE );
		exit( ERROR );
	}
	else
	{
		lpByte = (LPBYTE) m_lpoutwfx;
		memset( lpByte, 0, WFXEX_SIZE ); /* clear memory */
		memcpy( lpByte, WaveFormatEx, WFXEX_SIZE );
	}

	/* initialize m_decwfx */
	m_decwfx.wFormatTag = WAVE_FORMAT_PCM;
	m_decwfx.nChannels = channels;
	m_decwfx.nSamplesPerSec = rate;
	m_decwfx.nAvgBytesPerSec = channels * rate * ( bits >> 3 );
	m_decwfx.nBlockAlign = m_decwfx.nAvgBytesPerSec / rate;
	m_decwfx.wBitsPerSample = bits;
	m_decwfx.cbSize = 0; /* WAVE_FORMAT_PCM => cbSize = 0 */

	printf( "Audio is %d channel %dbit %dhz\n", channels, bits, rate );

	m_lpoutwfx->cbSize = WFXEX_SIZE - sizeof( WAVEFORMATEX );

	/* initialize acmformatchoose variable */
	acmformatchoose.cbStruct = sizeof( acmformatchoose ); /* initialize size */
	acmformatchoose.fdwStyle = ACMFORMATCHOOSE_STYLEF_INITTOWFXSTRUCT;
	acmformatchoose.hwndOwner = NULL; /* no parent window owns this dialog */
		/* user's selection will be stored in pwfx, cbwfx, */
	acmformatchoose.pwfx = m_lpoutwfx; /* pointer to OUTPUT FILE's waveformat */
		/* when acmFormatChoose() returns, .pwfx contains the user's selection*/
	acmformatchoose.cbwfx = WFXEX_SIZE; /* total sizeof "m_lpoutfx" */
	acmformatchoose.pszTitle = dialog_title; /* set dialog title text */
	//acmformatchoose.pwfltr = pwfltr;     // wfltr to receive selection
	//acmformatchoose.cbwfltr = cbwfltr;   // size of wfltr buffer
	acmformatchoose.fdwEnum = //ACM_FORMATENUMF_CONVERT ;
		 //ACM_FORMATENUMF_NSAMPLESPERSEC | ACM_FORMATENUMF_NCHANNELS |
		 //ACM_FORMATENUMF_SUGGEST;
		 ACM_FORMATENUMF_CONVERT;
	acmformatchoose.cchName = 0;	/* size of pszName */
	acmformatchoose.pszName = NULL; /* name of user-defined format name.  not used
	 for this example */

	//input format to ACM = decoder output's WAVEFORMATEX
	acmformatchoose.pwfxEnum = &m_decwfx;
	acmformatchoose.szFormatTag[0] = NULL; /* clear this field  */

	/* put user dialog on screen, audio codec selection */
	if ( m_output_pcm )
		mmresult = MMSYSERR_NOERROR;
	else
		mmresult = acmFormatChoose( &acmformatchoose );

	if ( mmresult != MMSYSERR_NOERROR )
	{
		fprintf( stderr, "\noutput_wavefile_open() : acmFormatChoose() failed. ");
		switch( mmresult )
		{
		case ACMERR_CANCELED : fprintf( stderr, "The user chose the Cancel button or the Close command on the System menu to close the dialog box.");
			break;
		case ACMERR_NOTPOSSIBLE : fprintf( stderr, "The buffer identified by the pwfx member of the ACMFORMATCHOOSE structure is too small to contain the selected format.");
			break;
		case MMSYSERR_INVALFLAG : fprintf( stderr, "At least one flag is invalid. ");
			break;
		case MMSYSERR_INVALHANDLE : fprintf( stderr, "The specified handle is invalid. ");
			break;
		case MMSYSERR_INVALPARAM : fprintf( stderr, "At least one parameter is invalid. ");
			break;
		case MMSYSERR_NODRIVER : fprintf( stderr, "A suitable driver is not available to provide valid format selections. ");
			break;
		default :
			fprintf( stderr, "Unknown error code (%ld)", mmresult );
		}
		exit( ERROR ); /* codec selection failed, abort */
	}  /* endif ( mmresult != MMSYSERR_NOERROR ) */

	/* acmformatchoose() successful, user's selection stored in .pwfx */
	if ( acmformatchoose.szFormatTag )
		printf("\nCodec selected : %s (wFormatTag = 0x%0X)",
		acmformatchoose.szFormatTag, m_lpoutwfx->wFormatTag );

    /* ------------------------------------------------------------
	 *
	 * workarounds, special cases, etc.
	 * some individual codecs require "special treatment."
	 *
	 * ------------------------------------------------------------ */

	/* if user selected WAVE_FORMAT_PCM, we should set cbSize to 0.  This
	   is the norm for PCM waveformats. */
	if ( m_lpoutwfx->wFormatTag == WAVE_FORMAT_PCM )
		m_lpoutwfx->cbSize = 0;

	/* if user selected Windows Media Audio codec, we set cbSize to 41.
	   Then we supply an "authorization key", to unlock the compressor.

		Fucking MS, those fuckers want asf/wma to be a popular container? go suck your selves dudes
		You wouldnt know how to even come close to QT really. MIT nerds or what.!!! Dick Wadds
	*/

	if ( m_lpoutwfx->wFormatTag == WAVE_FORMAT_WMA2 || m_lpoutwfx->wFormatTag == WAVE_FORMAT_WMA1 ) {
		printf(" <WMA1 or WMA2 workaround %d>", strlen( acm_wma_comp_key ) );
		lpByte = ( LPBYTE ) m_lpoutwfx;
		lpByte += 0x16;		// set LpByte to a specific starting point

		strcpy( lpByte, acm_wma_comp_key );
		lpByte += strlen( acm_wma_comp_key ) + 1; // include terminating \00
		//strcpy( lpByte, acm_wma_decomp_key );
	}

	/*MMRESULT acmStreamOpen(
	  LPHACMSTREAM phas,
	  HACMDRIVER had,
	  LPWAVEFORMATEX pwfxSrc,
	  LPWAVEFORMATEX pwfxDst,
	  LPWAVEFILTER pwfltr,
	  DWORD dwCallback,
	  DWORD dwInstance,
	  DWORD fdwOpen
	);*/

	/* create the audio conversion stream, the stream's input is the
	   raw AC3-WAV output, and the stream's output is the user-selected
	   audio format. */
	mmresult = acmStreamOpen( &m_hacmstream, NULL,  &m_decwfx, m_lpoutwfx, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME  );

	if ( mmresult != 0 ) /* error? */ {
		fprintf( stderr, "\noutput_wavefile_open() : acmStreamOpen() failed, ");
		switch( mmresult ){
			case ACMERR_NOTPOSSIBLE :
			  fprintf( stderr,"The requested operation cannot be performed." );
			  break;
			case MMSYSERR_INVALFLAG :
			  fprintf( stderr,"At least one flag is invalid. ");
			  break;
			case MMSYSERR_INVALHANDLE :
			  fprintf( stderr,"The specified handle is invalid. " );
			  break;
			case MMSYSERR_INVALPARAM :
			  fprintf( stderr,"At least one parameter is invalid. ");
			  break;
			case MMSYSERR_NOMEM :
			  fprintf( stderr,"The system is unable to allocate resources.");
			  break;
			default:
			  fprintf( stderr, "Unknown error code (%ld)", mmresult );
		}
		exit( ERROR );
	}  /* endif ( mmresult != 0 ) */


	/* now get recommended size for destination buffer.  The ACM source buffer
	   is the AC3-decoder's raw output (256 samples, stereo.)  So we'll
	   specify a source buffer size of 1024bytes, and acmStreamSize() will
	   give us its recommended dest buffer size. */
	mmresult = acmStreamSize( m_hacmstream, DECODED_CHUNK_SIZE,
		&m_cbOutputBuffer, ACM_STREAMSIZEF_SOURCE  );
	if ( mmresult != 0 ) /* error? */
	{
		fprintf( stderr, "\noutput_wavefile_open() : acmStreamSize() failed, ");
		switch( mmresult )
		{
			case ACMERR_NOTPOSSIBLE :
			  fprintf( stderr,"The requested operation cannot be performed." );
			  break;
			case MMSYSERR_INVALFLAG :
			  fprintf( stderr,"At least one flag is invalid. ");
			  break;
			case MMSYSERR_INVALHANDLE :
			  fprintf( stderr,"The specified handle is invalid. " );
			  break;
			case MMSYSERR_INVALPARAM :
			  fprintf( stderr,"At least one parameter is invalid. ");
			  break;
			default:
			  fprintf( stderr, "Unknown error code (%ld)", mmresult );
		}
		exit( ERROR );
	}  /* endif ( mmresult != 0 ) */
	else
		printf( "\noutput_wavefile_open() : acmStreamSize() %ld ", m_cbOutputBuffer );

	/* allocate ACM's output buffer, allocate 2X as much memory */
	m_lpacmbuffer = ( sint_16 * ) malloc( m_cbOutputBuffer * 4 );
	if ( m_lpacmbuffer == NULL )
	{
		fprintf( stderr, "\noutput_wavefile_open() : could not allocate output buffer.");
		exit( ERROR );
	}

	/* acmStreamSize(
	HACMSTREAM has,
	DWORD cbInput,
	LPDWORD pdwOutputBytes,
	DWORD fdwSize  )
	*/

	/* initialize the acmstreamheader variable */
	m_acmstreamheader.cbStruct = sizeof ( m_acmstreamheader );
	m_acmstreamheader.dwUser = 0; /* no user data */
	m_acmstreamheader.cbSrcLength = DECODED_CHUNK_SIZE;
	//m_acmstreamheader.cbSrcLengthUsed = 0; set by acmStreamConvert()
	m_acmstreamheader.dwSrcUser = 0; /* not in use */
	m_acmstreamheader.cbDstLength = m_cbOutputBuffer;
	//m_acmstreamheader.cbDstLengthUsed = set by acmStreamConvert()
	m_acmstreamheader.dwDstUser = 0; /* not in use */
	m_acmstreamheader.pbDst = (LPBYTE) m_lpacmbuffer; /* not done!*/
	m_acmstreamheader.pbSrc = (LPBYTE) m_lpdecbuffer; /* not done!*/

	/*typedef struct {
	    DWORD  cbStruct;
	    DWORD  fdwStatus;
	    DWORD  dwUser;
	    LPBYTE pbSrc;
	    DWORD  cbSrcLength;
	    DWORD  cbSrcLengthUsed;
	    DWORD  dwSrcUser;
	    LPBYTE pbDst;
	    DWORD  cbDstLength;
	    DWORD  cbDstLengthUsed;
	    DWORD  dwDstUser;
	    DWORD  dwReservedDriver[10];
	} ACMSTREAMHEADER; */

	/* prepare the streamheader(s) for source and dest buffers */
	mmresult = acmStreamPrepareHeader( m_hacmstream, &m_acmstreamheader, 0 );
	if ( mmresult != 0 ) /* error? */
	{
		fprintf( stderr, "\noutput_wavefile_open() : acmStreamPrepareHeader() failed,\n");
		switch( mmresult )
		{
			case MMSYSERR_INVALFLAG :
			  fprintf( stderr,"At least one flag is invalid. ");
			  break;
			case MMSYSERR_INVALHANDLE :
			  fprintf( stderr,"The specified handle is invalid. " );
			  break;
			case MMSYSERR_INVALPARAM :
			  fprintf( stderr,"At least one parameter is invalid. ");
			  break;
			case MMSYSERR_NOMEM :
			  fprintf( stderr,"The system is unable to allocate resources.");
			  break;
			default:
			  fprintf( stderr, "Unknown error code (%ld)", mmresult );
		}
		exit( ERROR );
	}  /* endif ( mmresult != 0 ) */

	/*&	MMRESULT acmStreamPrepareHeader(
	  HACMSTREAM has,
	  LPACMSTREAMHEADER pash,
	  DWORD fdwPrepare         */


  /* create the output file */
	m_fpout = fopen( outname, "rb"); /* write binary, modify mode */
	if ( m_fpout != NULL && !m_overwrite )
	{
		printf("\n%s already exists, overwrite? ", outname );
		ch = getchar();
		if ( ch != 'Y' && ch != 'y' )
		{
			fprintf( stderr, "\noutput_wavefile_open() : Aborted." );
			exit( ERROR );
		}
		fclose( m_fpout );
	}

	m_fpout = fopen( outname, "wb"); /* write binary, overwrite previous file*/
	if ( m_fpout == NULL )
	{
		fprintf( stderr, "\noutput_wavefile_open() : Could not create %s",
			outname );
		exit( ERROR );
	}

  /* reserve space in output-file for RIFF .WAV header */

	m_cbHeader = sizeof(RIFFLIST) + sizeof(RIFFCHUNK) +
		sizeof( WAVEFORMATEX ) + m_lpoutwfx->cbSize + sizeof(RIFFCHUNK);
	cbWritten = 0;
	for ( ch = 0; ch< m_cbHeader; ++ch ){
		cbWritten += fwrite( &ch_zero, 1, 1, m_fpout );
	}

	if ( cbWritten != m_cbHeader ) /* check the actual # written bytes */{
		fprintf( stderr, "\noutput_wavefile_open() : WAVheader broken!" );
		exit( ERROR );
	}
	else
		printf( "\noutput_wavefile_open() : Reserved WAVheader, %ld bytes",	m_cbHeader );

	printf("\n");

	m_DecoderIndex = 0;
	m_cbWritten = 0; /* reset WAVE-DATA counter to 0, not including header! */

	time( &start_time ); /* record the starting-time */

	return 1;
}	/* endfunction int output_wavefile_open() */










extern int	m_force44100;

long output_wavefile_write(stream_samples_t *samples, bsi_t *pbsi )
{
static	double l_time; /* ending time, elapsed time */
	DWORD cbLeft = 0; /* #bytes still need to be converted, in m_lpdecbuffer[] */
	DWORD cbStored; /* return code from fwrite() */
	MMRESULT mmresult;
	sint_16 *out_samples;
	long chunksize = DECODED_CHUNK_SIZE;
	static double out441count = 0;
	static int first_iteration = 1;

	/* downmix( source, destination ), downmix AC3-program to 2-channel Dolby */
	/* The downmix operation appends the new samples to the "leftover" samples
	   from last call.*/
	out_samples = &m_lpdecbuffer[ (m_DecoderIndex>>1) ];
	downmix( samples, out_samples, pbsi, FALSE );

	if ( m_force44100 ){
		conv480to441( out_samples, 256, 2 );
		out441count += (256*44100)/48000.0;
		chunksize = (long)(out441count);
		out441count -= chunksize;
		chunksize *= 4;
		//printf( "####### %d %f, %d\n", cbLeft, out441count, chunksize );
	}

	/* (m_decoderIndex>>1) converts the byte-index -> sint_16 index. */
	cbLeft = chunksize + m_DecoderIndex; /* update counter */

  /* now repeatedly call acmStreamConvert() until we deplete m_lpdecbuffer[] */
  do {
	if ( first_iteration )
	{
		mmresult = acmStreamConvert( m_hacmstream, &m_acmstreamheader, ACM_STREAMCONVERTF_START );
		first_iteration = 0;
		printf("\rBytes written,           Duration (HH:MM:SS) (seconds),    Output rate\n");
		//printf("\routput_wavefile_write() : initializing...");
	}
	else {
		if ( m_insertms ){
			long	*datams, len, i;

			len = (48000/1000) * 4;
			datams = malloc( len );
			memset( datams, 0, len );

			for(i=0;i<m_insertms;i++){
				m_acmstreamheader.cbSrcLength = len;
				mmresult = acmStreamConvert( m_hacmstream, &m_acmstreamheader, 0 );
				cbStored += SaveData( m_fpout, m_lpacmbuffer, m_acmstreamheader.cbDstLengthUsed );
			}
			m_acmstreamheader.cbSrcLength = DECODED_CHUNK_SIZE;
			m_insertms = 0;
		}
		mmresult = acmStreamConvert( m_hacmstream, &m_acmstreamheader, 0 );
	}
			//ACM_STREAMCONVERTF_BLOCKALIGN );
	if ( mmresult != 0 ) /* error? */
	{
		fprintf( stderr, "\noutput_wavefile_write() : acmStreamConvert() failed,\n");
		switch( mmresult )
		{
			case ACMERR_BUSY :
			  fprintf( stderr, "The stream header specified in pash is currently in use and cannot be unprepared.");
			  break;
			case ACMERR_UNPREPARED :
			  fprintf( stderr, "The stream header specified in pash is currently not prepared by the acmStreamPrepareHeader function.");
			  break;
			case MMSYSERR_INVALFLAG :
			  fprintf( stderr,"At least one flag is invalid. ");
			  break;
			case MMSYSERR_INVALHANDLE :
			  fprintf( stderr,"The specified handle is invalid. " );
			  break;
			case MMSYSERR_INVALPARAM :
			  fprintf( stderr,"At least one parameter is invalid. ");
			  break;
			default:
			  fprintf( stderr, "Unknown error code (%ld)", mmresult );
		}
		exit( ERROR );
	}  /* endif ( mmresult != 0 ) */
	else
	{

		/* write the compressed samples ( m_lpacmbuffer[] )to our outputfile */
		//cbStored = fwrite( m_lpacmbuffer, 1, m_acmstreamheader.cbDstLengthUsed, m_fpout );
		cbStored = SaveData( m_fpout, m_lpacmbuffer, m_acmstreamheader.cbDstLengthUsed );
		m_cbWritten += cbStored; /* update file-writer counter */

		/* cbLeft = #bytes that remain to be converted, in m_lpdecbuffer[] */
		//cbLeft -= chunksize;
		cbLeft -= m_acmstreamheader.cbSrcLengthUsed; 

		/* shift decoder samples so the first UNused sample is now in position &m_lpdecbuffer[ 0 ] */
		if ( cbLeft > 0 )
			memcpy( (LPVOID)m_lpdecbuffer, (LPVOID) &m_lpdecbuffer[ (chunksize >> 1 ) ], cbLeft );
			/* cbSrcLengthUsed is a BYTE counter...but lpdecbuffer[] is short-int (16-bit) */
	} /* endelse */

  } while ( cbLeft >= chunksize );

	m_DecoderIndex = cbLeft; /* set DecoderIndex for next time */

//	m_acmstreamheader.cbDstLengthUsed;
//	m_acmstreamheader.cbSrcLengthUsed;

	/* only update the counter only every X ms, to reduce overhead */
	if ( getms()-l_time > .2 )
	{
		if ( m_cbWritten < 1024*1024 )
			printf("\r[%ldKb]", m_cbWritten/1024 );
		else
		if ( m_cbWritten < 10*1024*1024 )
			printf("\r[%.2fMb]", m_cbWritten/1048576.0 );
		else
		if ( m_cbWritten < 1024*1024*1024 )
			printf("\r[%.1fMb]", m_cbWritten/1048576.0 );
		else
			printf("\r[%1.3fGb]", m_cbWritten/(double)(1024*1024*1024) );

		l_time = getms();
		return 1;
	}
	return 0;

} /* endfunction output_wavefile_write() */







/* properly closes the ACMstream, and closes the file. */
long
output_wavefile_close( const char outname[]  )
{
	time_t finish_time, el_time; /* ending time, elapsed time */
	MMRESULT mmresult;

	// make sure we save all data and not leave any hanging out
	SaveData( m_fpout, NULL, NULL );

	/* close the output wavefile, so we can re-open in MODIFY mode */
	if ( fclose( m_fpout ) != 0 ){
		fprintf( stderr, "\nnoutput_wavefile_close() : ERROR Could not close %s!", outname );
	}

	m_fpout = fopen( outname, "rb+" ); /* re-open in binary, MODIFY mode */
	if ( m_fpout != NULL ){
		write_wavheader(); /* write the RIFF .WAV header */
		if ( fclose( m_fpout ) != 0 )
			fprintf( stderr, "\nnoutput_wavefile_close() : ERROR Could not re-close %s!", outname );
	} else {
		fprintf( stderr, "\nnoutput_wavefile_close() : ERROR Could not re-open %s!", outname );
		fprintf( stderr, "\n\tWAVEHEADER could not be rewritten!");
	}

	/* unprepare ACMheader, must do this before closing the ACMstream */
	mmresult = acmStreamUnprepareHeader( m_hacmstream, &m_acmstreamheader,0 );
	if ( mmresult != 0 ) /* error? */
	{
		fprintf( stderr, "\noutput_wavefile_close() : acmStreamUnprepareHeader() failed,\n");
		switch( mmresult )
		{
			case ACMERR_BUSY :
			  fprintf( stderr, "The stream header specified in pash is currently in use and cannot be unprepared.");
			  break;
			case ACMERR_UNPREPARED :
			  fprintf( stderr, "The stream header specified in pash is currently not prepared by the acmStreamPrepareHeader function.");
			  break;
			case MMSYSERR_INVALFLAG :
			  fprintf( stderr,"At least one flag is invalid. ");
			  break;
			case MMSYSERR_INVALHANDLE :
			  fprintf( stderr,"The specified handle is invalid. " );
			  break;
			case MMSYSERR_INVALPARAM :
			  fprintf( stderr,"At least one parameter is invalid. ");
			  break;
			default:
			  fprintf( stderr, "Unknown error code (%ld)", mmresult );
		}
		exit( ERROR );
	}  /* endif ( mmresult != 0 ) */


	/* close ACMstream */
	mmresult = acmStreamClose( m_hacmstream, 0 );
	if ( mmresult != 0 ) /* error? */
	{
		fprintf( stderr, "\noutput_wavefile_close() : acmStreamClose() failed, ");
		switch( mmresult )
		{
			case ACMERR_BUSY :
			  fprintf( stderr, "The stream header specified in pash is currently in use and cannot be unprepared.");
			  break;
			case MMSYSERR_INVALFLAG :
			  fprintf( stderr,"At least one flag is invalid. ");
			  break;
			case MMSYSERR_INVALHANDLE :
			  fprintf( stderr,"The specified handle is invalid. " );
			  break;
			default:
			  fprintf( stderr, "Unknown error code (%ld)", mmresult );
		}
		exit( ERROR );
	}  /* endif ( mmresult != 0 ) */

	free( m_lpacmbuffer); /* deallocate memory */
	free( m_lpoutwfx ); /* deallocate memory */

	time( &finish_time );	/* Record the finish time */

	/* time code copied from John Schlichther's AVI2MPG1 */
	el_time = finish_time - start_time;


	return el_time;
} /* void output_wavefile_close( const char outname[]  ) */



long get_wavwrite_time( void )
{
	time_t finish_time, el_time; /* ending time, elapsed time */
	time( &finish_time );	/* Record the finish time */

	/* time code copied from John Schlichther's AVI2MPG1 */
	el_time = finish_time - start_time;
	return el_time;

}
/* write the WAVE header to the output file m_fpout */
void
write_wavheader( void )
{
	BYTE *pHeader = NULL;
	DWORD m_cbHeader = 0;
	RIFFLIST *pRiffWave = NULL;
	RIFFCHUNK *pRiffFmt = NULL;
	RIFFCHUNK *pRiffData = NULL;
	HRESULT hr;
	DWORD cbWritten = 0;
	DWORD wfxsize = sizeof( WAVEFORMATEX ) + m_lpoutwfx->cbSize ;

	/* this code was taken from DXMedia6 SDK, WAVDEST sample */

	/* calculate total header-size for RIFF .WAV file  */
	m_cbHeader = sizeof(RIFFLIST) + sizeof(RIFFCHUNK) + wfxsize +
		sizeof(RIFFCHUNK);

	pHeader = (BYTE *)malloc(m_cbHeader);

	pRiffWave = (RIFFLIST *)pHeader;
	pRiffFmt = (RIFFCHUNK *)(pRiffWave + 1);
	pRiffData = (RIFFCHUNK *)(((BYTE *)(pRiffFmt + 1)) + wfxsize );

	pRiffData->fcc = FCC('data');
	pRiffData->cb = m_cbWritten; /* # bytes written to FILE m_fpout */

	pRiffFmt->fcc = FCC('fmt ');
	pRiffFmt->cb = wfxsize;

	CopyMemory(pRiffFmt + 1, m_lpoutwfx, pRiffFmt->cb);

	pRiffWave->fcc = FCC('RIFF');
	pRiffWave->cb = m_cbWritten + m_cbHeader - sizeof(RIFFCHUNK);
	pRiffWave->fccListType = FCC('WAVE');

	hr = fseek(m_fpout, STREAM_SEEK_SET, 0);
	if ( hr == 0 )
		cbWritten = fwrite( pHeader, 1, m_cbHeader, m_fpout );
	else
		fprintf( stderr,"\nwrite_wavheader() : Couldn't fseek() to position 0.");

	if ( cbWritten != m_cbHeader )
	{
		fprintf(stderr,"\nwrite_wavheader() : RIFF .WAV header is broken.");
	}

} /* void write_wavheader() */
