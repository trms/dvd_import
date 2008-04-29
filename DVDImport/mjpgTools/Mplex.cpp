// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// Mplex.cpp:	implementation of the CMplex class, part of the mjpgTools Library										 //
//				Multi-Plexer of MPeG-Videostreams, MPeG-Audiostreams and AC3-Audiostreams into 1 MPeG (MPeG-1/-2)		 //
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
// This routine is based on :																							 //
//   mplex - MPEG1/SYSTEMS multiplexer - Copyright (C) 1994 1995 Christoph Moar	<moar@informatik.tu-muenchen.de>		 //
//	 Additional portions and modifications - Copyright (C) 2000 Andrew Stevens	<as@comlab.ox.ac.uk>					 //
//																														 //
//	 Research at Siemens ZFE ST SN 11 / T SN 6																			 //
//	 TECHNICAL UNIVERSITY OF MUNICH, GERMANY																			 //
//																														 //
//	 This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	 GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at	 //
//	 your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>

#include ".\mplex\bits.h"
#include ".\mplex\interact.h"
#include ".\mplex\videostrm.h"
#include ".\mplex\stillsstream.h"
#include ".\mplex\audiostrm.h"
//#include ".\mplex\mplexconsts.h"
#include ".\mplex\outputstream.h"

/*
#include ".\mplex\IBitStream.h"
#include ".\mplex\InputStream.h"
#include ".\mplex\MuxStream.h"
#include ".\mplex\ElementaryStream.h"
#include ".\mplex\OutputStream.h"
#include ".\mplex\VideoStream.h"
#include ".\mplex\stillsstreams.h"
#include ".\mplex\AudioStream.h"
#include ".\mplex\interact.h"
*/

#define MJPGTOOLS_INTERNALS
#include "mjpgTools.h"				// for specific tool-functions and 'lp_jpeg2yuv_switches'
#include "privateMessages.h"		// for 'lk_...' strings
#include "Mplex.h"

class CMplex
{
public:
	CMplex();
	virtual ~CMplex();

	int Run( lp_mplex_struct lpSwitches );

private:
	int Initialize( lp_mplex_struct lpSwitches );
	void Process(void); // raises an exception on error !!

