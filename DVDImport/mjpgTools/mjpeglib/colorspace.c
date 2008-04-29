// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// colorspace.c:	part of the mjpgTools Library																			 //
//					Routines to perform colorspace conversions.															 //
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
#include "mjpeg_types.h"

#include "colorspace.h"

#define FP_BITS		18

/* precomputed tables */

static INT32 Y_R[256];
static INT32 Y_G[256];
static INT32 Y_B[256];
static INT32 Cb_R[256];
static INT32 Cb_G[256];
static INT32 Cb_B[256];
static INT32 Cr_R[256];
static INT32 Cr_G[256];
static INT32 Cr_B[256];
static INT32 conv_RY_inited = 0;

static INT32 RGB_Y[256];
static INT32 R_Cr[256];
static INT32 G_Cb[256];
static INT32 G_Cr[256];
static INT32 B_Cb[256];
static INT32 conv_YR_inited = 0;


static INT32 myround(double n)
{
	if (n >= 0)
		return (INT32)(n + 0.5);
	else
		return (INT32)(n - 0.5);
}


static void init_RGB_to_YCbCr_tables(void)
{
	INT32 i;

	/*
	 * Q_Z[i] =   (coefficient * i
	 *             * (Q-excursion) / (Z-excursion) * fixed-point-factor)
	 *
	 * to one of each, add the following:
	 *             + (fixed-point-factor / 2)         --- for rounding later
	 *             + (Q-offset * fixed-point-factor)  --- to add the offset
	 */
	for (i = 0; i < 256; i++)
	{
		Y_R[i] = myround(0.299 * (double)i * 219.0 / 255.0 * (double)(1<<FP_BITS));
		Y_G[i] = myround(0.587 * (double)i * 219.0 / 255.0 * (double)(1<<FP_BITS));
		Y_B[i] = myround((0.114 * (double)i * 219.0 / 255.0 * (double)(1<<FP_BITS)) + (double)(1<<(FP_BITS-1)) + (16.0 * (double)(1<<FP_BITS)));

		Cb_R[i] = myround(-0.168736 * (double)i * 224.0 / 255.0 * (double)(1<<FP_BITS));
		Cb_G[i] = myround(-0.331264 * (double)i * 224.0 / 255.0 * (double)(1<<FP_BITS));
		Cb_B[i] = myround((0.500 * (double)i * 224.0 / 255.0 * (double)(1<<FP_BITS)) + (double)(1<<(FP_BITS-1)) + (128.0 * (double)(1<<FP_BITS)));

		Cr_R[i] = myround(0.500 * (double)i * 224.0 / 255.0 * (double)(1<<FP_BITS));
		Cr_G[i] = myround(-0.418688 * (double)i * 224.0 / 255.0 * (double)(1<<FP_BITS));
		Cr_B[i] = myround((-0.081312 * (double)i * 224.0 / 255.0 * (double)(1<<FP_BITS)) + (double)(1<<(FP_BITS-1)) + (128.0 * (double)(1<<FP_BITS)));
	}
	conv_RY_inited = 1;
}


