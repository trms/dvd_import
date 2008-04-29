// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// mpeg2enc.c: data and buffer creation, part of the mjpgTools Library													 //
// --------------------------------------------------------------------------------------------------------------------- //
// Copyleft(c)2002 by Laurens KOEHOORN <https://sourceforge.net/sendmessage.php?touser=240832>							 //
//																														 //
// This library is free software; you can redistribute it and/or modify it under the terms of the						 //
// GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License,	 //
// or (at your option) any later version.																				 //
//																														 //
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied	 //
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.														 //
// See the GNU Lesser General Public License for more details.															 //
//																														 //
// You should have received a copy of the GNU Lesser General Public License along with this library; if not,			 //
// write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA					 //
// --------------------------------------------------------------------------------------------------------------------- //
// This library is globaly based on :																					 //
//  - Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved.											 //
//																														 //
//		Disclaimer of Warranty																							 //
//		These software programs are available to the user without any license fee or royalty on an "as is" basis.		 //
//		The MPEG Software Simulation Group disclaims any and all warranties, whether express, implied, or statuary,		 //
//		including any implied warranties or merchantability or of fitness for a particular purpose.						 //
//		In no event shall the copyright-holder be liable for any incidental, punitive, or consequential damages of any	 //
//		kind whatsoever arising from the use of these programs.															 //
//		This disclaimer of warranty extends to the user of these programs and user's customers, employees, agents,		 //
//		transferees, successors, and assigns.																			 //
//		The MPEG Software Simulation Group does not represent or warrant that the programs furnished hereunder are free	 //
//		of infringement of any third-party patents.																		 //
//		Commercial implementations of MPEG-1 and MPEG-2 video, including shareware, are subject to royalty fees to		 //
//		patent holders. Many of these patents are general enough such that they are unavoidable regardless of			 //
//		implementation design.																							 //
//																														 //
// and partialy based on the modifications made by :																	 //
//	- Modifications and enhancements (C) 2000/2001 Andrew Stevens (MJPeGtools of the Linux-group)						 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//#include <config.h>
#include <stdio.h>
#include <stdlib.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <string.h>
//#include "types.h"
#include "global.h"
#include "mpeg2enc.h"
//#include "format_codes.h"
#include <windows.h>
//int verbose = 1; // already set in 'types.h'

/* global variables */

const char version[] = "MSSG+ 1.3 2001/6/10 (development of mpeg2encode V1.2, 96/07/19)";
const char author[]  = "(C) 1996, MPEG Software Simulation Group, (C) 2000, 2001 Andrew Stevens, Rainer Johanni";

/* zig-zag scan */
const uint8_t zig_zag_scan[64] = {
  0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,
  12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,
  35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,
  58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63
};

/* alternate scan */
const uint8_t alternate_scan[64] = {
  0,8,16,24,1,9,2,10,17,25,32,40,48,56,57,49,
  41,33,26,18,3,11,4,12,19,27,34,42,50,58,35,43,
  51,59,20,28,5,13,6,14,21,29,36,44,52,60,37,45,
  53,61,22,30,7,15,23,31,38,46,54,62,39,47,55,63
};

/* default intra quantization matrix */
const uint16_t default_intra_quantizer_matrix[64] = {
   8, 16, 19, 22, 26, 27, 29, 34,
  16, 16, 22, 24, 27, 29, 34, 37,
  19, 22, 26, 27, 29, 34, 34, 38,
  22, 22, 26, 27, 29, 34, 37, 40,
  22, 26, 27, 29, 32, 35, 40, 48,
  26, 27, 29, 32, 35, 40, 48, 58,
  26, 27, 29, 34, 38, 46, 56, 69,
  27, 29, 35, 38, 46, 56, 69, 83
};

const uint16_t hires_intra_quantizer_matrix[64] = {
   8, 16, 18, 20, 24, 25, 26, 30,
  16, 16, 20, 23, 25, 26, 30, 30,
  18, 20, 22, 24, 26, 28, 29, 31,
  20, 21, 23, 24, 26, 28, 31, 31,
  21, 23, 24, 25, 28, 30, 30, 33,
  23, 24, 25, 28, 30, 30, 33, 36,
  24, 25, 26, 29, 29, 31, 34, 38,
  25, 26, 28, 29, 31, 34, 38, 42
};

