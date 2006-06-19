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
#include "global.h"
#include "types.h"

#ifdef _DEBUG
bool				DEBUG_TO_STDERR	= false;
#endif
const char*			mpg2enc_defaultTitle	= "mjpegTools Library";
const char*			mpg2enc_msgTitle		= NULL;
const char*			mpg2enc_defTitle		= NULL;
#ifndef _DEBUG
uint8_t				mpg2enc_verbose		= 0;
#else
uint8_t				mpg2enc_verbose		= 2;
#endif



//////////////////////////////////////////////////////////////////////
// Message Routines
//////////////////////////////////////////////////////////////////////
static int Message(uint32_t buttons, LPCTSTR lpFilter, va_list argList)
{
	char ttl[128];
	char buf[4096];
	const char* tAddon = _T(" - ");
	int result = 0;

	strcpy(ttl, mpg2enc_defaultTitle);
	if ((mpg2enc_defTitle) && (mpg2enc_defTitle[0]))
	{
		strcat(ttl, tAddon);
		strcat(ttl, mpg2enc_defTitle);
	}

	if ((mpg2enc_msgTitle) && (mpg2enc_msgTitle[0]))
	{
		strcat(ttl, tAddon);
		strcat(ttl, mpg2enc_msgTitle);
	}

	buf[0]=0;
	vsprintf(buf, lpFilter, argList);

	// show message in a MessageBox
#ifdef DEBUG_AND_INFO_2CONSOLE
	if (DEBUG_TO_STDERR == true)
		fprintf(stderr, "%s\n", buf);
	else
#endif
		result = MessageBox(GetActiveWindow(), buf, ttl, buttons);

	return result;
}

void err_message(const char* lpFilter, ...)
{
	va_list argList;
	va_start(argList, lpFilter);
	Message(MB_OK | MB_ICONHAND, lpFilter, argList);
	va_end(argList);
}
void warn_message(const char* lpFilter, ...)
{
	va_list argList;
	va_start(argList, lpFilter);
	Message(MB_OK | MB_ICONEXCLAMATION, lpFilter, argList);
	va_end(argList);
}
void info_message(const char* lpFilter, ...)
{
	va_list argList;

	if (mpg2enc_verbose == 0) return;

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
void debug_message(const char* lpFilter, ...)
{
	char ddefTitle[256];
	const char* old_defTitle;
	va_list argList;

	old_defTitle = mpg2enc_defTitle;

	if (mpg2enc_verbose <= 1) return;

	if ((mpg2enc_defTitle) && (mpg2enc_defTitle[0]))
	{
		sprintf(ddefTitle, _T("%s ++ DEBUG"), mpg2enc_defTitle);
		mpg2enc_defTitle = ddefTitle;
	}
	else
		mpg2enc_defTitle = _T("DEBUG");

	va_start(argList, lpFilter);

#ifdef DEBUG_AND_INFO_2CONSOLE
	DEBUG_TO_STDERR = true;
#endif

	Message(MB_OK | MB_ICONINFORMATION, lpFilter, argList);

#ifdef DEBUG_AND_INFO_2CONSOLE
	DEBUG_TO_STDERR = false;
#endif

	va_end(argList);

	mpg2enc_defTitle = old_defTitle;
}
int ask_message(uint32_t buttons, const char* lpFilter, ...)
{
	va_list argList;
	int result = 0;
	UINT _buttons = buttons;

	va_start(argList, lpFilter);

	if (_buttons == 0)
		_buttons = MB_YESNO | MB_ICONQUESTION;
	else if ( !(_buttons & MB_ICONQUESTION))
		_buttons |= MB_ICONQUESTION;

	result = Message(_buttons, lpFilter, argList);

	va_end(argList);

	return result;
}

void errorExit()
{
	RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
}
void errorExit_msg(const char* lpFilter, ...)
{
	va_list argList;
	va_start(argList, lpFilter);
	Message(MB_OK | MB_ICONHAND, lpFilter, argList);
	va_end(argList);

	RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
}
