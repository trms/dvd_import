// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// MPeG.h:	interface for the CMPeG class, part of the mjpgTools Library												 //
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
#if !defined(AFX_MPEG_H__9E99392B_9955_11D6_811F_0050BF61B407__INCLUDED_)
#define AFX_MPEG_H__9E99392B_9955_11D6_811F_0050BF61B407__INCLUDED_

typedef unsigned __int8		UINT8;

#define bit_0				1	// 1
#define bit_1				2	// 2
#define bit_2				4	// 3
#define bit_3				8	// 4
#define bit_4				16	// 5
#define bit_5				32	// 6
#define bit_6				64	// 7
#define bit_7				128	// 8

//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code -- should be set at 11 at all time (to let the exception-handler work properly)
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//          00 = EXIT_SUCCESS
//          01 = EXIT_FAILURE
//          10 = EXIT_WARNING
//
//  In this version of the lib, Exceptions will be raised at abnormal termination instead of using
//  the 'exit()' and '_endthread()' functions
//
#define USER_EXCEPTION_SUCCESS				0xE0000000
#define USER_EXCEPTION_FAILURE				0xE0000001
#define USER_EXCEPTION_WARNING				0xE0000002

#define LK_MESSAGE_SUFFIX					"%s\n\nUnable to continue"

#define LK_MPEG_1							0x21
#define LK_MPEG_2							0x44

#define mpeg_E2								0xE2010000
#define mpeg_E1								0xE1010000
#define mpeg_E0								0xE0010000
#define mpeg_IFRAME							0xB3010000
#define mpeg_PFRAME							0x00010000

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef enum _LK_MSGTYPES_ {
	log_info,
	log_warning,
	log_error,
	log_ask_yesno
} lk_msg_enums;

class CMPeG  
{
public:
	CMPeG();
	CMPeG(LPCTSTR pSource, BOOL _verbose=TRUE);
	virtual ~CMPeG();

	BOOL IsStill(void);		// TRUE if a STILL, no matter wether it's a MPeG-1 or -2
	BOOL IsStill1(void);	// TRUE if MPeG-1 and STILL
	BOOL IsStill2(void);	// TRUE if MPeG-2 and STILL
	BOOL IsMpeg(void);		// TRUE if a MPeG, no matter wether it's a STILL or not
	BOOL IsMpeg1(void);		// TRUE if MPeG-1 wheter still or not
	BOOL IsMpeg2(void);		// TRUE if MPeG-2 wheter still or not

	// DO NOT CALL FROM Constructor !!
	virtual void Init(void); // reads data of source and analizes it -- raises an exception on error

protected:
	UINT8	mpegType;	// 0x21=MPeG-1 / 0x44=MPeG-2 / 0x?? = unspecified (0 = NOT a MPeG)
	BOOL	isStill;
	int		amountI;	// amount of I-frames
	UINT8*	fContents;	// binary contents of the file
	BOOL	verbose;	// TRUE : show all info/warning and error messages, FALSE : show nothing

	// local data
	UINT32	fSize;		// the size of the source file
	LPCTSTR	defTitle;	// title for any popup-box

	// next two functions will be called from within the constructor
	void Analize(void); // returns TRUE if starting unsigned long value == 0x000001Bx
	BOOL HasBeenPatched(void); // returns TRUE if the file has been patched for STILL

	int msgHandler(LPCTSTR msg, lk_msg_enums msg_type=log_warning);

protected:
	LPCTSTR	mSource;	// name of input-file
};

#endif // !defined(AFX_MPEG_H__9E99392B_9955_11D6_811F_0050BF61B407__INCLUDED_)