const uint16_t default_nonintra_quantizer_matrix[64] = {
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16,
	16, 16, 16, 16, 16, 16, 16, 16
};


/* Hires non intra quantization matrix.  THis *is*
	the MPEG default...	 */
const uint16_t *hires_nonintra_quantizer_matrix = &default_nonintra_quantizer_matrix[0];

/* non-linear quantization coefficient table */
const uint8_t non_linear_mquant_table[32] = {
   0, 1, 2, 3, 4, 5, 6, 7,
   8,10,12,14,16,18,20,22,
  24,28,32,36,40,44,48,52,
  56,64,72,80,88,96,104,112
};

/* non-linear mquant table for mapping from scale to code
 * since reconstruction levels are not bijective with the index map,
 * it is up to the designer to determine most of the quantization levels
 */

const uint8_t map_non_linear_mquant[113] = {
0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,
16,17,17,17,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,21,22,22,
22,22,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,
26,26,26,26,26,26,27,27,27,27,27,27,27,27,28,28,28,28,28,28,28,29,
29,29,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31
};

const char pict_type_char[6] = {'X', 'I', 'P', 'B', 'D', 'X'};

/* Support for the picture layer(!) insertion of scan data fieldsas
   MPEG user-data section as part of I-frames.  */

const uint8_t dummy_svcd_scan_data[14] = {
	0x10,                       /* Scan data tag */
	14,                         /* Length */
	0x00, 0x80, 0x81,            /* Dummy data - this will be filled */
	0x00, 0x80, 0x81,            /* By the multiplexor or cd image   */        
	0xff, 0xff, 0xff,            /* creation software                */        
	0xff, 0xff, 0xff
};


const char *statname = "mpeg2enc.stat";

/**************************************
 * global flags controlling MPEG syntax
 **************************************/

int opt_horizontal_size=0, opt_vertical_size=0; /* frame size (pels) */

int opt_aspectratio=0;			/* aspect ratio information (pel or display) */
int opt_frame_rate_code=0;		/* coded value of playback display
									 * frame rate */
int opt_dctsatlim=0;			/* Value to saturated DCT coeffs to */
double opt_frame_rate=0.0;		/* Playback display frames per
									   second N.b. when 3:2 pullback
									   is active this is higher than
									   the frame decode rate.
									*/
double opt_bit_rate=0.0;			/* bits per second */
int opt_seq_hdr_every_gop=0;
int opt_seq_end_every_gop=0;   /* Useful for Stills
									 * sequences... */
int opt_svcd_scan_data=0;
int opt_vbv_buffer_code=0;      /* Code for size of VBV buffer (*
									  * 16 kbit) */
double opt_vbv_buffer_size=0.0;
int opt_still_size=0;		     /* If non-0 encode a stills
									  * sequence: 1 I-frame per
									  * sequence pseudo VBR. Each
									  * frame sized to opt_still_size
									  * KB */
int opt_vbv_buffer_still_size=0;  /* vbv_buffer_size holds still
										  size.  Ensure still size
										  matches. */

int opt_constrparms=0;         /* constrained parameters flag (MPEG-1 only) */
int opt_load_iquant=0, opt_load_niquant=0;               /* use non-default quant. matrices */



int opt_profile=0, opt_level=0; /* syntax / parameter constraints */
int opt_prog_seq=0; /* progressive sequence */
int opt_chroma_format=0;
int opt_low_delay=0; /* no B pictures, skipped pictures */


/* sequence specific data (sequence display extension) */

int opt_video_format=0; /* component, PAL, NTSC, SECAM or MAC */
int opt_color_primaries=0; /* source primary chromaticity coordinates */
int opt_transfer_characteristics=0; /* opto-electronic transfer char. (gamma) */
int opt_matrix_coefficients=0; /* Eg,Eb,Er / Y,Cb,Cr matrix coefficients */
int opt_display_horizontal_size=0, opt_display_vertical_size=0; /* display size */


