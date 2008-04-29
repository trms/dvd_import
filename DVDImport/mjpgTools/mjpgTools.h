// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// mjpgTools.h:	part of the mjpgTools Library																			 //
//				Header file for the DLL																					 //
//																														 //
//				If you're using an import-lib file, to import this DLL into your application, you only need this file too//
// --------------------------------------------------------------------------------------------------------------------- //
// Copyleft(c)2002-03 by Laurens KOEHOORN <https://sourceforge.net/sendmessage.php?touser=240832>						 //
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
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __MJPGTOOLS_H__
#define __MJPGTOOLS_H__

//#include <comdef.h>
// these definitions are returned by '_GetFileType'
#define FILETYPE_UNKNOWN	0
#define FILETYPE_BMP		1
#define FILETYPE_GIF		2
#define FILETYPE_PPM		3
#define FILETYPE_RLE		4
#define FILETYPE_TGA		5	// TARGA
#define FILETYPE_JPG		6
#define FILETYPE_YUV		7	// Y4M stream
#define FILETYPE_MPG		8	// unspecified MPeG (perhaps a m1v or m2v ?)
#define FILETYPE_MPG1		9	// MPeG-1
#define FILETYPE_MPG2		10	// MPeG-2
#define FILETYPE_STILL1		11	// MPeG-1 STILL
#define FILETYPE_STILL2		12	// MPeG-2 STILL


// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'MakeJPeG' function													  XX //
typedef struct _CJPEG_SWITCHES_ {
	bool				DCT_islow;
	bool				DCT_ifast;
	bool				DCT_float;			// [default] best quality but will take more time on slow systems
	bool				grayscale;			// Force a monochrome JPEG file to be generated.
	int					quality;			// Quality factor (quantization table scaling factor) [0..100]
	bool				targa;				// overrider : Input file is Targa format
	// Advanced options :
	bool				arithmetic;			// arithmetic coding.
	bool				baseline;			// baseline-compatible output (8-bit quantizer values)
	bool				optimize;			// entropy parm optimization.
	bool				progressive;		// simple progressive mode
	int					smooth;				// input smoothing factor
	// standard options
	char				outfile[MAX_PATH];	// output file name (if empty, will be <infile>.jpg)
	char				infile[MAX_PATH];	// input file name << REQUIRED >>
	bool				overwrite;			// if true, suppress any overwrite-message (just overwrite existing file)
	int					m_ReturnCode;		// out : receives return code of the thread
} cjpeg_switches, *lpcjpeg_switches;
	// (m_ReturnCode!=0)	= failure
	// (m_ReturnCode==0)	= successfull
	// (m_ReturnCode==21)	= input/output file not defined in param-struct
	// (m_ReturnCode==22)	= input does not exist
	// (m_ReturnCode==23)	= output-path (disk+dir) does not exist
	// (m_ReturnCode==24)	= output exist and may not be overwritten
	// (m_ReturnCode==25)	= output protection-error : ReadOnly or Access Denied

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'JPG_2_...' functions													  XX //
typedef struct _DJPEG_SWITCHES_ {
	bool				DCT_islow;
	bool				DCT_ifast;
	bool				DCT_float;			// [default] best quality but will take more time on slow systems
	bool				grayscale;			// monochrome output
	// Advanced options :
	unsigned __int8		dither_level;		// 1 : default (Floyd-Steinberg dithering) | 2 : simple ordered dither | 3 : no dithering
	bool				nosmooth;			// suppress fancy upsampling (false = do 'fancy' upsampling)
	bool				onepass;			// fast one-pass quantization
	unsigned int		quantize;			// color quantization -- no more than <quantize> colours : 16 or 256
	// standard options
	char				outfile[MAX_PATH];	// output file name (if empty, will be <infile>.<ext> ext=format_dependent)
	char				infile[MAX_PATH];	// input file name << REQUIRED >>
	bool				overwrite;			// if true, suppress any overwrite-message (just overwrite existing file)
	int					m_ReturnCode;		// out : receives return code of the thread
} djpeg_switches, *lpdjpeg_switches;
	// (m_ReturnCode!=0)	= failure
	// (m_ReturnCode==0)	= successfull
	// (m_ReturnCode==21)	= input/output file not defined in param-struct
	// (m_ReturnCode==22)	= input does not exist
	// (m_ReturnCode==23)	= output-path (disk+dir) does not exist
	// (m_ReturnCode==24)	= output exist and may not be overwritten
	// (m_ReturnCode==25)	= output protection-error : ReadOnly or Access Denied

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX settings for the MJPGtools (ppm2yuv, yuv2ppm)																	  XX //
#ifndef HAVE__Y4M_RATIO_T__STRUCT

typedef struct _y4m_ratio {
  int n;  /* numerator   */
  int d;  /* denominator */
} y4m_ratio_t;


#define HAVE__Y4M_RATIO_T__STRUCT		1

/************************************************************************************************************************
 *  useful standard framerates (as ratios)																				*
 ************************************************************************************************************************/
