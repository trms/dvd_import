// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// tools.cpp:	part of the mjpgTools Library																			 //
//				Source File for specific commands.																		 //
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

//#include "..\jpeglib\jinclude.h"
#include <windows.h>
#include "Tools.h"

#define USE_WINDOWS_MESSAGEBOX

#ifndef USE_WINDOWS_MESSAGEBOX
	#include <stdio.h>		// for 'vsprintf'
	#include <stdarg.h>		// for the use of va_list
#endif

void _Message(const char* lpText, va_list argList, UINT icon=(MB_OK | MB_ICONEXCLAMATION))
{
	char buf[512]; buf[0]=0;
#ifdef USE_WINDOWS_MESSAGEBOX
	// show message in a MessageBox
	wvsprintf(buf, lpText, argList);
	MessageBox(GetActiveWindow(), buf, "mjpgTools Library", icon);
#else
	vsprintf(buf, lpText, argList);
	// Send it to stderr, adding a newline
	fprintf(stderr, "%s\n", buf);
#endif
}

void _Message2(const char* lpTitle, const char* lpText, va_list argList, UINT icon=(MB_OK | MB_ICONEXCLAMATION))
{
	char buf[512]; buf[0]=0;
	char sTitle[64];
	if ((lpTitle) && (lpTitle[0]))
	{
		strncpy(sTitle, lpTitle, 63);
		sTitle[63]=0;
	}
	else
		strcpy(sTitle, "mjpgTools Library");

#ifdef USE_WINDOWS_MESSAGEBOX
	// show message in a MessageBox
	wvsprintf(buf, lpText, argList);
	MessageBox(GetActiveWindow(), buf, sTitle, icon);
#else
	vsprintf(buf, lpText, argList);
	// Send it to stderr, adding a newline
	fprintf(stderr, "%s : %s\n", sTitle, buf);
#endif
}


void Message(const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message(lpText, argList);

	va_end(argList);
}

void infoMessage(const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message(lpText, argList, MB_OK | MB_ICONINFORMATION);

	va_end(argList);
}

void warnMessage(const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message(lpText, argList);

	va_end(argList);
}

void errMessage(const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message(lpText, argList, MB_OK | MB_ICONERROR);

	va_end(argList);
}


void Message2(const char* lpTitle, const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message2(lpTitle, lpText, argList);

	va_end(argList);
}

void infoMessage2(const char* lpTitle, const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message2(lpTitle, lpText, argList, MB_OK | MB_ICONINFORMATION);

	va_end(argList);
}

void warnMessage2(const char* lpTitle, const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message2(lpTitle, lpText, argList);

	va_end(argList);
}

void errMessage2(const char* lpTitle, const char* lpText, ...)
{
	va_list argList;
	va_start(argList, lpText);

	_Message2(lpTitle, lpText, argList, MB_OK | MB_ICONERROR);

	va_end(argList);
}



BOOL AskMessage(LPCTSTR lpMessage, LPCTSTR lpTitle)
{
	return (MessageBox(GetActiveWindow(), lpMessage, lpTitle, MB_YESNO | MB_ICONQUESTION) == IDYES);
}


// added since 1.5.0.0 -- progress-stuff

static void sendProgressInfo(const void* owner, const unsigned msg, lhkProgress_p p)
{
	if ((!owner) || (!msg) || (!p))
		return;

	SendMessage((HWND)owner, msg, 0, (LPARAM)p);
}

void lhkProgress_Init(const void* owner, const unsigned msg, const char* initText, const long initCount)
{
	lhkProgress_t prgData;
	ZeroMemory(&prgData, sizeof(lhkProgress_t));

	prgData.version		= 1;
	prgData.type		= init;
	prgData.initText	= initText;
	prgData.initCount	= initCount;

	sendProgressInfo(owner, msg, &prgData);
}

void lhkProgress_ReInit(const void* owner, const unsigned msg, const char* initText, const long initCount)
{
	lhkProgress_t prgData;
	ZeroMemory(&prgData, sizeof(lhkProgress_t));

	prgData.version		= 1;
	prgData.type		= reinit;
	prgData.initText	= initText;
	prgData.initCount	= initCount;

	sendProgressInfo(owner, msg, &prgData);
}

void lhkProgress_Step(const void* owner, const unsigned msg, const char* stepText, const long stepPos)
{
	lhkProgress_t prgData;
	ZeroMemory(&prgData, sizeof(lhkProgress_t));

	prgData.version		= 1;
	prgData.type		= step;
	prgData.stepText	= stepText;
	prgData.stepPos		= stepPos;

	sendProgressInfo(owner, msg, &prgData);
}

void lhkProgress_End(const void* owner, const unsigned msg)
{
	lhkProgress_t prgData;
	ZeroMemory(&prgData, sizeof(lhkProgress_t));

	prgData.version		= 1;
	prgData.type		= finish;

	sendProgressInfo(owner, msg, &prgData);
}