/* picture specific data (currently controlled by global flags) */

int opt_dc_prec=0;
int opt_topfirst=0;


int opt_mpeg1=1;				/* ISO/IEC IS 11172-2 sequence */
int opt_fieldpic=0;			/* use field pictures */
int opt_pulldown_32=0;			/* 3:2 Pulldown of movie material */



/* use only frame prediction and frame DCT (I,P,B) */
int opt_frame_pred_dct_tab[3];
int opt_qscale_tab[3]; 	/* linear/non-linear quantizaton table */
int opt_intravlc_tab[3]; /* intra vlc format (I,P,B) */
int opt_altscan_tab[3]; 	/* alternate scan (I,P,B */

struct motion_data *opt_motion_data=NULL;



/* Orginal intra / non_intra quantization matrices */
uint16_t opt_intra_q[64], opt_inter_q[64];


/* **************************
 * Global flags controlling encoding behaviour 
 ************************** */

double ctl_decode_frame_rate=0.0;	/* Actual stream frame
										   decode-rate. This is lower
										   than playback rate if 3:2
										   pulldown is active.
										*/
int ctl_video_buffer_size=0;    /* Video buffer requirement target */

int ctl_N_max=0;				/* number of frames in Group of Pictures (max) */
int ctl_N_min=0;				/* number of frames in Group of Pictures (min) */
int ctl_M=0;					/* distance between I/P frames */

int ctl_M_min=0;			    /* Minimum distance between I/P frames */

bool ctl_refine_from_rec=false;	/* Is final refinement of motion
								   compensation computed from
								   reconstructed reference frame image
								   (slightly higher quality, bad for
								   multi-threading) or original
								   reference frame (opposite) */

int ctl_44_red=0;			/* Sub-mean population reduction passes for 4x4 and 2x2 */
int ctl_22_red=0;			/* Motion compensation stages						*/
int ctl_seq_length_limit=0;
double ctl_nonvid_bit_rate=0.0;	/* Bit-rate for non-video to assume for
								   sequence splitting calculations */

double ctl_quant_floor=0.0;    /* quantisation floor [1..10] (0 for
										 * CBR) */


double ctl_act_boost=0.0;		/* Quantisation reduction for highly active blocks */


int ctl_max_encoding_frames=0; /* Maximum number of concurrent
									   frames to be concurrently encoded 
									   Used to control multi_threading.
									*/

bool ctl_parallel_read=false; /* Does the input reader / bufferer
								 run as a seperate thread?
							  */


/* *************************
 * input stream attributes
 ************************* */

int istrm_nframes=0;				/* total number of frames to encode
								   Note: this may start enormous and shrink
								   down later if the input stream length is
								   unknown at the start of encoding.
								   */
int istrm_fd=-1;

/* ***************************
 * Encoder internal derived values and parameters
 *************************** */


int opt_enc_width=0, opt_enc_height=0;   /* encoded frame size (pels) multiples of 16 or 32 */

int opt_phy_width=0, opt_phy_height=0;   /* Physical Frame buffer size (pels) may differ
					   * from encoded size due to alignment
					   * constraints */

int lum_buffer_size=0, chrom_buffer_size=0;

/* Buffers frame data */
uint8_t ***frame_buffers=NULL;

unsigned int frame_buffer_size=0;

/* Table driven intra / non-intra quantization matrices */
uint16_t i_intra_q[64], i_inter_q[64];
uint16_t intra_q_tbl[113][64], inter_q_tbl[113][64];
uint16_t i_intra_q_tbl[113][64], i_inter_q_tbl[113][64];
float intra_q_tblf[113][64], inter_q_tblf[113][64];
float i_intra_q_tblf[113][64], i_inter_q_tblf[113][64];



FILE *outfile=NULL, *statfile=NULL; /* file descriptors */
int inputtype=0; /* format of input frames */

/* sequence specific data (sequence header) */

