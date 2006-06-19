// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// Mpg2enc.cpp: part of the mjpgTools Library																			 //
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
// This library is globaly based on :																					 //
//  - Copyright (C) 1996, MPEG Software Simulation Group. All Rights Reserved.											 //
//																														 //
//		Disclaimer of Warranty																							 //
//		These software programs are available to the user without any license fee or royalty on an "as is" basis.		 //
//		The MPEG Software Simulation Group disclaims any and all warranties, whether express, implied, or statuary,		 //
//		including any implied warranties or merchantability or of fitness for a particular purpose.						 //
//		In no event shall the copyright-holder be liable for any incidental, punitive, or consequential damages of any	 //
//		kind whatsoever arising from the use of these programs.															 //
//		This disclaimer of warranty extends to the user of these programs and user's customers, employees, agents,		 //
//		transferees, successors, and assigns.																			 //
//		The MPEG Software Simulation Group does not represent or warrant that the programs furnished hereunder are free	 //
//		of infringement of any third-party patents.																		 //
//		Commercial implementations of MPEG-1 and MPEG-2 video, including shareware, are subject to royalty fees to		 //
//		patent holders. Many of these patents are general enough such that they are unavoidable regardless of			 //
//		implementation design.																							 //
//																														 //
// and partialy based on the modifications made by :																	 //
//	- Modifications and enhancements (C) 2000/2001 Andrew Stevens (MJPeGtools of the Linux-group)						 //
//	  GNU General Public License																						 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys\types.h>
#include <string.h>


#include ".\mjpeglib\config.h"
#include ".\mjpeglib\mpegconsts.h"
#include ".\mpg2enc\types.h"
#include ".\mpg2enc\MpegEnc.h"
#include ".\mpg2enc\global.h"

#define MJPGTOOLS_INTERNALS
#include "mjpgTools.h"				// for specific tool-functions and 'lp_jpeg2yuv_switches'
#include "privateMessages.h"		// for 'lk_...' strings
//#include ".\tools\tools.h"

#include "Mpg2enc.h"
#include "./tools/tools.h"

// interface for the CMpg2enc class
// ////////////////////////////////
class CMpg2enc : public MpegEnc
{
public:
	CMpg2enc(lp_mpg2enc_struct, HWND _ohWnd=0, UINT _ouMsg=0);
	virtual ~CMpg2enc();

	void Init_MainApp(void); // virtual in parent
	
	int Run(void);
private:
	lp_mpg2enc_struct	lpSwitches;
};


// implementation of the CMpg2enc class
// ////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMpg2enc::CMpg2enc(lp_mpg2enc_struct pSwitches, HWND _ohWnd, UINT _ouMsg) : MpegEnc(_ohWnd, _ouMsg)
{
	mpg2enc_defTitle	= _T("mpg2enc");
	lpSwitches			= pSwitches;
}

CMpg2enc::~CMpg2enc()
{
}

//////////////////////////////////////////////////////////////////////
// Local Routines
//////////////////////////////////////////////////////////////////////

