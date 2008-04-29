// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// djpeg.cpp:	implementation of the Cdjpeg class, part of the mjpgTools Library										 //
//				This file contains an abstract interface for the JPEG decompressor.										 //
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
//   <cjpeg.c> of the Independent JPEG Group's software																	 //
//																														 //
//	Copyright (C) 1991-1997, Thomas G. Lane.																			 //
//  The authors make NO WARRANTY or representation, either express or implied, with respect to this software, its		 //
//  quality, accuracy, merchantability, or fitness for a particular purpose.  This software is provided "AS IS", and	 //
//  you, its user, assume the entire risk as to its quality and accuracy.												 //
//  For conditions of distribution and use, see the accompanying README file in the original package (jpeg-mmx.zip)		 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "stdafx.h"

#include ".\tools\cdjpeg.h"			/* Common decls for cjpeg/djpeg applications */
#include ".\jpeglib\jversion.h"		/* for version message */

#define MJPGTOOLS_INTERNALS
#include ".\mjpeglib\config.h"
#include ".\mjpeglib\yuv4mpeg.h"
#include "mjpgTools.h"				// for specific tool-functions and 'lpdjpeg_switches'
#include "djpeg.h"
#include ".\tools\tools.h"			// for 'Message' and 'AskMessage'

#include "privateMessages.h"		// for 'lk_...' strings

/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include ".\tools\cderror.h"
  NULL
};

class Cdjpeg
{
public:
	Cdjpeg(unsigned int requested_format=0); // see declarations in 'djpeg.h'
	virtual ~Cdjpeg();

	virtual int Run( lpdjpeg_switches lpSwitches ); // The main App

protected:
	char					progname[64];					/* program name for error messages */
	char					outfilename[MAX_PATH];			/* for -outfile switch */
	char					infilename[MAX_PATH];			/* for -infile switch */
	unsigned int			requested_fmt;					// 1 of 'FMT_xxx' switches
	jpeg_decompress_struct	cinfo; /*struct*/
	jpeg_error_mgr			jerr; /*struct*/
	djpeg_dest_ptr			dest_mgr;
	FILE*					input_file;
	FILE*					output_file;
	JDIMENSION				num_scanlines;

	LPCTSTR					_default_extension; // initialy empty, while we don't know yet what to create -- always with the leading dot

private:
	int parse_switches(lpdjpeg_switches lpSwitches, boolean forReal);
	void select_export_type(void);

	int Initialize( lpdjpeg_switches lpSwitches );
	int Processor(void);
	int Finalize(void);
};

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Marker processor for COM and interesting APPn markers.
 * This replaces the library's built-in processor, which just skips the marker.
 * We want to print out the marker as text, to the extent possible.
 * Note this code relies on a non-suspending data source.
 */

LOCAL(unsigned int) jpeg_getc(j_decompress_ptr cinfo)
{ // Read next byte
	jpeg_source_mgr* datasrc = cinfo->src;

	if (datasrc->bytes_in_buffer == 0)
	{
		if (! (*datasrc->fill_input_buffer) (cinfo))
			ERREXIT(cinfo, JERR_CANT_SUSPEND);
	}
	datasrc->bytes_in_buffer--;
	return GETJOCTET(*datasrc->next_input_byte++);
}

METHODDEF(boolean) print_text_marker(j_decompress_ptr cinfo)
{
	char buffer[4096]; buffer[0]=0;
	boolean traceit = (cinfo->err->trace_level >= 1);
	INT32 length;
	unsigned int ch;
	unsigned int lastch = 0;

	length = jpeg_getc(cinfo) << 8;
	length += jpeg_getc(cinfo);
	length -= 2;			// discount the length word itself

	if (traceit)
	{
		if (cinfo->unread_marker == JPEG_COM)
			sprintf(buffer, "Comment, length %ld:\n", (long) length);
		else				// assume it is an APPn otherwise
			sprintf(buffer, "APP%d, length %ld:\n", cinfo->unread_marker - JPEG_APP0, (long) length);
	}

	while (--length >= 0)
	{
		ch = jpeg_getc(cinfo);
		if (traceit)
		{	// Emit the character in a readable form.
			// Nonprintables are converted to \nnn form,
			// while \ is converted to \\.
			// Newlines in CR, CR/LF, or LF form will be printed as one newline.
			if (ch == '\r')
			{
				sprintf(&buffer[strlen(buffer)], "\n");
			}
			else if (ch == '\n')
			{
				if (lastch != '\r')
					sprintf(&buffer[strlen(buffer)], "\n");
			}
			else if (ch == '\\')
			{
				sprintf(&buffer[strlen(buffer)], "\\\\");
			}
			else if (isprint(ch))
			{
				sprintf(&buffer[strlen(buffer)], "%c", ch);
			}
			else
			{
				sprintf(&buffer[strlen(buffer)], "\\%03o", ch);
			}
			lastch = ch;
		}
	}

	if (traceit)
		sprintf(&buffer[strlen(buffer)], "\n");

	return TRUE;
}