int opt_phy_chrom_width=0, opt_phy_chrom_height=0, block_count=0;
int mb_width=0, mb_height=0; /* frame size (macroblocks) */
int opt_phy_width2=0, opt_phy_height2=0, opt_enc_height2=0, mb_height2=0, opt_phy_chrom_width2=0; /* picture size */
int qsubsample_offset=0, fsubsample_offset=0, rowsums_offset=0, colsums_offset=0;		/* Offset from picture buffer start of sub-sampled data... */

int mb_per_pict=0;			/* Number of macro-blocks in a picture */						

int frame_num=0;			/* Useful for triggering debug information */


 
/* prediction values for DCT coefficient (0,0) 
   TODO: This is a per-picture value and thus should be part of
   a picture record.
*/
int dc_dct_pred[3];

/* clipping (=saturation) table 
   This is really a pseudo-constant.  
   Probably best handled by initialising only once with a re-entrancy
   check...
*/
uint8_t *clp_0_255=NULL;


/***************************************************************
 * Wrapper for malloc that allocates pbuffers aligned to the 
 * specified byte boundary and checks for failure.
 * N.b.  don't try to free the resulting pointers, eh...
 ***************************************************************/
uint8_t *bufalloc( size_t size )
{
	uint8_t *buf = (uint8_t*)(malloc( size + BUFFER_ALIGN ));
	unsigned long adjust;
	mpg2enc_msgTitle = "bufalloc";

	if( buf == NULL )
	{
		errorExit_msg("malloc failed");
	}
	adjust = BUFFER_ALIGN-((unsigned long)buf)%BUFFER_ALIGN;
	if( adjust == BUFFER_ALIGN )
		adjust = 0;
	return (uint8_t*)(buf+adjust);
}


