// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// cpu_accel.h:	part of the mjpgTools Library																			 //
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
//  The sixth public release of the Independent JPEG Group's free JPEG software, release 6b of 27-Mar-1998				 //
//																														 //
//  Copyright (C) 1991-1998, Thomas G. Lane.																			 //
//  This software is the work of Tom Lane, Philip Gladstone, Jim Boucher, Lee Crocker, Julian Minguillon, Luis Ortiz,	 //
//  George Phillips, Davide Rossi, Guido Vollbeding, Ge' Weijers, and other members of the Independent JPEG Group.		 //
//																														 //
//  The authors make NO WARRANTY or representation, either express or implied, with respect to this software, its		 //
//  quality, accuracy, merchantability, or fitness for a particular purpose.  This software is provided "AS IS", and	 //
//  you, its user, assume the entire risk as to its quality and accuracy.												 //
//  For conditions of distribution and use, see the accompanying README file in the original package (jpeg-mmx.zip)		 //
//																														 //
//  Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>													 //
//  This file is part of mpeg2dec, a free MPEG-2 video stream decoder.													 //
//	mpeg2dec is free software; you can redistribute it and/or modify it under the terms of the GNU General Public		 //
//	License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later //
//	version.																											 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __JCPU_ACCEL_H__
#define __JCPU_ACCEL_H__
// JPEG CUP_ACCEL

// x86 accelerations
// x86 accelerations
#ifndef ACCEL_X86_MMX
	#define ACCEL_X86_MMX		0x80000000
#endif
#ifndef ACCEL_X86_3DNOW
	#define ACCEL_X86_3DNOW		0x40000000
#endif
#ifndef ACCEL_X86_MMXEXT
	#define ACCEL_X86_MMXEXT	0x20000000
#endif
#ifndef ACCEL_X86_SSE
	#define ACCEL_X86_SSE		0x10000000
#endif

// INT32 cpu_accel (void);

#endif // __JCPU_ACCEL_H__