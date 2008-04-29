// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// yuvscaler.hpp:	interface for the CYuvScaler class, part of the mjpgTools Library																		 //
//					Scales arbitrary sized yuv frame to yuv frames suitable for VCD, SVCD or specified					 //
//																														 //
//					VC60Pro C++ Header File, strongly modified version, based on yuvscaler.h of 'Xavier Biquard'		 //
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
// This sub-project is a corrected version of :																			 //
//	MJPEGtools v1.6.0 (of the linux group)																				 //
//																														 //
//	Copyright (C) 2001 Xavier Biquard <xbiquard@free.fr>																 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#ifndef __YUVSCALER_H__
#define __YUVSCALER_H__

#define YUVSCALER_VERSION					LAVPLAY_VERSION
// For pointer adress alignement
#define ALIGNEMENT							16		// 16 bytes alignement for mmx registers in SIMD instructions for Pentium
#define MAX_EDIT_LIST_FILES					256
#define MAX_JPEG_LEN						(512*1024)
// Special BICUBIC algorithm
// 2048=2^11
#define FLOAT2INTEGER						2048
#define FLOAT2INTEGERPOWER					11
#define PARAM_LINE_MAX						256

#define FLOAT2INTEGERPOWER					11

class CYuvScaler
{
public:
	CYuvScaler();								// constructor
	~CYuvScaler();								// destructor

	// routines
	
	// *************************************************************************************
	// PREPROCESSING
	// *************************************************************************************

	// This function reads a frame from input stream. Same as y4m_read_frame function except line switching is implemented
	INT32		my_y4m_read_frame(INT32, y4m_frame_info_t*, UINT32, UINT8* /*, bool*/);
	
	// This function does line_switching on matrix input which contains a full frame 
	INT32		line_switch (UINT8* input, UINT8* line);

	// This function stores the current bottom field into tabular field[1 or 2] 
	INT32		bottom_field_storage(UINT8*, INT32, UINT8*, UINT8*);

	// This function stores the current top field into tabular field[1 or 2] 
	INT32		top_field_storage(UINT8*, INT32, UINT8*, UINT8*);

	// This function stores/replaces the current bottom field into tabular field[1 or 2] 
	INT32		bottom_field_replace(UINT8*, INT32, UINT8*, UINT8*);

	// This function stores the current top field into tabular field[1 or 2] 
	INT32		top_field_replace(UINT8*, INT32, UINT8*, UINT8*);

	INT32		blackout(UINT8* input_y, UINT8* input_u, UINT8* input_v);

	// Calculates the biggest common divider between num1 and num2, after Euclid's pgdc(a,b)=pgcd(b,a%b)
	UINT32		pgcd(UINT32, UINT32);

	// BICUBIC functions
	// -----------------
	INT32		cubic_scale(UINT8*, UINT8*, UINT32*, float*, UINT32*, float*, INT16*, INT16*, UINT32);
	INT32		cubic_spline(float, UINT32);
	INT32		cubic_scale_interlaced (UINT8*, UINT8*, UINT8*, UINT32* , float*, UINT32*, float*, INT16*, INT16*, UINT32);
	INT32		padding(UINT8*, UINT8*, UINT32); 
	INT32		padding_interlaced (UINT8*, UINT8*, UINT8*, UINT32);

	// RESAMPLE functions
	// ------------------
	// This function calculates multiplicative coeeficients to average an input (vector of) length
	// input_length into an output (vector of) length output_length;
	INT32		average_coeff(UINT32, UINT32, UINT32*);

	// This function average an input matrix of name input and of size local_input_width*(local_out_nb_line_slice*input_height_slice)
	// into an output matrix of name output and of size local_output_width*(local_out_nb_line_slice+output_height_slice)
	// input and output images are interleaved
	INT32		average(UINT8*, UINT8*, UINT32*, UINT32*, UINT32);

	// This function gathers code that are speed enhanced due to specific downscaling ratios
	INT32		average_specific(UINT8*, UINT8*, UINT32*, UINT32*, UINT32);
protected:
	// calculate the sample aspect ratio (SAR) for the output stream,
    // given the input->output scale factors, and the input SAR.
	y4m_ratio_t	calculate_output_sar(INT32 out_w, INT32 out_h, INT32 in_w, INT32 in_h, y4m_ratio_t in_sar);

protected:
	// variables
// For input
	UINT32			input_width;
	UINT32			input_height;
	y4m_ratio_t		input_sar;					// see yuv4mpeg.h and yuv4mpeg_intern.h for possible values
	UINT32			input_useful_width;
	UINT32			input_useful_height;
	UINT32			input_discard_col_left;
	UINT32			input_discard_col_right;
	UINT32			input_discard_line_above;
	UINT32			input_discard_line_under;
	UINT32			input_black_line_above;
	UINT32			input_black_line_under;
	UINT32			input_black_col_right;
	UINT32			input_black_col_left;
	UINT32			input_active_height;
	UINT32			input_active_width;
	INT32			input_interlaced;			// Y4M_ILACE_NONE (=default) | Y4M_ILACE_TOP_FIRST | Y4M_ILACE_BOTTOM_FIRST

