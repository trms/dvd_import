// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// jpegInfo.cpp:	implementation of the Cdjpeg class, part of the mjpgTools Library									 //
//					This file contains an abstract interface for all the JPEG GetInfo routines.							 //
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

#include ".\tools\cdjpeg.h"			/* Common decls for cjpeg/djpeg applications */
#include ".\jpeglib\jversion.h"		/* for version message */

#define MJPGTOOLS_INTERNALS
#include ".\mjpeglib\config.h"
#include ".\mjpeglib\yuv4mpeg.h"
#include "mjpgTools.h"				// for specific tool-functions and 'lpdjpeg_switches'
#include "jpegInfo.h"
#include ".\tools\tools.h"			// for 'Message' and 'AskMessage'

#include "privateMessages.h"		// for 'lk_...' strings

/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include ".\tools\cderror.h"
  NULL
};

class CJpegInfo
{
public:
	CJpegInfo( LPCTSTR lpSource ); // lpSource is a ptr to path of source-file
	~CJpegInfo();

	int GetInfo(void); // the actual routine, which retrieves all information

	unsigned	imgType; // FILETYPE_JPG, FILETYPE_TGA, FILETYPE_PPM, FILETYPE_GIF, FILETYPE_BMP or FILETYPE_UNKNOWN
	long		imgWidth;
	long		imgHeight;
	// maybe more properties to follow in the future...

protected:
	char					progname[64];					/* program name for error messages */
	FILE*					input_file;
	unsigned				fType;

	union _structs_ {
		// no-jpeg's
		struct _no_jpeg_ {
			jpeg_compress_struct	cinfo;	/* struct */
			cjpeg_source_ptr		src_mgr;
		}					noJpeg;
		struct _jpeg_ {
			jpeg_decompress_struct	cinfo; /*struct*/
//			djpeg_dest_ptr			dest_mgr;
		}					Jpeg;
	}	data;

	jpeg_error_mgr			jerr;	/* struct */
	JDIMENSION				num_scanlines;

