// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// djpeg.h:			part of the mjpgTools Library																		 //
//					Declaration of the function that launches the thread which does the 'real' work						 //
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
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#if !defined(AFX_DJPEG_H__0A0F2D7C_8A6F_11D6_810A_0050BF61B407__INCLUDED_)
#define AFX_DJPEG_H__0A0F2D7C_8A6F_11D6_810A_0050BF61B407__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	FMT_BMP			1		/* BMP format (Windows flavor) */
#define	FMT_GIF			2		/* GIF format */
#define	FMT_OS2			3		/* BMP format (OS/2 flavor) */
#define	FMT_PPM			4		/* PPM/PGM (PBMPLUS formats) */
#define	FMT_RLE			5		/* RLE format */
#define	FMT_TARGA		6		/* Targa format */

// these functions should only be available inside the DLL, not at the outside
void djpeg_Thread( void* arg ); // arg should be of type 'void** (void*[2])' : arg[0] = lpdjpeg_switches, arg[1] = unsigned int requested_output'
		// requested_output : one of the definitions as declared above

#endif // !defined(AFX_DJPEG_H__0A0F2D7C_8A6F_11D6_810A_0050BF61B407__INCLUDED_)
