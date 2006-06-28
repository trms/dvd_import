// This is the main DLL file.

#include "stdafx.h"
#include <windows.h>

#include "MultiplexerGlue.h"

#include "..\mjpgTools\mplex\bits.h"
#include "..\mjpgTools\mplex\interact.h"
#include "..\mjpgTools\mplex\videostrm.h"
#include "..\mjpgTools\mplex\stillsstream.h"
#include "..\mjpgTools\mplex\audiostrm.h"
//#include "..\mplex\mjpgTools\mplexconsts.h"
#define _HAS_LOG_LEVEL_
typedef int log_level_t;
#include "..\mjpgTools\mplex\outputstream.h"

OutputStream *ostrm = 0;

long long Utilities::DVDImport::MultiplexGlue::CurrentSCR()
{
	if(ostrm == 0)
		return(0);
	else
		return(ostrm->current_SCR / CLOCKS);
}

void Utilities::DVDImport::MultiplexGlue::Multiplex()
{
	// set up params
	opt_mux_format				= 3;
	opt_mpeg					= 2;
	opt_audio_offset			= 0;
	opt_video_offset			= Offset;
	IntPtr vidStr = System::Runtime::InteropServices::Marshal::StringToHGlobalAuto(Video);
	IntPtr audStr = System::Runtime::InteropServices::Marshal::StringToHGlobalAuto(Audio);
	IntPtr outStr = System::Runtime::InteropServices::Marshal::StringToHGlobalAuto(Output);
	LPTSTR lVideo = (LPTSTR)vidStr.ToPointer();
	LPTSTR lAudio = (LPTSTR)audStr.ToPointer();
	LPTSTR lOutput = (LPTSTR)outStr.ToPointer();


	vector<IBitStream *>		mpa_files;
	vector<IBitStream *>		video_files;
	//OutputStream				ostrm;
	vector<ElementaryStream *>	strms;
	clockticks first_frame_PTS = 0;
	UINT32						frame_interval;
	int							i;

	ostrm = new OutputStream();

	IBitStream *bs;
	bs = new IBitStream;
	char tmp[1024];
	sprintf(tmp, "%ls", lVideo);
    bs->open( tmp );
    video_files.push_back( bs );
    //bs->close();
    //delete bs;

	bs = new IBitStream;
	sprintf(tmp, "%ls", lAudio);
    bs->open( tmp );
    mpa_files.push_back( bs );
    //bs->close();
    //delete bs;


	//info_Message("Found %d video streams, %d MPEG audio streams, and %d AC3 streams.", video_files.size(),
	//			mpa_files.size(), ac3_files.size());

	ostrm->InitSyntaxParameters();

	//if ((video_files.size() < 1) && (opt_mux_format == MPEG_FORMAT_VCD))
	//	warn_Message("Multiplexing audio-only for a standard VCD is very inefficient");

	for ( i=0; i < video_files.size() ; i++)
	{
		VideoStream* videoStrm;
		// The first DVD video stream is made the master stream...

		if ((i == 0) && (opt_mux_format == MPEG_FORMAT_DVD))
			videoStrm = new DVDVideoStream( *video_files[i], *ostrm);
		else
			videoStrm = new VideoStream( *video_files[i],*ostrm);
		videoStrm->Init( i );
		strms.push_back( videoStrm );
	}
	for( i=0; i < mpa_files.size() ; i++ )
	{
		AudioStream* audioStrm = new MPAStream( *mpa_files[i], *ostrm);
		audioStrm->Init ( i );
		strms.push_back(audioStrm);
	}

	// MultiPlex !
	sprintf(tmp, "%ls", lOutput);
	ostrm->OutputMultiplex( &strms,  tmp);

	for( i=0; i < strms.size(); i++)
		delete strms[i];

	for ( i=0; i < video_files.size() ; i++)
	{
		IBitStream *bs = video_files[i];
		bs->close();
		delete bs;
	}

	for( i=0; i < mpa_files.size() ; i++ )
	{
		IBitStream *bs = mpa_files[i];
		bs->close();
		delete bs;
	}

	delete ostrm;
	ostrm = 0;

	System::Runtime::InteropServices::Marshal::FreeHGlobal(vidStr);
	System::Runtime::InteropServices::Marshal::FreeHGlobal(audStr);
	System::Runtime::InteropServices::Marshal::FreeHGlobal(outStr);
}