// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// _yuvscaler.cpp:	implementation of the CMainYuvscaler class, part of the mjpgTools Library							 //
//					Scales arbitrary sized yuv frame to yuv frames suitable for VCD, SVCD or specified					 //
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
// This routine is based on :																							 //
//	YUVscaler of the MJPEGtools v1.6.0 (of the linux group)																 //
//																														 //
//	Copyright (C) 2001 Xavier Biquard <xbiquard@free.fr>																 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#include "stdafx.h"

#include ".\mjpeglib\config.h"

#include <io.h>						// for low-level file-routines
#include <sys\stat.h>				// for low-level file-routines
#include <fcntl.h>					// for low-level file-routines

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>

#include ".\mjpeglib\lav_io.h"
//#include ".\mjpeglib\editlist.h"
#include ".\mjpeglib\jpegutils.h"
#include ".\mjpeglib\mjpeg_logging.h"
#include ".\mjpeglib\yuv4mpeg.h"
#include ".\mjpeglib\mjpeg_types.h"
#include ".\mjpeglib\mpegconsts.h"
#include ".\mjpeglib\yuvscaler.hpp"

#define MJPGTOOLS_INTERNALS
#include "mjpgTools.h"
#include "privateMessages.h"		// for 'lk_...' strings
#include ".\tools\tools.h"			// for 'Message' and 'AskMessage'
#include "_yuvscaler.h"


class CMainYuvscaler : public CYuvScaler
{
public:
	CMainYuvscaler(HWND _hOwner=0, UINT _uOwner=0);
	~CMainYuvscaler();

	virtual int Run( lp_yuvscaler_struct lpSwitches );				// will hold parts of the original 'Main'

protected:
	BOOL		m_HasProgressInit;
	HWND		m_hOwner;
	UINT		m_uOwner;
	unsigned	m_initialFrameCount;

protected:
	// Parse Switches-functions
	int ParseSwitches( lp_yuvscaler_struct lpSwitches );
	int ParseSwitches_Dependent( lp_yuvscaler_struct lpSwitches );	// based on 'void handle_args_dependent()'
	int ParseSwitches_Global( lp_yuvscaler_struct lpSwitches );		// based on 'void handle_args_global()'
	// no 'print_usage' in this class, while this will be an abstract class

	// Processing Routines
	int PreProcess1(void);
	int PreProcess2(void);
	int Init(void);
	int MainProcess(void);

	void Information(LPCTSTR param=NULL);		// displays information to the user #1

private:
	LPCTSTR		_internal_progname;
	char		infilename[MAX_PATH];
	char		outfilename[MAX_PATH];

	bool		norm;							// =false for PAL or =true for NTSC
	bool		wide;							// =true for wide (16:9) input to standard (4:3) output conversion
	bool		mono;							// =true for monochrome output
	bool		no_header;						// =true for no stream header output 
	bool		ratio;
	bool		algorithm;						// =false for resample, and =true for bicubic

	bool		vcd;							// =true if vcd output was selected
	bool		svcd;							// =true if svcd output was selected
												// if(!vcd)&&(!svcd) --> custom resizing
	bool		input_useful;					// =true if specified
	bool		input_black;					// =true if black borders on input frames

	bool		skip, skip_line, skip_col;		// =true if lines or columns from the active output will not be displayed on output frames
	bool		black, black_line, black_col;	// =true if black lines must be generated on output

	// variables, used in the Pre-/Post- and Process-ing routines
	// SPECIFIC TO YUV4MPEG 
	UINT32				nb_pixels;
	y4m_frame_info_t	frameinfo;
	y4m_stream_info_t	in_streaminfo , out_streaminfo;
	y4m_ratio_t			frame_rate;

	// SPECIFIC FOR 'Init' and to be used in 'MainProcess'
	UINT32		*height_coeff, *width_coeff, *in_col, *in_line;
	float		*a, *b;
	INT16		*cubic_spline_m, *cubic_spline_n;
	UINT8		*padded_input, *padded_top, *padded_bottom, *line, *field1, *field2, *input, *output;
	UINT8		**frame_y_p, **frame_u_p, **frame_v_p;	// size is not yet known => pointer of pointer
	// pointers to other variables
	UINT8		*u_c_p;		//u_c_p = UINT8 pointer -- not created using 'malloc', so no 'free' plz.
	UINT8		*input_y, *input_u, *input_v;
	UINT8		*output_y, *output_u, *output_v;
	UINT8		*frame_y, *frame_u, *frame_v;

	// taken from lav2yuv
	//char**	filename;
	//EditList	el;

	// private routines
	void DeleteOutput(void); // does cleanup of the files on error
};

// ///////////////////////////////////////////////////////////////////// //
// class CMainYuvscaler :: Constructor/Destructor						 //
CMainYuvscaler::CMainYuvscaler(HWND _hOwner, UINT _uOwner) : CYuvScaler()
{
	m_HasProgressInit				= FALSE;
	m_hOwner						= _hOwner;
	m_uOwner						= _uOwner;
	m_initialFrameCount				= 0;

	_internal_progname				= _T("CMainYuvscaler");
	infilename[0] = outfilename[0]	= 0;

	norm							= false;	// =false for PAL/SECAM and =true for NTSC
	wide							= false;	// =true for wide (16:9) input to standard (4:3) output conversion
	mono							= false;	// =true for monochrome output
	no_header						= false;	// =true for no stream header output 
	ratio							= false;
	algorithm						= false;	// =false for resample, and =true for bicubic

	vcd								= false;	// =true if VCD output was selected
	svcd							= false;	// =true if SVCD output was selected

	input_useful					= false;	// =true if specified
	input_black						= false;	// =true if black borders on input frames
	
	skip = skip_line = skip_col		= false;	// =true if lines or columns from the active output will not be displayed on output frames
	black = black_line = black_col	= false;	// =true if black lines must be generated on output

	frame_rate						= y4m_fps_UNKNOWN;

	height_coeff = width_coeff = in_col = in_line = NULL;
	a = b							= NULL;
	cubic_spline_m = cubic_spline_n	= NULL;	
	padded_input = padded_top = padded_bottom = NULL;
	line = field1 = field2 = input = output = NULL;
	frame_y_p = frame_u_p = frame_v_p = NULL;	// size is not yet known => pointer of pointer

	//filename						= NULL;
	//ZeroMemory(&el, sizeof(EditList));
}
CMainYuvscaler::~CMainYuvscaler()
{
	if ((m_hOwner) && (m_uOwner) && (m_HasProgressInit))
	{
		lhkProgress_End((const void*)m_hOwner, (const unsigned)m_uOwner);
		m_HasProgressInit = FALSE;
	}
}

// ///////////////////////////////////////////////////////////////////// //
// class CMainYuvscaler :: Routines										 //
int CMainYuvscaler::ParseSwitches( lp_yuvscaler_struct lpSwitches )
{
	if ( lpSwitches == NULL) return -1;

	// FileChecking
	if (lpSwitches->infile[0])
		strcpy(infilename, lpSwitches->infile); // Set output file name.
	else
	{
		errMessage(lk_NO_INPUT_MESSAGE, _internal_progname);
		return 21;
	}
	// See if the input really exists
	if (! FileExists(infilename))
	{
		errMessage(lk_INPUT_NOT_FOUND, infilename);
		return 22;
	}

	if (lpSwitches->outfile[0])
		strcpy(outfilename, lpSwitches->outfile); // Set output file name.
	else
	{
		errMessage(lk_NO_OUTPUT_MESSAGE, _internal_progname);
		return 21;
	}
	// See if the output-path is correct (existing)
	if (! PathExists(outfilename))
	{
		char fdisk[_MAX_DRIVE], fdir[_MAX_DIR];
		_splitpath( outfilename, fdisk, fdir, NULL, NULL );
		errMessage(lk_OUTPUT_PATH_DOES_NOT_EXIST, fdisk, fdir);
		return 23;
	}
	// Ask whether the output may be overwritten, if it already exists
	if (FileExists(outfilename))
	{
		// suppress message if 'lpSwitches->overwrite' was preset to TRUE
		if (lpSwitches->overwrite == false)
		{
			char szMessage[256];
			wsprintf(szMessage, lk_FILE_EXISTS_MESSAGE, outfilename);
			if (! AskMessage(szMessage, lk_FILE_EXISTS_TITLE))
				return 24;
		}

		// delete the file, so this message isn't asked when walking for the 2nd pass
		if (! DeleteFile(outfilename))
		{
			errMessage(lk_OVERWRITE_ERROR, outfilename);
			return 25;
		}
	}

	// open the output 'output_fd'
	if ((output_fd=_open(outfilename, _O_BINARY | _O_CREAT | _O_RDWR | _O_TRUNC, _S_IREAD | _S_IWRITE)) == -1)
	{
		errMessage(lk_OVERWRITE_ERROR, outfilename);
		return -1; // seems not to be able to open the output
	}

	// open the input 'infile'
	if ((infile=_open(infilename, _O_BINARY | _O_RDONLY)) == -1)
	{
		errMessage(lk_CANNOT_OPEN_FOR_READ, _internal_progname, infilename);
		return -2; // seems not to be able to open the input
	}

	if (lpSwitches->version >= 1)
		m_initialFrameCount = lpSwitches->m_TotalFrames;

	return 0;
}