	INT32			header_interlaced;
	// Downscaling ratios
	UINT32			input_height_slice;
	UINT32			output_height_slice;
	UINT32			input_width_slice;
	UINT32			output_width_slice;

	// For padded_input
	UINT32			padded_width;
	UINT32			padded_height;

	// For output
	UINT32			display_width;
	UINT32			display_height;
	UINT32			output_width;
	UINT32			output_height;
	UINT32			output_active_width;
	UINT32			output_active_height;
	UINT32			output_black_line_above;
	UINT32			output_black_line_under;
	UINT32			output_black_col_right;
	UINT32			output_black_col_left;
	UINT32			output_skip_line_above;
	UINT32			output_skip_line_under;
	UINT32			output_skip_col_right;
	UINT32			output_skip_col_left;
	INT32			output_interlaced;			// Y4M_ILACE_NONE (=default) | Y4M_ILACE_TOP_FIRST | Y4M_ILACE_BOTTOM_FIRST
// NB: as these number may not be multiple of output_[height,width]_slice, it is not possible to remove the corresponding pixels in
// the input frame, a solution that could speed up things. 

	// =Y4M_ILACE_NONE for not interlaced, =Y4M_ILACE_TOP_FIRST for top interlaced, =Y4M_ILACE_BOTTOM_FIRST for bottom interlaced
// double output_aspectratio;
// ************************************************************************************************************************ //
	INT32			output_fd;						// frames are written to stdout
// ************************************************************************************************************************ //
// Global variables
	//INT32			size_keyword;					// =1 is the SIZE keyword has been used
// ************************************************************************************************************************ //
	INT32			infile;							// =0 for stdin (default) =1 for file
// ************************************************************************************************************************ //
	bool			line_switching;					// =false('0') for no line switching, =true('1') for line switching
	UINT32			specific;						// is >0 if a specific downscaling speed enhanced treatment of data is possible

	// taken from lav2yuv
	INT32			chroma_format;
	UINT8			jpeg_data[MAX_JPEG_LEN];
/*
	const char*		VCD_KEYWORD;
	const char*		SVCD_KEYWORD;
	const char*		SIZE_KEYWORD;
	const char*		USE_KEYWORD;
	const char*		WIDE2STD_KEYWORD;
	const char*		INFILE_KEYWORD;
	const char*		RATIO_KEYWORD;
	const char*		TOP_FIRST;
	const char*		BOT_FIRST;
	const char*		NOT_INTER;
	const char*		PROGRESSIVE;
	const char*		MONO_KEYWORD;
	const char*		FASTVCD;
	const char*		FAST_WIDE2VCD;
	const char*		WIDE2VCD;
	const char*		RESAMPLE;
	const char*		BICUBIC;
	const char*		LINESWITCH;
	const char*		ACTIVE;
	const char*		NO_HEADER;
#ifdef HAVE_ASM_MMX
	const char*		MMX;
#endif
	const char*		TOP_FORWARD;
	const char*		BOTT_FORWARD;
*/
// Special BICUBIC algorithm
// 2048=2^11
	INT32			FLOAT2INTOFFSET;
	UINT32			bicubic_div_width, bicubic_div_height;
	float			bicubic_negative_max, bicubic_positive_max;
	INT32			bicubic_offset;
	UINT32			bicubic_max;
	UINT32			multiplicative;

// Unclassified
	UINT32			diviseur;
	UINT8*			divide;
	UINT32*			u_i_p;
	UINT32			out_nb_col_slice, out_nb_line_slice;
	INT32			verbose;

	UINT8			blacky;
	UINT8			blackuv;
	UINT8			field_move;							// =1 to move bottom field one frame forward, =-1 to move top field forward one frame

#ifdef HAVE_ASM_MMX
// MMX
	INT16*			mmx_padded, *mmx_cubic;
	INT32*			mmx_res;
	INT32			max_lint_neg;						// maximal negative number available for long int
	bool			mmx;								// =true for mmx activated
#endif
};

//void		print_usage(char* argv[]);
//void		handle_args_global(int32_t argc, char* argv[]);
//void		handle_args_dependent (int32_t argc, char* argv[]);

#endif // __YUVSCALER_H__