const y4m_ratio_t y4m_fps_UNKNOWN			= {     0,     0 }; // illegal
const y4m_ratio_t y4m_fps_NTSC_FILM			= { 24000,  1001 }; // 24000.0/1001.0 (NTSC 3:2 pulldown converted FILM)
const y4m_ratio_t y4m_fps_FILM				= {    24,     1 }; // 24.0 (NATIVE FILM)
const y4m_ratio_t y4m_fps_PAL				= {    25,     1 }; // 25.0 (PAL/SECAM VIDEO / converted FILM)
const y4m_ratio_t y4m_fps_NTSC				= { 30000,  1001 }; // 30000.0/1001.0 (NTSC VIDEO)
const y4m_ratio_t y4m_fps_30				= {    30,     1 }; // 30.0
const y4m_ratio_t y4m_fps_PAL_FIELD			= {    50,     1 }; // 50.0 (PAL FIELD RATE)
const y4m_ratio_t y4m_fps_NTSC_FIELD		= { 60000,  1001 }; // 60000.0/1001.0 (NTSC FIELD RATE)
const y4m_ratio_t y4m_fps_60				= {    60,     1 }; // 60.0

/************************************************************************************************************************
 *  useful standard sample (pixel) aspect ratios (W:H)																	*
 ************************************************************************************************************************/
const y4m_ratio_t y4m_sar_UNKNOWN			= {     0,     0 }; // illegal
const y4m_ratio_t y4m_sar_SQUARE			= {     1,     1 }; // square pixels
const y4m_ratio_t y4m_sar_SQR_ANA_16_9		= {     4,     3 }; // anamorphic 16:9 sampled from 4:3 with square pixels
const y4m_ratio_t y4m_sar_NTSC_CCIR601		= {    10,    11 }; // 525-line (NTSC) Rec.601
const y4m_ratio_t y4m_sar_NTSC_16_9			= {    40,    33 }; // 16:9 NTSC/Rec.601
const y4m_ratio_t y4m_sar_NTSC_SVCD_4_3		= {    15,    11 }; // NTSC SVCD 4:3
const y4m_ratio_t y4m_sar_NTSC_SVCD_16_9	= {    20,    11 }; // NTSC SVCD 16:9
const y4m_ratio_t y4m_sar_PAL_CCIR601		= {    59,    54 }; // 625-line (PAL) Rec.601
const y4m_ratio_t y4m_sar_PAL_16_9			= {   118,    81 }; // 16:9 PAL/Rec.601
const y4m_ratio_t y4m_sar_PAL_SVCD_4_3		= {    59,    36 }; // PAL SVCD 4:3
const y4m_ratio_t y4m_sar_PAL_SVCD_16_9		= {    59,    27 }; // PAL SVCD 16:9

#endif // HAVE__Y4M_RATIO_T__STRUCT

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'ppm2yuv' function													  XX //
typedef struct _PPM2YUV_STRUCT_ {
	short			verbose;			/*v*/	// 0=app_default (3:DEBUG, 0:RELEASE)
													// 1 = silent
													// 2 = informational
													// 3 = debug
	y4m_ratio_t		aspect_ratio;		/*A*/	// pixel aspect ratio [default : y4m_sar_SQUARE]
	unsigned int	chroma;				/*S*/	// chroma subsampling mode :
													// 0 = 420_jpeg (default)
													// 1 = unknown
													// 2 = 420_mpeg2
	y4m_ratio_t		frame_rate;			/*F*/	// framerate, one of the definitions as declared above [default : y4m_fps_PAL]
	char			interlacing;		/*I*/	// interlacing [default 'p']
													// p = none/progressive
													// t = top-field-first
													// b = bottom-field-first
	// Advanced options :
	unsigned int	frame_count;		/*n*/	// output n frames; 0 == all of them, [default '0'] (count per file)
	bool			interleave_2Pass;	/*L*/	// treat PPM image(s) as 2-field interleaved
	bool			repeat_last;		/*r*/	// repeat last input frame (don't do it if 'false')
	unsigned int	skip_frame_offset;	/*o*/	// skip n input frames, [default '0']
	// standard options
	char			infile[MAX_PATH];			// input PPM file << REQUIRED >>
	char			outfile[MAX_PATH];			// output YUV file (if empty, will be <infile>.yuv)
	bool			overwrite;					// true to suppress any overwrite message when the 'output' already exists
	int				m_ReturnCode;				// return value : in=9999 - out=return-code
	// added since 1.5.0.0
	// multiple files in here
	LPCTSTR			infile2;					// array of LPCTSTR ("str1\0str2\0str3\0strlast\0\0")
	unsigned		min_frame_count;			// min count of all frames of all files.
												// if total_files*framce_count < min_frame_count,
												// last file will be padded with extra frames
	unsigned		version;					// if '1', indicates rel. 1.5.0.0
	// following only valid if version>=1
	unsigned*		frames;						// if (!NULL) will receive total frames in the created YUV-file
	HWND			o_hWnd;						// if progress-info should be transmitted to caller, this must be valid
	UINT			o_uMsg;						// if progress-info should be transmitted to caller, this must be valid
} ppm2yuv_switches, *lp_ppm2yuv_switches;
	// (m_ReturnCode!=0)	= failure
	// (m_ReturnCode==0)	= successfull
	// (m_ReturnCode==21)	= input/output file not defined in param-struct
	// (m_ReturnCode==22)	= input does not exist
	// (m_ReturnCode==23)	= output-path (disk+dir) does not exist
	// (m_ReturnCode==24)	= output exist and may not be overwritten
	// (m_ReturnCode==25)	= output protection-error : ReadOnly or Access Denied


// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'yuv2ppm' function													  XX //
typedef struct _YUV2PPM_STRUCT_ {
	short			verbose;		/*v*/	// 0=app_default (3:DEBUG, 0:RELEASE)
												// 1 = silent
												// 2 = informational
												// 3 = debug
	unsigned int	chroma;			/*S*/	// chroma subsampling mode :
												// 0 = 420_jpeg [default]
												// 1 = unknown
												// 2 = 420_mpeg2
	// Advanced options :
	bool			interleave;		/*L*/	// if (true) --> interleave fields into single PPM image [default : false(off)]
	// standard options
	char			infile[MAX_PATH];		// input YUV file << REQUIRED >>
	char			outfile[MAX_PATH];		// output PPM file (if empty, will be <infile>.ppm)
	bool			overwrite;				// true to suppress any overwrite message when the 'output' already exists
	int				m_ReturnCode;			// return value : in=9999 - out=return-code
} yuv2ppm_switches, *lp_yuv2ppm_switches;
	// (m_ReturnCode!=0)	= failure
	// (m_ReturnCode==0)	= successfull
	// (m_ReturnCode==21)	= input/output file not defined in param-struct
	// (m_ReturnCode==22)	= input does not exist
	// (m_ReturnCode==23)	= output-path (disk+dir) does not exist
	// (m_ReturnCode==24)	= output exist and may not be overwritten
	// (m_ReturnCode==25)	= output protection-error : ReadOnly or Access Denied

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'jpeg2yuv' function													  XX //
typedef struct _JPEG2YUV_STRUCT_ {
	y4m_ratio_t			frame_rate;		/*f*/	// framerate for output stream (fps) [default : 'y4m_fps_PAL']
	const char*			filter;			/*j*/	// filter to use (use 'printf'-filter), eg : myjpg%03ld.jpg (don't forget the extension)
													// where '%06ld' will be translated to '000'..'999', if start=0 and end=1000
													// if no extension is given, the default extension will be added : ".jpg"
													// enter the full path to where the files can be located,
														// eg.: C:\\temp\\myjpg%03d.jpg
	unsigned int		interlace;		/*I*/	// interlacing mode, where
													// 0 = none/progressive [default]
													// 1 = top-field-first
													// 2 = bottom-field-first
	// Advanced options :
	bool				interleaving;			// interleaving mode, where
													// false = non-interleaved (two successive fields per JPEG file)
													//  true = interleaved fields
	unsigned __int32	start, end;				// the filter will start counting from 'start' and stop at 'end'
													// eg : start=1000, end=2000 :
														// the function will retrieve files from 1000 until (including) 1999
													// set start/end to '0' if you don't want to use the variable(s)
	// standard options
	char				outfile[MAX_PATH];		// output YUV file (required)
	bool				overwrite;				// true to suppress any overwrite message when the 'output' already exists
	int					m_ReturnCode;			// return value : in=9999 - out=return-code
} jpeg2yuv_switches, *lp_jpeg2yuv_switches;
	// (m_ReturnCode!=0)	= failure
	// (m_ReturnCode==0)	= successfull
	// (m_ReturnCode==21)	= input/output file not defined in param-struct
	// (m_ReturnCode==22)	= input does not exist
	// (m_ReturnCode==23)	= output-path (disk+dir) does not exist
	// (m_ReturnCode==24)	= output exist and may not be overwritten
	// (m_ReturnCode==25)	= output protection-error : ReadOnly or Access Denied


// ********************************************************************************************************************* //
// ** Following are used as flags in the _YUVSCALER_STRUCT_ structure, as mode										  ** //
	// For coherence reason, yuvscaler will use RESAMPLE if only downscaling is necessary, BICUBIC if upscaling is necessary