static void init_YCbCr_to_RGB_tables(void)
{
	INT32 i;

	/*
	 * Q_Z[i] =   (coefficient * i
	 *             * (Q-excursion) / (Z-excursion) * fixed-point-factor)
	 *
	 * to one of each, add the following:
	 *             + (fixed-point-factor / 2)         --- for rounding later
	 *             + (Q-offset * fixed-point-factor)  --- to add the offset
	 */

	/* clip Y values under 16 */
	for (i = 0; i < 16; i++)
	{
		RGB_Y[i] = myround((1.0 * (double)(16) * 255.0 / 219.0 * (double)(1<<FP_BITS)) + (double)(1<<(FP_BITS-1)));
	}
	for (i = 16; i < 236; i++)
	{
		RGB_Y[i] = myround((1.0 * (double)(i - 16) * 255.0 / 219.0 * (double)(1<<FP_BITS)) + (double)(1<<(FP_BITS-1)));
	}
	/* clip Y values above 235 */
	for (i = 236; i < 256; i++)
	{
		RGB_Y[i] = myround((1.0 * (double)(235) * 255.0 / 219.0 * (double)(1<<FP_BITS)) + (double)(1<<(FP_BITS-1)));
	}
    
	/* clip Cb/Cr values below 16 */	 
	for (i = 0; i < 16; i++)
	{
		R_Cr[i] = myround(1.402 * (double)(-112) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		G_Cr[i] = myround(-0.714136 * (double)(-112) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		G_Cb[i] = myround(-0.344136 * (double)(-112) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		B_Cb[i] = myround(1.772 * (double)(-112) * 255.0 / 224.0 * (double)(1<<FP_BITS));
	}
	for (i = 16; i < 241; i++)
	{
		R_Cr[i] = myround(1.402 * (double)(i - 128) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		G_Cr[i] = myround(-0.714136 * (double)(i - 128) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		G_Cb[i] = myround(-0.344136 * (double)(i - 128) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		B_Cb[i] = myround(1.772 * (double)(i - 128) * 255.0 / 224.0 * (double)(1<<FP_BITS));
	}
	/* clip Cb/Cr values above 240 */	 
	for (i = 241; i < 256; i++)
	{
		R_Cr[i] = myround(1.402 * (double)(112) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		G_Cr[i] = myround(-0.714136 * (double)(112) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		G_Cb[i] = myround(-0.344136 * (double)(i - 128) * 255.0 / 224.0 * (double)(1<<FP_BITS));
		B_Cb[i] = myround(1.772 * (double)(112) * 255.0 / 224.0 * (double)(1<<FP_BITS));
	}
	conv_YR_inited = 1;
}


/* 
 * in-place conversion [R', G', B'] --> [Y', Cb, Cr]
 *
 */
void convert_RGB_to_YCbCr(UINT8* planes[], INT32 length)
{
	UINT8* Y, *Cb, *Cr;
	INT32 i, r, g, b;

	if (!conv_RY_inited) init_RGB_to_YCbCr_tables();

	for ( i = 0, Y = planes[0], Cb = planes[1], Cr = planes[2]; i < length; i++, Y++, Cb++, Cr++ )
	{
		r = *Y;
		g = *Cb;
		b = *Cr;

		*Y = (UINT8)((Y_R[r] + Y_G[g]+ Y_B[b]) >> FP_BITS);
		*Cb = (UINT8)((Cb_R[r] + Cb_G[g]+ Cb_B[b]) >> FP_BITS);
		*Cr = (UINT8)((Cr_R[r] + Cr_G[g]+ Cr_B[b]) >> FP_BITS);
	}
}


/* 
 * in-place conversion [Y', Cb, Cr] --> [R', G', B']
 *
 */
void convert_YCbCr_to_RGB(UINT8* planes[], INT32 length)
{
	UINT8 *R, *G, *B;
	INT32 i, y, cb, cr, r, g, b;

	if (!conv_YR_inited) init_YCbCr_to_RGB_tables();

	for ( i = 0, R = planes[0], G = planes[1], B = planes[2]; i < length; i++, R++, G++, B++ )
	{
		y = *R;
		cb = *G;
		cr = *B;

		r = (RGB_Y[y] + R_Cr[cr]) >> FP_BITS;
		g = (RGB_Y[y] + G_Cb[cb]+ G_Cr[cr]) >> FP_BITS;
		b = (RGB_Y[y] + B_Cb[cb]) >> FP_BITS;

		*R = (UINT8)((r < 0) ? 0 : (r > 255) ? 255 : r );
		*G = (UINT8)((g < 0) ? 0 : (g > 255) ? 255 : g );
		*B = (UINT8)((b < 0) ? 0 : (b > 255) ? 255 : b );
	}
}

