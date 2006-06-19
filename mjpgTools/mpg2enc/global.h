// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// global.h, global variables, function prototypes, part of the mjpgTools Library										 //
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
#include <stdio.h>
#include "mpeg2enc.h"

/* choose between declaration (GLOBAL undefined)
 * and definition (GLOBAL defined)
 * GLOBAL is defined in exactly one file (mpeg2enc.c)
 */

#ifndef __cplusplus

#ifndef bool
typedef int16_t	bool;
#define false	0
#define true	!false
#endif

#ifndef NULL
#define NULL	0
#endif

#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif


//#define BITCOUNT_OFFSET  800000000LL
#define BITCOUNT_OFFSET					0

/* global variables */

extern const char version[];
extern const char author[];

/* zig-zag scan */
extern const uint8_t zig_zag_scan[64];

/* alternate scan */
extern const uint8_t alternate_scan[64];

/* default intra quantization matrix */
extern const uint16_t default_intra_quantizer_matrix[64];

extern const uint16_t hires_intra_quantizer_matrix[64];

extern const uint16_t default_nonintra_quantizer_matrix[64];


/* Hires non intra quantization matrix.  THis *is*
	the MPEG default...	 */
extern const uint16_t *hires_nonintra_quantizer_matrix;

/* non-linear quantization coefficient table */
extern const uint8_t non_linear_mquant_table[32];

/* non-linear mquant table for mapping from scale to code
 * since reconstruction levels are not bijective with the index map,
 * it is up to the designer to determine most of the quantization levels
 */

extern const uint8_t map_non_linear_mquant[113];

extern const char pict_type_char[6];

/* Support for the picture layer(!) insertion of scan data fieldsas
   MPEG user-data section as part of I-frames.  */

extern const uint8_t dummy_svcd_scan_data[14];


extern const char *statname;

/*
  How many frames to read in one go and the size of the frame data buffer.
*/

#define READ_CHUNK_SIZE								5
//#define FRAME_BUFFER_SIZE (READ_CHUNK_SIZE*9)

/*
  How many frames encoding may be concurrently under way.
  N.b. there is no point setting this greater than M.
  Additional parallelism can be exposed at a finer grain by
  parallelising per-macro-block computations.
 */

#define MAX_WORKER_THREADS							4


/* *************************************
 * global flags controlling MPEG syntax
 ************************************ */

extern int opt_horizontal_size, opt_vertical_size; /* frame size (pels) */

extern int opt_aspectratio;			/* aspect ratio information (pel or display) */
extern int opt_frame_rate_code;		/* coded value of playback display
									 * frame rate */
extern int opt_dctsatlim;			/* Value to saturated DCT coeffs to */
extern double opt_frame_rate;		/* Playback display frames per
									   second N.b. when 3:2 pullback
									   is active this is higher than
									   the frame decode rate.
									*/
extern double opt_bit_rate;			/* bits per second */
extern int opt_seq_hdr_every_gop;
extern int opt_seq_end_every_gop;   /* Useful for Stills
									 * sequences... */
extern int opt_svcd_scan_data;
extern int opt_vbv_buffer_code;      /* Code for size of VBV buffer (*
									  * 16 kbit) */
extern double opt_vbv_buffer_size;
extern int opt_still_size;		     /* If non-0 encode a stills
									  * sequence: 1 I-frame per
									  * sequence pseudo VBR. Each
									  * frame sized to opt_still_size
									  * KB */
extern int opt_vbv_buffer_still_size;  /* vbv_buffer_size holds still
										  size.  Ensure still size
										  matches. */

extern int opt_constrparms;         /* constrained parameters flag (MPEG-1 only) */
extern int opt_load_iquant, opt_load_niquant;               /* use non-default quant. matrices */



extern int opt_profile, opt_level; /* syntax / parameter constraints */
extern int opt_prog_seq; /* progressive sequence */
extern int opt_chroma_format;
extern int opt_low_delay; /* no B pictures, skipped pictures */


/* sequence specific data (sequence display extension) */

extern int opt_video_format; /* component, PAL, NTSC, SECAM or MAC */
extern int opt_color_primaries; /* source primary chromaticity coordinates */
extern int opt_transfer_characteristics; /* opto-electronic transfer char. (gamma) */
extern int opt_matrix_coefficients; /* Eg,Eb,Er / Y,Cb,Cr matrix coefficients */
extern int opt_display_horizontal_size, 
	       opt_display_vertical_size; /* display size */