#define mode_BICUBIC			0x00000001	// to use the (Mitchell-Netravalli) high-quality bicubic upsacling and/or downscaling algorithm
#define mode_RESAMPLE			0x00000002	// to use a classical resampling algorithm -only for downscaling- that goes much faster than bicubic
#define mode_WIDE2STD			0x00000004	// to converts widescreen (16:9) input frames into standard output (4:3), generating necessary black lines
#define mode_WIDE2VCD			0x00000008	// to transcode wide (16:9) frames  to VCD (equivalent to -M WIDE2STD -O VCD)
#define mode_FASTVCD			0x00000010	// to transcode full sized frames to VCD (equivalent to -M RATIO_2_1_2_1 -O VCD, see after)
#define mode_FAST_WIDE2VCD		0x00000020	// to transcode full sized wide (16:9) frames to VCD (equivalent to -M WIDE2STD -M RATIO_2_1_2_1 -O VCD, see after)
#define mode_NO_HEADER			0x00000040	// to suppress stream header generation on output (chaining yuvscaler conversions)
#define mode_MMX				0x00000080	// to use MMX functions for BICUBIC scaling (experimental feature!!)
												// this will only be available, if compiled with 'HAVE_ASM_MMX' defined
#define mode_RATIO				0x00001000	// to specified conversion ratios of :
						// WidthIn/WidthOut for width and HeightIN/HeightOut for height to be applied to the useful area.
						// The output active area that results from scaling the input useful area might be different
						// from the display area specified thereafter using the -O KEYWORD syntax.
						// In that case, yuvscaler will automatically generate necessary black lines and columns and/or
						// skip necessary lines and columns to get an active output centered within the display size.
						// ** IMPORTANT **
						// when this mode is defined, the variable '_RATIO_STRUCT_' must have values, or this
						// mode will be ignored !!
#define mode_USE_INPUT			0x00002000	// to select a useful area of the input frame
						// WidthxHeight+WidthOffset+HeightOffset (all multiple of 2, Height and HeightOffset multiple of 4
						// if interlaced), the rest of the image being discarded
						// ** IMPORTANT **
						// when this mode is defined, the variable '_USE_STRUCT_' must have values, or this
						// mode will be ignored !!
#define mode_ACTIVE_INPUT		0x00004000	// to select an active area of the input frame
						// WidthxHeight+WidthOffset+HeightOffset (all multiple of 2, Height and HeightOffset multiple of 4
						// if interlaced), the rest of the image being made black
						// ** IMPORTANT **
						// when this mode is defined, the variable '_ACTIVE_STRUCT_' must have values, or this
						// mode will be ignored !!
	// If you suspect that your video capture was given a wrong interlacing type, please use and combine:
