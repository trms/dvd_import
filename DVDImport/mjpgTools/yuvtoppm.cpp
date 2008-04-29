// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// yuvtoppm.cpp:	implementation of the Cyuv2ppm class, part of the mjpgTools Library									 //
//					Converts a (MJPEG-Tools-)YUV stream into 1 PPM file													 //
//																														 //
//					This implementation only accepts a single PPM, although the derived routine did accept multiple files//
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
//	<y4mtoppm.c> of the MJPEGtools v1.6.0 (of the linux group)															 //
//																														 //
//	Generate one or more PPM images from a YUV4MPEG2 stream.															 //
//	Performs 4:2:0->4:4:4 chroma supersampling and then converts ITU-Rec.601 Y'CbCr to R'G'B' colorspace.				 //
//																														 //
//	Copyright (C) 2002 Matthew J. Marjanovic <maddog@mir.com>															 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "stdafx.h"

#include <io.h>						// for low-level file-routines
#include <stdio.h>					// for low-level file-routines
#include <fcntl.h>					// for low-level file-routines

#include ".\mjpeglib\config.h"
#include ".\mjpeglib\yuv4mpeg.h"
#include ".\mjpeglib\mpegconsts.h"
#include ".\mjpeglib\subsample.h"
#include ".\mjpeglib\colorspace.h"

#define MJPGTOOLS_INTERNALS
#include "mjpgTools.h"
#include "privateMessages.h"		// for 'lk_...' strings
#include ".\tools\tools.h"			// for 'Message' and 'AskMessage'
#include "yuvtoppm.h"

/* command-line parameters */
typedef struct _cl_info_yuv2ppm {
	int		interleave;
	int		ss_mode;
	int		verbosity;
	int		fdin;
	FILE*	fdout;
} yuv2ppm_info_t;


// the class that actualy does do all the work
class Cyuv2ppm
{
public:
	Cyuv2ppm();
	~Cyuv2ppm();

	virtual int Run( lp_yuv2ppm_switches lpSwitches );

protected:
	char				infilename[MAX_PATH];	// filename of input
	char				outfilename[MAX_PATH];	// filename of output
	LPCTSTR				_internal_progname;
	LPCTSTR				_default_extension;		// ".ppm" (with the leading dot)

	yuv2ppm_info_t		cl;

private:
	int parse_switches( lp_yuv2ppm_switches lpSwitches );
	void write_ppm_from_two_buffers(UINT8* buffers[], UINT8* buffers2[], UINT8* rowbuffer, int width, int height);
	void write_ppm_from_one_buffer(UINT8* buffers[], UINT8* rowbuffer, int width, int height);

	int Initialize( lp_yuv2ppm_switches lpSwitches );
	int Process();
};


//////////////////////////////////////////////////////////////////////
// Cyuv2ppm : Construction/Destruction
//////////////////////////////////////////////////////////////////////
Cyuv2ppm::Cyuv2ppm()
{
	_internal_progname	= _T("YUVtoPPM");
	_default_extension	= _T(".ppm");

	ZeroMemory(&cl, sizeof(yuv2ppm_info_t));
	// filling 'cl'-struct with defaults
	cl.interleave		= 0;
	cl.ss_mode			= SSM_420_JPEG;
	cl.fdin				= -1;	// default error : use '_open()' to get a handle to the input-file
	cl.fdout			= NULL;	// default error : use 'fcreate()' to get a handle to the output-file
#ifdef _DEBUG
	cl.verbosity		= 2; // verbose
#else
	cl.verbosity		= 0; // no verbosity
#endif
}
Cyuv2ppm::~Cyuv2ppm()
{
}

//////////////////////////////////////////////////////////////////////
// Routines
//////////////////////////////////////////////////////////////////////