	char**		inFiles;
	char		outFile[MAX_PATH];
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMplex::CMplex()
{
	defTitle	= _T("mplex");

	inFiles		= NULL;
	outFile[0]	= 0;
}

CMplex::~CMplex()
{
}

//////////////////////////////////////////////////////////////////////
// private routines
//////////////////////////////////////////////////////////////////////

int CMplex::Initialize( lp_mplex_struct lpSwitches )
{
	if (lpSwitches == NULL) return -1;

	msgTitle = _T("CMplex::Initialize");

	// check the basic file-settings
	char fdisk[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME], fext[_MAX_EXT];

	if (lpSwitches->infiles == NULL)
	{
		err_Message(lk_NO_INPUT_MESSAGE, _T("**ERROR**"));
		return 21;
	}
	inFiles = (char**)lpSwitches->infiles;
	
	// See if the input really exists
	for (int i=0; inFiles[i] != NULL; i++)
	{
		if (! FileExists(inFiles[i]))
		{
			err_Message(lk_INPUT_NOT_FOUND, inFiles[i]);
			return 22;
		}
	}

	if (lpSwitches->outfile[0])
		strcpy(outFile, lpSwitches->outfile); // Set output file name.
	else
	{ // create a new filename ourself
		err_Message(lk_NO_OUTPUT_MESSAGE, _T("**ERROR**"));
		return 21;
	}

	_splitpath(outFile, fdisk, fdir, fname, fext);
	if ((!fext[0]) || ((stricmp(fext, _T(".mpg")) != 0) && (stricmp(fext, _T(".mpeg")) != 0)))
		wsprintf(outFile, _T("%s%s%s.%s"), fdisk, fdir, fname, _T("mpg"));

	// See if the output-path is correct (existing)
	if (! PathExists(outFile))
	{
		_splitpath( outFile, fdisk, fdir, NULL, NULL );
		err_Message(lk_OUTPUT_PATH_DOES_NOT_EXIST, fdisk, fdir);
		return 23;
	}
	// Ask whether the output may be overwritten, if it already exists
	if (FileExists(outFile))
	{
		// suppress message if 'lpSwitches->overwrite' was preset to TRUE
		if (lpSwitches->overwrite == false)
		{
			if (ask_Message(MB_YESNO | MB_ICONQUESTION, lk_FILE_EXISTS_MESSAGE, outFile) == IDNO)
				return 24;
		}

		// delete the file, so this message isn't asked when walking for the 2nd pass
		if (! DeleteFile(outFile))
		{
			err_Message(lk_OVERWRITE_ERROR, outFile);
			return 25;
		}
	}

	opt_mux_format				= ((lpSwitches->format >= 0) && (lpSwitches->format <= 8)) ? lpSwitches->format : 0;

	opt_mpeg					= ((opt_mux_format >= 3) && (opt_mux_format <= 8)) ? 2 : 1;

	verbose						= ((lpSwitches->verbose > 0) && (lpSwitches->verbose <= 3)) ? lpSwitches->verbose -1 : verbose;

	opt_VBR						= (lpSwitches->vbr == true) ? 1 : opt_VBR;

	opt_always_system_headers	= (lpSwitches->sys_headers == true) ? 1 : opt_always_system_headers;

	opt_buffer_size				= ((lpSwitches->video_buffer > 0) && (lpSwitches->video_buffer <= 1000)) ? lpSwitches->video_buffer : opt_buffer_size;

	opt_data_rate				= (lpSwitches->mux_rate > 0) ? (((lpSwitches->mux_rate  * 1000 / 8 + 49) / 50 ) * 50) : opt_data_rate;

	opt_video_offset			= (lpSwitches->sync_offset != 0) ? lpSwitches->sync_offset : opt_video_offset;
	if( opt_video_offset < 0 )
	{
		opt_audio_offset = - opt_video_offset;
		opt_video_offset = 0;
	}

	opt_max_PTS					= (lpSwitches->mux_limit > 0) ? (static_cast<clockticks>(lpSwitches->mux_limit) * CLOCKS) : opt_max_PTS;

	opt_packets_per_pack		= ((lpSwitches->packets_per_pack >= 1) && (lpSwitches->packets_per_pack <= 100)) ? lpSwitches->packets_per_pack : opt_packets_per_pack;

	opt_sector_size				= ((lpSwitches->sector_size >= 256) && (lpSwitches->sector_size <= 16384)) ? lpSwitches->sector_size : opt_sector_size;

	opt_max_segment_size		= (lpSwitches->max_seg_size > 0) ? lpSwitches->max_seg_size : opt_max_segment_size;

	opt_multifile_segment		= (lpSwitches->split_segment == true) ? 1 : opt_multifile_segment;

//	opt_emul_vcdmplex			= 0;

	return 0;
}

void CMplex::Process()
{
	msgTitle = _T("CMplex::Process");

	vector<IBitStream *>		mpa_files;
	vector<IBitStream *>		ac3_files;
	vector<IBitStream *>		video_files;
	OutputStream				ostrm;
	vector<ElementaryStream *>	strms;
	clockticks first_frame_PTS = 0;
	UINT32						frame_interval;
	int							i;

	check_files(inFiles, mpa_files, ac3_files, video_files);
	info_Message("Found %d video streams, %d MPEG audio streams, and %d AC3 streams.", video_files.size(),
				mpa_files.size(), ac3_files.size());

	if( MPEG_STILLS_FORMAT(opt_mux_format) )
	{
		info_Message("Multiplexing stills stream!");
		ostrm.InitSyntaxParameters();
		switch ( opt_mux_format )
		{
			case MPEG_FORMAT_VCD_STILL :
			{
				frame_interval = 30; // 30 Frame periods
				if ((mpa_files.size() > 0) && (video_files.size() > 2))
				{
					error_exit_msg("VCD stills: no more than two streams (one normal one hi-res) possible");
					return;
				}
				VCDStillsStream* str[2];
				ConstantFrameIntervals* intervals[2];

				for (i=0; i<video_files.size(); i++ )
				{
					intervals[i] = new ConstantFrameIntervals( frame_interval );
					str[i] = new VCDStillsStream( *video_files[i], ostrm, intervals[i] );
					strms.push_back( str[i] );
					str[i]->Init();
				}
				if( video_files.size() == 2 )
				{
					str[0]->SetSibling(str[1]);
					str[1]->SetSibling(str[0]);
				}
				break;
			}
			case MPEG_FORMAT_SVCD_STILL :
			{
				frame_interval = 30;

				if (video_files.size() > 1)
				{
					error_exit_msg("SVCD stills streams may only contain a single video stream");
					return;
				}
				else if (video_files.size() > 0)
				{
					ConstantFrameIntervals *intervals;
					StillsStream *str;
					intervals = new ConstantFrameIntervals( frame_interval );
					str = new StillsStream( *video_files[0],ostrm, intervals );
					strms.push_back( str );
					str->Init();
				}
				else 
					for( i = 0 ; i < mpa_files.size() ; ++i )
					{
						AudioStream* audioStrm = new MPAStream( *mpa_files[i], ostrm);
						audioStrm->Init (i);
						strms.push_back(audioStrm);
					}
				break;
			}
			default :
			{
				error_exit_msg("Only (S)VCD Stills format for the moment...");
				return;
			}
		}
		// MultiPlex !
		ostrm.OutputMultiplex( &strms,  outFile);
	}
	else
	{
//		off_t audio_bytes, video_bytes;
		ostrm.InitSyntaxParameters();

		if ((video_files.size() < 1) && (opt_mux_format == MPEG_FORMAT_VCD))
			warn_Message("Multiplexing audio-only for a standard VCD is very inefficient");

		for ( i=0; i < video_files.size() ; i++)
		{
			VideoStream* videoStrm;
			// The first DVD video stream is made the master stream...

			if ((i == 0) && (opt_mux_format == MPEG_FORMAT_DVD))
				videoStrm = new DVDVideoStream( *video_files[i], ostrm);
			else
				videoStrm = new VideoStream( *video_files[i],ostrm);
			videoStrm->Init( i );
			strms.push_back( videoStrm );
		}
		for( i=0; i < mpa_files.size() ; i++ )
		{
			AudioStream* audioStrm = new MPAStream( *mpa_files[i], ostrm);
			audioStrm->Init ( i );
			strms.push_back(audioStrm);
		}
		for( i=0; i < ac3_files.size() ; i++ )
		{
			AudioStream* audioStrm = new AC3Stream( *ac3_files[i], ostrm);
			audioStrm->Init ( i );
			strms.push_back(audioStrm);
		}

		// MultiPlex !
		ostrm.OutputMultiplex( &strms,  outFile);
	}

	for( i=0; i < strms.size(); i++)
		delete strms[i];
}

//////////////////////////////////////////////////////////////////////
// public routines
//////////////////////////////////////////////////////////////////////

int CMplex::Run( lp_mplex_struct lpSwitches )
{
	int iError = Initialize(lpSwitches);
	if (iError != 0) return (iError > 0) ? iError : -1;

	__try
	{
		Process();
	}
	__except( GetExceptionCode() != 0 )
	{
		return -2;
	}

	return 0;
}


// ///////////////////////////////////////////////////////////////////
// The Thread that let "mplex" work
// ///////////////////////////////////////////////////////////////////
void mplex_Thread( void* arg )
{
	lp_mplex_struct lpSwitches = (arg) ? (lp_mplex_struct)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!lpSwitches)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	CMplex* mplexClass = new CMplex(); // run in the heap instead in our stack
	if (mplexClass == NULL)
	{
		lpSwitches->m_ReturnCode = -9999;
		if (event) SetEvent(*event); // notifying caller-process we're done
		_endthread();
		return;
	}

	int retVal = mplexClass->Run(lpSwitches); // if Run don't exceeds normaly, will have an error in mplex-lib
	delete mplexClass;
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}