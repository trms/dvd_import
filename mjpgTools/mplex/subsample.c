// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// subsample.c:	part of the mjpgTools Library																			 //
//				Routines to do chroma subsampling																		 //
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
//	Copyright (C) 2001 Matthew J. Marjanovic <maddog@mir.com>															 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "config.h"
//#define USE_WINDOWS_MESSAGEBOX
#ifdef USE_WINDOWS_MESSAGEBOX
	#include <windows.h>				// for 'RaiseException(...)'
	#include "tools.h"					// for 'message2(...)'
#else
	#include <assert.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "mjpeg_types.h"
#include "mjpeg_logging.h"
#include "subsample.h"

const char* ssm_id[SSM_COUNT] = {
	"unknown",
	"420_jpeg",
	"420_mpeg2",
#if 0
	"420_dv_pal",
	"411_dv_ntsc"
#endif
};


const char* ssm_description[SSM_COUNT] = {
	"unknown/illegal",
	"4:2:0, JPEG/MPEG-1, interstitial siting",
	"4:2:0, MPEG-2, horizontal cositing",
#if 0
	"4:2:0, DV-PAL, cosited, Cb/Cr line alternating",
	"4:1:1, DV-NTSC"
	"4:2:2",
#endif
};


/****************************************************************************
 * Chroma Subsampling														*
 ****************************************************************************/

/* vertical/horizontal interstitial siting
 *
 *    Y   Y   Y   Y
 *      C       C
 *    Y   Y   Y   Y
 *
 *    Y   Y   Y   Y
 *      C       C
 *    Y   Y   Y   Y
 *
 */

static void ss_444_to_420jpeg(UINT8* buffer, INT32 width, INT32 height)
{
	UINT8* in0, *in1, *out;
	INT32 x, y;

	in0 = buffer;
	in1 = buffer + width;
	out = buffer;
	for (y = 0; y < height; y += 2)
	{
		for (x = 0; x < width; x += 2)
		{
			*out = (in0[0] + in0[1] + in1[0] + in1[1]) >> 2;
			in0 += 2;
			in1 += 2;
			out++;
		}
		in0 += width;
		in1 += width;
	}
}


/* vertical/horizontal interstitial siting
 *
 *    Y   Y   Y   Y
 *      C       C       C      inm
 *    Y   Y   Y   Y           
 *                  
 *    Y   Y   Y - Y           out0
 *      C     | C |     C      in0
 *    Y   Y   Y - Y           out1
 *
 *
 *      C       C       C      inp
 *
 *
 *  Each iteration through the loop reconstitutes one 2x2 block of 
 *   pixels from the "surrounding" 3x3 block of samples...
 *  Boundary conditions are handled by cheap reflection; i.e. the
 *   center sample is simply reused.
 *              
 */             

#define BLANK_CRB		in0[1]