#ifdef __cplusplus
	}
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cdjpeg::Cdjpeg(unsigned int requested_format)
{
	strcpy(progname, _T("dJPeG"));
	outfilename[0]		= 0;
	infilename[0]		= 0;
	requested_fmt		= requested_format; // if (0) = unspecified

	dest_mgr			= NULL;

	switch (requested_fmt)
	{
		case FMT_BMP :
		case FMT_OS2 :
		{
			_default_extension	= _T(".bmp");
			break;
		}
		case FMT_GIF :
		{
			_default_extension	= _T(".gif");
			break;
		}
		case FMT_PPM :
		{
			_default_extension	= _T(".ppm");
			break;
		}
#ifdef RLE_SUPPORTED
		case FMT_RLE :
		{
			_default_extension	= _T(".rle");
			break;
		}
#endif
		case FMT_TARGA :
		{
			_default_extension	= _T(".tga");
			break;
		}
		default :
		{ // unknown / unspecified
			_default_extension	= _T("");
			break;
		}
	}
}

Cdjpeg::~Cdjpeg()
{

}

/* Parse optional switches.
 * Returns argv[] index of first file-name argument (== argc if none).
 * Any file names with indexes <= last_file_arg_seen are ignored;
 * they have presumably been processed in a previous iteration.
 * (Pass 0 for last_file_arg_seen on the first or only iteration.)
 * for_real is FALSE on the first (dummy) pass; we may skip any expensive
 * processing.
 */
// for the 'lk_...' strings, see definitions in 'privateMessages.h'
int Cdjpeg::parse_switches(lpdjpeg_switches lpSwitches, boolean forReal)
{
	if (lpSwitches == NULL) return 5;

	if (!forReal)
	{
		char fdisk[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME];

		if (lpSwitches->infile[0])
			strcpy(infilename, lpSwitches->infile); // Set input file name.
		else
		{
			errMessage(lk_NO_INPUT_MESSAGE, progname);
			return -1;
		}
		// See if the input really exists
		if (! FileExists(infilename))
		{
			errMessage(lk_INPUT_NOT_FOUND, infilename);
			return -2;
		}

		if (lpSwitches->outfile[0])
			strcpy(outfilename, lpSwitches->outfile); // Set output file name.
		else
		{ // create a new filename ourself
			// Message(lk_NO_OUTPUT_MESSAGE, progname);
			// return 21;
			_splitpath(lpSwitches->infile, fdisk, fdir, fname, NULL);
			wsprintf(outfilename, _T("%s%s%s%s"), fdisk, fdir, fname, _default_extension);
		}
		// See if the output-path is correct (existing)
		if (! PathExists(outfilename))
		{
			_splitpath( outfilename, fdisk, fdir, NULL, NULL );
			errMessage(lk_OUTPUT_PATH_DOES_NOT_EXIST, fdisk, fdir);
			return -3;
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
					return -4;
			}

			// delete the file, so this message isn't asked when walking for the 2nd pass
			if (! DeleteFile(outfilename))
			{
				errMessage(lk_OVERWRITE_ERROR, outfilename);
				return -5;
			}
		}
	} // else : we've got these already

	cinfo.err->trace_level = 0;

	// Do color quantization?
	if ((lpSwitches->quantize == 16) || (lpSwitches->quantize == 256))
	{
		cinfo.desired_number_of_colors = lpSwitches->quantize;
		cinfo.quantize_colors = TRUE;
	}

	// Select IDCT algorithm
	if (lpSwitches->DCT_islow == true)
		cinfo.dct_method = JDCT_ISLOW;
	else if (lpSwitches->DCT_ifast == true)
		cinfo.dct_method = JDCT_IFAST;
	else if (lpSwitches->DCT_float == true)
		cinfo.dct_method = JDCT_FLOAT;

	// Select dithering algorithm
	switch (lpSwitches->dither_level)
	{
		case 2 :
		{ // simple ordered dither
			cinfo.dither_mode = JDITHER_ORDERED;
			break;
		}
		case 3 :
		{ // no dithering
			cinfo.dither_mode = JDITHER_NONE;
			break;
		}
		case 1 :
		{ // Floyd-Steinberg error diffusion dither
			cinfo.dither_mode = JDITHER_FS;
			break;
		}
	}

	// Force monochrome output?
	if (lpSwitches->grayscale == true)
		cinfo.out_color_space = JCS_GRAYSCALE;

	// Suppress fancy upsampling?
	if (lpSwitches->nosmooth == true)
		cinfo.do_fancy_upsampling = FALSE;

	// Use fast one-pass quantization?
	if (lpSwitches->onepass == true)
		cinfo.two_pass_quantize = FALSE;

	return 0;
	// return -1	= input/output file not defined in param-struct
	// return -2	= input does not exist
	// return -3	= output-path (disk+dir) does not exist
	// return -4	= output exist and may not be overwritten
	// return -5	= output protection-error : ReadOnly or Access Denied
}

