// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// jpeg2yuv.cpp:	implementation of the CYuvScaler class, part of the mjpgTools Library								 //
//					Converts a collection of JPEG images to a YUV4MPEG stream.											 //
//																														 //
//					!!! It seems the function <decode_jpeg_raw> isn't working properly - I (laurens) don't know why !!!	 //
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
// This routine is based on the MJPEGtools v1.6.0 (of the linux group)													 //
//																														 //
//	Copyright (C) 1999 Gernot Ziegler (gz@lysator.liu.se)																 //
//	Copyright (C) 2001 Matthew Marjanovic (maddog@mir.com)																 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "stdafx.h"

#include <io.h>						// for low-level file-routines
#include <sys\stat.h>				// for low-level file-routines
#include <fcntl.h>					// for low-level file-routines

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include ".\mjpeglib\config.h"

#include ".\mjpeglib\mjpeg_types.h"
#include ".\mjpeglib\yuv4mpeg.h"
#include ".\mjpeglib\mpegconsts.h"

#include ".\jpeglib\jpeglib.h"

#include ".\mjpeglib\jpegutils.h"
#include ".\mjpeglib\lav_io.h"
#include ".\mjpeglib\mjpeg_logging.h"

#define MJPGTOOLS_INTERNALS
#include "mjpgTools.h"				// for specific tool-functions and 'lp_jpeg2yuv_switches'
#include "privateMessages.h"		// for 'lk_...' strings
#include ".\tools\tools.h"			// for 'Message' and 'AskMessage'
#include "jpeg2yuv.h"


#define MAXPIXELS						(1280*1024)		// Maximum size of final image


typedef struct _parameters {
	const char*			jpegformatstr;
	y4m_ratio_t			framerate;
	INT32				interlace;			// will the YUV4MPEG stream be interlaced?
	INT32				interleave;			// are the JPEG frames field-interleaved?
	UINT32				start, end;

	INT32				verbose;			// the verbosity of the program (see mjpeg_logging.h)

	INT32				width;
	INT32				height;
} parameters_t;

static struct jpeg_error_mgr			jerr;

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// The Class that does do the actual work																				//
class Cjpeg2yuv
{
public:
	Cjpeg2yuv();
	~Cjpeg2yuv();

	virtual int Run( lp_jpeg2yuv_switches lpSwitches );

	jpeg_decompress_struct	dinfo;
protected:
	// variables
	parameters_t			param;

	char					outfile[MAX_PATH];
	LPCTSTR					_default_extension;		// _T(".jpg") -- with the leading dot
	char					progname[64];			// program name for error messages

	int						fOUT;					// using _..-functions
	BOOL					done__YUV4MPEG_Init;	// should be done only once

	UINT8*					yuv[3];  // buffer for Y/U/V planes of decoded JPEG
	y4m_stream_info_t		streaminfo;
	y4m_frame_info_t		frameinfo;

private:
	// routines
	int parse_switches( lp_jpeg2yuv_switches lpSwitches );
	int init_parse_files(LPCTSTR lpFileName);
	int YUV4MPEG_Init(LPCTSTR lpFilename);		// lpFilename = name of 1st JPeG
	int YUV4MPEG_process(LPCTSTR lpFilename);
	int generate_YUV4MPEG(void);
};

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// Class Cjpeg2yuv : Constructor/Destructor																				//
Cjpeg2yuv::Cjpeg2yuv()
{
	ZeroMemory(&dinfo, sizeof(jpeg_decompress_struct));

	done__YUV4MPEG_Init			= FALSE;
	strcpy(progname, _T("cJPeG2YUV"));
	_default_extension			= _T(".jpg");
	
	fOUT						= -1;
	outfile[0]					= 0;

	ZeroMemory(&param, sizeof(parameters_t));
	// set defaults
	param.jpegformatstr			= NULL;
	param.framerate				= y4m_fps_PAL;
	param.interlace				= Y4M_ILACE_NONE;
#ifdef _DEBUG
	param.verbose				= 2; // verbose complete
#else
	param.verbose				= 0; // no verbosity
#endif
}
Cjpeg2yuv::~Cjpeg2yuv()
{
}

// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// Class Cjpeg2yuv : Routines																							 //
int Cjpeg2yuv::parse_switches( lp_jpeg2yuv_switches lpSwitches )
{
	if (lpSwitches == NULL) return -1;

	char fdisk[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME], fext[_MAX_EXT];

	if (lpSwitches->outfile[0])
		strcpy(outfile, lpSwitches->outfile); // Set output file name.
	else
	{ // create a new filename ourself
		Message(lk_NO_OUTPUT_MESSAGE, progname);
		 return 21;
	}
	
	// add an extension if no extension was given
	_splitpath(outfile, fdisk, fdir, fname, fext);
	if ((!fext[0]) || (strcmp(fext, _T(".")) == 0))
		wsprintf(outfile, _T("%s%s%s.yuv"), fdisk, fdir, fname);

	// See if the output-path is correct (existing)
	if (! PathExists(outfile))
	{
		_splitpath( outfile, fdisk, fdir, NULL, NULL );
		Message(lk_OUTPUT_PATH_DOES_NOT_EXIST, fdisk, fdir);
		return 23;
	}
	// Ask whether the output may be overwritten, if it already exists
	if (FileExists(outfile))
	{
		// suppress message if 'lpSwitches->overwrite' was preset to TRUE
		if (lpSwitches->overwrite == false)
		{
			char szMessage[256];
			wsprintf(szMessage, lk_FILE_EXISTS_MESSAGE, outfile);
			if (! AskMessage(szMessage, lk_FILE_EXISTS_TITLE))
				return 24;
		}

		// delete the file, so this message isn't asked when walking for the 2nd pass
		if (! DeleteFile(outfile))
		{
			Message(lk_OVERWRITE_ERROR, outfile);
			return 25;
		}
	}

	// jpegformatstr (filter)
	if (!lpSwitches->filter[0])
	{
		Message2(progname, "ERROR : input format string not specified");
		return -2;
	}
	param.jpegformatstr = lpSwitches->filter;

	// create the output-file
	if ((fOUT=_open(outfile, _O_BINARY | _O_CREAT | _O_RDWR | _O_TRUNC, _S_IREAD | _S_IWRITE)) == -1)
	{
		Message2(progname, "ERROR : unable to create the output file");
		return -3;
	}

	// framerate
	if ((lpSwitches->frame_rate.n != y4m_fps_UNKNOWN.n) && (lpSwitches->frame_rate.d != y4m_fps_UNKNOWN.d))
	{ // no checking will be done : that's the user responsibility !
		param.framerate.n = lpSwitches->frame_rate.n;
		param.framerate.d = lpSwitches->frame_rate.d;
	}

	// interlace
	param.interlace		= (lpSwitches->interlace == 1) ? Y4M_ILACE_TOP_FIRST : ((lpSwitches->interlace == 2) ? Y4M_ILACE_BOTTOM_FIRST : param.interlace);

	// interleave (interleaving)
	param.interleave	= (lpSwitches->interleaving == true) ? 1 : 0;
	
	// start, end
	param.start			= lpSwitches->start;
	param.end			= lpSwitches->end;

	return 0;
}


/** init_parse_files
 * Verifies the JPEG input files and prepares YUV4MPEG header information.
 * @returns 0 on success
 */