void CMpg2enc::Init_MainApp()
{
	mpg2enc_msgTitle = "CMpg2enc::Init_MainApp";

	if (lpSwitches == NULL)
		errorExit(); // shouldn't occure...

	LPCTSTR _internal_progname = mpg2enc_defTitle;
	if ((lpSwitches->frame_rate > -1) && (lpSwitches->aspect_ratio > -1))
	{
		// check the basic file-settings
		char fdisk[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME], fext[_MAX_EXT];

		if (lpSwitches->infile[0])
			strcpy(param_inFile, lpSwitches->infile); // Set output file name.
		else
			errorExit_msg(lk_NO_INPUT_MESSAGE, "Param ERROR***");

		// See if the input really exists
		if (! FileExists(param_inFile))
			errorExit_msg(lk_INPUT_NOT_FOUND, param_inFile);

		if (lpSwitches->outfile[0])
			strcpy(outfilename, lpSwitches->outfile); // Set output file name.
		else
		{ // create a new filename ourself
			// Message(lk_NO_OUTPUT_MESSAGE, _internal_progname);
			// return 21;
			_splitpath(lpSwitches->infile, fdisk, fdir, fname, fext);
			if ((!fext[0]) || ((strnicmp(fext, _T(".m1v"), 4) != 0) && (strnicmp(fext, _T(".m2v"), 4) != 0) &&
				(strnicmp(fext, _T(".mpg"), 4) != 0)))
				wsprintf(outfilename, _T("%s%s%s.%s"), fdisk, fdir, fname, ((lpSwitches->format == 0) || (lpSwitches->format == 1) ||
				(lpSwitches->format == 2) || (lpSwitches->format == 6)) ? _T("m1v") : _T("m2v"));
		}

		// See if the output-path is correct (existing)
		if (! PathExists(outfilename))
		{
			_splitpath( outfilename, fdisk, fdir, NULL, NULL );
			errorExit_msg(lk_OUTPUT_PATH_DOES_NOT_EXIST, fdisk, fdir);
		}
		// Ask whether the output may be overwritten, if it already exists
		if (FileExists(outfilename))
		{
			// suppress message if 'lpSwitches->overwrite' was preset to TRUE
			if (lpSwitches->overwrite == false)
				if (ask_message(MB_YESNO | MB_ICONQUESTION, lk_FILE_EXISTS_MESSAGE, outfilename) == IDNO)
					errorExit();

			// delete the file, so this message isn't asked when walking for the 2nd pass
			if (! DeleteFile(outfilename))
				errorExit_msg(lk_OVERWRITE_ERROR, outfilename);
		}
	}
	// fill settings with user-made param-struct
	if (lpSwitches->v_bitrate > 0)
		param_bitrate				= lpSwitches->v_bitrate*1000;

	if ((lpSwitches->vcd_ssize > 0) &&
		( ((lpSwitches->vcd_ssize >= 20*1024) && (lpSwitches->vcd_ssize <= 500*1024)) ||
		((lpSwitches->vcd_ssize >= 20) && (lpSwitches->vcd_ssize <= 500)) )) 
	{
		param_still_size			= ((lpSwitches->vcd_ssize>=20) && (lpSwitches->vcd_ssize<=500)) ? lpSwitches->vcd_ssize*1024 : lpSwitches->vcd_ssize;
	}

	if (lpSwitches->nv_bitrate > 0)
		param_nonvid_bitrate		= lpSwitches->nv_bitrate;
	
	if ((lpSwitches->intra_dc_prec-8 >=0) && (lpSwitches->intra_dc_prec-8 <= 2))
		param_mpeg2_dc_prec		= lpSwitches->intra_dc_prec-8;

	if (lpSwitches->correct_svcd_hds == true)
		param_hack_svcd_hds_bug	= 0;
	
	if((lpSwitches->quantisation>=1) && (lpSwitches->quantisation<=32))
		param_quant				= lpSwitches->quantisation;

	if (lpSwitches->aspect_ratio == -1)
	{
		DisplayAspectRatios();
		errorExit();
	}
	else if (lpSwitches->aspect_ratio > 0)
		param_aspect_ratio			= lpSwitches->aspect_ratio;
//	else
//		param_aspect_ratio			= 2; // 4:3

	if (lpSwitches->frame_rate == -1)
	{
		DisplayFrameRates();
		errorExit();
	}
	else if (lpSwitches->frame_rate > 0)
		param_frame_rate			= lpSwitches->frame_rate;

	if ((lpSwitches->interlace >= 1) && (lpSwitches->interlace <= 3))
		param_fieldenc				= lpSwitches->interlace-1;

	if ((lpSwitches->motionsearch >= 1) && (lpSwitches->motionsearch <= 33))
		param_searchrad				= lpSwitches->motionsearch-1;

	param_num_cpus					= 0;

	if ((lpSwitches->reduction4 >= 1) && (lpSwitches->reduction4 <= 4))
		param_44_red				= lpSwitches->reduction4;
	else
		param_44_red				= 1; // max quality

	if ((lpSwitches->reduction2 >= 1) && (lpSwitches->reduction2 <= 4))
		param_22_red				= lpSwitches->reduction2;
	else
		param_22_red				= 1; // max quality

	if ((lpSwitches->verbose >= 1) && (lpSwitches->verbose <= 3))
		mpg2enc_verbose				= lpSwitches->verbose-1;

	if ((lpSwitches->vbv >= 20) && (lpSwitches->vbv <= 4000))
		param_video_buffer_size		= lpSwitches->vbv;

	if (lpSwitches->seq_length > 1)
		param_seq_length_limit		= lpSwitches->seq_length;

	if (lpSwitches->pulldown3_2 == true)
		param_32_pulldown			= 1;

	if (lpSwitches->play_fieldorder == 1)
		param_force_interlacing		= Y4M_ILACE_TOP_FIRST;
	else if (lpSwitches->play_fieldorder == 2)
		param_force_interlacing		= Y4M_ILACE_BOTTOM_FIRST;

	if ((lpSwitches->format >= MPEG_FORMAT_FIRST) && (lpSwitches->format <= MPEG_FORMAT_LAST))
		param_format				= lpSwitches->format;
		
	//if ((lpSwitches->norm >= 0) && (lpSwitches->norm <= 2))
		param_norm					= lpSwitches->norm;

	if (lpSwitches->min_gop > 0)
		param_min_GOP_size			= lpSwitches->min_gop;

	if (lpSwitches->max_gop > 0)
		param_max_GOP_size			= lpSwitches->max_gop;

	if (lpSwitches->force_bbp == true)
		param_preserve_B			= 1;

	// if = 1 : GOP looks like this : IPPPPP..., if = 2 GOP=IBPBPB..., if = 3 GOP=IBBPBBPBB... etc.
	if (lpSwitches->bgrp_size <= 0)
		param_Bgrp_size				= 3;
	else
		param_Bgrp_size				= lpSwitches->bgrp_size;

	if (!param_Bgrp_size) param_Bgrp_size = 1;

	if (lpSwitches->reduce_hf == true)
		param_hf_quant				= 1;

	if (lpSwitches->keep_hf == true)
		param_hf_quant				= 2;

	if (lpSwitches->seqhead_egop == true)
		param_seq_hdr_every_gop	= 1;

	if (lpSwitches->no_dummy == true)
		param_svcd_scan_data		= 0;

	if ((lpSwitches->quant_reduc >= -4.0) && (lpSwitches->quant_reduc <= 10.0))
		param_act_boost				= lpSwitches->quant_reduc;

	mjpeg_default_handler_verbosity(mpg2enc_verbose);
}