int CMainYuvscaler::ParseSwitches_Dependent( lp_yuvscaler_struct lpSwitches )
{
	// This function takes care of the global variables 
	// initialisation that may depend on the input stream
	// It does also coherence check on input, useful_input, display, output_active sizes and ratio sizes

	// OUTPUT
	switch (lpSwitches->Output)
	{
		case 3 :
		{ // resize : /*SIZE_KEYWORD="SIZE_"*/
			if ((lpSwitches->resize[0] == 0) || (lpSwitches->resize[0] % 2 != 0))
			{
				mjpeg_error(lk_INVALID_SIZE, _internal_progname, "height");
				return -1;
			}
			if ((lpSwitches->resize[1] == 0) || (lpSwitches->resize[1] % 2 != 0))
			{
				mjpeg_error(lk_INVALID_SIZE, _internal_progname, "width");
				return -1;
			}
			display_height	= lpSwitches->resize[0];
			display_width	= lpSwitches->resize[1];
			svcd			= false;
			vcd				= false;
			break;
		}
		case 2 :
		{ // SVCD : /*SVCD_KEYWORD="SVCD"*/
			svcd			= true;
			vcd				= false;
			display_width	= 480;
			switch (lpSwitches->outputNORM)
			{
				case 2 :
				{ // NTSC
					display_height = 480;
					break;
				}
				case 1 : // SECAM
				case 0 : // PAL
				default :
				{
					display_height = 576;
					break;
				}
			}
			break;
		}
		case 1 :
		{ // VCD : /*VCD_KEYWORD="VCD"*/
			vcd				= true;
			svcd			= false;
			display_width	= 352;
			switch (lpSwitches->outputNORM)
			{
				case 2 :
				{ // NTSC
					display_height = 240;
					break;
				}
				case 1 : // SECAM
				case 0 : // PAL
				default :
				{
					display_height = 288;
					break;
				}
			}
			break;
		}
		default :
		{ // By default, display sizes is the same as input size
			display_width	= input_width; // 'input_width' defined elsewhere...
			display_height	= input_height; // 'input_height' defined elsewhere...
			svcd			= false;
			vcd				= false;
			// ERROR -- Required option not entered
			// mjpeg_error("[%s]\nRequired switch 'Output' contained nothing.", _internal_progname);
			// return -2;
			break;
		}
	}

	// output_interlaced
	output_interlaced	= (lpSwitches->interlace == 3) ? Y4M_ILACE_BOTTOM_FIRST : \
		(lpSwitches->interlace == 2) ? Y4M_ILACE_TOP_FIRST : (lpSwitches->interlace == 1) ? Y4M_ILACE_NONE : output_interlaced;

	// mono (grayscaling)
	mono				= lpSwitches->grayscale; // true | false

	// MODE
	/*WIDE2STD_KEYWORD="WIDE2STD"*/
	wide				= (lpSwitches->flags & mode_WIDE2STD)		? true			: wide;
	/*RESAMPLE="RESAMPLE"*/
	algorithm			= (lpSwitches->flags & mode_RESAMPLE)		? false			: algorithm;
	/*BICUBIC="BICUBIC"*/
	algorithm			= (lpSwitches->flags & mode_BICUBIC)		? true			: algorithm;
	/*BOTT_FORWARD="BOTT_FORWARD"*/
	field_move			= (lpSwitches->flags & mode_BOTT_FORWARD)	? field_move+1	: field_move;
	/*TOP_FORWARD="TOP_FORWARD"*/
	field_move			= (lpSwitches->flags & mode_TOP_FORWARD)	? field_move-1	: field_move;
#ifdef HAVE_ASM_MMX
	/*MMX="MMX"*/
	mmx					= (lpSwitches->flags & mode_MMX)			? true			: mmx;
#endif
	/*LINESWITCH="LINE_SWITCH"*/
	line_switching		= (lpSwitches->flags & mode_LINE_SWITCH)	? true			: line_switching;
	/*NO_HEADER="NO_HEADER"*/
	no_header			= (lpSwitches->flags & mode_NO_HEADER)		? 1				: no_header;
	/*RATIO_KEYWORD="RATIO_"*/
	if (lpSwitches->flags & mode_RATIO)
	{
		ratio			= true;
		// verify that the entered heights and widths are valable
		if ((lpSwitches->ratio_data.h_in == 0) || (lpSwitches->ratio_data.h_out == 0) ||
			(lpSwitches->ratio_data.w_in == 0) || (lpSwitches->ratio_data.w_out == 0))
			ratio	= false;
		else
		{ // all contains a value>0
			// A coherence check will be done when the useful input sizes are known
			input_width_slice	= lpSwitches->ratio_data.w_in;
			output_width_slice	= lpSwitches->ratio_data.w_out;
			input_height_slice	= lpSwitches->ratio_data.h_in;
			output_height_slice	= lpSwitches->ratio_data.h_out;
		}
	}
	/*FAST_WIDE2VCD="FAST_WIDE2VCD"*/
	if (lpSwitches->flags & mode_FAST_WIDE2VCD)
	{
		wide					= true;
		ratio					= true;
		input_width_slice		= 2;
		output_width_slice		= 1;
		input_height_slice		= 2;
		output_height_slice		= 1;
		vcd						= true;
		svcd					= false;
		display_width			= 352;
		display_height			= (norm==true) ? 240 : 288;
	}
	/*WIDE2VCD="WIDE2VCD"*/
	if (lpSwitches->flags & mode_WIDE2VCD)
	{
		wide					= true;
		vcd						= true;
		svcd					= false;
		display_width			= 352;
		display_height			= (norm==true) ? 240 : 288;
	}
	/*FASTVCD="FASTVCD"*/
	if (lpSwitches->flags & mode_FASTVCD)
	{
		vcd						= true;
		svcd					= false;
		ratio					= true;
		input_width_slice		= 2;
		output_width_slice		= 1;
		input_height_slice		= 2;
		output_height_slice		= 1;
		display_width			= 352;
		display_height			= (norm==true) ? 240 : 288;
	}
	/*USE_KEYWORD="USE_"*/
		// Coherence check: 
		// every values must be multiple of 2
		// if input is interlaced, height offsets must be multiple of 4
		// since U and V have half Y resolution and are interlaced
		// and the required zone must be inside the input size
	if ( (lpSwitches->flags & mode_USE_INPUT) && ((lpSwitches->use_data.use_height != 0) &&
		(lpSwitches->use_data.use_width != 0) && (lpSwitches->use_data.discard_above != 0) &&
		(lpSwitches->use_data.discard_left != 0)) &&
		((lpSwitches->use_data.use_height % 2 == 0) && (lpSwitches->use_data.use_width % 2 == 0) &&
		(lpSwitches->use_data.discard_above % 2 == 0) && (lpSwitches->use_data.discard_left % 2 == 0)) &&
		((lpSwitches->use_data.use_width + lpSwitches->use_data.discard_left <= input_width) && 
		(lpSwitches->use_data.use_height + lpSwitches->use_data.discard_above <= input_height)) &&
		((input_interlaced == Y4M_ILACE_NONE) || ((input_interlaced != Y4M_ILACE_NONE) &&
		(lpSwitches->use_data.use_height % 4 == 0) && (lpSwitches->use_data.discard_above % 4 == 0))) )
	{
		input_useful_width			= lpSwitches->use_data.use_width;
		input_useful_height			= lpSwitches->use_data.use_height;
		input_discard_col_left		= lpSwitches->use_data.discard_left;
		input_discard_line_above	= lpSwitches->use_data.discard_above;

		input_discard_col_right		= input_width - input_useful_width - input_discard_col_left;
		input_discard_line_under	= input_height - input_useful_height - input_discard_line_above;

		input_useful				= true;
	}
	// /*TOP_FIRST="INTERLACED_TOP_FIRST"*/ + /*BOT_FIRST="INTERLACED_BOTTOM_FIRST"*/ + /*NOT_INTER="NOT_INTERLACED"*/ + /*PROGRESSIVE="PROGRESSIVE"*/
	input_interlaced = (lpSwitches->flags & mode_INTERLACED_TOP_FIRST) ? Y4M_ILACE_TOP_FIRST : \
		(lpSwitches->flags & mode_INTERLACED_BOTTOM_FIRST) ? Y4M_ILACE_BOTTOM_FIRST : \
		(lpSwitches->flags & mode_INTERLACED_PROGRESSIVE) ? Y4M_ILACE_NONE : input_interlaced;
	/*ACTIVE="ACTIVE"*/
		// Coherence check : offsets must be multiple of 2 since U and V have half Y resolution 
		// if interlaced, height must be multiple of 4
		// and the required zone must be inside the input size

	if ( (lpSwitches->flags & mode_ACTIVE_INPUT) && ((lpSwitches->active_data.act_height != 0) &&
		(lpSwitches->active_data.act_width != 0) && (lpSwitches->active_data.black_col_above != 0) &&
		(lpSwitches->active_data.black_col_left != 0)) && ((lpSwitches->active_data.act_height % 2 == 0) &&
		(lpSwitches->active_data.act_width % 2 == 0) && (lpSwitches->active_data.black_col_above % 2 == 0) &&
		(lpSwitches->active_data.black_col_left % 2 == 0)) && 
		((lpSwitches->active_data.act_width + lpSwitches->active_data.black_col_left <= input_width) &&
		(lpSwitches->active_data.act_height + lpSwitches->active_data.black_col_above <= input_height)) &&
		((input_interlaced == Y4M_ILACE_NONE) || ((input_interlaced != Y4M_ILACE_NONE) &&
		(lpSwitches->active_data.act_height % 4 == 0) && (lpSwitches->active_data.black_col_above % 4 == 0))) )
	{
		input_active_width			= lpSwitches->active_data.act_width;
		input_active_height			= lpSwitches->active_data.act_height;
		input_black_col_left		= lpSwitches->active_data.black_col_left;
		input_black_line_above		= lpSwitches->active_data.black_col_above;

		input_black_col_right		= input_width - input_active_width - input_black_col_left;
		input_black_line_under		= input_height - input_active_height - input_black_line_above;

		input_black					= true;
	}
	
// Interlacing, line-switching and field move
// 
// Line_switching is not equivalent to a change in interlacing type from TOP_FIRST to BOTTOM_FIRST or vice_versa since
// such conversion needs to interleave field 1 from frame 0 with field 0 from frame 1. Whereas line switching only works 
// with frame 0.
// 
// In fact, conversion from TOP_FIRST to BOTTOM_FIRST only needs to move bottom field one frame forward : keyword BOTT_FORWARD
// and conversion from BOTTOM_FIRST to TOP_FIRST needs to move top field one frame forward : keyword TOP_FORWARD.
// 
// By default, there is no line switching. Line switching will occur only if specified
// 
	if (vcd == true)
	{
		if ((output_interlaced == Y4M_ILACE_TOP_FIRST) || (output_interlaced == Y4M_ILACE_BOTTOM_FIRST))
			mjpeg_warn("VCD requires non-interlaced output frames. Ignoring interlaced specification");
		output_interlaced = Y4M_ILACE_NONE;
		if ((input_interlaced == Y4M_ILACE_TOP_FIRST) || (input_interlaced == Y4M_ILACE_BOTTOM_FIRST))
			mjpeg_warn("Interlaced input frames will be downscaled to non-interlaced VCD frames\nIf quality is an issue, please consider deinterlacing input frames with \"YUVDENOISE -I\"");
	}
	else
	{
		// Output may be of any kind of interlacing : svcd, size or simply same size as input
		// First case: output interlacing type has not been specified. By default, same as input
		if (output_interlaced == -1)
			output_interlaced = input_interlaced;
		else
		{
			// Second case: output interlacing has been specified and it is different from input => do necessary preprocessing
			if (output_interlaced != input_interlaced)
			{
				switch (input_interlaced)
				{
					case Y4M_ILACE_NONE :
					{
						mjpeg_warn("input not_interlaced/progressive but interlaced output required: hope you know what you're doing!");
						break;
					}
					case Y4M_ILACE_TOP_FIRST :
					{
						if (output_interlaced == Y4M_ILACE_BOTTOM_FIRST)
							field_move += 1;
						else
							mjpeg_warn("not_interlaced/progressive output required but top-interlaced input: hope you know what you're doing!");
						break;
					}
					case Y4M_ILACE_BOTTOM_FIRST :
					{
						if (output_interlaced == Y4M_ILACE_TOP_FIRST)
							field_move -= 1;
						else
							mjpeg_warn("not_interlaced/progressive output required but bottom-interlaced input: hope you know what you're doing!");
						break;
					}
				}
			}
		}
	}

	field_move = (field_move > 0) ? 1 : (field_move < 0) ? -1 : field_move;

	// Size Keyword final coherence check
	if ( ((output_interlaced != Y4M_ILACE_NONE) && ((vcd==false) && (svcd==false))) && (display_height % 4 != 0) )
    {
		mjpeg_error("[%s]\nInvalid 'height' entered for resizing.\nHeight is not multiple of 4 but output interlaced!!", _internal_progname);
		return -1;
    }

	// Unspecified input variables specification
	input_useful_width	= (input_useful_width == 0) ? input_width : input_useful_width;
	input_useful_height	= (input_useful_height == 0) ? input_height : input_useful_height;

	// Ratio coherence check against input_useful size
	if (ratio == true)
	{
		if ((input_useful_width % input_width_slice == 0) && (input_useful_height % input_height_slice == 0))
		{
			output_active_width		= (input_useful_width / input_width_slice) * output_width_slice;
			output_active_height	= (input_useful_height / input_height_slice) * output_height_slice;
		}
		else
		{
			mjpeg_error("[%s]\nSpecified input ratios (%u and %u) does not divide input useful size (%u and %u)!", _internal_progname,
				input_width_slice, input_height_slice, input_useful_width, input_useful_height);
			return -2;
		}
	}

	// if USE and ACTIVE keywords were used, redefined input ACTIVE size relative to USEFUL zone
	if ((input_black == true) && (input_useful == true))
	{
		input_black_line_above	= (input_black_line_above > input_discard_line_above) ? \
			input_black_line_above - input_discard_line_above : 0;
		input_black_line_under	= (input_black_line_under > input_discard_line_under) ? \
			input_black_line_under - input_discard_line_under : 0;
		input_black_col_left	= (input_black_col_left > input_discard_col_left) ? \
			input_black_col_left - input_discard_col_left : 0;
		input_black_col_right	= (input_black_col_right > input_discard_col_right) ? \
			input_black_col_right - input_discard_col_right : 0;

		input_active_width		= input_useful_width - input_black_col_left - input_black_col_right;
		input_active_height		= input_useful_height - input_black_line_above - input_black_line_under;

		if ((input_active_width == input_useful_width) && (input_active_height == input_useful_height))
			input_black = false;	// black zone is not enterely inside useful zone
	}

	// Unspecified output variables specification
	output_active_width			= (output_active_width == 0) ? display_width : output_active_width;
	output_active_height		= (output_active_height == 0) ? display_height : output_active_height;
//  display_width				= (display_width == 0) ? output_active_width : display_width;
//	display_height				= (display_height == 0) ? output_active_height : display_height

	if (wide == true)
		output_active_height = (output_active_height * 3) / 4;
	// Common pitfall! it is 3/4 not 9/16!
	// Indeed, Standard ratio is 4:3, so 16:9 has an height that is 3/4 smaller than the display_height

	// At this point, input size, input_useful size, output_active size and display size are specified
	// Time for the final coherence check and black and skip initialisations
	// Final check
	output_width	= (output_active_width > display_width) ? output_active_width : display_width;
	output_height	= (output_active_height > display_height) ? output_active_height : display_height;

	if ((output_active_width % 4 != 0) || (output_active_height % 4 != 0) ||
		(display_width % 4 != 0) || (display_height % 4 != 0))
	{
		mjpeg_error("[%s]\nOutput sizes are not multiple of 4! %ux%u, %ux%u being displayed.", _internal_progname,
			output_active_width, output_active_height, display_width, display_height);
		return -3;
	}

	// Skip and black initialisations
	if (output_active_width > display_width)
	{
		skip		= true;
		skip_col	= true;
		output_skip_col_right	= (output_active_width - display_width) / 2;
		output_skip_col_left	= output_active_width - display_width - output_skip_col_right;
	}
	if (output_active_width < display_width)
	{
		black		= true;
		black_col	= true;
		output_black_col_right	= (display_width - output_active_width) / 2;
		output_black_col_left	= display_width - output_active_width - output_black_col_right;
	}
	if (output_active_height > display_height)
	{
		skip		= true;
		skip_line	= true;
		output_skip_line_above	= (output_active_height - display_height) / 2;
		output_skip_line_under	= output_active_height - display_height - output_skip_line_above;
	}
	if (output_active_height < display_height)
	{
		black		= true;
		black_line	= true;
		output_black_line_above	= (display_height - output_active_height) / 2;
		output_black_line_under	= display_height - output_active_height - output_black_line_above;
	}

	// Scaling algorithm determination
	if (algorithm == false)
	{
		// Coherences check: downscale not upscale
		if ((input_useful_width < output_active_width) || (input_useful_height < output_active_height))
		{
			mjpeg_info("Resampling algorithm can only downscale, not upscale.\nSwitching to bicubic algorithm");
			algorithm = true;
		}
		else
			algorithm = false;
	}

	return 0;
}