#if 1 /* triangle/linear filter (otherwise, use the lame box filter) */
static void ss_420jpeg_to_444(UINT8* buffer, INT32 width, INT32 height)
{
	UINT8* inm, *in0, *inp, *out0, *out1;
	UINT8 cmm, cm0, cmp, c0m, c00, c0p, cpm, cp0, cpp;
	INT32 x, y;
	static UINT8* saveme = NULL;
	static INT32 saveme_size = 0;

	if (width > saveme_size)
	{
		free(saveme);
		saveme_size = width;
		if ((saveme = malloc(saveme_size * sizeof(saveme[0]))) == NULL)
#ifdef USE_WINDOWS_MESSAGEBOX
		{
			errMessage2("MJPeGlib :: ASSERT", "(saveme == NULL)\nPropperly insufficient memory available.\n\n>> subsample.c, line 143");
			RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
		}
#else
			assert(saveme != NULL);
#endif
	}
	memcpy(saveme, buffer, width);

	in0 = buffer + (width * height / 4) - 2;
	inm = in0 - width/2;
	inp = in0 + width/2;
	out1 = buffer + (width * height) - 1;
	out0 = out1 - width;

	for (y = height; y > 0; y -= 2)
	{
		if (y == 2)
		{
			in0 = saveme + width/2 - 1;
			inp = in0 + width/2;
		}
		for (x = width; x > 0; x -= 2)
		{
#if 0
			if ((x == 2) && (y == 2))
			{
				cmm = in0[1];
				cm0 = in0[1];
				cmp = in0[2];
				c0m = in0[1];
				c0p = in0[2];
				cpm = inp[1];
				cp0 = inp[1];
				cpp = inp[2];
			}
			else if ((x == 2) && (y == height))
			{
				cmm = inm[1];
				cm0 = inm[1];
				cmp = inm[2];
				c0m = in0[1];
				c0p = in0[2];
				cpm = in0[1];
				cp0 = in0[1];
				cpp = in0[2];
			}
			else if ((x == width) && (y == height))
			{
				cmm = inm[0];
				cm0 = inm[1];
				cmp = inm[1];
				c0m = in0[0];
				c0p = in0[1];
				cpm = in0[0];
				cp0 = in0[1];
				cpp = in0[1];
			}
			else if ((x == width) && (y == 2))
			{
				cmm = in0[0];
				cm0 = (y == 2) ? BLANK_CRB : inm[1];
				cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
				c0m = (x == 2) ? BLANK_CRB : in0[0];
				c0p = (x == width) ? BLANK_CRB : in0[2];
				cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
				cp0 = (y == height) ? BLANK_CRB : inp[1];
				cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
			}
			else if (x == 2)
			{
				cmm = ((x == 2) || (y == 2)) ? BLANK_CRB : inm[0];
				cm0 = (y == 2) ? BLANK_CRB : inm[1];
				cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
				c0m = (x == 2) ? BLANK_CRB : in0[0];
				c0p = (x == width) ? BLANK_CRB : in0[2];
				cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
				cp0 = (y == height) ? BLANK_CRB : inp[1];
				cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
			}
			else if (y == 2)
			{
				cmm = ((x == 2) || (y == 2)) ? BLANK_CRB : inm[0];
				cm0 = (y == 2) ? BLANK_CRB : inm[1];
				cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
				c0m = (x == 2) ? BLANK_CRB : in0[0];
				c0p = (x == width) ? BLANK_CRB : in0[2];
				cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
				cp0 = (y == height) ? BLANK_CRB : inp[1];
				cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
			}
			else if (x == width)
			{
				cmm = ((x == 2) || (y == 2)) ? BLANK_CRB : inm[0];
				cm0 = (y == 2) ? BLANK_CRB : inm[1];
				cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
				c0m = (x == 2) ? BLANK_CRB : in0[0];
				c0p = (x == width) ? BLANK_CRB : in0[2];
				cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
				cp0 = (y == height) ? BLANK_CRB : inp[1];
				cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
			}
			else if (y == height)
			{
				cmm = ((x == 2) || (y == 2)) ? BLANK_CRB : inm[0];
				cm0 = (y == 2) ? BLANK_CRB : inm[1];
				cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
				c0m = (x == 2) ? BLANK_CRB : in0[0];
				c0p = (x == width) ? BLANK_CRB : in0[2];
				cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
				cp0 = (y == height) ? BLANK_CRB : inp[1];
				cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
			}
			else
			{
				cmm = ((x == 2) || (y == 2)) ? BLANK_CRB : inm[0];
				cm0 = (y == 2) ? BLANK_CRB : inm[1];
				cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
				c0m = (x == 2) ? BLANK_CRB : in0[0];
				c0p = (x == width) ? BLANK_CRB : in0[2];
				cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
				cp0 = (y == height) ? BLANK_CRB : inp[1];
				cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
			}
			c00 = in0[1];

			cmm = ((x == 2) || (y == 2)) ? BLANK_CRB : inm[0];
			cm0 = (y == 2) ? BLANK_CRB : inm[1];
			cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
			c0m = (x == 2) ? BLANK_CRB : in0[0];
			c0p = (x == width) ? BLANK_CRB : in0[2];
			cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
			cp0 = (y == height) ? BLANK_CRB : inp[1];
			cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
#else
			cmm = ((x == 2) || (y == 2)) ? BLANK_CRB : inm[0];
			cm0 = (y == 2) ? BLANK_CRB : inm[1];
			cmp = ((x == width) || (y == 2)) ? BLANK_CRB : inm[2];
			c0m = (x == 2) ? BLANK_CRB : in0[0];
			c00 = in0[1];
			c0p = (x == width) ? BLANK_CRB : in0[2];
			cpm = ((x == 2) || (y == height)) ? BLANK_CRB : inp[0];
			cp0 = (y == height) ? BLANK_CRB : inp[1];
			cpp = ((x == width) || (y == height)) ? BLANK_CRB : inp[2];
#endif
			inm--;
			in0--;
			inp--;

			*(out1--) = (1*cpp + 3*(cp0+c0p) + 9*c00 + 8) >> 4;
			*(out1--) = (1*cpm + 3*(cp0+c0m) + 9*c00 + 8) >> 4;
			*(out0--) = (1*cmp + 3*(cm0+c0p) + 9*c00 + 8) >> 4;
			*(out0--) = (1*cmm + 3*(cm0+c0m) + 9*c00 + 8) >> 4;
		}
		out1 -= width;
		out0 -= width;
	}
}