#define mode_INTERLACED_TOP_FIRST		0x00010000	// to specify top_field_first as input interlacing
#define mode_INTERLACED_BOTTOM_FIRST	0x00020000	// to specify bottom_field_first as input interlacing
#define mode_INTERLACED_PROGRESSIVE		0x00040000	// to specify not-interlaced/progressive as input interlacing
#define mode_LINE_SWITCH				0x00100000	// to preprocess frames by switching lines two by two
#define mode_BOTT_FORWARD				0x00200000	// to preprocess frames by moving bottom field one frame forward
#define mode_TOP_FORWARD				0x00400000	// to preprocess frames by moving top  field one frame forward
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'YUVscaler' function													  XX //
typedef struct _YUVSCALER_STRUCT_ {
	unsigned short		verbosity;		/*v*/	// verbose mode : DEBUG (default=2), RELEASE (default=0)
													// 0 : take program default : 2 at _DEBUG, 0 at RELEASE
													// 1 : no verbose
													// 2 : informational
													// 3 : debug
	unsigned short		Output;			/*O*/	// OUTPUT frame characteristics, one of these : << REQUIRED >>
													// 0 : <take height/width of input> [default]
													// 1 : VCD		[352x288 if (outputNORM==0|1)] or [352x240 if (outputNORM==2)]
													// 2 : SVCD		[576x480 if (outputNORM==0|1)] or [480x480 if (outputNORM==2)]
													// 3 : resize	(resize to 'resize[0]'x'resize[1]')
															// ::the values in the 'resize[]' array must be multiples of '2'
	unsigned short		outputNORM;		/*n*/	// if norm could not be determined from data flux, specifies the
												// OUTPUT norm for VCD/SVCD - (usually not necessary) :
													// 0 = PAL /*p*/ [default]
													// 1 = SECAM /*s*/ [default] -- PAL and SECAM are the same
													// 2 = NTSC /*n*/
	unsigned __int32	resize[2];				// resize[0]=height, resize[1]=width, see 'Output'-option 3
	unsigned short		interlace;				// 'force' output to be interlaced, using :
													// 0 : same as input [default]
													// 1 : no interlacing/progressive
													// 2 : top-field-first
													// 3 : bottom-field-first
	bool				grayscale;				// grayscales the output
													// false : don't grayscale [default]
													//  true : DO grayscale
	// Advanced options :
	unsigned __int32	flags;			/*M*/	// see the 'mode'-definitions above

	struct _RATIO_STRUCT_ {
		unsigned __int32	h_in, h_out;	// height IN/OUT ratio (like '2' and '1' for ratio 2:1)
		unsigned __int32	w_in, w_out;	// width IN/OUT ratio
	}					ratio_data;				// << REQUIRED >> when flag mode_RATIO was specified, ignored otherwise

	struct _USE_STRUCT_ {
		unsigned __int32	use_height, use_width;			// useful_height, useful_width
		unsigned __int32	discard_above, discard_left;	// discard_line_above, discard_col_left
	}					use_data;				// << REQUIRED >> when flag mode_USE_INPUT was specified, ignored otherwise

	struct _ACTIVE_STRUCT_ {
		unsigned __int32	act_height, act_width;				// height and width of active window from the input
		unsigned __int32	black_col_above, black_col_left;	// offsets of black columns at top and left side
	}					active_data;			// << REQUIRED >> when flag mode_ACTIVE_INPUT was specified, ignored otherwise

	// standard options :
	char				infile[MAX_PATH];		// input YUV file << REQUIRED >>
	char				outfile[MAX_PATH];		// output YUV file << REQUIRED >>
	bool				overwrite;				// true to suppress any overwrite message when the 'output' already exists
	int					m_ReturnCode;			// return value : in=9999 - out=return-code

	unsigned short		version;				// starting from 1.5.0.0, folowing will be handled as well (is '1' for 1.5.0.0)
	HWND				m_hOwner;				// HWND of owner, if progress-info is requested
	UINT				m_uOwner;				// MSG to send to owner, if progress-info is requested
	unsigned			m_TotalFrames;			// amount of frames to send as 'maxRange' to owner (retrieved from ppm2yuv)
} yuvscaler_struct, *lp_yuvscaler_struct;

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'mpg2enc' function													  XX //
typedef struct _MPG2ENC_STRUCT_ {
	short	verbose;		/*v*/	// Level of verbosity :: 0 : system-default (release='1', debug='3'),
									// 1 : silent (only errors and warnings are displayed), 2 : informational, 3 : debug
	short	format;			/*f*/	// Set pre-defined mux format fmt ::
									// 0 = Generic MPEG1, 1 = standard VCD, 2 = VCD,
									// 3 = Generic MPEG2, 4 = standard SVCD, 5 = user SVCD
									// 6 = VCD Stills sequences, 7 = SVCD Stills sequences, 8 = DVD
	short	aspect_ratio;	/*a*/	// Set displayed image aspect ratio image (default: 2 = 4:3)
									// 1 = 1:1,  2 = 4:3,  3 = 16:9,  4 = 2.21:1
	int		frame_rate;		/*F*/	// Set playback frame rate of encoded video
									// [0 = frame rate of input stream (default)], -1 to display a frame rate code table in a popup-window
	unsigned int v_bitrate;	/*b*/	// Set Bitrate of compress video in KBit/sec
									// (default: 1152 for VCD, 2500 for SVCD, 3800 for DVD)
	// Advanced options :
	unsigned int nv_bitrate;/*B*/	// Non-video data bitrate to assume for sequence splitting calculations
									// see also 'seq_length'
	short	quantisation;	/*q*/	// Image data quantisation factor [1..31] (1 is best quality, no default)

	unsigned int vcd_ssize;	/*T*/	// Size in KB of VCD stills [requires 20..500]
	short	interlace;		/*I*/	// Sets MPEG 2 motino estimation and encoding modes:
									//  1    = Progressive (non-interlaced)(Movies) /*0*/
									//  2..3 = Interlaced source material (video) /*1..2*/
	short	motionsearch;	/*r*/	// Motion compensation search radius [1..33]/*[0..32]*/ (default 16)
	short	reduction4;		/*4*/	// Reduction factor for 4x4 subsampled candidate motion estimates
									//  [1..4] [1 = max quality, 4 = max. speed] (default: 1) /*2*/
	short	reduction2;		/*2*/	// Reduction factor for 2x2 subsampled candidate motion estimates
									//  [1..4] [1 = max quality, 4 = max. speed] (default: 1) /*3*/

	short	min_gop;		/*g*/	// Minimum size Group-of-Pictures (default 12)
	short	max_gop;		/*G*/	// Maximum size Group-of-Pictures (default 12)
									//  If min-gop is less than  max-gop, mpeg2enc attempts to place GOP boundaries to coincide with scene changes
	bool	force_bbp;		/*P*/	// Preserve two B frames between I/P frames when placing GOP boundaries
	short	bgrp_size;				// size of B group, default = 3 (IBBPBBPBB...), if=1 (IPPPP...)

	float	quant_reduc;	/*Q*/	// Max. quantisation reduction for highly active blocks
									//  [0.0 .. 5.0] (default: 0.0)
	short	vbv;			/*V*/	// Target decoders video buffer size in KB (default 46) [20..4000]
	char	norm;			/*n*/	// Tag output to suit playback in specified video norm
									//  ('n' = NTSC, 'p' = PAL, 's' = SECAM) (default: PAL)
	unsigned int seq_length;/*S*/	// Place a sequence boundary in the video stream so they occur every
									// num Mbytes once the video is multiplexed with audio etc.
									// N.b. --non-video-bitrate is used to the bitrate of the other
									// data that will be multiplexed with this video stream
	bool	pulldown3_2;	/*p*/	// Generate header flags for 3-2 pull down of 24fps movie material
	short	intra_dc_prec;	/*D*/	// Set number of bits precision for DC (base colour) of blocks in MPEG-2
	bool	reduce_hf;		/*N*/	// Reduce high frequency resolution - useful as a mild noise reduction
	bool	keep_hf;		/*h*/	// Maximise high-frequency resolution - useful for high quality sources and/or high bit-rates)
	bool	seqhead_egop;	/*s*/	// Include a sequence header every GOP if the selected format doesn't do so by default.
	bool	no_dummy;		/*d*/	// Don't generate of dummy SVCD scan-data for the ISO CD image generator "vcdimager" to fill in.
	short	play_fieldorder;/*z*/	// Force setting of playback field order to bottom or top first
									// (1 : top-first, 2 : bottom-first) (no default : 0)
	short	multi_thread;	/*M*/	// Activate multi-threading to optimise through on a system with num CPU's
									// [0..32], 0=no multithreading, (default: 1)
										// Currently, this option is disabled : NO MULTITHREADING=0
	bool	correct_svcd_hds;/*C*/	// Force SVCD horizontal_display_size to be 480
									//  standards say 540 or 720. But many DVD/SVCD players screw up with these values.
	bool	no_altscan_mpeg2;		// Force MPEG2 *not* to use alternate block scanning.
									// This may allow some buggy players to play SVCD streams
	// standard options :
	char	infile[MAX_PATH];/*o*/	// input YUV file << REQUIRED >>
	char	outfile[MAX_PATH];		// output YUV file << REQUIRED >>
	bool	overwrite;				// true to suppress any overwrite message when the 'output' already exists
	int		m_ReturnCode;			// return value : in=9999 - out=return-code

	unsigned short	version;		// since 1.5.0.0, set to '1' or higher (depending of DLL-version), if >='1' following will be handled as well
	HWND			m_hWnd;			// HWND of caller-window, to which progress-info is send
	unsigned		m_uMsg;			// MSG id used to send the progress-info
	unsigned		m_framecount;	// initial frame-count used to initialize the 'maxRange' of the progressbar-ctrl of the caller
} mpg2enc_struct, *lp_mpg2enc_struct;


// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'mplex' function														  XX //
typedef struct _MPLEX_STRUCT_ {
	short	verbose;		/*v*/	// 0=system default (1 in 'Release', 3 in 'Debug'),
										// 1 = quiet - 2 = info only - 3 = debug
	short	format;			/*f*/	// Set defaults for particular MPEG profiles
										// 0 = Generic MPEG1, 1 = VCD, 2 = user-rate VCD, 3 = Generic MPEG2,
										// 4 = SVCD, 5 = user-rate SVCD, 6 = VCD Stills, 7 = SVCD Stills, 8 = DVD
	// Advanced options :
	int		mux_rate;		/*r*/	// Specify data rate of output stream in kbit/sec
										// default 0=Compute from source streams
	int		video_buffer;	/*b*/	// Specifies decoder buffers size in kB.  [ 20...2000]
	int		mux_limit;		/*l*/	// Multiplex only num seconds of material (default 0=multiplex all)
	int		sync_offset;	/*O*/	// Specify offset of timestamps (video-audio) in mSec
	int		sector_size;	/*s*/	// Specify sector size in bytes for generic formats [256..16384]
	bool	vbr;			/*V*/	// Multiplex variable bit-rate video
	short	packets_per_pack;/*p*/	// Number of packets per pack generic formats [1..100]
	bool	sys_headers;	/*h*/	// Create System header in every pack in generic formats
	int		max_seg_size;	/*S*/	// Maximum size of output file(s) in Mbyte (default: 2000) (0 = no limit)
	bool	split_segment;	/*M*/	// Simply split a sequence across files rather than building run-out/run-in
	// standard options :
	void*	infiles;		/*o*/	// input files << REQUIRED >> array of LPCTSTR[], last entry should be NULL
	char	outfile[MAX_PATH];		// output MPG file << REQUIRED >>
	bool	overwrite;				// true to suppress any overwrite message when the 'output' already exists
	int		m_ReturnCode;			// return value : in=9999 - out=return-code
} mplex_struct, *lp_mplex_struct;

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX switches-struct to use when calling the 'patchSTILL' function													  XX //
typedef struct _PATCHSTILL_STRUCT_ {
	bool	verbose;				// false to show only error messages, true to show ALL message
	bool	reverse;				// false to patch to a STILL, true to undo the patching
	// standard options :
	char	infile[MAX_PATH];		// input MPG file << REQUIRED >>
	char	outfile[MAX_PATH];		// output MPG file
									//  if left empty, will assume to overwrite the input
	bool	overwrite;				// true to suppress any overwrite message when the 'output' already exists,
									//  or is the same as the input
	int		m_ReturnCode;			// return value : in=9999 - out=return-code
} stillpatch_struct, *lp_stillpatch_struct;


// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/ XX //
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
#ifdef MJPGTOOLS_INTERNALS

// added since 1.6.0.0
// XXXXXXXXXXXXXXXXXXX
// this struct is used internally : it is used to call the threads using an Event for notification
typedef struct _caller2thread_s {
	void*	arg;	// parameters - could be anything
	HANDLE	event;	// event that will be set as soon the calling-thread has finished
} caller2thread_t, *caller2thread_p;

// ====<start of introduction>=============================================================================================
//
// these functions should only be available INSIDE the dll, not outside!
// to import these functions use the "LoadLibrary" and "GetProcAddress" functions,
//
//   Example : to get a pointer to (_GetFileType), use following syntax
//
//	typedef unsigned int	(*lp_GetFileType)(LPCTSTR);
//	HINSTANCE hinstLib=NULL;
//	lp_GetFileType	lpGetFileType;
//	int fType=FILETYPE_UNKNOWN;
//	const char* inFile = "D:\\Temp\\Vinca.jpg";
//	...
//	if ((hinstLib=LoadLibrary("mjpgTools")) != NULL)
//	{
//		if ((lpGetFileType = (lp_GetFileType)GetProcAddress(hinstLib, "_GetFileType")) != NULL)
//		{
//			fType = (lpGetFileType)(inFile);
//		}
//		FreeLibrary(hinstLib);
//	}
//
// Remember that all functions were declared using the __cdecl* prototype,
// I don't know whether this can give problems when importing these functions in non-C/C++ environments (check your manual)
//
// ====<end of introduction>===============================================================================================
//
// try's to detect which type the file could be : this is not reliable while a file may be corrupted
// this function only tests the 1st byte of the file, in case of targa it also may test the extension
// in case of JPeG, it verifies the 1st 2 bytes, the function also tests whether the file is a MPeG (1/2) and/or a STILL
unsigned int WINAPI _GetFileType(LPCTSTR inputFile);

// MakeJPeG try's to convert the input-file (as entered in the param-switches-struct) to JPeG
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI MakeJPeG( lpcjpeg_switches );

// Following functions try's to convert the entered JPeG-file into the desired graphic file-image
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI JPG_2_BMP( lpdjpeg_switches, unsigned int winBMP=1 ); // set winBMP to '0' if you want to have an OS/2-like BMP
void WINAPI JPG_2_GIF( lpdjpeg_switches );
void WINAPI JPG_2_PPM( lpdjpeg_switches );
void WINAPI JPG_2_TGA( lpdjpeg_switches ); // convert into TARGA format

// Following are some handy functions
BOOL WINAPI FileExists(LPCTSTR pPath);		// TRUE if specified file exists
BOOL WINAPI PathExists(LPCTSTR pPath);		// TRUE if PATH-part (disk+dir) of the path does exist
BOOL WINAPI GetCorrectPath(LPTSTR pPath);	// TRUE on found, changes the path-seperators to specification of OS ('\\' or '/')
BOOL WINAPI IsWindowsNT(void);				// TRUE if it is
BOOL WINAPI IsWindowsNT5(void);			// TRUE if running on Win2000, winXP or even higher
BOOL WINAPI IsWindows98h(void);			// TRUE if running on Win98+ (w98, wME, ...)

// Convert a PPM/PBMPLUS to YUV (YUV is the 'avi' format of the MJPEGTOOLS)
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI PPM_2_YUV( lp_ppm2yuv_switches arg );

// Convert a YUV to PPM/PBMPLUS (YUV is the 'avi' format of the MJPEGTOOLS)
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI YUV_2_PPM( lp_yuv2ppm_switches arg );

// Convert 1.. to many JPeG's to a single YUV
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI JPG_2_YUV( lp_jpeg2yuv_switches arg );

// Rescales a YUV stream
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI YuvScaler( lp_yuvscaler_struct arg );

// MPeG-encode (accepts for input YUV-formats only, avi's needs to be converted to YUV)
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI mpg2enc( lp_mpg2enc_struct arg );

// MultiPlexer of MPG, M1V, M2V, MP2 and AC3 into MP(e)G
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI mplex( lp_mplex_struct arg );

// Patches a MPeG that assumes to be a STILL to be a compliant STILL for VCDimager (and VCDwizard)
// returncode will be entered in the struct = 0 on success, (! 0) otherwise
void WINAPI stillPatcher( lp_stillpatch_struct arg );

// retrieve particular information of a graphical image (currently only JPG, BMP, GIF, PPM and TGA are supported)
int WINAPI GetGraphicalInformation( LPCTSTR lpSource, long* imgHeight=NULL, long* imgWidth=NULL );

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //
// XX Tools Section																									  XX //
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX //

// Attempts to create a STILL (MPeG-1/-2 from a BMP/GIF/TGA/PPM/JPG
// ----------------------------------------------------------------
// inStream		= full path of input file
// outStream	= on successfull completion, will receive the full path of the created STILL (must be of at least the same size as inStream)
// still1		= TRUE to create a 'MPeG-1 STILL', FALSE to create a 'MPeG-2 STILL'
// pal			= TRUE (default) to creat a PAL/SECAM STILL, FALSE to create a NTSC STILL
// returns true when successfull
bool WINAPI create_still(const char* inStream, char* outStream, BOOL still1, BOOL pal=TRUE, short csVerbose=0);