int CMainYuvscaler::ParseSwitches_Global( lp_yuvscaler_struct lpSwitches )
{
	// This function takes care of the global variables 
	// initialisation that are independent of the input stream
	// The main goal is to know whether input frames originate from file or stdin

	// verbose
	verbose = ((lpSwitches->verbosity >= 1) && (lpSwitches->verbosity <= 3)) ? lpSwitches->verbosity-1 : verbose; // else : take default from main initialisation

	// TV norm for SVCD/VCD output
	switch (lpSwitches->outputNORM)
	{
		case 2 :
		{ // NTSC
			norm = true;
			break;
		}
		case 1 : // SECAM
		case 0 : // PAL
		default :
		{
			norm = false;
			break;
		}
	}

	return 0;
}

void CMainYuvscaler::DeleteOutput()
{
	if (output_fd != -1)
	{
		_close(output_fd);
		output_fd=-1;
		if ((outfilename[0]) && (FileExists(outfilename)))
			DeleteFile(outfilename); // delete destination
	}

	if (infile != -1)
	{
		_close(infile);
		infile=-1;
	}
}

// /////////////////////////////////////////////////////////////////////////////////
// Class CMainYuvscaler : Informational Routines

void CMainYuvscaler::Information(LPCTSTR param)
{
	LPCTSTR NOT_INTER		= _T("Not Interlaced");
	LPCTSTR PROGRESSIVE		= _T("Progressive");
	LPCTSTR TOP_FIRST		= _T("Interlaces TOP first");
	LPCTSTR BOT_FIRST		= _T("Interlaces BOTTOM first");

	char buffer[1024]; buffer[0]=0;
	// USER'S INFORMATION OUTPUT
	if ((param) && (param[0]))
		strcpy(buffer, param);

	// scaling
	wsprintf(&buffer[strlen(buffer)], _T("From %ux%u, take %ux%u+%u+%u"),
		input_width, input_height, input_useful_width, input_useful_height,
		input_discard_col_left, input_discard_line_above);

	switch (input_interlaced)
	{
		case Y4M_ILACE_NONE :
		{
			wsprintf(&buffer[strlen(buffer)], _T(", %s/%s\n"), NOT_INTER, PROGRESSIVE);
			break;
		}
		case Y4M_ILACE_TOP_FIRST :
		case Y4M_ILACE_BOTTOM_FIRST :
		{
			wsprintf(&buffer[strlen(buffer)], _T(", %s\n"), (input_interlaced==Y4M_ILACE_TOP_FIRST) ? TOP_FIRST : BOT_FIRST);
			break;
		}
		default :
		{
			// no interlacing - properly
			strcat(buffer, _T("\n"));
			break;
		}
	}

	if (input_black == true)
		wsprintf(&buffer[strlen(buffer)], _T("With %u black line above and %u black line under\nand %u black col left and %u black line right\nThe active width = %u, and the active height = %u\n"),
			input_black_line_above, input_black_line_under, input_black_col_left, input_black_col_right,
			input_active_width, input_active_height);

	// scaling
	wsprintf(&buffer[strlen(buffer)], _T("Scale to %ux%u, %ux%u being displayed"),
		output_active_width, output_active_height, display_width, display_height);

	switch (output_interlaced)
	{
		case Y4M_ILACE_NONE :
		{
			wsprintf(&buffer[strlen(buffer)], _T(", %s/%s\n"), NOT_INTER, PROGRESSIVE);
			break;
		}
		case Y4M_ILACE_TOP_FIRST :
		case Y4M_ILACE_BOTTOM_FIRST :
		{
			wsprintf(&buffer[strlen(buffer)], _T(", %s\n"), (output_interlaced==Y4M_ILACE_TOP_FIRST) ? TOP_FIRST : BOT_FIRST);
			break;
		}
		default :
		{
			// no interlacing - properly
			strcat(buffer, _T("\n"));
			break;
		}
	}

	wsprintf(&buffer[strlen(buffer)], _T("Scaling uses the %s algorithm, with%s line switching, "),
		(algorithm==true) ? _T("BICUBIC") : _T("RESAMPLE"),
		(line_switching==true) ? _T("") : _T("out"));

	switch (field_move)
	{
		case 0 :
		{
			wsprintf(&buffer[strlen(buffer)], _T("without time forwarding.\n"));
			break;

		}
		case 1 :
		{
			wsprintf(&buffer[strlen(buffer)], _T("with bottom field one frame forward.\n"));
			break;
		}
		case -1 :
		{
			wsprintf(&buffer[strlen(buffer)], _T("with top field one frame forward.\n"));
			break;
		}
	}

	if (black == true)
		wsprintf(&buffer[strlen(buffer)], _T("black lines: %u above and %u under -- black columns: %u left and %u right\n"),
			output_black_line_above, output_black_line_under, output_black_col_left, output_black_col_right);
	if (skip == true)
		wsprintf(&buffer[strlen(buffer)], _T("skipped lines: %u above and %u under -- skipped columns: %u left and %u right\n"),
			output_skip_line_above, output_skip_line_under, output_skip_col_left, output_skip_col_right);
/*
	int  decimal, sign;
	char *_buffer = _fcvt( Y4M_RATIO_DBL(frame_rate), 7, &decimal, &sign );
	if (_buffer != NULL)
	{
		strcpy(&buffer[strlen(buffer)], _T("frame rate: ~"));
		sign = strlen(buffer) + decimal;
		strncpy(&buffer[strlen(buffer)], _buffer, decimal);
		buffer[sign]=0;
		strcpy(&buffer[strlen(buffer)], _T("/"));
		strcpy(&buffer[strlen(buffer)], &_buffer[decimal]);
		strcpy(&buffer[strlen(buffer)], _T(" fps"));
		_buffer = NULL;
	}
*/
	wsprintf(&buffer[strlen(buffer)], _T("frame rate: ~%d/%d fps"), frame_rate.n, frame_rate.d);

	if (verbose > 0)
	{
		char bufT[64];
		wsprintf(bufT, _T("mjpgTools Library :: %s"), _internal_progname);
		infoMessage2(bufT, buffer);
	}
}