void Cdjpeg::select_export_type()
{
	dest_mgr = NULL;

	switch (requested_fmt)
	{
#ifdef BMP_SUPPORTED
		case FMT_BMP :
		{
			dest_mgr = jinit_write_bmp(&cinfo, FALSE);
			break;
		}
		case FMT_OS2 :
		{
			dest_mgr = jinit_write_bmp(&cinfo, TRUE);
			break;
		}
#endif
#ifdef GIF_SUPPORTED
		case FMT_GIF :
		{
			dest_mgr = jinit_write_gif(&cinfo);
			break;
		}
#endif
#ifdef PPM_SUPPORTED
		case FMT_PPM :
		{
			dest_mgr = jinit_write_ppm(&cinfo);
			break;
		}
#endif
#ifdef RLE_SUPPORTED
		case FMT_RLE :
		{
			dest_mgr = jinit_write_rle(&cinfo);
			break;
		}
#endif
#ifdef TARGA_SUPPORTED
		case FMT_TARGA :
		{
			dest_mgr = jinit_write_targa(&cinfo);
			break;
		}
#endif
		default :
		{ // unknown / unspecified
			ERREXIT(&cinfo, JERR_UNSUPPORTED_FORMAT);
			break;
		}
	}
}

int Cdjpeg::Initialize( lpdjpeg_switches lpSwitches )
{
	if (lpSwitches == NULL) return -1;
	if ((requested_fmt <= 0) || (requested_fmt > FMT_TARGA)) return -1;

	/* Initialize the JPEG decompression object with default error handling. */
	__try
	{
		cinfo.err = jpeg_std_error(&jerr);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		return -2;
	}

	__try
	{
		jpeg_create_decompress(&cinfo);
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
		enable_signal_catcher((j_common_ptr) &cinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		return -4;
	}
#endif

	/* Scan command line to find file names. */
	/* It is convenient to use just one switch-parsing routine, but the switch
	 * values read here are ignored; we will rescan the switches after opening
	 * the input file.
	 * (Exception: tracing level set here controls verbosity for COM markers
	 * found during jpeg_read_header...)
	 */
	int paramError;
	if ((paramError=parse_switches(lpSwitches, FALSE)) != 0)
		return (paramError * -1); // some error occured : exiting

	/* Open the input file. */
	if ((input_file = fopen(infilename, READ_BINARY)) == NULL)
	{
		errMessage(_T("%s: can't open [%s] for READING"), progname, infilename);
		return -6;
    }

	/* Open the output file. */
	if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL)
	{
		errMessage(_T("%s: can't open [%s] for WRITING"), progname, outfilename);
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		return -7;
	}

	/* Specify data source for decompression */
	__try
	{
		jpeg_stdio_src(&cinfo, input_file);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -8;
	}

	/* Read file header, set default decompression parameters */
	__try
	{
		(void) jpeg_read_header(&cinfo, TRUE);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -9;
	}

	/* Adjust default decompression parameters by re-parsing the options */
	parse_switches(lpSwitches, TRUE); // must assume all wend well, while first time wend well too

	/* Initialize the output module now to let it override any crucial
	 * option settings (for instance, GIF wants to force color quantization).
	 */
	__try
	{
		select_export_type();
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -10;
	}
	if (! dest_mgr)
	{ // == NULL
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -11;
	}

	dest_mgr->output_file = output_file;

	return 0;
}

