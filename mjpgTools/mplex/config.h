// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// config.h:	part of the mjpgTools Library																			 //
//				Global CONFIG file for this sub-project, required by all files which needs the use of the mjpegLib		 //
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
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __CONFIG_H__
#define __CONFIG_H__

#undef _GNU_SOURCE

#ifdef VERSION
	#undef VERSION
#endif // VERSION
#define VERSION						"1.6.0"		/* Mjpeg tools release version */

/* Large file support ? */
#ifndef _FILE_OFFSET_BITS
	#define _FILE_OFFSET_BITS			32
	#define _LARGEFILE_SOURCE
	#define _LARGEFILE64_SOURCE
#endif // _FILE_OFFSET_BITS

/* Define pthread lib stack size */
#undef HAVE_PTHREADSTACKSIZE

/* Define for an Intel based CPU */
#ifndef HAVE_X86CPU
	#define HAVE_X86CPU					1
#endif // HAVE_X86CPU

#ifdef HAVE_X86CPU
	/* For HAVEX86CPU: Define for availability of CMOV instruction (P6, P7
	 * and Athlon cores).*/
	#undef HAVE_CMOV

	/* For HAVEX86CPU: Define if the installed GCC tool chain can generate
	 * MMX instructions */
	#undef HAVE_ASM_MMX

	/* For HAVEX86CPU: Define if the installed GCC tool-chain can generate
	 * 3DNow instructions */
	#undef HAVE_ASM_3DNOW

	/* For HAVEX86CPU: Define if the nasm assembler is available -- also set if MASM is available (in VC6 it is) */
	#ifndef HAVE_ASM_NASM
		#define HAVE_ASM_NASM			1
	#endif
#else
	#undef HAVE_CMOV
	#undef HAVE_ASM_MMX
	#undef HAVE_ASM_3DNOW
	#undef HAVE_ASM_NASM
#endif

/* Define for a PowerPPC CPU */
#undef HAVE_PPCCPU

/* whether we're in linux or not (video4linux?) */
#undef HAVE_V4L

/* Define if the libpthread library is present */
#undef HAVE_LIBPTHREAD

/* Define if the quicktime for linux library is present */
#undef HAVE_LIBQUICKTIME
#undef HAVE_OPENQUICKTIME

/* Define if the libXxf86dga library is available */
#undef HAVE_LIBXXF86DGA

/* Define if the libmovtar library is available */
#undef HAVE_LIBMOVTAR

/* do we have some cool thing for 64bits integers? */
#ifndef PRID64_STRING_FORMAT
	#define PRID64_STRING_FORMAT		"l64"
#endif // PRID64_STRING_FORMAT

/* Define for software MJPEG playback */
#undef BUILD_MJPEG

/* Define for libDV and possibly YV12 support */
#undef LIBDV_PAL_YV12
#undef SUPPORT_READ_DV2
#undef SUPPORT_READ_YUV420
#undef LIBDV_PRE_0_9_5

/* whether we have avifile/gtk+/sdl */
#ifndef HAVE_AVIFILE
	#define HAVE_AVIFILE				1
	#define COMPILE_LAV_IO_C			1
#endif
#undef AVIFILE_USE_DESTFMT
#undef HAVE_GTK
#undef HAVE_SDL

/* Name of package */
#ifndef PACKAGE
	#define PACKAGE						"MJPeGtools"
	#define MJPEGTOOLS					1
#endif // PACKAGE

/* Define if disable assert() */
#ifdef _DEBUG
	#undef NDEBUG // we want 'asserts' when in debug-mode
#else
	#ifndef NDEBUG
		#define NDEBUG
	#endif
#endif

/* Define if you have the <alloca.h> header file. */
// #undef HAVE_ALLOCA_H

/* Define if you have the <dlfcn.h> header file. */
#undef HAVE_DLFCN_H

/* Define if you have the <fenv.h> header file. */
#undef HAVE_FENV_H

/* Define if you have the <getopt.h> header file. */
#undef HAVE_GETOPT_H

/* getopt support */
#undef HAVE_GETOPT_LONG

/* Define if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define if you have the <memory.h> header file. */
#ifndef HAVE_MEMORY_H
	#define HAVE_MEMORY_H				1
#endif // HAVE_MEMORY_H

/* Define if you have the <stdbool.h> header file. */
#undef HAVE_STDBOOL_H

/* Define if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
	#define HAVE_STDLIB_H				1
#endif // HAVE_STDLIB_H

/* Define if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define if you have the <string.h> header file. */
#ifndef HAVE_STRING_H
	#define HAVE_STRING_H				1
#endif // HAVE_STRING_H

/* Define if you have the <sys/soundcard.h> header file. */
#undef HAVE_SYS_SOUNDCARD_H

/* Define if you have the <sys/stat.h> header file. */
#ifndef HAVE_SYS_STAT_H
	#define HAVE_SYS_STAT_H				1
#endif // HAVE_SYS_STAT_H

/* Define if you have the <sys/types.h> header file. */
#ifndef HAVE_SYS_TYPES_H
	#define HAVE_SYS_TYPES_H			1
#endif // HAVE_SYS_TYPES_H

/* Define if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Is __progname defined by system? */
#ifndef __progname
	#define __progname					PACKAGE		// "MJPeGtools"
	#define HAVE___PROGNAME
#endif

/* Name of package */
//#undef PACKAGE

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE	void
/* Use the same type for `jmp_buf' and `sigjmp_buf'.
   The `__mask_was_saved' flag determines whether
   or not `longjmp' will restore the signal mask.  */

/* The size of a `int', as computed by sizeof. */
#ifndef SIZEOF_INT
	#define SIZEOF_INT					sizeof(int)
#endif // SIZEOF_INT

/* The size of a `long', as computed by sizeof. */
#ifndef SIZEOF_LONG
	#define SIZEOF_LONG					sizeof(long)
#endif // SIZEOF_LONG

/* The size of a `long long', as computed by sizeof. */
#ifndef SIZEOF_LONG_LONG
	#define SIZEOF_LONG_LONG			sizeof(__int64)
#endif // SIZEOF_LONG_LONG

/* Define if you have the ANSI C header files. */
#ifndef STDC_HEADERS
	#define STDC_HEADERS				1
#endif // STDC_HEADERS

/* Version number of package */
//#undef VERSION

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

/* Define if the X Window System is missing or not being used. */
#ifndef X_DISPLAY_MISSING
	#define X_DISPLAY_MISSING			1
#endif // X_DISPLAY_MISSING

/* Define to empty if `const' does not conform to ANSI C. */
//#undef const

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
//#undef __inline inline

#ifndef USE_WINDOWS_MESSAGEBOX
	#define USE_WINDOWS_MESSAGEBOX
#endif // USE_WINDOWS_MESSAGEBOX

#endif // __CONFIG_H__