// /////////////////////////////////////////////////////////////////////////////////
// Class CMainYuvscaler : Processing Routines

int CMainYuvscaler::Run( lp_yuvscaler_struct lpSwitches )
{
	// Initialisations
	// ===============
	int iError = ParseSwitches(lpSwitches);
	if (iError != 0)
	{
		DeleteOutput();
		return (iError > 0) ? iError : -1;
	}

	if (ParseSwitches_Global(lpSwitches) != 0)
	{
		DeleteOutput();
		return -2;
	}

	y4m_init_stream_info(&in_streaminfo);
	y4m_init_stream_info(&out_streaminfo);
	y4m_init_frame_info(&frameinfo);

	iError = (PreProcess1() != 0) ? -3 : 0;

	if (iError == 0)
		iError = (ParseSwitches_Dependent(lpSwitches) != 0) ? -4 : 0;

	if (iError == 0)
		iError = (PreProcess2() != 0) ? -5 : 0;

	if (iError == 0)
		iError = (Init() != 0) ? -6 : 0;

	if (iError == 0)
		iError = (MainProcess() != 0) ? -7 : 0;

	// Free-up possible left-overs
	y4m_fini_stream_info(&in_streaminfo);
	y4m_fini_stream_info(&out_streaminfo);
	y4m_fini_frame_info (&frameinfo);

	if (divide			!= NULL) free(divide);
	if (height_coeff	!= NULL) free(height_coeff);
	if (width_coeff		!= NULL) free(width_coeff);

#ifdef HAVE_ASM_MMX
	if (mmx_padded		!= NULL) free(mmx_padded);
	if (mmx_cubic		!= NULL) free(mmx_cubic);
	if (mmx_res			!= NULL) free(mmx_res);
#endif

	if (in_col			!= NULL) free(in_col);
	if (in_line			!= NULL) free(in_line);
	if (a				!= NULL) free(a);
	if (b				!= NULL) free(b);
	if (cubic_spline_m	!= NULL) free(cubic_spline_m);
	if (cubic_spline_n	!= NULL) free(cubic_spline_n);
	if (padded_input	!= NULL) free(padded_input);
	if (padded_top		!= NULL) free(padded_top);
	if (padded_bottom	!= NULL) free(padded_bottom);
	if (line			!= NULL) free(line);
	if (field1			!= NULL) free(field1);
	if (field2			!= NULL) free(field2);
	if (input			!= NULL) free(input);
	if (output			!= NULL) free(output);
	if (frame_y_p		!= NULL) free(frame_y_p);
	if (frame_u_p		!= NULL) free(frame_u_p);
	if (frame_v_p		!= NULL) free(frame_v_p);

	if (iError != 0)
	{
		DeleteOutput();
		return iError;
	}
	else
	{ // close in-/output
		_close(infile); infile=-1;
		_close(output_fd); output_fd=-1;
	}

	return 0;
}

