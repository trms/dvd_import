#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/timeb.h>

#include "ac3.h"
#include "ac3decode.h"
#include "bitstream.h"
#include "imdct.h"
#include "exponent.h"
#include "mantissa.h"
#include "bit_allocate.h"
#include "uncouple.h"
#include "parse.h"
#include "output.h"
#include "crc.h"
#include "rematrix.h"
#include "time.h"
#include "debug.h"

//static void decode_find_sync(bitstream_t *bs);
static void decode_get_sync(bitstream_t *bs);
static int decode_resync(bitstream_t *bs);



double getms( void )
{
	double  usec1;
	struct timeb t;

//	ftime( &t );

	usec1 = time(0);	//t.time + t.millitm/1000.0;
	return usec1;
}


char *strstri(const char * str, const char * pat )
{
	unsigned char * s1 = (unsigned char *) str;
	unsigned char * p1 = (unsigned char *) pat;
	unsigned char firstc, c1, c2;
	
	if ( !(firstc = tolower(*p1)) )		/* if pat is an empty string we return str */
		return((char *) str);
	p1++;

	while(c1 = tolower(*s1) ){
		s1++;
		if (c1 == firstc)
		{
			const unsigned char * s2 = s1;
			const unsigned char * p2 = p1;
			short cnt=0;

			while ( (c1 = tolower(*s2) ) == (c2 = tolower(*p2) ) && c1 ){
				s2++;
				p2++;
			}
			
			if (!c2)
				return((char *) s1 - 1);
		}
	}
	return(NULL);
}














static stream_coeffs_t stream_coeffs;
static stream_samples_t stream_samples;
static audblk_t audblk;
static bsi_t bsi;
static syncinfo_t syncinfo;
static uint_32 frame_count = 0;

/* global variables */
FILE *g_pInfile; /* filepointer to input bitstream */
int g_fUseDemuxer; /* flag, UseDemuxer (bbdmux) ? */

int	m_force44100 = 0;
int	m_lba_seek = 0;
int	m_allvobs = 0;
int	m_showsummary = 0;
int m_overwrite = 0;
int m_substream = 0;
int m_gainlevel = 0;
int m_gain2level = 0;
int m_gaincenter = 0;
int m_gainrear = 0;
int m_gainlfe = 0;
long m_maxlength = 0;
char m_inputfile[256];

int i, ch;
int n; /* return value from decode_resync() */
bitstream_t *bs;
int fUserQuit = 0; /* flag, user wants to quit? */
long cbIteration = 0; /* while loop counter  */
long SampleRate = 0; /* bitstream sample-rate */
uint_32 cbErrors = 0; /* bitstream error count returned by
						decode_sanity_check() */
uint_32 cbSkippedFrames = 0; /* running total of skipped frames */
uint_32 cbDecodedFrames = 0; /* running total, #decoded frames */
uint_32 cbMantErrors, cbExpErrors;
	/* error counters for mantissa & exponent unpacking errors */
double	time_start, time_end, time_other=0, timesofar, x;
audblk_t audblk_blank = {0};
bsi_t bsi_blank = {0};



FILE *OpenAc3( char *filename )
{
	g_fUseDemuxer = 0; /* initially set to false */

	strcpy( m_inputfile, filename );
	/* check input-filename's extension for *.VOB, *.M2V, *.MPG */
	/* if so, we want to use BBDMUX and the demux_buffer system */
	if ( strstri(filename, ".vob")  || strstri(filename, ".mpg")  ||
		strstri(filename, ".m2v") )
		g_fUseDemuxer = 1;
	  /* g_fUseDemuxer is used by only one other file, my_fread.cpp */

	g_pInfile = fopen( filename ,"rb");	

	if( g_pInfile ){
		bs = bitstream_open(g_pInfile);

		/* initialize decoder and counter variables */
		imdct_init();
		(bsi) = bsi_blank;
		(audblk) = audblk_blank;
		decode_sanity_check_init();
		bsi_blank = bsi;
		audblk_blank = audblk;
		cbErrors = 0;
		cbIteration = 0;
		cbSkippedFrames = 0;
		cbDecodedFrames = 0;
		cbMantErrors = 0;
		cbExpErrors = 0;
		frame_count = 0;

		audblk = audblk_blank; /* clear out audioblock */

		n = decode_resync(bs);
		if ( n >= 0 ){
			bsi = bsi_blank; /* v0.04 wipe bsi clear, not really necessary */
			parse_syncinfo(&syncinfo,bs);
			parse_bsi(&bsi,bs);

			switch (syncinfo.fscod)	{
				case 2:		SampleRate = 32000;		break;
				case 1:		SampleRate = 44100;		break;
				case 0:		SampleRate = 48000;		break;
				default:	exit( 1);
			}
			time_start = getms();
		}
	}
	return g_pInfile;
}

