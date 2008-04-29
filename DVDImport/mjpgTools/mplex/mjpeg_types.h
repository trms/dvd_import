// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// mjpeg_types.h:	part of the mjpgTools Library																		 //
//					$Id$														 //
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
//	Copyright (C) 2000 Herbert Valerio Riedel <hvr@gnu.org>																 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#ifndef __MJPEG_TYPES_H__
#define __MJPEG_TYPES_H__
#include "config.h"

#ifdef HAVE_STDINT_H
	#include <stdint.h>
#elif HAVE_INTTYPES_H
	#include <inttypes.h>
//#elif defined(__CYGWIN__)
#else
	#include <stddef.h>	
	#include <sys/types.h>
	typedef signed __int8				INT8;			// 8-bit signed integral type. 
	typedef unsigned __int8				UINT8;		// 8-bit unsigned integral type.

	typedef signed __int16				INT16;		// 16-bit signed integral type. 
	typedef unsigned __int16			UINT16;		// 16-bit unsigned integral type.

	#ifndef _BASETSD_H_
		// INT32 and UINT32 are already defined in <BaseTsd.h>
		typedef signed __int32				INT32;		// 32-bit signed integral type. 
		typedef unsigned __int32			UINT32;		// 32-bit unsigned integral type.
	#endif

	typedef signed __int64				INT64;		// 64-bit signed integral type. 
	typedef unsigned __int64			UINT64;		// 64-bit unsigned integral type.
	//typedef signed void*				intptr_t;		// Signed integral type large enough to hold any pointer.
	//typedef unsigned void*			uintptr_t;		// Unsigned integral type large enough to hold any pointer
	typedef INT32						ssize_t;

	#define INT8_C(c)					c
	#define INT16_C(c)					c
	#define INT32_C(c)					c
	#define INT64_C(c)					c ## LL
	#define UINT8_C(c)					c ## U
	#define UINT16_C(c)					c ## U
	#define UINT32_C(c)					c ## U
	#define UINT64_C(c)					c ## ULL
//#else
/* warning ISO/IEC 9899:1999 <stdint.h> was missing and even <inttypes.h> */
/* fixme */
#endif /* HAVE_STDINT_H */

#ifdef __FreeBSD__
	#include <sys/types.h> /* FreeBSD - ssize_t */
#endif

#if defined(HAVE_STDBOOL_H) && !defined(__cplusplus)
	#include <stdbool.h>
#else
	/* ISO/IEC 9899:1999 <stdbool.h> missing -- enabling workaround */

	#ifndef __cplusplus
		//typedef enum {
		//    false = 0,
		//    true = 1
		//  } locBool;

		//#  define false   false
		//#  define true    true
		//#  define bool locBool
	#endif
#endif

#ifndef PRId64
	#define PRId64						PRID64_STRING_FORMAT
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
	#define GNUC_PRINTF( format_idx, arg_idx )    \
	  __attribute__((format (printf, format_idx, arg_idx)))
	#define GNUC_SCANF( format_idx, arg_idx )     \
	  __attribute__((format (scanf, format_idx, arg_idx)))
	#define GNUC_FORMAT( arg_idx )                \
	  __attribute__((format_arg (arg_idx)))
	#define GNUC_NORETURN                         \
	  __attribute__((noreturn))
	#define GNUC_CONST                            \
	  __attribute__((const))
	#define GNUC_UNUSED                           \
	  __attribute__((unused))
	#define GNUC_PACKED                           \
	  __attribute__((packed))
#else   /* !__GNUC__ */
	#define GNUC_PRINTF( format_idx, arg_idx )
	#define GNUC_SCANF( format_idx, arg_idx )
	#define GNUC_FORMAT( arg_idx )
	#define GNUC_NORETURN
	#define GNUC_CONST
	#define GNUC_UNUSED
	#define GNUC_PACKED
#endif  /* !__GNUC__ */


#endif /* __MJPEG_TYPES_H__ */

/* 
 * Local variables:
 *  c-file-style: "gnu"
 *  tab-width: 8
 *  indent-tabs-mode: nil
 * End:
 */
