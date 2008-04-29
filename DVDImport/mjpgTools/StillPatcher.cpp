// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// StillPatcher.cpp: implementation of the CStillPatcher class, part of the mjpgTools Library							 //
//					 Verifies MPeG's and patches them to be suitable STILL's for VCDimager								 //
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
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>

#include ".\STILLpatcher\MPeG.h"

#define MJPGTOOLS_INTERNALS
#include "mjpgTools.h"				// for specific tool-functions and 'lp_jpeg2yuv_switches'
#include "privateMessages.h"		// for 'lk_...' strings
#include "StillPatcher.h"

class CStillPatcher : public CMPeG
{
public:
	CStillPatcher(lp_stillpatch_struct);
	virtual ~CStillPatcher();

	virtual void Init(void);			// reads data of source and analizes it -- raises an exception on error
	int Run(void);						// does all the work...

protected:
	void PatchIt(void);					// patches onto the buffer, undoSTILL==TRUE - undoes STILL patching->normal MPeG
	void WriteData(void);				// writes the (patched) data to the file -- raises an exception on error
										// (unless no patching was required and output == input)

private:
	lp_stillpatch_struct	lpSwitches;
	char					mDestination[MAX_PATH];
	BOOL					hasPatched;	// TRUE if the PatchIt function wend fine
	BOOL					undoSTILL;	// TRUE to undo the STILL-patching
	// tempory : set to TRUE when no patching is required but all others is okay :: set in Init
	BOOL					dontPatch_dontwrite;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStillPatcher::CStillPatcher(lp_stillpatch_struct pSwitches) : CMPeG()
{
	lpSwitches			= pSwitches;

	defTitle			= _T("mjpgTools -- STILL Patcher");
	mDestination[0]		= 0;
	hasPatched			= FALSE;
	dontPatch_dontwrite	= FALSE;
}

CStillPatcher::~CStillPatcher()
{
}

//////////////////////////////////////////////////////////////////////
// Public Routines
//////////////////////////////////////////////////////////////////////

void CStillPatcher::Init()
{
	char s[1024];

	if (lpSwitches == NULL)
	{ // normally shouldn't occure...
		wsprintf(s, LK_MESSAGE_SUFFIX, _T("No switches entered in call to this function."));
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	// set verbose-mode
	verbose = lpSwitches->verbose;

	// verify input/output files
	// check the basic file-settings

	if (lpSwitches->infile[0])
		mSource = lpSwitches->infile; // Set input file name.
	else
	{
		wsprintf(s, LK_MESSAGE_SUFFIX, _T("Required parameter \"InputFile\" not entered !"));
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}
	// See if the input really exists
	if (! FileExists(mSource))
	{
		wsprintf(s, lk_INPUT_NOT_FOUND, mSource);
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	// now call parent's INIT to do main initialisation...
	CMPeG::Init();

	// the parent's Init, raises an exception if it seems we're not dealing with an mpeg, so don't bother to test on that
#ifndef _DEBUG
	if (IsStill())
	{ // already a still, no need to continue
		wsprintf(s, _T("It seems the input file <%s>\nalready is a STILL.\n\nNo need to continue..."), mSource);
		msgHandler(s, log_info);
		dontPatch_dontwrite = TRUE;
		return;
	}
#endif
	
	// what should we do?
	undoSTILL = (lpSwitches->reverse == true);

	if (lpSwitches->outfile[0])
		strcpy(mDestination, lpSwitches->outfile); // Set output file name.
	else
	{ // assuming to overwrite the source file ?
		if (lpSwitches->overwrite == false)
		{
			wsprintf(s, _T("No OUTPUT file has been given.\nIs it okay to overwrite the input file?"));
			if (msgHandler(s, log_ask_yesno) == IDNO)
			{ // i gues not to overwrite...
				RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
				return;
			}
		}
		// else
		strcpy(mDestination, mSource);
	}

	// See if the output-path is correct (existing)
	if (! PathExists(mDestination))
	{
		char fdisk[_MAX_DRIVE], fdir[_MAX_DIR];
		_splitpath( mDestination, fdisk, fdir, NULL, NULL );
		wsprintf(s, lk_OUTPUT_PATH_DOES_NOT_EXIST, fdisk, fdir);
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}
	
	// Ask whether the output may be overwritten, if it already exists
	if ((stricmp(mDestination, mSource) != 0) && (FileExists(mDestination)))
	{
		// suppress message if 'lpSwitches->overwrite' was preset to TRUE
		if (lpSwitches->overwrite == false)
		{
			wsprintf(s, lk_FILE_EXISTS_MESSAGE, mDestination);
			if (msgHandler(s, log_ask_yesno) == IDNO)
			{ // i gues not to overwrite...
				RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
				return;
			}
		} // else we may overwrite...

		// delete the file, so this message isn't asked again
		if (! DeleteFile(mDestination))
		{
			wsprintf(s, lk_OVERWRITE_ERROR, mDestination);
			msgHandler(s, log_error);
			RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

			return;
		}
	}
	
	// all done..
}

int CStillPatcher::Run()
{
	int iError = 0;

	__try
	{
		Init(); // initialize our settings
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		iError = -1;
	}

	if (dontPatch_dontwrite)
		return iError;

	if ((iError == 0) && (((undoSTILL) && (IsStill())) || ((!undoSTILL) && (!IsStill()))))
		PatchIt(); // do the patching...

	// write the data now
	if (iError == 0)
	{
		__try
		{
			WriteData();
		}
		__except( GetExceptionCode() != 0 )
		{
			iError = -2;
		}
	}

	return iError;
}


//////////////////////////////////////////////////////////////////////
// Protected Routines
//////////////////////////////////////////////////////////////////////

void CStillPatcher::PatchIt()
{
	if (fContents == NULL) return;
	DWORD fPos, t=0, i;
	DWORD E0=mpeg_E0, E1=mpeg_E1, E2=mpeg_E2;

	// first patching : from 0xE1 to 0xE2
	fPos=0;
	while (fPos <= fSize-4) // not a still
	{
		memcpy(&t, &fContents[fPos++], 4);

		// find I-FRAME
		if (t == mpeg_IFRAME)
		{
			i = (mpegType == 0x21) ? fPos - 19 : (mpegType == 0x44) ? fPos - 23 : 0;
			memcpy(&t, &fContents[i], 4);

			if ((undoSTILL) && (t == mpeg_E1))
			{
				// replace the current LONG in the buffer with the patched value
				memcpy(&fContents[i], &E0, 4);
			}
			if ((!undoSTILL) && (t == mpeg_E1))
			{
				// replace the current LONG in the buffer with the patched value
				memcpy(&fContents[i], &E2, 4);
			}
		}
	}
	// now do the same for 0xE0 -> 0xE1
	fPos=0;
	while (fPos <= fSize-4) // not a still
	{
		memcpy(&t, &fContents[fPos++], 4);

		// find I-FRAME
		if (t == mpeg_IFRAME)
		{
			i = (mpegType == 0x21) ? fPos - 19 : (mpegType == 0x44) ? fPos - 23 : 0;
			memcpy(&t, &fContents[i], 4);

			if ((undoSTILL) && (t == E2))
			{
				// replace the current LONG in the buffer with the patched value
				memcpy(&fContents[i], &E1, 4);
			}
			if ((!undoSTILL) && (t == E0))
			{
				// replace the current LONG in the buffer with the patched value
				memcpy(&fContents[i], &E1, 4);
			}
		}
	}
	hasPatched = TRUE;
} // all done

void CStillPatcher::WriteData()
{
	if ((fContents == NULL) || (fSize == 0))
	{
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );
		return;
	}

	char s[1024];

	if ((!hasPatched) && (stricmp(mDestination, mSource) == 0))
	{
		wsprintf(s, _T("Nothing done.\nFile <%s>\n"), mSource);
		if ((undoSTILL) && (!IsStill()))
			strcat(s, _T("seems to be no Still, so unpatching\nis unnecessary."));
		else // if ((!undoSTILL) && (IsStill()))
			strcat(s, _T("already is a Still, so patching\nis unnecessary."));
		msgHandler(s, log_info);
		return; // no need
	}

	int f;
	// open the output for writing
	if ((f=_open(mDestination, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE)) == -1)
	{
		wsprintf(s, _T("A system-error occured while trying to open for writing\n<%s> :\n\n\"%s\""), mDestination, strerror(errno));
		msgHandler(s, log_error);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	long bytes_written;
	if ((bytes_written=_write(f, fContents, fSize)) != (long)fSize)
	{
		wsprintf(s, _T("A system-error occured while trying to write\nthe data to <%s> :\n\n\"%s\""), mDestination, strerror(errno));
		msgHandler(s, log_error);
		_close(f);
		// if (FileExists(mDestination)) DeleteFile(mDestination);
		RaiseException( USER_EXCEPTION_FAILURE, 0, 0, 0 );

		return;
	}

	_close(f);
	// all wend fine
	wsprintf(s, _T("File <%s>\nsuccessfully %spatched to\n<%s>"), mSource, (undoSTILL) ? _T("un") : _T(""), mDestination);
	msgHandler(s, log_info);
}


// ///////////////////////////////////////////////////////////////////
// The Thread that let "mpg2enc" work
// ///////////////////////////////////////////////////////////////////
void stillPatcher_Thread( void* arg )
{
	lp_stillpatch_struct lpSwitches = (arg) ? (lp_stillpatch_struct)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!lpSwitches)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	// we're gonna allocate the class onto the heap, possible our stack will be too small !!
	CStillPatcher* stillpatcherClass = new CStillPatcher(lpSwitches);
	if (stillpatcherClass  == NULL)
	{ // possible not enough RAM, exiting
		lpSwitches->m_ReturnCode = -9999;
		if (event) SetEvent(*event); // notifying caller-process we're done
		_endthread(); // shouldn't actualy occure...
		return;
	}

	int retVal = stillpatcherClass ->Run(); // if Run don't exceeds normaly, will have an error in MJPeG-lib
	// removing our class from the heap (hopely it works...)
	delete stillpatcherClass ; stillpatcherClass =NULL;
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}

// /////////////////////////////////////////////////////////////////////
// Extra Tool : determines the type of the MPEG (if it's a MPeG at all)
int GetMpegType(LPCTSTR stream)
{
	if ((stream==NULL) || (!stream[0]))
		return 0;
	if (!FileExists(stream)) return 0;

	CMPeG* Mpeg = new CMPeG(stream, FALSE);

	int i = 0;

	Mpeg->Init(); // initialize our settings
	// if an exception occures, it will be dealed with at the caller's routine

	// Are we dealing with a STILL ?
	if (Mpeg->IsStill())
		i = (Mpeg->IsStill2()) ? FILETYPE_STILL2 : FILETYPE_STILL1;
	else if (Mpeg->IsMpeg())
		i = (Mpeg->IsMpeg2()) ? FILETYPE_MPG2 : (Mpeg->IsMpeg1()) ? FILETYPE_MPG1 : FILETYPE_MPG;
	else
		i = FILETYPE_UNKNOWN;

	// closing down
	delete Mpeg;

	return i;
}