/* picture specific data (currently controlled by global flags) */

extern int opt_dc_prec;
extern int opt_topfirst;


extern int opt_mpeg1;				/* ISO/IEC IS 11172-2 sequence */
extern int opt_fieldpic;			/* use field pictures */
extern int opt_pulldown_32;			/* 3:2 Pulldown of movie material */



/* use only frame prediction and frame DCT (I,P,B) */
extern int opt_frame_pred_dct_tab[3];
extern int opt_qscale_tab[3]; 	/* linear/non-linear quantizaton table */
extern int opt_intravlc_tab[3]; /* intra vlc format (I,P,B) */
extern int opt_altscan_tab[3]; 	/* alternate scan (I,P,B */

extern struct motion_data *opt_motion_data;



/* Orginal intra / non_intra quantization matrices */
extern uint16_t opt_intra_q[64], opt_inter_q[64];


/* **************************
 * Global flags controlling encoding behaviour 
 ************************** */

extern double ctl_decode_frame_rate;	/* Actual stream frame
										   decode-rate. This is lower
										   than playback rate if 3:2
										   pulldown is active.
										*/
extern int ctl_video_buffer_size;    /* Video buffer requirement target */

extern int ctl_N_max;				/* number of frames in Group of Pictures (max) */
extern int ctl_N_min;				/* number of frames in Group of Pictures (min) */
extern int ctl_M;					/* distance between I/P frames */

extern int ctl_M_min;			    /* Minimum distance between I/P frames */

extern bool ctl_refine_from_rec;	/* Is final refinement of motion
								   compensation computed from
								   reconstructed reference frame image
								   (slightly higher quality, bad for
								   multi-threading) or original
								   reference frame (opposite) */

extern int ctl_44_red;			/* Sub-mean population reduction passes for 4x4 and 2x2 */
extern int ctl_22_red;			/* Motion compensation stages						*/
extern int ctl_seq_length_limit;
extern double ctl_nonvid_bit_rate;	/* Bit-rate for non-video to assume for
								   sequence splitting calculations */

extern double ctl_quant_floor;    /* quantisation floor [1..10] (0 for
										 * CBR) */


extern double ctl_act_boost;		/* Quantisation reduction for highly active blocks */


extern int ctl_max_encoding_frames; /* Maximum number of concurrent
									   frames to be concurrently encoded 
									   Used to control multi_threading.
									*/

extern bool ctl_parallel_read; /* Does the input reader / bufferer
								 run as a seperate thread?
							  */


/* *************************
 * input stream attributes
 ************************* */

extern int istrm_nframes;				/* total number of frames to encode
								   Note: this may start enormous and shrink
								   down later if the input stream length is
								   unknown at the start of encoding.
								   */
extern int istrm_fd;

/* ***************************
 * Encoder internal derived values and parameters
 *************************** */


extern int 
    opt_enc_width, 
	opt_enc_height;   /* encoded frame size (pels) multiples of 16 or 32 */

extern int 
    opt_phy_width, 
	opt_phy_height;   /* Physical Frame buffer size (pels) may differ
					   * from encoded size due to alignment
					   * constraints */

extern int lum_buffer_size, chrom_buffer_size;

/* Buffers frame data */
extern uint8_t ***frame_buffers;
extern unsigned int frame_buffer_size;

/* Table driven intra / non-intra quantization matrices */
extern uint16_t i_intra_q[64], i_inter_q[64];
extern uint16_t intra_q_tbl[113][64], inter_q_tbl[113][64];
extern uint16_t i_intra_q_tbl[113][64], i_inter_q_tbl[113][64];
extern float intra_q_tblf[113][64], inter_q_tblf[113][64];
extern float i_intra_q_tblf[113][64], i_inter_q_tblf[113][64];



extern FILE *outfile, *statfile; /* file descriptors */
extern int inputtype; /* format of input frames */

/* sequence specific data (sequence header) */

extern int opt_phy_chrom_width,opt_phy_chrom_height, block_count;
extern int mb_width, mb_height; /* frame size (macroblocks) */
extern int opt_phy_width2, opt_phy_height2, opt_enc_height2,
	mb_height2, opt_phy_chrom_width2; /* picture size */
extern int qsubsample_offset, 
           fsubsample_offset,
	       rowsums_offset,
	       colsums_offset;		/* Offset from picture buffer start of sub-sampled data... */