int CMainYuvscaler::PreProcess1()
{
	// mjpeg tools global initialisations
	mjpeg_default_handler_verbosity (verbose);

	// ***************************************************************************
	// Get video stream informations (size, framerate, interlacing, aspect ratio).
	// The streaminfo structure is filled in 
	// ***************************************************************************

	// we check for a correct file header
	if (y4m_read_stream_header (infile, &in_streaminfo) != Y4M_OK)
	{
		mjpeg_error("Could'nt read YUV4MPEG header!");
		return -1;
	}
	input_width = y4m_si_get_width (&in_streaminfo);
	input_height = y4m_si_get_height (&in_streaminfo);
	frame_rate = y4m_si_get_framerate (&in_streaminfo);
	input_interlaced = y4m_si_get_interlace (&in_streaminfo);

	// ***************
	// INITIALISATIONS
	// ***************
	// Norm determination from header (this has precedence over user's specification through the -n flag)
	norm = (Y4M_RATIO_EQL (frame_rate, y4m_fps_NTSC)) ? true : false;

	return 0; // returning to RUN, for launcing the 'Dependant' SwitchesParser

}

int CMainYuvscaler::PreProcess2()
{
	INT32 nb;
	UINT32 divider;

	// Show some information first
	y4m_log_stream_info (LOG_INFO, "yuvscaler input: ", &in_streaminfo);
	Information(NULL); // our own information

	divider				= pgcd(input_useful_width, output_active_width);
	input_width_slice	= input_useful_width / divider;
	output_width_slice	= output_active_width / divider;
#ifdef _DEBUG
	mjpeg_debug("divider,i_w_s,o_w_s = %d,%d,%d", divider, input_width_slice, output_width_slice);
#endif

	divider				= pgcd(input_useful_height, output_active_height);
	input_height_slice	= input_useful_height / divider;
	output_height_slice	= output_active_height / divider;
#ifdef _DEBUG
	mjpeg_debug("divider,i_w_s,o_w_s = %d,%d,%d", divider, input_height_slice, output_height_slice);
#endif

	diviseur			= input_height_slice * input_width_slice;
#ifdef _DEBUG
	mjpeg_debug ("Diviseur=%ld", diviseur);
#endif

	mjpeg_info ("Scaling ratio for width is %u to %u and\nfor height it is %u to %u",
		input_width_slice, output_width_slice, input_height_slice, output_height_slice);

	// Now that we know about scaling ratios, we can optimize treatment of an active input zone:
	// we must also check final new size is multiple of 2 on width and 2 or 4 on height

	if (input_black == true)
	{
		if ( ((nb = input_black_line_above / input_height_slice) > 0) && ((nb * input_height_slice) % 2 == 0) )
		{
			if (input_interlaced == Y4M_ILACE_NONE)
			{
				input_useful	= true;
				black			= true;
				black_line		= true;
				output_black_line_above		+= nb * output_height_slice;
				input_black_line_above		-= nb * input_height_slice;
				input_discard_line_above	+= nb * input_height_slice;
			}
	  		if ((input_interlaced != Y4M_ILACE_NONE) && ((nb * input_height_slice) % 4 == 0))
			{
				input_useful	= true;
				black			= true;
				black_line		= true;
				output_black_line_above		+= nb * output_height_slice;
				input_black_line_above		-= nb * input_height_slice;
				input_discard_line_above	+= nb * input_height_slice;
			}
		}
		if ( ((nb = input_black_line_under / input_height_slice) > 0) && ((nb * input_height_slice) % 2 == 0) )
		{
			if (input_interlaced == Y4M_ILACE_NONE)
			{
				input_useful	= true;
				black			= true;
				black_line		= true;
				output_black_line_under		+= nb * output_height_slice;
				input_black_line_under		-= nb * input_height_slice;
				input_discard_line_under	+= nb * input_height_slice;
			}
			if ((input_interlaced != Y4M_ILACE_NONE) && ((nb * input_height_slice) % 4 == 0))
			{
				input_useful	= true;
				black			= true;
				black_line		= true;
				output_black_line_under		+= nb * output_height_slice;
				input_black_line_under		-= nb * input_height_slice;
				input_discard_line_under	+= nb * input_height_slice;
			}
		}
		if ( ((nb = input_black_col_left / input_width_slice) > 0) && ((nb * input_height_slice) % 2 == 0) )
		{
			input_useful	= true;
			black			= true;
			black_col		= true;
			output_black_col_left	+= nb * output_width_slice;
			input_black_col_left	-= nb * input_width_slice;
			input_discard_col_left	+= nb * input_width_slice;
		}
		if ( ((nb = input_black_col_right / input_width_slice) > 0) && ((nb * input_height_slice) % 2 == 0) )
		{
			input_useful	= true;
			black			= true;
			black_col		= true;
			output_black_col_right	+= nb * output_width_slice;
			input_black_col_right	-= nb * input_width_slice;
			input_discard_col_right	+= nb * input_width_slice;
		}
		input_useful_height	= input_height - input_discard_line_above - input_discard_line_under;
		input_useful_width	= input_width - input_discard_col_left - input_discard_col_right;
		input_active_width	= input_useful_width - input_black_col_left - input_black_col_right;
		input_active_height	= input_useful_height - input_black_line_above - input_black_line_under;

		if ( (input_active_width == input_useful_width) && (input_active_height == input_useful_height) )
			input_black	= false; // black zone doesn't go beyong useful zone
		output_active_width		= (input_useful_width / input_width_slice) * output_width_slice;
		output_active_height	= (input_useful_height / input_height_slice) * output_height_slice;

		// Show some NEW information first
		Information(_T(" --- Newly speed optimized parameters ---\n\n"));
	}

	return 0;
}