//	Attempts to create a MPeG of a given filelist of images (MPeG-1/-2 from a BMP/GIF/TGA/PPM/JPG)
// -----------------------------------------------------------------------------------------------
// inStream			= FileList, like "file1\0file2\0...\0lastfile\0\0"
// outStream		= buffer to receive path of returning MPeG, this may already contain a full path of a MPeG
//					  in which case the output will be directed to
// vcd				= create a MPeG-1 if TRUE, create a SVCD if FALSE
// pal				= create PAL if TRUE, NTSC if FALSE
// len				= length (in seconds) of each stream of each images
// forceCompliance	= force to use a compliant bitrate (for vcd that is 1150, for SVCD 2520),
//					  instead of the optimized bitrates we use (for vcd that is 1150+384, for SVCD 2520+384)
// csVerbose		= 0: Silent, 1: Info, 2: Debug
// o_hWnd			= (optional) : handle of window to receive Progress-messages
// o_uMsg			= (optional) : msg-id to use when sending Progress-messages
// returns TRUE when successfull
BOOL WINAPI createMxV(LPCTSTR inStream, LPTSTR outStream, BOOL vcd, BOOL pal=TRUE, float len=0,
					  BOOL forceCompliance=FALSE, short csVerbose=0, HWND o_hWnd=0, UINT o_uMsg=0);

#else

// -- This struct (and enum) is send to the caller in case of a Progress-notification (only when using the special functions)
typedef enum {
	invalid		= 0,
	init,	// start
	reinit,	// re-initializes
	finish,	// end
	step
} lhk_progress_type_e;

#pragma pack(push, mjpgTools, 1)
// added since 1.5.0.0 -- progress-stuff
typedef struct _progress_struct_s { // a progressbar is intended not to use percents (NOT 0%..100%, but 0..initCount)
	unsigned short			version;	// indicates version of struct, curversion = '1' (rel. 1.5.0.0)
	lhk_progress_type_e		type;
	// valid in case of type::init
	const char*				initText;	// what will be processed
	long					initCount;	// 'maxRange' for a progressbar
	// valid for type::step
	const char*				stepText;	// what is currently done (should be a 'short' message)
	long					stepPos;	// new pos for a progressbar
} lhkProgress_t, *lhkProgress_p;
#pragma pack(pop, mjpgTools)
// --

typedef unsigned int	(WINAPI *lp_GetFileType)(LPCTSTR);
typedef void			(WINAPI *lp_MakeJPeG)(lpcjpeg_switches);
typedef void			(WINAPI *lp_JPG_2_BMP)(lpdjpeg_switches, unsigned int winBMP);
typedef void			(WINAPI *lp_JPG_2_GIF)(lpdjpeg_switches);
typedef void			(WINAPI *lp_JPG_2_PPM)(lpdjpeg_switches);
typedef void			(WINAPI *lp_JPG_2_TGA)(lpdjpeg_switches);
typedef BOOL			(WINAPI *lp_FileExists)(LPCTSTR);
typedef BOOL			(WINAPI *lp_PathExists)(LPCTSTR);
typedef BOOL			(WINAPI *lp_GetCorrectPath)(LPTSTR);
typedef BOOL			(WINAPI *lp_IsWindowsNT)(void);
typedef BOOL			(WINAPI *lp_IsWindowsNT5)(void);
typedef BOOL			(WINAPI *lp_IsWindows98h)(void);
typedef void			(WINAPI *lp_PPM_2_YUV)(lp_ppm2yuv_switches);
typedef void			(WINAPI *lp_YUV_2_PPM)(lp_yuv2ppm_switches);
typedef void			(WINAPI *lp_JPG_2_YUV)(lp_jpeg2yuv_switches);
typedef void			(WINAPI *lp_YuvScaler)(lp_yuvscaler_struct);
typedef void			(WINAPI *lp_mpg2enc)(lp_mpg2enc_struct);
typedef void			(WINAPI *lp_mplex)(lp_mplex_struct);
typedef void			(WINAPI *lp_stillPatcher)(lp_stillpatch_struct);
typedef int				(WINAPI *lp_GetGraphicalInformation)(LPCTSTR, LPLONG, LPLONG);
typedef bool			(WINAPI *lp_create_still)(LPCTSTR, LPTSTR, BOOL, BOOL pal, short csVerbose);
typedef BOOL			(WINAPI *lp_createMxV)(LPCTSTR, LPTSTR, BOOL, BOOL pal, float len, BOOL forceCompliance, short csVerbose, HWND o_hWnd, UINT o_uMsg);

#endif // ifdef MJPGTOOLS_INTERNALS
#endif // __MJPGTOOLS_H__