int Cyuv2ppm::parse_switches( lp_yuv2ppm_switches lpSwitches )
{
	if (lpSwitches==NULL) return -1;

	char fdisk[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME];

	if (lpSwitches->infile[0])
		strcpy(infilename, lpSwitches->infile); // Set output file name.
	else
	{
		errMessage(lk_NO_INPUT_MESSAGE, _internal_progname);
		return 21;
	}
	// See if the input really exists
	if (! FileExists(infilename))
	{
		errMessage(lk_INPUT_NOT_FOUND, infilename);
		return 22;
	}

	if (lpSwitches->outfile[0])
		strcpy(outfilename, lpSwitches->outfile); // Set output file name.
	else
	{ // create a new filename ourself
		// Message(lk_NO_OUTPUT_MESSAGE, _internal_progname);
		// return 21;
		_splitpath(lpSwitches->infile, fdisk, fdir, fname, NULL);
		wsprintf(outfilename, _T("%s%s%s%s"), fdisk, fdir, fname, _default_extension);
	}
	// See if the output-path is correct (existing)
	if (! PathExists(outfilename))
	{
		_splitpath( outfilename, fdisk, fdir, NULL, NULL );
		errMessage(lk_OUTPUT_PATH_DOES_NOT_EXIST, fdisk, fdir);
		return 23;
	}
	// Ask whether the output may be overwritten, if it already exists
	if (FileExists(outfilename))
	{
		// suppress message if 'lpSwitches->overwrite' was preset to TRUE
		if (lpSwitches->overwrite == false)
		{
			char szMessage[256];
			wsprintf(szMessage, lk_FILE_EXISTS_MESSAGE, outfilename);
			if (! AskMessage(szMessage, lk_FILE_EXISTS_TITLE))
				return 24;
		}
		// delete the file, so this message isn't asked when walking for the 2nd pass
		if (! DeleteFile(outfilename))
		{
			errMessage(lk_OVERWRITE_ERROR, outfilename);
			return 25;
		}
	}

	// verbosity
	cl.verbosity = ((lpSwitches->verbose >= 1) && (lpSwitches->verbose <= 3)) ? lpSwitches->verbose -1 : cl.verbosity;

	// open the input-file
	if ((cl.fdin=_open(infilename, _O_BINARY | _O_RDONLY)) == -1) return -2; // seems not to be able to open the input
	// create the output-file
	if ((cl.fdout=fopen(outfilename, "wb")) == NULL)
	{
		_close(cl.fdin); // close input-file
		cl.fdin=-1;
		return -3; // seems we're not able to create this file
	}

	// chroma (ss_mode)
	cl.ss_mode		= (lpSwitches->chroma == 1) ? SSM_UNKNOWN : ((lpSwitches->chroma == 2) ? SSM_420_MPEG2 : SSM_420_JPEG);

	// interleave (interleave)
	cl.interleave	= (lpSwitches->interleave == true) ? 1 : cl.interleave;

	// All Done
	return 0;
}

void Cyuv2ppm::write_ppm_from_two_buffers(UINT8* buffers[], UINT8* buffers2[], UINT8* rowbuffer, int width, int height)
{
	int x, y;
	UINT8* pixels;
	UINT8* R	= buffers[0];
	UINT8* G	= buffers[1];
	UINT8* B	= buffers[2];
	UINT8* R2	= buffers2[0];
	UINT8* G2	= buffers2[1];
	UINT8* B2	= buffers2[2];

	mjpeg_debug("write PPM image from two buffers, %dx%d", width, height);
	fprintf(cl.fdout, "P6\n%d %d 255\n", width, height);
	
	for (y = 0; y < height; y += 2)
	{
		pixels = rowbuffer;
		for (x = 0; x < width; x++)
		{
			*(pixels++) = *(R++);
			*(pixels++) = *(G++);
			*(pixels++) = *(B++);
		}
		fwrite(rowbuffer, sizeof(rowbuffer[0]), width * 3, cl.fdout);
		pixels = rowbuffer;
		for (x = 0; x < width; x++)
		{
			*(pixels++) = *(R2++);
			*(pixels++) = *(G2++);
			*(pixels++) = *(B2++);
		}
		fwrite(rowbuffer, sizeof(rowbuffer[0]), width * 3, cl.fdout);
	}
}