#else
static void ss_420jpeg_to_444(UINT8* buffer, INT32 width, INT32 height)
{
	UINT8* in, *out0, *out1;
	INT32 x, y;

	in = buffer + (width * height / 4) - 1;
	out1 = buffer + (width * height) - 1;
	out0 = out1 - width;
	for (y = height - 1; y >= 0; y -= 2)
	{
		for (x = width - 1; x >= 0; x -=2)
		{
			UINT8 val = *(in--);
			*(out1--) = val;
			*(out1--) = val;
			*(out0--) = val;
			*(out0--) = val;
		}
		out0 -= width;
		out1 -= width;
	}
}
#endif      


/* vertical intersitial siting; horizontal cositing
 *
 *    Y   Y   Y   Y
 *    C       C
 *    Y   Y   Y   Y
 *
 *    Y   Y   Y   Y
 *    C       C
 *    Y   Y   Y   Y
 *
 * [1,2,1] kernel for horizontal subsampling:
 *
 *    inX[0] [1] [2]
 *        |   |   |
 *    C   C   C   C
 *         \  |  /
 *          \ | /
 *            C
 */

static void ss_444_to_420mpeg2(UINT8* buffer, INT32 width, INT32 height)
{
	UINT8* in0, *in1, *out;
	INT32 x, y;

	in0 = buffer;          /* points to */
	in1 = buffer + width;  /* second of pair of lines */
	out = buffer;
	for (y = 0; y < height; y += 2)
	{
		/* first column boundary condition -- just repeat it to right */
		*out = (in0[0] + (2 * in0[0]) + in0[1] + in1[0] + (2 * in1[0]) + in1[1]) >> 3;
		out++;
		in0++;
		in1++;
		/* rest of columns just loop */
		for (x = 2; x < width; x += 2)
		{
			*out = (in0[0] + (2 * in0[1]) + in0[2] + in1[0] + (2 * in1[1]) + in1[2]) >> 3;
			in0 += 2;
			in1 += 2;
			out++;
		}
		in0 += width + 1;
		in1 += width + 1;
	}
}



void chroma_subsample(subsample_mode_t mode, UINT8* ycbcr[], INT32 width, INT32 height)
{
	switch (mode)
	{
		case SSM_420_JPEG :
		{
			ss_444_to_420jpeg(ycbcr[1], width, height);
			ss_444_to_420jpeg(ycbcr[2], width, height);
			break;
		}
		case SSM_420_MPEG2 :
		{
			ss_444_to_420mpeg2(ycbcr[1], width, height);
			ss_444_to_420mpeg2(ycbcr[2], width, height);
			break;
		}
//		default :
//		{
//			break;
//		}
	}
}


void chroma_supersample(subsample_mode_t mode, UINT8* ycbcr[], INT32 width, INT32 height)
{
	switch (mode)
	{
		case SSM_420_JPEG :
		{
			ss_420jpeg_to_444(ycbcr[1], width, height);
			ss_420jpeg_to_444(ycbcr[2], width, height);
			break;
		}
		case SSM_420_MPEG2 :
		{
			// ss_420mpeg2_to_444(ycbcr[1], width, height);
			// ss_420mpeg2_to_444(ycbcr[2], width, height);
#ifdef USE_WINDOWS_MESSAGEBOX
			RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
#else
			exit(4);
#endif
			break;
		}
//		default:
//		{
//			break;
//		}
	}
}