int DecodeAc3( char *output_buffer )
{
	int	process = 0;

	fUserQuit = 0;

	while( !process && output_buffer )
	{
		if ( cbIteration != 0 ){
			audblk = audblk_blank; /* clear out audioblock */

			n = decode_resync(bs);
			if ( n < 0 )	/* stop if we have serious file errors */
				fUserQuit = 2; /* set flag */
			if ( !bs->file )
				break;

			/* if the error count (calculated by previous loop iteration)
			   exceeds our defined threshold, force a resync. */
			if ( cbErrors > ERR_RESYNC_THRESHOLD ||
				cbMantErrors > ERR_MANT_THRESHOLD ||
				cbExpErrors > ERR_EXP_THRESHOLD ){
				++cbSkippedFrames;
			}

			bsi = bsi_blank; /* v0.04 wipe bsi clear, not really necessary */
			parse_syncinfo(&syncinfo,bs);
			parse_bsi(&bsi,bs);
		}

		++cbIteration; /* increment counter */

		cbErrors = 0; /* reset bitstream error counter */
		cbExpErrors = 0;/* reset bitstream error counter */
		cbMantErrors = 0;/* reset bitstream error counter */
		for(i=0; (i < 6) && (cbErrors < ERR_RESYNC_THRESHOLD ); i++)
		{
			/* Extract most of the audblk info from the bitstream
			 * (minus the mantissas */
			parse_audblk(&bsi,&audblk,bs,i);					// CPU time 10%
			cbErrors = decode_sanity_check();
			if ( cbErrors >= ERR_RESYNC_THRESHOLD )
				break;

			/* Take the differential exponent data and turn it into
			 * absolute exponents */
			cbExpErrors += exponent_unpack(&bsi,&audblk,&stream_coeffs);
			cbErrors = decode_sanity_check();					// CPU TIME .03%
			if ( cbErrors >= ERR_RESYNC_THRESHOLD ||
				cbExpErrors >= ERR_EXP_THRESHOLD )
				break;

			/* Figure out how many bits per mantissa */
			bit_allocate(syncinfo.fscod,&bsi,&audblk);			// CPU TIME 1.2%
			cbErrors = decode_sanity_check();
			if ( cbErrors >= ERR_RESYNC_THRESHOLD )
				break;

			/* Extract the mantissas from the data stream */
			x = getms();
			cbMantErrors += mantissa_unpack(&bsi,&audblk,bs);	// CPU TIME 62.0%
			time_other += (getms() - x );
			cbErrors = decode_sanity_check();
			if ( cbErrors >= ERR_RESYNC_THRESHOLD ||
				cbMantErrors >= ERR_MANT_THRESHOLD )
				break;

			/* Uncouple the coupling channel if it exists and
			 * convert the mantissa and exponents to IEEE floating
			 * point format */
			uncouple(&bsi,&audblk,&stream_coeffs);				// CPU TIME 1.7%
			cbErrors = decode_sanity_check();
			if ( cbErrors >= ERR_RESYNC_THRESHOLD )
				break;

			if(bsi.acmod == 0x2)
				rematrix(&audblk,&stream_coeffs);				// CPU TIME 0.1%

			/* Convert the frequency data into time samples */
			imdct(&bsi,&audblk,&stream_coeffs,&stream_samples);	// CPU TIME 11.2%
			cbErrors = decode_sanity_check();
			if ( cbErrors >= ERR_RESYNC_THRESHOLD )
				break;

			/* Send the samples to the output device */
			n = 0;

			downmix( &stream_samples, output_buffer, &bsi );
			output_buffer+=(256*2*2);

			process++;
			timesofar = getms() - time_start;
		} /* endfor ( i = 0 ... ) */

		/* v0.04, if cbErrors exceeds defined threshold, force frame-resync */
		if ( cbErrors >= ERR_RESYNC_THRESHOLD ||
			cbExpErrors >= ERR_EXP_THRESHOLD ||
			cbMantErrors >= ERR_MANT_THRESHOLD )
		{
			continue; /* return to TOP of while-loop */
		}

		parse_auxdata(&syncinfo,bs);			// CPU TIME 2.0%

		/* v0.04, if cbErrors exceeds defined threshold, force frame-resync */
		cbErrors = decode_sanity_check();
		if ( cbErrors >= ERR_RESYNC_THRESHOLD )
		{
			continue; /* return to TOP of while-loop */
		}

		++cbDecodedFrames; /* update # successfully decoded frames */

		if ( m_maxlength && cbIteration ) {
			long samplesout = (double)cbDecodedFrames * 1536.0;
			long secondsout = samplesout / SampleRate;
			if ( secondsout >= m_maxlength )
				fUserQuit = 3; /* set flag */
		}
	} /* endwhile (1) */
/* ################ end of main decoding loop ################## */
	if( fUserQuit ){
		time_end = getms();

		bitstream_close(bs);
		g_pInfile = 0;
		demux_ac3(0);
	}
	return fUserQuit;
}