	int Initialize(void);
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJpegInfo::CJpegInfo( LPCTSTR lpSource )
{
	strcpy(progname, _T("JPeGinfo"));
	input_file = NULL;

	if ((lpSource) && (lpSource[0]))
	{
		fType = _GetFileType(lpSource); // pre-determining which type of image we have
		switch (fType)
		{
			case FILETYPE_JPG :
				break;
			case FILETYPE_TGA :
			case FILETYPE_PPM :
			case FILETYPE_GIF :
			case FILETYPE_BMP :
			{
				data.noJpeg.src_mgr = NULL;
				break;
			}
			default :
			{
				fType = FILETYPE_UNKNOWN; // prevent us from actualy quering this file
				break;
			}
		}
		if (fType != FILETYPE_UNKNOWN)
			input_file = fopen(lpSource, READ_BINARY);
			// if, for some reason we cannot open this file, it will be set to NULL anyway, which makes us stop later
	}

	imgType		= fType;
	imgWidth	= 0;
	imgHeight	= 0;
}

CJpegInfo::~CJpegInfo()
{
	if (input_file) fclose(input_file);
}

int CJpegInfo::Initialize()
{
	if ((fType == FILETYPE_UNKNOWN) || (!input_file))
		return -1;
	else if (fType == FILETYPE_JPG)
	{
		// Initialize the JPEG compression object with default error handling.
		__try
		{
			data.Jpeg.cinfo.err = jpeg_std_error(&jerr);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -2;
		}

		__try
		{
			jpeg_create_decompress(&data.Jpeg.cinfo);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -3;
		}

		/* Add some application-specific error messages (from cderror.h) */
		jerr.addon_message_table = cdjpeg_message_table;
		jerr.first_addon_message = JMSG_FIRSTADDONCODE;
		jerr.last_addon_message = JMSG_LASTADDONCODE;

		/* Insert custom marker processor for COM and APP12.
		 * APP12 is used by some digital camera makers for textual info,
		 * so we provide the ability to display it as text.
		 * If you like, additional APPn marker types can be selected for display,
		 * but don't try to override APP0 or APP14 this way (see libjpeg.doc).
		 */
		//jpeg_set_marker_processor(&cinfo, JPEG_COM, print_text_marker);
		//jpeg_set_marker_processor(&cinfo, JPEG_APP0+12, print_text_marker);

		/* Now safe to enable signal catcher. */
#ifdef NEED_SIGNAL_CATCHER
		__try
		{
			enable_signal_catcher((j_common_ptr) &data.Jpeg.cinfo);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -4;
		}
#endif

		data.Jpeg.cinfo.err->trace_level = 0;
		//dinfo.dct_method = JDCT_FLOAT;

		/* Specify data source for decompression */
		__try
		{
			jpeg_stdio_src(&data.Jpeg.cinfo, input_file);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -5;
		}

		/* Read file header, set default decompression parameters */
		__try
		{
			(void) jpeg_read_header(&data.Jpeg.cinfo, TRUE);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -6;
		}
	}
	else
	{
		// Initialize the JPEG decompression object with default error handling.
		__try
		{
			data.noJpeg.cinfo.err = jpeg_std_error(&jerr);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -2;
		}

		__try
		{
			jpeg_create_compress(&data.noJpeg.cinfo);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -3;
		}

		/* Add some application-specific error messages (from cderror.h) */
		jerr.addon_message_table = cdjpeg_message_table;
		jerr.first_addon_message = JMSG_FIRSTADDONCODE;
		jerr.last_addon_message = JMSG_LASTADDONCODE;

		/* Now safe to enable signal catcher. */
#ifdef NEED_SIGNAL_CATCHER
		__try
		{
			enable_signal_catcher((j_common_ptr) &data.noJpeg.cinfo);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -4;
		}
#endif

		/* Initialize JPEG parameters.
		 * Much of this may be overridden later.
		 * In particular, we don't yet know the input file's color space,
		 * but we need to provide some value for jpeg_set_defaults() to work.
		 */
		data.noJpeg.cinfo.in_color_space = JCS_RGB; /* arbitrary guess */

		__try
		{
			jpeg_set_defaults(&data.noJpeg.cinfo);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -5;
		}

		data.noJpeg.cinfo.err->trace_level = 0;

		// selecting 'src_mgr'
		switch (fType)
		{
			case FILETYPE_BMP :
			{
				data.noJpeg.src_mgr = jinit_read_bmp(&data.noJpeg.cinfo);
				break;
			}
			case FILETYPE_GIF :
			{
				data.noJpeg.src_mgr = jinit_read_gif(&data.noJpeg.cinfo);
				break;
			}
			case FILETYPE_PPM :
			{
				data.noJpeg.src_mgr = jinit_read_ppm(&data.noJpeg.cinfo);
				break;
			}
			case FILETYPE_TGA :
			{
				data.noJpeg.src_mgr = jinit_read_targa(&data.noJpeg.cinfo);
				break;
			}
		}

		data.noJpeg.src_mgr->input_file = input_file;
		/* Read the input file header to obtain file size & colorspace. */
		__try
		{
			(*data.noJpeg.src_mgr->start_input) (&data.noJpeg.cinfo, data.noJpeg.src_mgr);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			return -6;
		}
	}

	return 0;
}

int CJpegInfo::GetInfo()
{
	// first initializing
	int retVal = Initialize();

	if (!retVal)
	{ // still okay - we can continue...
		switch (fType)
		{
			case FILETYPE_JPG :
			{
				imgWidth	= data.Jpeg.cinfo.image_width;
				imgHeight	= data.Jpeg.cinfo.image_height;
				break;
			}
			case FILETYPE_TGA :
			case FILETYPE_PPM :
			case FILETYPE_GIF :
			case FILETYPE_BMP :
			{
				imgWidth	= data.noJpeg.cinfo.image_width;
				imgHeight	= data.noJpeg.cinfo.image_height;
				break;
			}
		}
	}

	if (input_file)
	{
		fclose(input_file);
		input_file = NULL;
	}

	return retVal;
}


// ///////////////////////////////////////////////////////////////////
// The Thread that let things work
// ///////////////////////////////////////////////////////////////////
void jpegInfo_Thread( void* arg )
{
	jpegInfoData_t* pArg = (arg) ? (jpegInfoData_t*)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!pArg)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	CJpegInfo jpegInfoClass = CJpegInfo(pArg->lpSource);
	int retVal = 0; // 0 = OKAY
	retVal = jpegInfoClass.GetInfo(); // if Run don't exceeds normaly, will have an error in JPeG-lib
	// set the return values
	if (!retVal)
	{
		pArg->iHeight	= jpegInfoClass.imgHeight;
		pArg->iWidth	= jpegInfoClass.imgWidth;
	}

	pArg->retVal = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}