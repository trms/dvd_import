// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// jinclude.h:	part of the mjpgTools Library																			 //
//				This file exists to provide a single place to fix any problems with including the wrong system include	 //
//				files. (Common problems are taken care of by the standard jconfig symbols, but on really weird systems	 //
//				you may have to edit this file.)																		 //
//				NOTE: this file is NOT intended to be included by applications using the JPEG library.					 //
//				Most applications need only include jpeglib.h.															 //
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
/* Include auto-config file to find out which system include files we need. */

#include "jconfig.h"		/* auto configuration options */
#define JCONFIG_INCLUDED	/* so that jpeglib.h doesn't do it again */

/*
 * We need the NULL macro and size_t typedef.
 * On an ANSI-conforming system it is sufficient to include <stddef.h>.
 * Otherwise, we get them from <stdlib.h> or <stdio.h>; we may have to
 * pull in <sys/types.h> as well.
 * Note that the core JPEG library does not require <stdio.h>;
 * only the default error handler and data source/destination modules do.
 * But we must pull it in because of the references to FILE in jpeglib.h.
 * You can remove those references if you want to compile without <stdio.h>.
 */

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef NEED_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <stdio.h>

/*
 * We need memory copying and zeroing functions, plus strncpy().
 * ANSI and System V implementations declare these in <string.h>.
 * BSD doesn't have the mem() functions, but it does have bcopy()/bzero().
 * Some systems may declare memset and memcpy in <memory.h>.
 *
 * NOTE: we assume the size parameters to these functions are of type size_t.
 * Change the casts in these macros if not!
 */

#ifdef NEED_BSD_STRINGS

#include <strings.h>
#define MEMZERO(target,size)	bzero((void *)(target), (size_t)(size))
#define MEMCOPY(dest,src,size)	bcopy((const void *)(src), (void *)(dest), (size_t)(size))

#else /* not BSD, assume ANSI/SysV string lib */

#include <string.h>
#define MEMZERO(target,size)	memset((void *)(target), 0, (size_t)(size))
#define MEMCOPY(dest,src,size)	memcpy((void *)(dest), (const void *)(src), (size_t)(size))

#endif

/*
 * In ANSI C, and indeed any rational implementation, size_t is also the
 * type returned by sizeof().  However, it seems there are some irrational
 * implementations out there, in which sizeof() returns an int even though
 * size_t is defined as long or unsigned long.  To ensure consistent results
 * we always use this SIZEOF() macro in place of using sizeof() directly.
 */

#define SIZEOF(object)	((size_t) sizeof(object))

/*
 * The modules that use fread() and fwrite() always invoke them through
 * these macros.  On some systems you may need to twiddle the argument casts.
 * CAUTION: argument order is different from underlying functions!
 */

#define JFREAD(file,buf,sizeofbuf)  \
  ((size_t) fread((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))
#define JFWRITE(file,buf,sizeofbuf)  \
  ((size_t) fwrite((const void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))