//////////////////////////////////////////////////////////////////////
// Global Routines
//////////////////////////////////////////////////////////////////////

int CMpg2enc::Run()
{ // do the real job
	int iError = 0;
	__try
	{
		Run_Main(); // the REAL job
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		iError = -1;
	}

	if (iError != 0)
	{ // delete the output-stream
		if ((outfilename[0]) && (outfile != NULL))
		{
			fclose(outfile); outfile=NULL;
			DeleteFile(outfilename);
		}
	}
	//else // all oke

	// files are close in parent's destructor, or -when al wend fine- at the end of 'Run_Main'
	return iError;
}

// ///////////////////////////////////////////////////////////////////
// The Thread that let "mpg2enc" work
// ///////////////////////////////////////////////////////////////////
void mpg2enc_Thread( void* arg )
{
	lp_mpg2enc_struct lpSwitches = (arg) ? (lp_mpg2enc_struct)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!lpSwitches)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	// we're gonna allocate the class onto the heap, possible our stack will be too small !!
	CMpg2enc* mpg2encClass;
	if (lpSwitches->version >= 1)
		mpg2encClass = new CMpg2enc(lpSwitches, lpSwitches->m_hWnd, lpSwitches->m_uMsg);
	else
		mpg2encClass = new CMpg2enc(lpSwitches);
	if (mpg2encClass == NULL)
	{ // possible not enough RAM, exiting
		lpSwitches->m_ReturnCode = -9999;
		if (event) SetEvent(*event); // notifying caller-process we're done
		_endthread(); // shouldn't actualy occure...
		return;
	}

	if ((lpSwitches->version >= 1) && (lpSwitches->m_hWnd) && (lpSwitches->m_uMsg))
	{
		// initializing
		lhkProgress_Init((const void*)lpSwitches->m_hWnd, lpSwitches->m_uMsg, "Encoding MPeG", (const long)lpSwitches->m_framecount);
	}

	int retVal = mpg2encClass->Run(); // if Run don't exceeds normaly, will have an error in MJPeG-lib
	// removing our class from the heap (hopely it works...)
	delete mpg2encClass; mpg2encClass=NULL;

	if ((lpSwitches->version >= 1) && (lpSwitches->m_hWnd) && (lpSwitches->m_uMsg))
	{
		// closing
		lhkProgress_End((const void*)lpSwitches->m_hWnd, lpSwitches->m_uMsg);
	}

	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}