void CloseAc3( void )
{
	fUserQuit = 1;

	bitstream_close(bs);
	g_pInfile = 0;
	demux_ac3(0);

	timesofar = time_end - time_start;

}

static
void decode_get_sync(bitstream_t *bs)
{
	uint_16 sync_word;
	uint_32 i = 0;

	sync_word = bitstream_get(bs,16);

	frame_count++;

	bs->total_bits_read = 16;
	crc_init();
}


static  int decode_resync(bitstream_t *bs) {
	uint_16 sync_word;
	int i = 0;

	if ( !bs->file ) return -1;

	sync_word = bitstream_get(bs,16);

	/* Make sure we sync'ed */
	while(1)
	{
		if(sync_word == 0x0b77 )
			break;
		sync_word = ( sync_word << 1 );
		sync_word |= bitstream_get(bs,1);
		i++;
		if ( i>32 ){		// fix added to handle EOF or closed files
			if ( ftell( bs->file )==-1 || feof(bs->file) ){
				i = -1;
				break;
			}
		}
	}
	frame_count++;

	bs->total_bits_read = 16;
	crc_init();
	return i; /* return # bits skipped */
}


void
decode_sanity_check_init(void)
{
	syncinfo.magic = DECODE_MAGIC_NUMBER;
	bsi.magic = DECODE_MAGIC_NUMBER;
	audblk.magic1 = DECODE_MAGIC_NUMBER;
	audblk.magic2 = DECODE_MAGIC_NUMBER;
	audblk.magic3 = DECODE_MAGIC_NUMBER;
}


/* decode_sanity_check() now returns # errors detected */
long
decode_sanity_check(void)
{
	int i;
	long cbError = 0; /* error count */

	if(syncinfo.magic != DECODE_MAGIC_NUMBER)
	{
		++cbError;
	}

	if(bsi.magic != DECODE_MAGIC_NUMBER)
	{
		++cbError;
	}

	if(audblk.magic1 != DECODE_MAGIC_NUMBER)
	{
		++cbError;
	}

	if(audblk.magic2 != DECODE_MAGIC_NUMBER)
	{
		++cbError;
	}

	if(audblk.magic3 != DECODE_MAGIC_NUMBER)
	{
		++cbError;
	}

	for(i = 0;i < 5 ; i++)
	{
		if (audblk.fbw_exp[i][255] !=0 || audblk.fbw_exp[i][254] !=0 ||
				audblk.fbw_exp[i][253] !=0)
		{
			audblk.fbw_exp[i][255] = 0;
			audblk.fbw_exp[i][254] = 0;
			audblk.fbw_exp[i][253] = 0;
			++cbError;
		}

		if (audblk.fbw_bap[i][255] !=0 || audblk.fbw_bap[i][254] !=0 ||
				audblk.fbw_bap[i][253] !=0)
		{
			audblk.fbw_bap[i][255] = 0;
			audblk.fbw_bap[i][254] = 0;
			audblk.fbw_bap[i][253] = 0;
			++cbError;
		}

		if (audblk.chmant[i][255] !=0 || audblk.chmant[i][254] !=0 ||
				audblk.chmant[i][253] !=0)
		{
			audblk.chmant[i][255] = 0;
			audblk.chmant[i][254] = 0;
			audblk.chmant[i][253] = 0;
			++cbError;
		}
	} /* endfor ( i=0 ... ) */

	if (audblk.cpl_exp[255] !=0 || audblk.cpl_exp[254] !=0 ||
			audblk.cpl_exp[253] !=0)
	{
		audblk.cpl_exp[255] = 0;
		audblk.cpl_exp[254] = 0;
		audblk.cpl_exp[253] = 0;
		++cbError;
	}

	if (audblk.cpl_bap[255] !=0 || audblk.cpl_bap[254] !=0 ||
			audblk.cpl_bap[253] !=0)
	{
		audblk.cpl_bap[255] = 0;
		audblk.cpl_bap[254] = 0;
		audblk.cpl_bap[253] = 0;
		++cbError;
	}

	if (audblk.cplmant[255] !=0 || audblk.cplmant[254] !=0 ||
			audblk.cplmant[253] !=0)
	{
		audblk.cplmant[255] = 0;
		audblk.cplmant[254] = 0;
		audblk.cplmant[253] = 0;
		++cbError;
	}

	if ((audblk.cplinu == 1) && (audblk.cplbegf > (audblk.cplendf+2)))
	{
		++cbError;
	}

	for(i=0; i < bsi.nfchans; i++)
	{
		if((audblk.chincpl[i] == 0) && (audblk.chbwcod[i] > 60))
		{
			audblk.chbwcod[i] = 60;
			++cbError;
		}
	} /* endfor ( i = 0 ... ) */

	return cbError;
}