/*
static void Usage(char *str)
{
	fprintf(stderr,
"--verbose|-v num\n" 
"    Level of verbosity. 0 = quiet, 1 = normal 2 = verbose/debug\n"
"--format-f fmt\n"
"    Set pre-defined mux format fmt.\n"
"    [0 = Generic MPEG1, 1 = standard VCD, 2 = VCD,\n"
"     3 = Generic MPEG2, 4 = standard SVCD, 5 = user SVCD,\n"
"     6 = VCD Stills sequences, 7 = SVCD Stills sequences, 8 = DVD]\n"
"--aspect|-a num\n"
"    Set displayed image aspect ratio image (default: 2 = 4:3)\n"
"    [1 = 1:1, 2 = 4:3, 3 = 16:9, 4 = 2.21:1]\n"
"--frame-rate|-F num\n"
"    Set playback frame rate of encoded video\n"
"    (default: frame rate of input stream)\n"
"    0 = Display frame rate code table\n"
"--video-bitrate|-b num\n"
"    Set Bitrate of compress video in KBit/sec\n"
"    (default: 1152 for VCD, 2500 for SVCD, 3800 for DVD)\n"
"--nonvideo-birate|-B num\n"
"    Non-video data bitrate to assume for sequence splitting\n"
"    calculations (see also --sequence-length).\n"
"--quantisation|-q num\n"
"    Image data quantisation factor [1..31] (1 is best quality, no default)\n"
"    When quantisation is set variable bit-rate encoding is activated and\n"
"    the --bitrate value sets an *upper-bound* video data-rate\n"
"--output|-o pathname\n"
"    pathname of output file or fifo (REQUIRED!!!)\n"
"--vcd-still-size|-T size\n"
"    Size in KB of VCD stills\n"
"--interlace-mode|-I num\n"
"    Sets MPEG 2 motino estimation and encoding modes:\n"
"    0 = Progressive (non-interlaced)(Movies)\n"
"    1 = Interlaced source material (video)\n"
"--motion-search-radius|-r num\n"
"    Motion compensation search radius [0..32] (default 16)\n"
"--reduction-4x4|num\n"
"    Reduction factor for 4x4 subsampled candidate motion estimates\n"
"    [1..4] [1 = max quality, 4 = max. speed] (default: 2)\n"
"--reduction-2x2|-2 num\n"
"    Reduction factor for 2x2 subsampled candidate motion estimates\n"
"    [1..4] [1 = max quality, 4 = max. speed] (default: 3)\n"
"--min-gop-size|-g num\n"
"    Minimum size Group-of-Pictures (default 12)\n"
"--max-gop-size|-G num\n"
"    Maximum size Group-of-Pictures (default 12)\n"
"    If min-gop is less than  max-gop, mpeg2enc attempts to place GOP\n"
"    boundaries to coincide with scene changes\n"
"--force-b-b-p|-P\n"
"    Preserve two B frames between I/P frames when placing GOP boundaries\n"
"--quantisation-reduction|-Q num\n"
"    Max. quantisation reduction for highly active blocks\n"
"    [0.0 .. 5] (default: 0.0)\n"
"--video-buffer|-V num\n"
"    Target decoders video buffer size in KB (default 46)\n"
"--video-norm|-n n|p|s\n"
"    Tag output to suit playback in specified video norm\n"
"    (n = NTSC, p = PAL, s = SECAM) (default: PAL)\n"
"--sequence-length|-S num\n"
"    Place a sequence boundary in the video stream so they occur every\n"
"    num Mbytes once the video is multiplexed with audio etc.\n"
"    N.b. --non-video-bitrate is used to the bitrate of the other\n"
"    data that will be multiplexed with this video stream\n"
"--3-2-pulldown|-p\n"
"    Generate header flags for 3-2 pull down of 24fps movie material\n"
"--intra_dc_prec|-D [8..10]\n"
"    Set number of bits precision for DC (base colour) of blocks in MPEG-2\n"
"--reduce-hf|-N\n"
"    Reduce high frequency resolution - useful as a mild noise reduction\n"
"--keep-hf|-h\n"
"    Maximise high-frequency resolution - useful for high quality sources\n"
"    and/or high bit-rates)\n"
"--sequence-header-every-gop|-s\n"
"    Include a sequence header every GOP if the selected format doesn't\n"
"    do so by default.\n"
"--no-dummy-svcd-SOF|-d\n"
"    Don't generate of dummy SVCD scan-data for the ISO CD image\n"
"    generator \"vcdimager\" to fill in.\n"
"--playback-field-order|-z b|t\n"
"    Force setting of playback field order to bottom or top first\n"
"--multi-thread|-M num\n"
"    Activate multi-threading to optimise through on a system with num CPU's\n""    [0..32], 0=no multithreading, (default: 1)\n"
"--correct-svcd-hds|-C\n"
"    Force SVCD horizontal_display_size to be 480 - standards say 540 or 720\n"
"    But many DVD/SVCD players screw up with these values.\n"
"--no-altscan-mpeg2\n"
"    Force MPEG2 *not* to use alternate block scanning.  This may allow some\n"
"    buggy players to play SVCD streams\n"
"--help|-?\n"
"    Print this lot out!\n"
	);
	exit(0);
}





int main(argc,argv)
	int argc;
	char *argv[];
{
	char *outfilename=0;
	int nerr = 0;
	int n;

	// Set up error logging.  The initial handling level is LOG_INFO

static const char	short_options[]=
	"m:a:f:n:b:z:T:B:q:o:S:I:r:M:4:2:Q:D:g:G:v:V:F:tpdsZNhOCP";

#ifdef HAVE_GETOPT_LONG
static struct option long_options[]={
     { "verbose",           1, 0, 'v' },
     { "format",            1, 0, 'f' },
     { "aspect",            1, 0, 'a' },
     { "frame-rate",        1, 0, 'F' },
     { "video-bitrate",     1, 0, 'b' },
     { "nonvideo-bitrate",  1, 0, 'B' },
     { "intra_dc_prec",     1, 0, 'D' },
     { "quantisation",      1, 0, 'q' },
     { "output",            1, 0, 'o' },
     { "vcd-still-size",    1, 0, 'T' },
     { "interlace-mode",    1, 0, 'I' },
     { "motion-search-radius", 1, 0, 'r'},
     { "reduction-4x4",  1, 0, '4'},
     { "reduction-2x2",  1, 0, '2'},
     { "min-gop-size",      1, 0, 'g'},
     { "max-gop-size",      1, 0, 'G'},
     { "force-b-b-p", 0, &param_preserve_B, 1},
     { "quantisation-reduction", 1, 0, 'Q' },
     { "video-buffer",      1, 0, 'V' },
     { "video-norm",        1, 0, 'n' },
     { "sequence-length",   1, 0, 'S' },
     { "3-2-pulldown",      1, &param_32_pulldown, 1 },
     { "keep-hf",           0, &param_hf_quant, 2 },
     { "reduce-hf",         0, &param_hf_quant, 1 },
     { "sequence-header-every-gop", 0, &param_seq_hdr_every_gop, 1},
     { "no-dummy-svcd-SOF", 0, &param_svcd_scan_data, 0 },
     { "correct-svcd-hds", 0, &param_hack_svcd_hds_bug, 0},
     { "no-altscan-mpeg2", 0, &param_hack_altscan_bug, 1},
     { "playback-field-order", 1, 0, 'z'},
     { "multi-thread",      1, 0, 'M' },
     { "help",              0, 0, '?' },
     { 0,                   0, 0, 0 }
};

    while( (n=getopt_long(argc,argv,short_options,long_options, NULL)) != -1 )
#else
    while( (n=getopt(argc,argv,short_options)) != -1)
#endif
	{
		switch(n) {
        case 0 :                // Flag setting handled by getopt-long
            break;
		case 'b':
			param_bitrate = atoi(optarg)*1000;
			break;

		case 'T' :
			param_still_size = atoi(optarg)*1024;
			if( param_still_size < 20*1024 || param_still_size > 500*1024 )
			{
				mjpeg_error( "-T requires arg 20..500" );
				++nerr;
			}
			break;

		case 'B':
			param_nonvid_bitrate = atoi(optarg);
			if( param_nonvid_bitrate < 0 )
			{
				mjpeg_error("-B requires arg > 0");
				++nerr;
			}
			break;

        case 'D':
            param_mpeg2_dc_prec = atoi(optarg)-8;
            if( param_mpeg2_dc_prec < 0 || param_mpeg2_dc_prec > 2 )
            {
                mjpeg_error( "-D requires arg [8..10]" );
                ++nerr;
            }
            break;
        case 'C':
            param_hack_svcd_hds_bug = 0;
            break;

		case 'q':
			param_quant = atoi(optarg);
			if(param_quant<1 || param_quant>32)
			{
				mjpeg_error("-q option requires arg 1 .. 32");
				++nerr;
			}
			break;

        case 'a' :
			param_aspect_ratio = atoi(optarg);
            if( param_aspect_ratio == 0 )
				DisplayAspectRatios();
			// Checking has to come later once MPEG 1/2 has been selected...
			if( param_aspect_ratio < 0 )
			{
				mjpeg_error( "-a option must be positive");
				++nerr;
			}
			break;

       case 'F' :
			param_frame_rate = atoi(optarg);
            if( param_frame_rate == 0 )
				DisplayFrameRates();
			if( param_frame_rate < 0 || 
				param_frame_rate >= mpeg_num_framerates)
			{
				mjpeg_error( "-F option must be [0..%d]", 
						 mpeg_num_framerates-1);
				++nerr;
			}
			break;

		case 'o':
			outfilename = optarg;
			break;

		case 'I':
			param_fieldenc = atoi(optarg);
			if( param_fieldenc < 0 || param_fieldenc > 2 )
			{
				mjpeg_error("-I option requires 0,1 or 2");
				++nerr;
			}
			break;

		case 'r':
			param_searchrad = atoi(optarg);
			if(param_searchrad<0 || param_searchrad>32)
			{
				mjpeg_error("-r option requires arg 0 .. 32");
				++nerr;
			}
			break;

		case 'M':
			param_num_cpus = atoi(optarg);
			if(param_num_cpus<0 || param_num_cpus>32)
			{
				mjpeg_error("-M option requires arg 0..32");
				++nerr;
			}
			break;

		case '4':
			param_44_red = atoi(optarg);
			if(param_44_red<0 || param_44_red>4)
			{
				mjpeg_error("-4 option requires arg 0..4");
				++nerr;
			}
			break;
			
		case '2':
			param_22_red = atoi(optarg);
			if(param_22_red<0 || param_22_red>4)
			{
				mjpeg_error("-2 option requires arg 0..4");
				++nerr;
			}
			break;

		case 'v':
			verbose = atoi(optarg);
			if( verbose < 0 || verbose > 2 )
				++nerr;
			break;
		case 'V' :
			param_video_buffer_size = atoi(optarg);
			if(param_video_buffer_size<20 || param_video_buffer_size>4000)
			{
				mjpeg_error("-v option requires arg 20..4000");
				++nerr;
			}
			break;

		case 'S' :
			param_seq_length_limit = atoi(optarg);
			if(param_seq_length_limit<1 )
			{
				mjpeg_error("-S option requires arg > 1");
				++nerr;
			}
			break;
		case 'p' :
			param_32_pulldown = 1;
			break;

		case 'z' :
			if( strlen(optarg) != 1 || (optarg[0] != 't' && optarg[0] != 'b' ) )
			{
				mjpeg_error("-z option requires arg b or t" );
				++nerr;
			}
			else if( optarg[0] == 't' )
				param_force_interlacing = Y4M_ILACE_TOP_FIRST;
			else if( optarg[0] == 'b' )
				param_force_interlacing = Y4M_ILACE_BOTTOM_FIRST;
			break;

		case 'f' :
			param_format = atoi(optarg);
			if( param_format < MPEG_FORMAT_FIRST ||
				param_format > MPEG_FORMAT_LAST )
			{
				mjpeg_error("-f option requires arg [%d..%d]", 
							MPEG_FORMAT_FIRST, MPEG_FORMAT_LAST);
				++nerr;
			}
				
			break;

		case 'n' :
			switch( optarg[0] )
			{
			case 'p' :
			case 'n' :
			case 's' :
				param_norm = optarg[0];
				break;
			default :
				mjpeg_error("-n option requires arg n or p, or s.");
				++nerr;
			}
			break;
		case 'g' :
			param_min_GOP_size = atoi(optarg);
			break;
		case 'G' :
			param_max_GOP_size = atoi(optarg);
			break;
		case 'P' :
			param_preserve_B = true;
			break;
		case 'N':
			param_hf_quant = 1;
			break;
		case 'h':
			param_hf_quant = 2;
			break;
		case 's' :
			param_seq_hdr_every_gop = 1;
			break;
		case 'd' :
			param_svcd_scan_data = 0;
			break;
		case 'Q' :
			param_act_boost = atof(optarg);
			if( param_act_boost <-4.0 || param_act_boost > 10.0)
			{
				mjpeg_error( "-q option requires arg -4.0 .. 10.0");
				++nerr;
			}
			break;
		case ':' :
			mjpeg_error( "Missing parameter to option!" );
		case '?':
		default:
			++nerr;
		}
	}

    if( nerr )
		Usage(argv[0]);

	mjpeg_default_handler_verbosity(verbose);



	return 0;
}
*/