int Cjpeg2yuv::init_parse_files(LPCTSTR lpFileName)
{ // lpFileName = filename of the JPeG to process
	if ((lpFileName==NULL) || (!lpFileName[0])) return -1;

	mjpeg_debug("Analyzing [%s] to get the right pic params", lpFileName);
	FILE* jpegfile = fopen(lpFileName, "rb");

	if (jpegfile == NULL)
	{
		Message2(progname, "System error while opening: \"%s\": %s", lpFileName, strerror(errno));
		return -2;
	}

	// Now open this JPEG file, and examine its header to retrieve the YUV4MPEG info that shall be written
	__try
	{
		dinfo.err = jpeg_std_error(&jerr);  /* ?????????? */
	}
	__except( GetExceptionCode() != 0 )
	{
		fclose(jpegfile);
		return -3;
	}

	__try
	{
		jpeg_create_decompress(&dinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		fclose(jpegfile);
		return -4;
	}

	__try
	{
		jpeg_stdio_src(&dinfo, jpegfile);
	}
	__except( GetExceptionCode() != 0 )
	{
		fclose(jpegfile);
		return -5;
	}

	__try
	{
		jpeg_read_header(&dinfo, 1);
	}
	__except( GetExceptionCode() != 0 )
	{
		fclose(jpegfile);
		return -6;
	}

	dinfo.out_color_space = JCS_YCbCr;      

	__try
	{
		jpeg_start_decompress(&dinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		fclose(jpegfile);
		return -7;
	}


	if (dinfo.output_components != 3)
	{
		Message2(progname, "Output components of JPEG image = %d, must be 3.", dinfo.output_components);
		return -8;
	}
  
	mjpeg_info("Image dimensions are %dx%d", dinfo.image_width, dinfo.image_height);

	param.width = dinfo.image_width;
	param.height = dinfo.image_height;
  
	__try
	{
		jpeg_destroy_decompress(&dinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		fclose(jpegfile);
		return -9;
	}

	fclose(jpegfile);

	mjpeg_info("Movie frame rate is:  %f frames/second", Y4M_RATIO_DBL(param.framerate));

	switch (param.interlace)
	{
		case Y4M_ILACE_BOTTOM_FIRST :
		{
			mjpeg_info("Interlaced frames, bottom field first.");      
			break;
		}
		case Y4M_ILACE_TOP_FIRST :
		{
			mjpeg_info("Interlaced frames, top field first.");      
			break;
		}
		case Y4M_ILACE_NONE :
		default :
		{
			mjpeg_info("Non-interlaced/progressive frames.");
			break;
		}
	}

	if (!(param.interleave) && (param.interlace != Y4M_ILACE_NONE))
	{
		param.height *= 2;
		mjpeg_info("Non-interleaved fields (image height doubled)");
	}
	mjpeg_info("Frame size:  %d x %d", param.width, param.height);

	return 0;
}

int Cjpeg2yuv::YUV4MPEG_Init(LPCTSTR lpFilename)
{
	if (done__YUV4MPEG_Init) return 0; // already performed, no need to repeat again

	if ((lpFilename==NULL) || (!lpFilename[0])) return -1;
	if (init_parse_files(lpFilename) != 0)
	{ // error while initilizing the JPeG
		Message2(progname, "ERROR ** Some exception occured while initializing the JPeG");
		return -2;
	}

	mjpeg_info("Now initializing the YUV4MPEG stream.");

	y4m_init_stream_info(&streaminfo);
	y4m_init_frame_info(&frameinfo);

	y4m_si_set_width(&streaminfo, param.width);
	y4m_si_set_height(&streaminfo, param.height);
	y4m_si_set_interlace(&streaminfo, param.interlace);
	y4m_si_set_framerate(&streaminfo, param.framerate);

	yuv[0] = (UINT8*)(malloc(param.width * param.height * sizeof(yuv[0][0])));
	yuv[1] = (UINT8*)(malloc(param.width * param.height / 4 * sizeof(yuv[1][0])));
	yuv[2] = (UINT8*)(malloc(param.width * param.height / 4 * sizeof(yuv[2][0])));

	y4m_write_stream_header(fOUT, &streaminfo);

	done__YUV4MPEG_Init = TRUE;

	return 0;
}

int Cjpeg2yuv::YUV4MPEG_process(LPCTSTR lpFilename)
{
	if ((lpFilename==NULL) || (!lpFilename[0])) return -1;

	FILE*					jpegfile;
	size_t					jpegsize;
	static UINT8			jpegdata[MAXPIXELS];  // that ought to be enough

	mjpeg_info("Now generating YUV4MPEG stream.");

	if ((jpegfile = fopen(lpFilename, "rb")) == NULL)
	{
		mjpeg_info("Read from '%s' failed:  %s", lpFilename, strerror(errno));
		return -2;
	}

	mjpeg_debug("Preparing frame");

	jpegsize = fread(jpegdata, sizeof(UINT8), MAXPIXELS, jpegfile); 
	fclose(jpegfile);

	/* decode_jpeg_raw:s parameters from 20010826
	 * jpeg_data:       buffer with input / output jpeg
	 * len:             Length of jpeg buffer
	 * itype:           0: Interleaved/Progressive
	 *                  1: Not-interleaved, Top field first
	 *                  2: Not-interleaved, Bottom field first
	 * ctype            Chroma format for decompression.
	 *                  Currently always 420 and hence ignored.
	 * raw0             buffer with input / output raw Y channel
	 * raw1             buffer with input / output raw U/Cb channel
	 * raw2             buffer with input / output raw V/Cr channel
	 * width            width of Y channel (width of U/V is width/2)
	 * height           height of Y channel (height of U/V is height/2) */

	if ((param.interlace == Y4M_ILACE_NONE) || (param.interleave == 1))
	{
		mjpeg_info("Processing non-interlaced/interleaved %s, size %ul.", lpFilename, jpegsize);
		if (decode_jpeg_raw(jpegdata, jpegsize, 0, 420, param.width, param.height, yuv[0], yuv[1], yuv[2]) != 0)
			return -3;
	}
	else
	{
		switch (param.interlace)
		{
			case Y4M_ILACE_TOP_FIRST :
			{
				mjpeg_info("Processing interlaced, top-first %s, size %ul.", lpFilename, jpegsize);
				if (decode_jpeg_raw(jpegdata, jpegsize, LAV_INTER_TOP_FIRST, 420, param.width, param.height,
					yuv[0], yuv[1], yuv[2]) != 0) return -3;
				break;
			}
			case Y4M_ILACE_BOTTOM_FIRST :
			{
				mjpeg_info("Processing interlaced, bottom-first %s, size %ul.", lpFilename, jpegsize);
				if (decode_jpeg_raw(jpegdata, jpegsize, LAV_INTER_BOTTOM_FIRST, 420, param.width, param.height,
					yuv[0], yuv[1], yuv[2]) != 0) return -3;
				break;
			}
			default :
			{
				Message2(progname, "FATAL logic error?!?");
				return -4;
			}
		}
		mjpeg_debug("Frame decoded, now writing to output stream.");
	}
    y4m_write_frame(fOUT, &streaminfo, &frameinfo, yuv);

	return 0;
}

int Cjpeg2yuv::generate_YUV4MPEG()
{
	char	jpegname[MAX_PATH], jpegTmp[MAX_PATH], fdrive[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME], fext[_MAX_EXT];
	int		retcode=0;

	if (strchr(param.jpegformatstr, '%') == NULL)
	{ // no printf-format to be use : properly only 1 file to convert
		_splitpath(param.jpegformatstr, fdrive, fdir, fname, fext);
		if ((!fext[0]) || (strcmp(fext, _T(".")) == 0))
			strcpy(fext, _default_extension);
		wsprintf(jpegname, _T("%s%s%s%s"), fdrive, fdir, fname, fext);

		// initilizing first
		if (YUV4MPEG_Init(jpegname) != 0)
			return -1; // some error occured

		if (YUV4MPEG_process(jpegname) != 0)
			retcode = -2; // an error occured, propperly in the 'decode_jpeg_raw'-call
	}
	else
	{ // uses printf-format
		for (UINT32 frame = param.start; (frame < param.end) || ((param.start == 0) && (param.end == 0)); frame++)
		{
			// check whether an extension was added
			wsprintf(jpegTmp, param.jpegformatstr, frame);
			_splitpath(jpegTmp, fdrive, fdir, fname, fext);
			if ((!fext[0]) || (strcmp(fext, _T(".")) == 0))
				strcpy(fext, _default_extension);
			wsprintf(jpegname, _T("%s%s%s%s"), fdrive, fdir, fname, fext);

			// initilizing first
			if (!done__YUV4MPEG_Init)
				if (YUV4MPEG_Init(jpegname) != 0)
					return -1; // some error occured

			if ((retcode=YUV4MPEG_process(jpegname)) != 0)
			{
				if ((retcode == -2) && (param.end == 0))
				{
					mjpeg_info("No more frames.  Stopping.");
					retcode = 0;
				}
				else
					retcode = -2; // propperly in the 'decode_jpeg_raw'-call

				break;
			}
		}
	}

	// finalizing : always must be performed to avoid 'memory-leaks'
	y4m_fini_stream_info(&streaminfo);
	y4m_fini_frame_info(&frameinfo);
	free(yuv[0]);
	free(yuv[1]);
	free(yuv[2]);

	return retcode;
}

int Cjpeg2yuv::Run( lp_jpeg2yuv_switches lpSwitches )
{
	// parse switches first
	int retCode = parse_switches(lpSwitches);
	if (retCode != 0)
	{
		if (fOUT != -1)
		{
			_close(fOUT);
			if (FileExists(outfile)) DeleteFile(outfile);
		}
		return (retCode > 0) ? retCode : -1;
	}

	// Now processing
	if (generate_YUV4MPEG() != 0)
	{
		if (fOUT != -1)
		{
			_close(fOUT);
			if (FileExists(outfile)) DeleteFile(outfile);
		}
		return -2;
	}

	mjpeg_info("Generation Succeeded");

	return 0;
}



// ///////////////////////////////////////////////////////////////////
// The Thread that let "jpeg2yuv" work
// ///////////////////////////////////////////////////////////////////
void jpeg2yuv_Thread( void* arg )
{
	lp_jpeg2yuv_switches lpSwitches = (arg) ? (lp_jpeg2yuv_switches)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!lpSwitches)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	Cjpeg2yuv jpg2yuvClass = Cjpeg2yuv();
	int retVal = 1; // 1 : General Failure, possible in mjpeglib-lib
	retVal = jpg2yuvClass.Run(lpSwitches); // if Run don't exceeds normaly, will have an error in MJPeG-lib
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}