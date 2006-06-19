// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// tools.cpp:	part of the mjpgTools Library																			 //
//				Source File for specific commands																		 //
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
#include "config.h"
#include "tools.h"

#ifndef USE_WINDOWS_MESSAGEBOX
	#include <stdio.h>		// for 'vsprintf'
	#include <stdarg.h>		// for the use of va_list
#else
	#include <windows.h>
#endif

#include <string.h>

void infoMessage(const char* lpText, ...)
{
	char buf[512];
	va_list argList;

	va_start(argList, lpText);
	buf[0]=0;

#ifdef USE_WINDOWS_MESSAGEBOX
	// show message in a MessageBox
	wvsprintf(buf, lpText, argList);
	MessageBox(GetActiveWindow(), buf, "mjpgTools Library Error", MB_OK | MB_ICONEXCLAMATION);
#else
	vsprintf(buf, lpText, argList);
	// Send it to stderr, adding a newline
	fprintf(stderr, "%s\n", buf);
#endif

	va_end(argList);
}

void warnMessage2(const char* lpTitle, const char* lpText, ...)
{
	char buf[512];
	char sTitle[64];
	va_list argList;

	if ((lpTitle) && (lpTitle[0]))
	{
		strncpy(sTitle, lpTitle, 63);
		sTitle[63]=0;
	}
	else
		strcpy(sTitle, "mjpgTools Library Error");

	buf[0]=0;
	va_start(argList, lpText);

#ifdef USE_WINDOWS_MESSAGEBOX
	// show message in a MessageBox
	wvsprintf(buf, lpText, argList);
	MessageBox(GetActiveWindow(), buf, sTitle, MB_OK | MB_ICONEXCLAMATION);
#else
	vsprintf(buf, lpText, argList);
	// Send it to stderr, adding a newline
	fprintf(stderr, "%s : %s\n", sTitle, buf);
#endif

	va_end(argList);
}

void infoMessage2(const char* lpTitle, const char* lpText, ...)
{
	char buf[512];
	va_list argList;

	va_start(argList, lpText);
	buf[0]=0;

#ifdef USE_WINDOWS_MESSAGEBOX
	// show message in a MessageBox
	wvsprintf(buf, lpText, argList);
	MessageBox(GetActiveWindow(), buf, lpTitle, MB_OK | MB_ICONINFORMATION);
#else
	vsprintf(buf, lpText, argList);
	// Send it to stderr, adding a newline
	fprintf(stderr, "%s\n", buf);
#endif

	va_end(argList);
}

void errMessage2(const char* lpTitle, const char* lpText, ...)
{
	char buf[512];
	va_list argList;

	va_start(argList, lpText);
	buf[0]=0;

#ifdef USE_WINDOWS_MESSAGEBOX
	// show message in a MessageBox
	wvsprintf(buf, lpText, argList);
	MessageBox(GetActiveWindow(), buf, lpTitle, MB_OK | MB_ICONHAND);
#else
	vsprintf(buf, lpText, argList);
	// Send it to stderr, adding a newline
	fprintf(stderr, "%s\n", buf);
#endif

	va_end(argList);
}