void Cyuv2ppm::write_ppm_from_one_buffer(UINT8* buffers[], UINT8* rowbuffer, int width, int height)
{
	int x, y;
	UINT8* pixels;
	UINT8* R	= buffers[0];
	UINT8* G	= buffers[1];
	UINT8* B	= buffers[2];

	mjpeg_debug("write PPM image from one buffer, %dx%d", width, height);
	fprintf(cl.fdout, "P6\n%d %d 255\n", width, height);

	for (y = 0; y < height; y++)
	{
		pixels = rowbuffer;
		for (x = 0; x < width; x++)
		{
			*(pixels++) = *(R++);
			*(pixels++) = *(G++);
			*(pixels++) = *(B++);
		}
		fwrite(rowbuffer, sizeof(rowbuffer[0]), width * 3, cl.fdout);
	}
}

//////////////////////////////////////////////////////////////////////
// MAIN Routines
//////////////////////////////////////////////////////////////////////

int Cyuv2ppm::Initialize( lp_yuv2ppm_switches lpSwitches )
{
	// parse params
	int InitError;
	if ( (InitError=parse_switches(lpSwitches)) != 0) return (InitError > 0) ? InitError : -1;

	return 0;
}

int Cyuv2ppm::Process()
{
	y4m_stream_info_t	streaminfo;
	y4m_frame_info_t	frameinfo;
	UINT8*			buffers[3];  // * R'G'B' or Y'CbCr *
	UINT8*			buffers2[3]; // * R'G'B' or Y'CbCr *
	int					err, i;
	int					width, height;
	int					interlace;
	UINT8*			rowbuffer;

	y4m_init_stream_info(&streaminfo);
	y4m_init_frame_info(&frameinfo);

	if ((err = y4m_read_stream_header(cl.fdin, &streaminfo)) != Y4M_OK)
	{
		mjpeg_error("Couldn't read YUV4MPEG2 header: %s!", y4m_strerr(err));
		return -1;
	}

	mjpeg_info("input stream parameters:");
	y4m_log_stream_info(LOG_INFO, "<<<", &streaminfo);

	width		= y4m_si_get_width(&streaminfo);
	height		= y4m_si_get_height(&streaminfo);
	interlace	= y4m_si_get_interlace(&streaminfo);

	// *** Allocate buffers ***
	mjpeg_debug("allocating buffers...");
	rowbuffer	= (UINT8*)(malloc(width * 3 * sizeof(rowbuffer[0])));
	mjpeg_debug("  rowbuffer %p", rowbuffer);
	// allocate buffers big enough for 4:4:4 supersampled components
	for (i = 0; i < 3; i++)
	{
		if (interlace == Y4M_ILACE_NONE)
		{
			buffers[i] = (UINT8*)(malloc(width * height * sizeof(buffers[i][0])));
			buffers2[i] = NULL;
		}
		else
		{
			buffers[i] = (UINT8*)(malloc(width * height / 2 * sizeof(buffers[i][0])));
			buffers2[i] = (UINT8*)(malloc(width * height / 2 * sizeof(buffers[i][0])));
		}
		mjpeg_debug("  buffers[%d] %p   buffers2[%d] %p", i, buffers[i], i, buffers2[i]);
	}

	// *** Process frames ***
	while (1)
	{
		if (interlace == Y4M_ILACE_NONE)
		{
			mjpeg_debug("reading noninterlaced frame...");
			if ((err = y4m_read_frame(cl.fdin, &streaminfo, &frameinfo, buffers)) != Y4M_OK) break;
			mjpeg_debug("supersampling noninterlaced frame...");
			chroma_supersample((subsample_mode_t)cl.ss_mode, buffers, width, height);
			mjpeg_debug("color converting noninterlaced frame...");
			convert_YCbCr_to_RGB(buffers, width * height);
			write_ppm_from_one_buffer(buffers, rowbuffer, width, height);
		}
		else
		{
			if ((err = y4m_read_fields(cl.fdin, &streaminfo, &frameinfo, buffers, buffers2)) != Y4M_OK) break;
			mjpeg_debug("supersampling top field...");
			chroma_supersample((subsample_mode_t)cl.ss_mode, buffers, width, height / 2);
			mjpeg_debug("supersampling bottom field...");
			chroma_supersample((subsample_mode_t)cl.ss_mode, buffers2, width, height / 2);
			mjpeg_debug("color converting top field...");
			convert_YCbCr_to_RGB(buffers, width * height / 2);
			mjpeg_debug("color converting bottom field...");
			convert_YCbCr_to_RGB(buffers2, width * height / 2);
			if (cl.interleave)
				write_ppm_from_two_buffers(buffers, buffers2, rowbuffer, width, height);
			else if (interlace == Y4M_ILACE_TOP_FIRST)
			{
				write_ppm_from_one_buffer(buffers, rowbuffer, width, height / 2);
				write_ppm_from_one_buffer(buffers2, rowbuffer, width, height / 2);
			}
			else
			{ // ilace == Y4M_ILACE_BOTTOM_FIRST
				write_ppm_from_one_buffer(buffers2, rowbuffer, width, height / 2);
				write_ppm_from_one_buffer(buffers, rowbuffer, width, height / 2);
			}
		}
	}     
	if ((err != Y4M_OK) && (err != Y4M_ERR_EOF))
		mjpeg_error("Couldn't read frame:  %s", y4m_strerr(err));

	// *** Clean-up after ourselves ***
	mjpeg_debug("freeing buffers; cleaning up");

	free(rowbuffer);
	for (i = 0; i < 3; i++)
	{
		free(buffers[i]);
		free(buffers2[i]);
	}

	y4m_fini_stream_info(&streaminfo);
	y4m_fini_frame_info(&frameinfo);

	mjpeg_debug("Done.");
	
	return 0;
}