void Init_Data(void)
{
	/**************************************
	 * global flags controlling MPEG syntax
	 **************************************/
	opt_horizontal_size								= 0;
	opt_vertical_size								= 0;
	opt_aspectratio									= 0;
	opt_frame_rate_code								= 0;
	opt_dctsatlim									= 0;
	opt_frame_rate									= 0.0;
	opt_bit_rate									= 0.0;
	opt_seq_hdr_every_gop							= 0;
	opt_seq_end_every_gop							= 0;
	opt_svcd_scan_data								= 0;
	opt_vbv_buffer_code								= 0;
	opt_vbv_buffer_size								= 0.0;
	opt_still_size									= 0;
	opt_vbv_buffer_still_size						= 0;
	opt_constrparms									= 0;
	opt_load_iquant									= 0;
	opt_load_niquant								= 0;
	opt_profile										= 0;
	opt_level										= 0;
	opt_prog_seq									= 0;
	opt_chroma_format								= 0;
	opt_low_delay									= 0;
	opt_video_format								= 0;
	opt_color_primaries								= 0;
	opt_transfer_characteristics					= 0;
	opt_matrix_coefficients							= 0;
	opt_display_horizontal_size						= 0;
	opt_display_vertical_size						= 0;
	opt_dc_prec										= 0;
	opt_topfirst									= 0;
	opt_mpeg1										= 1;
	opt_fieldpic									= 0;
	opt_pulldown_32									= 0;
	ZeroMemory(&opt_frame_pred_dct_tab, sizeof(int[3]));
	ZeroMemory(&opt_qscale_tab, sizeof(int[3]));
	ZeroMemory(&opt_intravlc_tab, sizeof(int[3]));
	ZeroMemory(&opt_altscan_tab, sizeof(int[3]));
	opt_motion_data									= NULL;
	ZeroMemory(&opt_intra_q, sizeof(uint16_t[64]));
	ZeroMemory(&opt_inter_q, sizeof(uint16_t[64]));
	/**********************************************
	 * Global flags controlling encoding behaviour 
	 **********************************************/
	ctl_decode_frame_rate							= 0.0;
	ctl_video_buffer_size							= 0;
	ctl_N_max										= 0;
	ctl_N_min										= 0;
	ctl_M											= 0;
	ctl_M_min										= 0;
	ctl_refine_from_rec								= false;
	ctl_44_red										= 0;
	ctl_22_red										= 0;
	ctl_seq_length_limit							= 0;
	ctl_nonvid_bit_rate								= 0.0;
	ctl_quant_floor									= 0.0;
	ctl_act_boost									= 0.0;
	ctl_max_encoding_frames							= 0;
	ctl_parallel_read								= false;
	/***************************
	 * input stream attributes
	 ***************************/
	istrm_nframes									= 0;
	istrm_fd										= -1;
	/*************************************************
	 * Encoder internal derived values and parameters
	 *************************************************/
	opt_enc_width									= 0;
	opt_enc_height									= 0;
	opt_phy_width									= 0;
	opt_phy_height									= 0;
	lum_buffer_size									= 0;
	chrom_buffer_size								= 0;
	frame_buffers									= NULL;
	frame_buffer_size								= 0;
	ZeroMemory(&i_intra_q, sizeof(uint16_t[64]));
	ZeroMemory(&i_inter_q, sizeof(uint16_t[64]));
	ZeroMemory(&intra_q_tbl, sizeof(uint16_t[113][64]));
	ZeroMemory(&inter_q_tbl, sizeof(uint16_t[113][64]));
	ZeroMemory(&i_intra_q_tbl, sizeof(uint16_t[113][64]));
	ZeroMemory(&i_inter_q_tbl, sizeof(uint16_t[113][64]));
	ZeroMemory(&intra_q_tblf, sizeof(float[113][64]));
	ZeroMemory(&inter_q_tblf, sizeof(float[113][64]));
	ZeroMemory(&i_intra_q_tblf, sizeof(float[113][64]));
	ZeroMemory(&i_inter_q_tblf, sizeof(float[113][64]));
	outfile											= NULL;
	statfile										= NULL;
	inputtype										= 0;
	opt_phy_chrom_width								= 0;
	opt_phy_chrom_height							= 0;
	block_count										= 0;
	mb_width										= 0;
	mb_height										= 0;
	opt_phy_width2									= 0;
	opt_phy_height2									= 0;
	opt_enc_height2									= 0;
	mb_height2										= 0;
	opt_phy_chrom_width2							= 0;
	qsubsample_offset								= 0;
	fsubsample_offset								= 0;
	rowsums_offset									= 0;
	colsums_offset									= 0;
	mb_per_pict										= 0;
	frame_num										= 0;
	ZeroMemory(&dc_dct_pred, sizeof(int[3]));
	clp_0_255										= NULL;
}