// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// yuv4mpeg_ratio.c:	part of the mjpgTools Library																	 //
//						Functions for dealing with y4m_ratio_t datatype.												 //
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

#include <string.h>
#include "yuv4mpeg.h"
#include "yuv4mpeg_intern.h"


/* useful list of standard framerates */
const y4m_ratio_t y4m_fps_UNKNOWN			= Y4M_FPS_UNKNOWN;
const y4m_ratio_t y4m_fps_NTSC_FILM			= Y4M_FPS_NTSC_FILM;
const y4m_ratio_t y4m_fps_FILM				= Y4M_FPS_FILM;
const y4m_ratio_t y4m_fps_PAL				= Y4M_FPS_PAL;
const y4m_ratio_t y4m_fps_NTSC				= Y4M_FPS_NTSC;
const y4m_ratio_t y4m_fps_30				= Y4M_FPS_30;
const y4m_ratio_t y4m_fps_PAL_FIELD			= Y4M_FPS_PAL_FIELD;
const y4m_ratio_t y4m_fps_NTSC_FIELD		= Y4M_FPS_NTSC_FIELD;
const y4m_ratio_t y4m_fps_60				= Y4M_FPS_60;

/* useful list of standard sample aspect ratios */
const y4m_ratio_t y4m_sar_UNKNOWN			= Y4M_SAR_UNKNOWN;
const y4m_ratio_t y4m_sar_SQUARE			= Y4M_SAR_SQUARE;
const y4m_ratio_t y4m_sar_SQR_ANA_16_9		= Y4M_SAR_SQR_ANA_16_9;
const y4m_ratio_t y4m_sar_NTSC_CCIR601		= Y4M_SAR_NTSC_CCIR601;
const y4m_ratio_t y4m_sar_NTSC_16_9			= Y4M_SAR_NTSC_16_9;
const y4m_ratio_t y4m_sar_NTSC_SVCD_4_3		= Y4M_SAR_NTSC_SVCD_4_3;
const y4m_ratio_t y4m_sar_NTSC_SVCD_16_9	= Y4M_SAR_NTSC_SVCD_16_9;
const y4m_ratio_t y4m_sar_PAL_CCIR601		= Y4M_SAR_PAL_CCIR601;
const y4m_ratio_t y4m_sar_PAL_16_9			= Y4M_SAR_PAL_16_9;
const y4m_ratio_t y4m_sar_PAL_SVCD_4_3		= Y4M_SAR_PAL_SVCD_4_3;
const y4m_ratio_t y4m_sar_PAL_SVCD_16_9		= Y4M_SAR_PAL_SVCD_16_9;

/* useful list of standard display aspect ratios */
const y4m_ratio_t y4m_dar_4_3				= Y4M_DAR_4_3;
const y4m_ratio_t y4m_dar_16_9				= Y4M_DAR_16_9;
const y4m_ratio_t y4m_dar_221_100			= Y4M_DAR_221_100;

/*
 *  Euler's algorithm for greatest common divisor
 */

static INT32 gcd(INT32 a, INT32 b)
{
	a = (a >= 0) ? a : (a * -1);
	b = (b >= 0) ? b : (b * -1);

	// INT32 x=0;
	while (b > 0)
	{
		INT32 x = b;
		b = a % b;
		a = x;
	}
	return a;
}


/************************************************************************
 *																		*
 * Remove common factors from a ratio									*
 *																		*
 ************************************************************************/

void y4m_ratio_reduce(y4m_ratio_t* r)
{
	INT32 d;
	if ((r->n == 0) && (r->d == 0)) return;  /* "unknown" */
	d = gcd(r->n, r->d);
	r->n /= d;
	r->d /= d;
}


/************************************************************************
 *																		*
 * Parse "nnn:ddd" into a ratio											*
 *																		*
 * returns:         Y4M_OK  - success									*
 *           Y4M_ERR_RANGE  - range error								*
 *																		*
 ************************************************************************/

INT32 y4m_parse_ratio(y4m_ratio_t* r, const char* s)
{
	char* t = strchr(s, ':');
	if (t == NULL) return Y4M_ERR_RANGE;
	r->n = atoi(s);
	r->d = atoi(t+1);
	if (r->d < 0) return Y4M_ERR_RANGE;
	/* 0:0 == unknown, so that is ok, otherwise zero denominator is bad */
	if ((r->d == 0) && (r->n != 0)) return Y4M_ERR_RANGE;
	y4m_ratio_reduce(r);
	return Y4M_OK;
}


/************************************************************************
 *																		*
 * Guess the true SAR (sample aspect ratio) from a list of commonly		*
 * encountered values, given the "suggested" display aspect ratio, and	*
 * the true frame width and height.										*
 *																		*
 * Returns y4m_sar_UNKNOWN if no match is found.						*
 *																		*
 ************************************************************************/

/* this is big enough to accommodate the difference between 720 and 704 */
#define GUESS_ASPECT_TOLERANCE		0.03

y4m_ratio_t y4m_guess_sar(INT32 width, INT32 height, y4m_ratio_t dar)
{
	INT32 i;
	double implicit_sar = (double)(dar.n * height) / (double)(dar.d * width);
	y4m_ratio_t sarray[10];
	sarray[0]	= y4m_sar_SQUARE;
	sarray[1]	= y4m_sar_NTSC_CCIR601;
	sarray[2]	= y4m_sar_NTSC_16_9;
	sarray[3]	= y4m_sar_NTSC_SVCD_4_3;
	sarray[4]	= y4m_sar_NTSC_SVCD_16_9;
	sarray[5]	= y4m_sar_PAL_CCIR601;
	sarray[6]	= y4m_sar_PAL_16_9;
	sarray[7]	= y4m_sar_PAL_SVCD_4_3;
	sarray[8]	= y4m_sar_PAL_SVCD_16_9;
	sarray[9]	= y4m_sar_UNKNOWN;

	// double ratio;
	for (i = 0; !(Y4M_RATIO_EQL(sarray[i],y4m_sar_UNKNOWN)); i++)
	{
		double ratio = implicit_sar / Y4M_RATIO_DBL(sarray[i]);
		if ( (ratio > (1.0 - GUESS_ASPECT_TOLERANCE)) && (ratio < (1.0 + GUESS_ASPECT_TOLERANCE)) )
			return sarray[i];
	}
	return y4m_sar_UNKNOWN;
}