int CMainYuvscaler::Init()
{
	// int err = Y4M_OK;
	UINT32	i, j; // used in 'for' loops

	// things for the messages
	LPCTSTR specific_message=_T("Specific downscaling routing number %u");
	LPCTSTR malloc_error=_T("Could not allocate memory for '%s'. -- STOP ! --");
	char bufT[64]; wsprintf(bufT, _T("mjpgTools Library :: %s -- Initialisation"), _internal_progname);

	if (algorithm == false)
	{
		// ********************************** //
		// <start> RESAMPLE RESAMPLE RESAMPLE //
		// ********************************** //

		// SPECIFIC
		// Is a specific downscaling speed enhanced treatment is available?
		if ( (output_width_slice == 1) && (input_width_slice == 1) )
			specific = 5;
		if ( (output_width_slice == 1) && (input_width_slice == 1) && (input_height_slice == 4) && (output_height_slice == 3) )
			specific = 7;
		if ( (input_height_slice == 2) && (output_height_slice == 1) )
			specific = 3;
		if ( (output_height_slice == 1) && (input_height_slice == 1) )
			specific = 1;
		if ( (output_height_slice == 1) && (input_height_slice == 1) && (output_width_slice == 2) && (input_width_slice == 3) )
			specific = 6;
		if ( (output_height_slice == 1) && (input_height_slice == 1) && (output_width_slice == 1) && (input_width_slice == 1) )
			specific = 4;
		if ( (input_height_slice == 2) && (output_height_slice == 1) && (input_width_slice == 2) && (output_width_slice == 1) )
			specific = 2;
		if ( (input_height_slice == 8) && (output_height_slice == 3) )
			specific = 9;
		if ( (input_height_slice == 8) && (output_height_slice == 3) && (input_width_slice == 2) && (output_width_slice == 1) )
			specific = 8;

		if (specific)
			mjpeg_info(specific_message, specific);

		// To speed up downscaling, we tabulate the integral part of the division by "diviseur" which is inside [0;255]
		// we use UINT8
		// But division of integers is always made by smaller => this results in a systematic drift towards smaller values.
		// To avoid that, we need a division that takes the nearest integral part.
		// So, prior to the division by smaller, we add 1/2 of the divider to the value to be divided
		if (! (divide = (UINT8*)(malloc(256 * diviseur * sizeof (UINT8) + ALIGNEMENT))) )
		{
			errMessage2(bufT, malloc_error, _T("divide table"));
			return -1;
		}

		// **********************************************************************
		// VERY DIRTY :
		// ------------
		// alignement instructions -- this will be disabled !!
		// if (((UINT32)divide % ALIGNEMENT) != 0)
		//	divide = (UINT8*)((((UINT32) divide / ALIGNEMENT) + 1) * ALIGNEMENT);
		// **********************************************************************

		u_c_p = divide;
		for (i=0; i < 256 * diviseur; i++)
		{
			*(u_c_p++) = (UINT8)(floor(((double) i + (double) diviseur / 2.0) / diviseur));
			// mjpeg_error("%ld: %d",i,(unsigned short int)(i/diviseur));
		}

		// Calculate averaging coefficient
		// -------------------------------
		// For the height
		if ( !(height_coeff = (UINT32*)(malloc((input_height_slice + 1) * output_height_slice * sizeof(UINT32)))) )
		{ // cannot use _alloca when var is send to external function
			errMessage2(bufT, malloc_error, _T("height_coeff"));
			return -2;
		}
		if (average_coeff(input_height_slice, output_height_slice, height_coeff))
			return -51; // error in the function
		// For the width
		if ( !(width_coeff = (UINT32*)(malloc((input_width_slice + 1) * output_width_slice * sizeof(UINT32)))) )
		{
			errMessage2(bufT, malloc_error, _T("width_coeff"));
			return -2;
		}
		if (average_coeff(input_width_slice, output_width_slice, width_coeff))
			return -51; // error in the function

		// ******************************** //
		// <end> RESAMPLE RESAMPLE RESAMPLE //
		// ******************************** //
	}
	else // (algorithm == true)
	{
		// ******************************* //
		// <start> BICUBIC BICUBIC BICUBIC //
		// ******************************* //
		UINT32	out_col, somme=0, out_line;
		INT32	value;

		// SPECIFIC
		// Is a specific downscaling speed enhanced treatment is available?
		if ((output_height_slice == 1) && (input_height_slice == 1))
		{
			specific			= 1;
			bicubic_div_height	= 18;
			bicubic_offset		= (UINT32)(ceil(bicubic_div_height * bicubic_div_width * bicubic_negative_max * 255.0));
			bicubic_max			= (UINT32)(ceil(bicubic_div_height * bicubic_div_width * bicubic_positive_max * 255.0));
			// en hauteur, les coefficients bicubic  cubic_spline_m valent 1/18, 16/18, 1/18 et 0
			// ^^^^ -->> whatever the original author means by this ???
		}
		if ( (output_height_slice == 1) && (input_height_slice == 1) && (output_width_slice == 2) && (input_width_slice == 3) )
		{
			specific			= 6;
			bicubic_div_height	= 18;
			bicubic_div_width	= 144;
			bicubic_offset		= (UINT32)(ceil(bicubic_div_height * bicubic_div_width * bicubic_negative_max * 255.0));
			bicubic_max			= (UINT32)(ceil(bicubic_div_height * bicubic_div_width * bicubic_positive_max * 255.0));
// /*debug*/ fprintf(stderr,"%ld %ld %ld %ld\n",bicubic_div_height,bicubic_div_width,bicubic_offset,bicubic_max);
			// en hauteur, les coefficients bicubic cubic_spline_m valent 1/18, 16/18, 1/18 et 0 (a=0)
			// en largeur, les coefficients bicubic cubic_spline_n valent pour les numéros de colonnes impaires (b=0.5) :
			// -5/144, 77/144, 77/144 et -5/144. Même chose qu'en hauteur pour les numéros de colonnes paires 
			// ^^^^ -->> same here ???
		}

		if (specific)
		{
			mjpeg_info(specific_message, specific);

			if (! (divide = (UINT8*)(malloc((bicubic_offset + bicubic_max) * sizeof(UINT8) + ALIGNEMENT))))
			{
				errMessage2(bufT, malloc_error, _T("divide table"));
				return -1;
// /*debug*/ fprintf (stderr, "%p\n", divide);
			}

			// **********************************************************************
			// VERY DIRTY :
			// ------------
			// alignement instructions -- this will be disabled !!
			// if (((UINT32)divide % ALIGNEMENT) != 0)
			//	divide = (UINT8*)((((UINT32) divide / ALIGNEMENT) + 1) * ALIGNEMENT);
			// **********************************************************************
			// alignement instructions

			u_c_p = divide;

			for (UINT32 lint=0; lint < bicubic_max + bicubic_offset; lint++)
			{
				value = (INT32)((lint - bicubic_offset + (INT32)(bicubic_div_height * bicubic_div_width / 2)) /
					(INT32)(bicubic_div_height * bicubic_div_width));
				if (value < 0)		value = 0;
				if (value > 255)	value = 255;
				*(u_c_p++) = (UINT8)value;
			}
		}

//		mjpeg_info("Specific routing for BICUBIC desactivated!!!!!!");       
//		specific = 0;
//		bicubic_div_height=FLOAT2INTEGER;
//		bicubic_div_width=FLOAT2INTEGER;

#ifdef HAVE_ASM_MMX
		if ( !(mmx_padded	= (INT16*)(malloc(4 * sizeof(INT16) + ALIGNEMENT))) ||
			 !(mmx_cubic	= (INT16*)(malloc(4 * sizeof(INT16) + ALIGNEMENT))) ||
			 !(mmx_res		= (INT32*)(malloc(2 * sizeof(INT32) + ALIGNEMENT))) )
		{
			errMessage2(bufT, malloc_error, _T("mmx registers"));
			return -3;
		}

		mjpeg_info("Alignement :\nmmx_padded = %u at [0x%p],\nmmx_cubic = %u at [0x%p]\n, mmx_res = %u at [0x%p]",
			(UINT32)mmx_padded, mmx_padded, (UINT32)mmx_cubic, mmx_cubic, (UINT32)mmx_res, mmx_res);


		// **********************************
		// TO DIRTY, removing from my code !!
		// ----------------------------------
		/*
		// alignement instructions
		if ( ((UINT32)mmx_padded % ALIGNEMENT) != 0)
			mmx_padded	= (INT16*)((((UINT32) mmx_padded / ALIGNEMENT) + 1) * ALIGNEMENT);
		if ( ((UINT32)mmx_cubic % ALIGNEMENT) != 0)
			mmx_cubic	= (INT16*)((((UINT32) mmx_cubic / ALIGNEMENT) + 1) * ALIGNEMENT);
		if ( ((UINT32)mmx_res % ALIGNEMENT) != 0)
			mmx_res		= (INT32*)((((UINT32) mmx_res / ALIGNEMENT) + 1) * ALIGNEMENT);

		mjpeg_info("(after) re-Alignement :\nmmx_padded = %u at [0x%p],\nmmx_cubic = %u at [0x%p]\n, mmx_res = %u at [0x%p]",
			(UINT32)mmx_padded, mmx_padded, (UINT32)mmx_cubic, mmx_cubic, (UINT32)mmx_res, mmx_res);
		*/
		// **********************************
#endif

		// Then, we can also tabulate several values
		// To the output pixel of coordinates (out_col,out_line) corresponds the input pixel (in_col,in_line), in_col and in_line being the nearest smaller values.
		if ( !(in_col = (UINT32*)(malloc(output_active_width * sizeof(UINT32)))) ||
			 !(b = (float*)(malloc(output_active_width * sizeof(float)))) )
		{
			errMessage2(bufT, malloc_error, _T("in_col' or 'b table"));
			return -4;
		}

		for (out_col=0; out_col < output_active_width; out_col++)
		{
			in_col[out_col]	= (out_col * input_width_slice) / output_width_slice;
			b[out_col]		= (float)((out_col * input_width_slice) % output_width_slice) / (float)output_width_slice;
//             fprintf(stderr,"out_col=%u,in_col=%u,b=%f\n",out_col,in_col[out_col],b[out_col]);
		}

		// Tabulation of the height : in_line and a;
		if ( !(in_line	= (UINT32*)(malloc(output_active_height * sizeof(UINT32)))) ||
			 !(a		= (float*)(malloc(output_active_height * sizeof(float)))) )
		{
			errMessage2(bufT, malloc_error, _T("in_line' or 'a table"));
			return -4;
		}

		for (out_line=0; out_line < output_active_height; out_line++)
		{
			in_line[out_line]	= (out_line * input_height_slice) / output_height_slice;
			a[out_line]			= (float)((out_line * input_height_slice) % output_height_slice) / (float)output_height_slice;
//             fprintf(stderr,"out_line=%u,in_line=%u,a=%f\n",out_line,in_line[out_line],a[out_line]);
		}
		// Tabulation of the cubic values 
		if ( !(cubic_spline_n = (INT16*)(malloc(4 * output_active_width * sizeof(INT16)))) ||
			 !(cubic_spline_m = (INT16*)(malloc(4 * output_active_height * sizeof(INT16)))) )
		{
			errMessage2(bufT, malloc_error, _T("cubic_spline_ntable' or 'cubic_spline_mtable"));
			return -5;
		}

		for (int n=-1; n <= 2; n++)
		{
			for (out_col=0; out_col < output_active_width; out_col++)
			{
				cubic_spline_n[out_col + (n + 1) * output_active_width] =
					cubic_spline(b[out_col] - (float)n, bicubic_div_width);
//				fprintf(stderr,"n=%d,out_col=%u,cubic=%ld\n",n,out_col,cubic_spline(b[out_col]-(float)n));;
			}
		}

		// Normalisation test and normalisation
		for (out_col=0; out_col < output_active_width; out_col++)
		{
			somme = cubic_spline_n[out_col + 0 * output_active_width] + cubic_spline_n[out_col + 1 * output_active_width] +
				cubic_spline_n[out_col + 2 * output_active_width] + cubic_spline_n[out_col + 3 * output_active_width];
			if (somme != bicubic_div_width)
			{
//				fprintf(stderr,"somme = %d\n",somme);
				cubic_spline_n[out_col + 3 * output_active_width] -= somme - bicubic_div_width;
			}
		}

		for (int m=-1; m <= 2; m++)
		{
			for (out_line = 0; out_line < output_active_height; out_line++)
			{
				cubic_spline_m[out_line + (m + 1) * output_active_height] = 
					cubic_spline((float)m - a[out_line], bicubic_div_height);
//				fprintf(stderr,"m=%d,out_line=%u,cubic=%ld\n",m,out_line,cubic_spline((float)m-a[out_line]));
			}
		}

		// Normalisation test and normalisation
		for (out_line=0; out_line < output_active_height; out_line++)
		{
			somme = cubic_spline_m[out_line + 0 * output_active_height] + cubic_spline_m[out_line + 1 * output_active_height] +
				cubic_spline_m[out_line + 2 * output_active_height] + cubic_spline_m[out_line + 3 * output_active_height];
			if (somme != bicubic_div_height)
			{
//				fprintf(stderr,"somme = %d\n",somme);
				cubic_spline_m[out_line + 3 * output_active_height] -= somme - bicubic_div_height;
			}
		}

		if ( (output_interlaced == Y4M_ILACE_NONE) || (input_interlaced == Y4M_ILACE_NONE) )
		{
			if ( !(padded_input = (UINT8*)(malloc((input_useful_width + 3) * (input_useful_height + 3)))) )
			{
				errMessage2(bufT, malloc_error, _T("padded_input table"));
				return -6;
			}
		}
		else
		{
			if ( !(padded_top = (UINT8*)(malloc((input_useful_width + 3) * (input_useful_height / 2 + 3)))) ||
				 !(padded_bottom = (UINT8*)(malloc((input_useful_width + 3) * (input_useful_height / 2 + 3)))) )
			{
				errMessage2(bufT, malloc_error, _T("padded_top|bottom tables"));
				return -7;
			}
		}

		// ***************************** //
		// <end> BICUBIC BICUBIC BICUBIC //
		// ***************************** //
	}

	// Pointers allocations
	//  input = alloca ((input_width * input_height * 3) / 2);
	//  output = alloca ((output_width * output_height * 3) / 2);
	if ( !(line		= (UINT8*)(malloc(input_width))) ||
		 !(field1	= (UINT8*)(malloc(3 * (input_width / 2) * (input_height / 2)))) ||
		 !(field2	= (UINT8*)(malloc(3 * (input_width / 2) * (input_height / 2)))) ||
		 !(input	= (UINT8*)(malloc(((input_width * input_height * 3) / 2) + ALIGNEMENT))) ||
		 !(output	= (UINT8*)(malloc(((output_width * output_height * 3) / 2) + ALIGNEMENT))) )
	{
		errMessage2(bufT, malloc_error, _T("line-', 'field1-', 'field2-', 'input-' or 'output-tables"));
		return -8;
	}
	// ************************************************************
	// TO DIRTY, removing 'alignement instructions' from my code !!
	// ------------------------------------------------------------

	// if skip_col==true
	if ( !(frame_y_p = (UINT8**)(malloc(display_height * sizeof(UINT8*)))) ||
		 !(frame_u_p = (UINT8**)(malloc(display_height / 2 * sizeof(UINT8*)))) ||
		 !(frame_v_p = (UINT8**)(malloc(display_height / 2 * sizeof(UINT8*)))) )
	{
		errMessage2(bufT, malloc_error, _T("frame_y_p-', 'frame_u_p-' or 'frame_v_p-tables"));
		return -9;
	}

	// Incorporate blacks lines and columns directly into output matrix since this will never change. 
	// BLACK pixel in YUV = (16,128,128)
	if (black == true)
	{
		u_c_p = output;
		// Y component
		for (i=0; i < output_black_line_above * output_width; i++)
			*(u_c_p++) = blacky;
		if (black_col == false)
			u_c_p += output_active_height * output_width;
		else
		{
			for (i=0; i < output_active_height; i++)
			{
				for (j=0; j < output_black_col_left; j++)
					*(u_c_p++) = blacky;
				u_c_p += output_active_width;
				for (j=0; j < output_black_col_right; j++)
					*(u_c_p++) = blacky;
			}
		}
		for (i=0; i < output_black_line_under * output_width; i++)
			*(u_c_p++) = blacky;

		// U component
		//   u_c_p=output+output_width*output_height;
		for (i=0; i < output_black_line_above / 2 * output_width / 2; i++)
			*(u_c_p++) = blackuv;
		if (black_col == false)
			u_c_p += output_active_height / 2 * output_width / 2;
		else
		{
			for (i=0; i < output_active_height / 2; i++)
			{
				for (j=0; j < output_black_col_left / 2; j++)
					*(u_c_p++) = blackuv;
				u_c_p += output_active_width / 2;
				for (j=0; j < output_black_col_right / 2; j++)
					*(u_c_p++) = blackuv;
			}
		}
		for (i=0; i < output_black_line_under / 2 * output_width / 2; i++)
			*(u_c_p++) = blackuv;

		// V component
		//   u_c_p=output+(output_width*output_height*5)/4;
		for (i=0; i < output_black_line_above / 2 * output_width / 2; i++)
			*(u_c_p++) = blackuv;
		if (black_col == false)
			u_c_p += output_active_height / 2 * output_width / 2;
		else
		{
			for (i=0; i < output_active_height / 2; i++)
			{
				for (j=0; j < output_black_col_left / 2; j++)
					*(u_c_p++) = blackuv;
				u_c_p += output_active_width / 2;
				for (j=0; j < output_black_col_right / 2; j++)
					*(u_c_p++) = blackuv;
			}
		}
		for (i=0; i < output_black_line_under / 2 * output_width / 2; i++)
			*(u_c_p++) = blackuv;
	} // (black == true)

	// MONOCHROME FRAMES
	if (mono == true)
	{
		// the U and V components of output frame will always be 128
		u_c_p = output + output_width * output_height;
		for (i=0; i < 2 * output_width / 2 * output_height / 2; i++)
			*(u_c_p++) = blackuv;
	}

	// Various initialisatiosn for input and output   
	out_nb_col_slice	= output_active_width / output_width_slice;
	out_nb_line_slice	= output_active_height / output_height_slice;
	input_y	 = input + input_discard_line_above * input_width + input_discard_col_left;
	input_u	 = input + input_width * input_height + input_discard_line_above / 2 * input_width / 2 + input_discard_col_left / 2;
	input_v	 = input + (input_height * input_width * 5) / 4 + input_discard_line_above / 2 * input_width / 2 + input_discard_col_left / 2;
	output_y = output + output_black_line_above * output_width + output_black_col_left;
	output_u = output + output_width * output_height + output_black_line_above / 2 * output_width / 2 + output_black_col_left / 2;
	output_v = output + (output_width * output_height * 5) / 4 + output_black_line_above / 2 * output_width / 2 + output_black_col_left / 2;

	// Other initialisations for frame output
	frame_y	= output + output_skip_line_above * output_width + output_skip_col_left;
	frame_u	= output + output_width * output_height + output_skip_line_above / 2 * output_width / 2 + output_skip_col_left / 2;
	frame_v	= output + (output_width * output_height * 5) / 4 + output_skip_line_above / 2 * output_width / 2 + output_skip_col_left / 2;

	if (skip_col == true)
	{
		for (i=0; i < display_height; i++)
			frame_y_p[i] = frame_y + i * output_width;
		for (i=0; i < display_height / 2; i++)
		{
			frame_u_p[i] = frame_u + i * output_width / 2;
			frame_v_p[i] = frame_v + i * output_width / 2;
		}
	}

	nb_pixels = (input_width * input_height * 3) / 2;
	mjpeg_debug ("End of Initialisation");
  // END OF INITIALISATION -- finally!

	return 0;
}

