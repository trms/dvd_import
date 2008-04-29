// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// MPeG.cpp:	implementation of the CMPeG class, part of the mjpgTools Library										 //
//				Verifies a MPeG's																						 //
//																														 //
//				Although the project-name indicates so, this class doesn't modify the source, it just verifies.			 //
//				The actual work is done in the inherited class in the main project 'mjpgTool'							 //
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

#include "stdafx.h"
#include "MPeG.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMPeG::CMPeG()
{
	// initialize data
	defTitle		= _T("mjpgTools -- MPeg Analizer");
	mpegType		= 0;
	isStill			= FALSE;
	amountI			= -1;
	fContents		= NULL;
	verbose			= TRUE;

	mSource			= NULL;
	fSize			= 0;
}

CMPeG::CMPeG(LPCTSTR pSource, BOOL _verbose)
{
	// initialize data
	defTitle		= _T("mjpgTools -- MPeg Analizer");
	mpegType		= 0;
	isStill			= FALSE;
	amountI			= -1;
	fContents		= NULL;
	verbose			= _verbose;

	mSource			= pSource;
	fSize			= 0;
}

CMPeG::~CMPeG()
{
	if (fContents != NULL) { delete fContents; fContents=NULL; }
}

//////////////////////////////////////////////////////////////////////
// Public Routines
//////////////////////////////////////////////////////////////////////

void CMPeG::Init()
{
	char s[1024], ss[1024];
	if ((mSource==NULL) || (!mSource[0]))
	{
		wsprintf(s, LK_MESSAGE_SUFFIX, _T("No SOURCE filename given."));
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	int f;
	// open the file
	if ((f=_open(mSource, _O_RDONLY | _O_RANDOM | _O_BINARY)) == -1)
	{
		wsprintf(s, _T("A system-error occured while trying to open\n<%s> :\n\n\"%s\""), mSource, strerror(errno));
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	long len;
	// try to get the filelength
	if ((len=_lseek(f, 0, SEEK_END)) == -1)
	{
		wsprintf(s, _T("A system-error occured while trying to get\nthe filesize of <%s> :\n\n\"%s\""), mSource, strerror(errno));
		msgHandler(s, log_error);
		_close(f);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	if (len == 0)
	{
		wsprintf(ss, LK_MESSAGE_SUFFIX, _T("It seems the source file <%s> is empty!"));
		wsprintf(s, ss, mSource);
		msgHandler(s, log_error);
		_close(f);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}
	else
		fSize = (UINT32)len;

	_lseek(f, 0, SEEK_SET); // go back to beginning of file

	int bytes_read;
	// now initialize our buffer and fill it with the file contents
	if (fContents != NULL) delete fContents;
	fContents = new UINT8[len]; // no need to set all entry's to zero's, while we're gonna read the data immediately
	if ((bytes_read=_read(f, fContents, len)) != len)
	{
		if (bytes_read != -1)
		{
			wsprintf(ss, LK_MESSAGE_SUFFIX, _T("Not all data from the source file could be read.\nRead bytes : %ld, filesize : %ld."));
			wsprintf(s, ss, bytes_read, len);
		}
		else
			wsprintf(s, _T("A system-error occured while trying to read %ld bytes\nfrom <%s> :\n\n\"%s\""), len, mSource, strerror(errno));
		msgHandler(s, log_error);
		delete fContents; fContents=NULL;
		_close(f);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	// no more need of the file, closing it
	_close(f); f=-1;
	// continue the initialisation by analizing the header
	Analize();
}

BOOL CMPeG::IsStill()
{
	return isStill;
}
BOOL CMPeG::IsStill1()
{
	return ((isStill) && (IsMpeg1()) && (amountI == 2));
}
BOOL CMPeG::IsStill2()
{
	return ((isStill) && (IsMpeg2()) && (amountI == 1));
}
BOOL CMPeG::IsMpeg()
{
	return (mpegType != 0x00);
}
BOOL CMPeG::IsMpeg1()
{
	return (mpegType == LK_MPEG_1); // 0x21
}
BOOL CMPeG::IsMpeg2()
{
	return (mpegType == LK_MPEG_2); // 0x44
}

//////////////////////////////////////////////////////////////////////
// Protected Routines
//////////////////////////////////////////////////////////////////////

int CMPeG::msgHandler(LPCTSTR msg, lk_msg_enums msg_type)
{
	if ((msg_type != log_ask_yesno) && (!verbose))
		return 0; // quiet

	int retVal = 0;
	if ((msg) && (msg[0]))
	{
		if (msg_type == log_ask_yesno)
			retVal = MessageBox(GetActiveWindow(), msg, defTitle, MB_YESNO | MB_ICONQUESTION);
		else
			MessageBox(GetActiveWindow(), msg, defTitle, MB_OK | 
				(msg_type==log_warning) ? MB_ICONEXCLAMATION : (msg_type==log_error) ? MB_ICONHAND : MB_ICONINFORMATION);
	}
	return retVal;
}

void CMPeG::Analize()
{
	if (fContents == NULL) return;

	char s[1024], ss[1024];

	// verify the type of the MPeG
	if ((mpegType = fContents[4]) == 0x00)
	{
		wsprintf(ss, LK_MESSAGE_SUFFIX, _T("The specified file <%s>\nis NOT a MPeG !"));
		wsprintf(s, ss, mSource);
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}
	else
	{
		wsprintf(s, _T("Processing a%s MPeG%s"), (IsMpeg2() || IsMpeg1()) ? _T("") : __T("n unspecified"), (IsMpeg2()) ? _T("-2") : (IsMpeg1()) ? _T("-1") : __T(""));
		msgHandler(s, log_info);
	}

	unsigned long start_code; // only at this value will be verified.
		// Information on MPeG is too outdated or too complex to handle by me in this GUI.
	memcpy(&start_code, fContents, 4); // we only want to have the first 4 bytes

	if ((start_code & 0xB0010000) == 0xB0010000)
		isStill = HasBeenPatched();
	else
	{
		wsprintf(ss, LK_MESSAGE_SUFFIX, _T("The specified file <%s>\nis NOT a MPeG or an INVALID MPeG !"));
		wsprintf(s, ss, mSource);
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}
}

BOOL CMPeG::HasBeenPatched()
{
	if (fContents == NULL) return FALSE;
	DWORD fPos=0, t1=0, j, t2;
	BOOL result=FALSE;

	fPos=0;
	amountI=0;
	while ((amountI<4) && (fPos <= fSize-4)) // amountI>2 : not a still
	{
		memcpy(&t1, &fContents[fPos++], 4);

		if (t1 == mpeg_IFRAME)
		{
			amountI++;
			t2=0;
			j = (mpegType == 0x21) ? fPos - 19 : (mpegType == 0x44) ? fPos - 23 : 0;

			memcpy(&t2, &fContents[j], 4);

			if (mpegType == 0x21)
			{
				switch (amountI)
				{
					case 1 :
					{
						result = (t2 == mpeg_E1);
						break;
					}
					case 2 :
					{
						if (result) result = (t2 == mpeg_E2);
						break;
					}
					default :
					{
						result = FALSE;
						break;
					}
				}
			}
			else if (mpegType == 0x44)
			{
				switch (amountI)
				{
					case 1 :
					{
						result = (t2 == mpeg_E1);
						break;
					}
					default :
					{
						result = FALSE;
						break;
					}
				}
			}
		}
		if (amountI>3) break;
	}

	result = ( ((mpegType == 0x21) && (amountI != 2)) || ((mpegType == 0x44) && (amountI != 1)) ) ? FALSE : result;
	return result;
}