int Cyuv2ppm::Run( lp_yuv2ppm_switches lpSwitches )
{
	// Initialize first
	int m_Error=0;
	__try
	{
		m_Error = Initialize(lpSwitches);
	}
	__except( GetExceptionCode() != 0 )
	{
		if (cl.fdin != -1) _close(cl.fdin); // close input-file
		if (cl.fdout != NULL) fclose(cl.fdout); // and close output-file
		// something wend wrong, exiting
		return -1;
	}
	if (m_Error != 0) return (m_Error > 0) ? m_Error : -1;

	// Then do the 'real' work
	__try
	{
		m_Error = Process();
	}
	__except( GetExceptionCode() != 0 )
	{
		if (cl.fdin != -1) _close(cl.fdin); // close input-file
		if (cl.fdout != NULL) fclose(cl.fdout); // and close output-file
		// something wend wrong, exiting
		return -2;
	}
	if (m_Error != 0) return -2;

	// some cleanup : closing of files
	if (cl.fdin != -1) _close(cl.fdin); // close input-file
	if (cl.fdout != NULL) fclose(cl.fdout); // and close output-file

	return 0; // all well
}


// ///////////////////////////////////////////////////////////////////
// The Thread that makes "yuv2ppm" work
// ///////////////////////////////////////////////////////////////////
void yuv2ppm_Thread( void* arg )
{
	lp_yuv2ppm_switches lpSwitches = (arg) ? (lp_yuv2ppm_switches)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!lpSwitches)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	Cyuv2ppm yuv2ppmClass = Cyuv2ppm();
	int retVal = 1; // 1 : General Failure, possible in mjpeglib-lib
	retVal = yuv2ppmClass.Run(lpSwitches); // if Run don't exceeds normaly, will have an error in MJPeG-lib
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}
