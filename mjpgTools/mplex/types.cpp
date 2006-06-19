// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// types.cpp : part of the mjpgTools Library																			 //
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
#include <windows.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include "types.h"

#ifdef _DEBUG
bool				DEBUG_TO_STDERR	= false;
#endif
const char*			defaultTitle	= "mjpegTools Library";
const char*			msgTitle		= NULL;
const char*			defTitle		= NULL;
#ifndef _DEBUG
uint8_t				verbose		= 0;
#else
uint8_t				verbose		= 2;
#endif



//////////////////////////////////////////////////////////////////////
// Message Routines
//////////////////////////////////////////////////////////////////////
static int Message(uint32_t buttons, LPCTSTR lpFilter, va_list argList)
{
	char ttl[128];
	LPCTSTR tAddon = _T(" - ");

	strcpy(ttl, defaultTitle);
	if ((defTitle) && (defTitle[0]))
	{
		strcat(ttl, tAddon);
		strcat(ttl, defTitle);
	}

	if ((msgTitle) && (msgTitle[0]))
	{
		strcat(ttl, tAddon);
		strcat(ttl, msgTitle);
	}

	TCHAR buf[512]; buf[0]=0;
	wvsprintf(buf, lpFilter, argList);

	// show message in a MessageBox
	int result = 0;
#ifdef DEBUG_AND_INFO_2CONSOLE
	if (DEBUG_TO_STDERR == true)
		fprintf(stderr, "%s\n", buf);
	else
#endif
		// TODO: report messages
		//result = MessageBox(GetActiveWindow(), buf, ttl, buttons);

	return result;
}

void err_Message(const char* lpFilter, ...)
{
	va_list argList;
	va_start(argList, lpFilter);
	Message(MB_OK | MB_ICONHAND, lpFilter, argList);
	va_end(argList);
}
void warn_Message(const char* lpFilter, ...)
{
	va_list argList;
	va_start(argList, lpFilter);
	Message(MB_OK | MB_ICONEXCLAMATION, lpFilter, argList);
	va_end(argList);
}
void info_Message(const char* lpFilter, ...)
{
	if (verbose == 0) return;

	va_list argList;
	va_start(argList, lpFilter);

#ifdef DEBUG_AND_INFO_2CONSOLE
	DEBUG_TO_STDERR = true;
#endif

	Message(MB_OK | MB_ICONINFORMATION, lpFilter, argList);

#ifdef DEBUG_AND_INFO_2CONSOLE
	DEBUG_TO_STDERR = false;
#endif

	va_end(argList);
}
void debug_Message(const char* lpFilter, ...)
{
	if (verbose <= 1) return;

	char ddefTitle[64];
	LPCTSTR old_defTitle = defTitle;
	if ((defTitle) && (defTitle[0]))
	{
		wsprintf(ddefTitle, _T("%s ++ DEBUG"), defTitle);
		defTitle = ddefTitle;
	}
	else
		defTitle = _T("DEBUG");

	va_list argList;
	va_start(argList, lpFilter);

#ifdef DEBUG_AND_INFO_2CONSOLE
	DEBUG_TO_STDERR = true;
#endif

	Message(MB_OK | MB_ICONINFORMATION, lpFilter, argList);

#ifdef DEBUG_AND_INFO_2CONSOLE
	DEBUG_TO_STDERR = false;
#endif

	va_end(argList);

	defTitle = old_defTitle;
}
int ask_Message(uint32_t buttons, const char* lpFilter, ...)
{
	va_list argList;
	va_start(argList, lpFilter);
	UINT _buttons = buttons;
	if (_buttons == 0)
		_buttons = MB_YESNO | MB_ICONQUESTION;
	else if ( !(_buttons & MB_ICONQUESTION))
		_buttons |= MB_ICONQUESTION;
	int result = Message(_buttons, lpFilter, argList);
	va_end(argList);

	return result;
}

void error_exit()
{
	RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
}
void error_exit_msg(const char* lpFilter, ...)
{
	va_list argList;
	va_start(argList, lpFilter);
	Message(MB_OK | MB_ICONHAND, lpFilter, argList);
	va_end(argList);

	RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
}
