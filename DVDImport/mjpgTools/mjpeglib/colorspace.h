// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// colorspace.h:	part of the mjpgTools Library																			 //
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

#ifndef __COLORSPACE_H__
#define __COLORSPACE_H__

#ifdef __cplusplus
extern "C" {
#endif

void convert_RGB_to_YCbCr(UINT8* planes[], INT32 length);
void convert_YCbCr_to_RGB(UINT8* planes[], INT32 length);

#ifdef __cplusplus
	}
#endif

#endif // __COLORSPACE_H__