// ************************
// The real work start here
// ************************
int CMainYuvscaler::MainProcess()
{
	LPCTSTR unspec_error=_T("Terminating routine after unspecified error, code #%03d");
	LPCTSTR out_error=_T("Unable to write to output - aborting!");
	char bufT[64]; wsprintf(bufT, _T("mjpgTools Library :: %s -- MainProcess"), _internal_progname);

	int		err = Y4M_OK;
	UINT32	i;
	UINT32	frame_num=0;
	char	prgText[256];

	// Output file header
	y4m_copy_stream_info(&out_streaminfo, &in_streaminfo);
	y4m_si_set_width(&out_streaminfo, display_width);
	y4m_si_set_height(&out_streaminfo, display_height);
	y4m_si_set_interlace(&out_streaminfo, output_interlaced);
	y4m_si_set_sampleaspect(&out_streaminfo,
		calculate_output_sar(output_width_slice, output_height_slice, input_width_slice, input_height_slice,
			y4m_si_get_sampleaspect(&in_streaminfo)));
	if (no_header == false)
		y4m_write_stream_header(output_fd, &out_streaminfo);
	y4m_log_stream_info(LOG_INFO, "yuvscaler output : ", &out_streaminfo);

	// Master loop : continue until there is no next frame in stdin
		// Je sais pas pourquoi, mais y4m_read_frame merde, y4m_read_frame_header suivi de y4m_read marche !!!!!!!
		// ^^^^ -> more french comments of the original author...
	// Line switch if necessary
	while ((err = my_y4m_read_frame(infile, &frameinfo, nb_pixels, input/*, line_switching*/)) == Y4M_OK)
	{
		if ((m_hOwner) && (m_uOwner))
		{
			if (!m_HasProgressInit)
			{
				// notify owner we're about to start
				lhkProgress_Init((const void*)m_hOwner, (const unsigned)m_uOwner, "Scaling YUV", (const long)m_initialFrameCount);
				m_HasProgressInit = TRUE;
			}

			sprintf(prgText, "Processing frame #%ld", frame_num+1);
			lhkProgress_Step((const void*)m_hOwner, (const unsigned)m_uOwner, prgText, (const long)frame_num+1);
		}

		mjpeg_info ("Frame number %ld", frame_num);

		// PREPROCESSING
		if (field_move != 0)
		{
			if (field_move == 1)
			{ // Bottom field one frame forward
				if (frame_num == 0)
				{
					bottom_field_storage(input, frame_num, field1, field2);
					if ( my_y4m_read_frame(infile, &frameinfo, nb_pixels, input/*, line_switching*/) != Y4M_OK)
					{
						// error #001
						errMessage2(bufT, unspec_error, 1);
						return -1;
					} // else
					frame_num++;
					if ((m_hOwner) && (m_uOwner))
					{
						sprintf(prgText, "Processing frame #%ld", frame_num+1);
						lhkProgress_Step((const void*)m_hOwner, (const unsigned)m_uOwner, prgText, (const long)frame_num+1);
					}
					mjpeg_info ("Frame number %ld", frame_num);
				}
				bottom_field_storage(input, frame_num, field1, field2);
				bottom_field_replace(input, frame_num, field1, field2);
			}
			else
			{ // Top field one frame forward
				if (frame_num == 0)
				{
					top_field_storage(input, frame_num, field1, field2);
					if ( my_y4m_read_frame(infile, &frameinfo, nb_pixels, input/*, line_switching*/) != Y4M_OK)
					{
						// error #002
						errMessage2(bufT, unspec_error, 2);
						return -2;
					} // else
					frame_num++;
					if ((m_hOwner) && (m_uOwner))
					{
						sprintf(prgText, "Processing frame #%ld", frame_num+1);
						lhkProgress_Step((const void*)m_hOwner, (const unsigned)m_uOwner, prgText, (const long)frame_num+1);
					}
					mjpeg_info("Frame number %ld", frame_num);
				}
				top_field_storage(input, frame_num, field1, field2);
				top_field_replace(input, frame_num, field1, field2);
			}
		}
		frame_num++;
		if ((m_hOwner) && (m_uOwner))
		{
			sprintf(prgText, "Processing frame #%ld", frame_num+1);
			lhkProgress_Step((const void*)m_hOwner, (const unsigned)m_uOwner, prgText, (const long)frame_num+1);
		}

		// Output Frame Header
		if ( y4m_write_frame_header(output_fd, &frameinfo) != Y4M_OK)
		{
			errMessage2(bufT, out_error);
			return -3;
		}

		// Blackout if necessary
		if (input_black == true)
			blackout(input_y, input_u, input_v);

		// SCALE THE FRAME
		if (algorithm == false)
		{ // RESAMPLE
			if (mono == true)
			{
				if (!specific) // == 0
					average(input_y, output_y, height_coeff, width_coeff, 0);
				else
				{
					if (average_specific(input_y, output_y, height_coeff, width_coeff, 0))
						return -52; // error in the function
				}
			}
			else
			{
				if (!specific)
				{
					average(input_y, output_y, height_coeff, width_coeff, 0);
					average(input_u, output_u, height_coeff, width_coeff, 1);
					average(input_v, output_v, height_coeff, width_coeff, 1);
				}
				else
				{
					if (average_specific(input_y, output_y, height_coeff, width_coeff, 0)) return -52; // error in the function
					if (average_specific(input_u, output_u, height_coeff, width_coeff, 1)) return -52; // error in the function
					if (average_specific(input_v, output_v, height_coeff, width_coeff, 1)) return -52; // error in the function
				}
			}
		}
		else // if !algorithm
		{ // BICIBIC
			// INPUT FRAME PADDING BEFORE BICUBIC INTERPOLATION PADDING IS DONE SEPARATELY FOR EACH COMPONENT
			if (mono == true)
			{
				if ( (output_interlaced == Y4M_ILACE_NONE) || (input_interlaced == Y4M_ILACE_NONE) )
				{
					padding(padded_input, input_y, 0);
					cubic_scale(padded_input, output_y, in_col, b, in_line, a, cubic_spline_n, cubic_spline_m, 0);
				}
				else
				{
					padding_interlaced(padded_top, padded_bottom, input_y, 0);
					cubic_scale_interlaced (padded_top, padded_bottom, output_y, in_col, b, in_line, a,
						cubic_spline_n, cubic_spline_m, 0);
				}
			}
			else
			{
				if ( (output_interlaced == Y4M_ILACE_NONE) || (input_interlaced == Y4M_ILACE_NONE) )
				{
					padding (padded_input, input_y, 0);
					cubic_scale(padded_input, output_y, in_col, b, in_line, a, cubic_spline_n, cubic_spline_m, 0);
					padding(padded_input, input_u, 1);
					cubic_scale(padded_input, output_u, in_col, b, in_line, a, cubic_spline_n, cubic_spline_m, 1);
					padding(padded_input, input_v, 1);
					cubic_scale(padded_input, output_v, in_col, b, in_line, a, cubic_spline_n, cubic_spline_m, 1);
				}
				else
				{
					padding_interlaced(padded_top, padded_bottom, input_y, 0);
					cubic_scale_interlaced(padded_top, padded_bottom, output_y, in_col, b, in_line, a,
						cubic_spline_n, cubic_spline_m, 0);
					padding_interlaced(padded_top, padded_bottom, input_u, 1);
					cubic_scale_interlaced(padded_top, padded_bottom, output_u, in_col, b, in_line, a,
						cubic_spline_n, cubic_spline_m, 1);
					padding_interlaced(padded_top, padded_bottom, input_v, 1);
					cubic_scale_interlaced(padded_top, padded_bottom, output_v, in_col, b, in_line, a,
						cubic_spline_n, cubic_spline_m, 1);
				}
			}
		}

		// OUTPUT FRAME CONTENTS
		if (skip == false)
		{
			// Here, display=output_active 
			if ( y4m_write(output_fd, output, (display_width * display_height * 3) / 2) != Y4M_OK)
			{
				errMessage2(bufT, out_error);
				return -4;
			}
		}
		else // skip == true
		{ 
			if (skip_col == false)
			{
				// output_active_width==display_width, component per component frame output
				if ( y4m_write(output_fd, frame_y, display_width * display_height) != Y4M_OK)
				{
					errMessage2(bufT, out_error);
					return -5;
				}
				if ( y4m_write(output_fd, frame_u, display_width / 2 * display_height / 2) != Y4M_OK)
				{
					errMessage2(bufT, out_error);
					return -6;
				}
				if ( y4m_write(output_fd, frame_v, display_width / 2 * display_height / 2) != Y4M_OK)
				{
					errMessage2(bufT, out_error);
					return -7;
				}
			}
			else
			{
				// output_active_width > display_width, line per line frame output for each component
				for (i=0; i < display_height; i++)
				{
					if ( y4m_write(output_fd, frame_y_p[i], display_width) != Y4M_OK)
					{
						errMessage2(bufT, out_error);
						return -8;
					}
				}

				for (i=0; i < display_height / 2; i++)
				{
					if ( y4m_write(output_fd, frame_u_p[i], display_width / 2) != Y4M_OK)
					{
						errMessage2(bufT, out_error);
						return -9;
					}
				}

				for (i=0; i < display_height / 2; i++)
				{
					if ( y4m_write(output_fd, frame_v_p[i], display_width / 2) != Y4M_OK)
					{
						errMessage2(bufT, out_error);
						return -10;
					}
				}
			}
		}
	} // End of master loop

	if (err != Y4M_ERR_EOF)
		mjpeg_info("Couldn't read FRAME number %ld!", frame_num);
	else
		mjpeg_info("End of stream with FRAME number %ld!", frame_num);

	if ((m_hOwner) && (m_uOwner))
	{
		lhkProgress_End((const void*)m_hOwner, (const unsigned)m_uOwner);
		m_HasProgressInit = FALSE;
	}

	return 0;
}


// ///////////////////////////////////////////////////////////////////
// The Thread that let "CMainYuvscaler" work
// ///////////////////////////////////////////////////////////////////
void yuvscaler_Thread( void* arg )
{
	lp_yuvscaler_struct lpSwitches = (arg) ? (lp_yuvscaler_struct)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!lpSwitches)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	CMainYuvscaler* mainYuvScaler;
	if (lpSwitches->version >= 1)
		mainYuvScaler = new CMainYuvscaler(lpSwitches->m_hOwner, lpSwitches->m_uOwner);
	else
		mainYuvScaler = new CMainYuvscaler();

	int retVal = EXIT_FAILURE; // 1 : General Failure, possible in JPeG-lib
	retVal = mainYuvScaler->Run(lpSwitches); // if Run don't exceeds normaly, will have an error in JPeG-lib

	delete mainYuvScaler; mainYuvScaler=NULL;
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}