int Cdjpeg::Processor()
{
	/* Start decompressor */
	__try
	{
		jpeg_start_decompress(&cinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -1;
	}

	/* Write output file header */
	__try
	{
		(*dest_mgr->start_output) (&cinfo, dest_mgr);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -2;
	}

	/* Process data */
	while (cinfo.output_scanline < cinfo.output_height)
	{
		__try
		{
			num_scanlines = jpeg_read_scanlines(&cinfo, dest_mgr->buffer, dest_mgr->buffer_height);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			fclose(input_file); input_file=NULL; // be sure it's closed on exit
			fclose(output_file); output_file=NULL; // be sure it's closed on exit
			DeleteFile(outfilename); // remove the outfile
			return -3;
		}

		__try
		{
		    (*dest_mgr->put_pixel_rows) (&cinfo, dest_mgr, num_scanlines);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			fclose(input_file); input_file=NULL; // be sure it's closed on exit
			fclose(output_file); output_file=NULL; // be sure it's closed on exit
			DeleteFile(outfilename); // remove the outfile
			return -4;
		}
	}

	return 0;
}

int Cdjpeg::Finalize()
{
	/* Finish decompression and release memory.
	 * I must do it in this order because output module has allocated memory
	 * of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
	 */
	__try
	{
		(*dest_mgr->finish_output) (&cinfo, dest_mgr);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -1;
	}

	__try
	{
		(void) jpeg_finish_decompress(&cinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -2;
	}

	__try
	{
		jpeg_destroy_decompress(&cinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -3;
	}

	/* Close files, if we opened them */
    fclose(input_file); input_file=NULL;
    fclose(output_file); output_file=NULL;

	return 0;
}

int Cdjpeg::Run( lpdjpeg_switches lpSwitches )
{
	// Start Initialisation
	int initError;
	if ((initError=Initialize(lpSwitches)) != 0) return (initError > 0) ? (initError+20) : -1;
	// return 21	= input/output file not defined in param-struct
	// return 22	= input does not exist
	// return 23	= output-path (disk+dir) does not exist
	// return 24	= output exist and may not be overwritten
	// return 25	= output protection-error : ReadOnly or Access Denied

	// Start decompressor
	if (Processor() != 0) return -2;

	// Finalize
	if (Finalize() != 0) return -3;

	/* All done. */
	return (jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS); /* suppress no-return-value warnings */
	// finalize_failure		= -3
	// process_failure		= -2
	// init_failure			= -1
	// EXIT_SUCCESS			= 0
	// EXIT_FAILURE			= 1
	// EXIT_WARNING			= 2
}


// ///////////////////////////////////////////////////////////////////
// The Thread that let "djpeg" work
// ///////////////////////////////////////////////////////////////////
void djpeg_Thread( void* arg )
{
	// added since 1.6.0.0
	void** pArg = (arg) ? (void**)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	lpdjpeg_switches lpSwitches = (lpdjpeg_switches)(pArg[0]);
	unsigned int requested_output = (unsigned int)(pArg[1]);

	if (lpSwitches == NULL)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	Cdjpeg djpegClass = Cdjpeg(requested_output);
	int retVal = EXIT_FAILURE; // 1 : General Failure, possible in JPeG-lib
	retVal = djpegClass.Run(lpSwitches); // if Run don't exceeds normaly, will have an error in JPeG-lib
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}