extern int mb_per_pict;			/* Number of macro-blocks in a picture */						


extern int frame_num;			/* Useful for triggering debug information */


 
/* prediction values for DCT coefficient (0,0) 
   TODO: This is a per-picture value and thus should be part of
   a picture record.
*/
extern int dc_dct_pred[3];

/* clipping (=saturation) table 
   This is really a pseudo-constant.  
   Probably best handled by initialising only once with a re-entrancy
   check...
*/
extern uint8_t *clp_0_255;

/* prototypes of global functions */
/* conform.c */
void range_checks (void);
void profile_and_level_checks (void);

/* fdctref.c */
void init_fdct (void);
void fdct (int16_t *block);

/* idct.c */
void idct (int16_t *block);
void init_idct (void);

/* motionest.c */

void init_motion (void);
void motion_estimation (pict_data_s *picture);
void motion_subsampled_lum( pict_data_s *picture );

/* mpeg2enc.c */
uint8_t *bufalloc( size_t size );
void Init_Data(void); // resets all data

/* predict.c */
void init_predict(void);

void predict (pict_data_s *picture);

/* putbits.c */
void initbits (void);
void putbits (uint32_t val, int n);
void alignbits (void);
int64_t bitcount (void);

/* puthdr.c */
void putseqhdr (void);
void putseqext (void);
void putseqdispext (void);
void putuserdata (const uint8_t *userdata, int len);
void putgophdr (int frame, int closed_gop);
void putpicthdr (pict_data_s *picture);
void putpictcodext (pict_data_s *picture);
void putseqend (void);

/* putmpg.c */
void putintrablk (pict_data_s *picture, int16_t *blk, int cc);
void putnonintrablk (pict_data_s *picture,int16_t *blk);
void putmv (int dmv, int f_code);

/* putpic.c */
void putpict (pict_data_s *picture);

/* putseq.c */
void putseq (void);
void putseq2(const void* o_hWnd, const unsigned o_uWnd); // same like 'putseq', but sends progess-info to caller

/* putvlc.c */
void putDClum (int val);
void putDCchrom (int val);
void putACfirst (int run, int val);
void putAC (int run, int signed_level, int vlcformat);
void putaddrinc (int addrinc);
void putmbtype (int pict_type, int mb_type);
void putmotioncode (int motion_code);
void putdmv (int dmv);
void putcbp (int cbp);

/* quantize.c */

void iquantize( pict_data_s *picture );
void quant_intra (	pict_data_s *picture,
					int16_t *src, int16_t *dst, 
					int mquant, int *nonsat_mquant);
int quant_non_intra( pict_data_s *picture,
						   int16_t *src, int16_t *dst,
							int mquant, int *nonsat_mquant);
void iquant_intra ( int16_t *src, int16_t *dst, int dc_prec, int mquant);
void iquant_non_intra (int16_t *src, int16_t *dst, int mquant);
void init_quantizer(void);
int  next_larger_quant( pict_data_s *picture, int quant );

extern int (*pquant_non_intra)(pict_data_s *picture, int16_t *src, int16_t *dst,
						int mquant, int *nonsat_mquant);

extern int (*pquant_weight_coeff_sum)(int16_t *blk, uint16_t*i_quant_mat );


/* ratectl.c */
void rc_init_seq (int reinit);
void rc_init_GOP ( int np, int nb);
void rc_init_pict (pict_data_s *picture);
void rc_update_pict (pict_data_s *picture);
int rc_start_mb (pict_data_s *picture);
int rc_calc_mquant (pict_data_s *picture,int j);
void vbv_end_of_picture (pict_data_s *picture);
void calc_vbv_delay (pict_data_s *picture);

/* readpic.c */
int readframe (int frame_num, uint8_t *frame[]);
int frame_lum_mean(int frame_num);
void read_stream_params( int *hsize, int *vsize, int *frame_rate_code,
						 int *topfirst, unsigned int *aspect_code );
void Cleanup_lum_mean(void); // added by LK : a second call, just adds data !

/* stats.c */
void calcSNR (pict_data_s *picture);
void stats (void);

/* transfrm.c */
void transform (pict_data_s *picture);

void itransform ( pict_data_s *picture);
void dct_type_estimation (pict_data_s *picture );

void init_transform(void);

/* writepic.c */
void writeframe (int frame_num, uint8_t *frame[]);


#ifdef __cplusplus
}
#endif

