// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// jconfig.h:	part of the mjpgTools Library																			 //
//				jconfig.h for Microsoft Visual C++ on Windows 95 or NT.													 //
//				(see jconfig.doc for explanations)																		 //
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
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#ifndef USE_WINDOWS_MESSAGEBOX
	#define USE_WINDOWS_MESSAGEBOX
#endif // USE_WINDOWS_MESSAGEBOX

#ifdef JDCT_DEFAULT
	#undef JDCT_DEFAULT
#endif
#define JDCT_DEFAULT JDCT_FLOAT

#ifndef HAVE_PROTOTYPES
	#define HAVE_PROTOTYPES
#endif // HAVE_PROTOTYPES
#ifndef HAVE_UNSIGNED_CHAR
	#define HAVE_UNSIGNED_CHAR
#endif // HAVE_UNSIGNED_CHAR
#ifndef HAVE_UNSIGNED_SHORT
	#define HAVE_UNSIGNED_SHORT
#endif // HAVE_UNSIGNED_SHORT
/* #define void char */
/* #define const */
#undef CHAR_IS_UNSIGNED
#ifndef HAVE_STDDEF_H
	#define HAVE_STDDEF_H
#endif // HAVE_STDDEF_H
#ifndef HAVE_STDLIB_H
	#define HAVE_STDLIB_H
#endif // HAVE_STDLIB_H
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS	/* we presume a 32-bit flat memory model */
#undef NEED_SHORT_EXTERNAL_NAMES
#undef INCOMPLETE_TYPES_BROKEN

/* Define "boolean" as unsigned char, not int, per Windows custom */
#ifndef __RPCNDR_H__		/* don't conflict if rpcndr.h already read */
	typedef unsigned char boolean;
#endif
#ifndef HAVE_BOOLEAN
	#define HAVE_BOOLEAN		/* prevent jmorecfg.h from redefining it */
#endif

#ifdef JPEG_INTERNALS
	#undef RIGHT_SHIFT_IS_UNSIGNED
#endif /* JPEG_INTERNALS */

#ifdef JPEG_CJPEG_DJPEG
	#define BMP_SUPPORTED		/* BMP image file format */
	#define GIF_SUPPORTED		/* GIF image file format */
	#define PPM_SUPPORTED		/* PBMPLUS PPM/PGM image file format */
	#undef RLE_SUPPORTED		/* Utah RLE image file format */ // is only supported in Linux and Unix !! You need the URT btw.
	#define TARGA_SUPPORTED		/* Targa image file format */

	#define TWO_FILE_COMMANDLINE	/* optional */
	#define USE_SETMODE		/* Microsoft has setmode() */
	#undef NEED_SIGNAL_CATCHER
	#undef DONT_USE_B_MODE
	#undef PROGRESS_REPORT		/* optional */	// we may never use this definition, because we don't know whether this module
												// is gonna used in a DLL, WINAPI or ConsoleAPI
#endif /* JPEG_CJPEG_DJPEG */
