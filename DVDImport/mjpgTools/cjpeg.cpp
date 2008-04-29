// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// cjpeg.cpp:	implementation of the Ccjpeg class, part of the mjpgTools Library										 //
//				This file contains an abstract interface for the JPEG compressor.										 //
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
//	Copyright (C) 1991-1998, Thomas G. Lane.																			 //
//  The authors make NO WARRANTY or representation, either express or implied, with respect to this software, its		 //
//  quality, accuracy, merchantability, or fitness for a particular purpose.  This software is provided "AS IS", and	 //
//  you, its user, assume the entire risk as to its quality and accuracy.												 //
//  For conditions of distribution and use, see the accompanying README file in the original package (jpeg-mmx.zip)		 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "stdafx.h"

#include ".\tools\cdjpeg.h"			/* Common decls for cjpeg/djpeg applications */

#define MJPGTOOLS_INTERNALS
#include ".\mjpeglib\config.h"
#include ".\mjpeglib\yuv4mpeg.h"
#include "mjpgTools.h"				// for specific tool-functions and 'lpcjpeg_switches'
#include "cjpeg.h"
#include ".\tools\tools.h"			// for 'Message' and 'AskMessage'

#include "privateMessages.h"		// for 'lk_...' strings

/* Create the add-on message string table. */

#define JMESSAGE(code,string)	string ,

static const char * const cdjpeg_message_table[] = {
#include ".\tools\cderror.h"
  NULL
};


class Ccjpeg
{ // this class has to be called ONLY from a new thread !!
public:
	Ccjpeg();
	virtual ~Ccjpeg();

	virtual int Run(lpcjpeg_switches lpSwitches);	// the main app

protected:
	// these vars are used in the class and in the thread, so need to be public
	boolean					is_targa;						/* records user -targa switch */
	char					progname[64];					/* program name for error messages */
	char					outfilename[MAX_PATH];			/* for -outfile switch */
	char					infilename[MAX_PATH];			/* for -infile switch */
	jpeg_compress_struct	cinfo;	/* struct */
	jpeg_error_mgr			jerr;	/* struct */
	cjpeg_source_ptr		src_mgr;
	FILE*					input_file;
	FILE*					output_file;
	JDIMENSION				num_scanlines;

	LPCTSTR					_default_extension; // _T(".jpg") -- with the leading dot

private:
	void select_file_type(void);
	int parse_switches(lpcjpeg_switches lpSwitches, boolean isReal);

	int Initialize(lpcjpeg_switches lpSwitches);
	int Processor(void);
	int Finalize(void);
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Ccjpeg::Ccjpeg()
{
	strcpy(progname, _T("cJPeG"));
	_default_extension	= _T(".jpg");
	is_targa			= FALSE;
	outfilename[0]		= 0;
	infilename[0]		= 0;

	src_mgr				= NULL;
}

Ccjpeg::~Ccjpeg()
{

}

//////////////////////////////////////////////////////////////////////
// Routines
//////////////////////////////////////////////////////////////////////

/*
 * This routine determines what format the input file is,
 * and selects the appropriate input-reading module.
 *
 * To determine which family of input formats the file belongs to,
 * we may look only at the first byte of the file, since C does not
 * guarantee that more than one character can be pushed back with ungetc.
 * Looking at additional bytes would require one of these approaches:
 *     1) assume we can fseek() the input file (fails for piped input);
 *     2) assume we can push back more than one character (works in
 *        some C implementations, but unportable);
 *     3) provide our own buffering (breaks input readers that want to use
 *        stdio directly, such as the RLE library);
 * or  4) don't put back the data, and modify the input_init methods to assume
 *        they start reading after the start of file (also breaks RLE library).
 * #1 is attractive for MS-DOS but is untenable on Unix.
 *
 * The most portable solution for file types that can't be identified by their
 * first byte is to make the user tell us what they are.  This is also the
 * only approach for "raw" file types that contain only arbitrary values.
 * We presently apply this method for Targa files.  Most of the time Targa
 * files start with 0x00, so we recognize that case.  Potentially, however,
 * a Targa file could start with any byte value (byte 0 is the length of the
 * seldom-used ID field), so we provide a switch to force Targa input mode.
 */

void Ccjpeg::select_file_type()
{
	unsigned int fType		= _GetFileType(infilename);
	src_mgr					= NULL;

	switch (fType)
	{
#ifdef BMP_SUPPORTED
		case FILETYPE_BMP :
		{
			src_mgr = jinit_read_bmp(&cinfo);
			break;
		}
#endif
#ifdef GIF_SUPPORTED
		case FILETYPE_GIF :
		{
			src_mgr = jinit_read_gif(&cinfo);
			break;
		}
#endif
#ifdef PPM_SUPPORTED
		case FILETYPE_PPM :
		{
			src_mgr = jinit_read_ppm(&cinfo);
			break;
		}
#endif
#ifdef RLE_SUPPORTED
		case FILETYPE_RLE :
		{
			src_mgr = jinit_read_rle(&cinfo);
			break;
		}
#endif
#ifdef TARGA_SUPPORTED
		case FILETYPE_TGA :
		{
			src_mgr = jinit_read_targa(&cinfo);
			break;
		}
#endif
		default:
		{
			if (is_targa)
			{
#ifdef TARGA_SUPPORTED
				src_mgr = jinit_read_targa(&cinfo);
#else
				ERREXIT(&cinfo, JERR_TGA_NOTCOMP);
#endif
				break;
			}
			ERREXIT(&cinfo, JERR_UNKNOWN_FORMAT);
			break;
		}
	}
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
int Ccjpeg::parse_switches (lpcjpeg_switches lpSwitches, boolean isReal)
{
	if (lpSwitches == NULL) return 6;

	if (!isReal)
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
	} // else we've got these already

		/* Set up default JPEG parameters. */
		/* Note that default -quality level need not, and does not,
		 * match the default scaling for an explicit -qtables argument.
		 */
	int quality					= 75;		/* -quality parameter */
	int q_scale_factor			= 100;		/* scaling percentage for -qtables */
	boolean force_baseline		= FALSE;
	boolean simple_progressive	= FALSE;
//	char* qtablefile = NULL;		/* saves -qtables filename if any */
//	char* qslotsarg = NULL;			/* saves -qslots parm if any */
//	char* samplearg = NULL;			/* saves -sample parm if any */
//	char* scansarg = NULL;			/* saves -scans parm if any */
	cinfo.err->trace_level = 0;

	if (lpSwitches->arithmetic == true)
#ifdef C_ARITH_CODING_SUPPORTED
		cinfo.arith_code = TRUE; // Use arithmetic coding.
#else
		warnMessage("%s: sorry, arithmetic coding not supported", progname);
#endif

	if (lpSwitches->baseline == true)
		force_baseline = TRUE; // Force baseline-compatible output (8-bit quantizer values)

	if (lpSwitches->DCT_islow == true)
		cinfo.dct_method = JDCT_ISLOW;
	else if (lpSwitches->DCT_ifast == true)
		cinfo.dct_method = JDCT_IFAST;
	else if (lpSwitches->DCT_float == true)
		cinfo.dct_method = JDCT_FLOAT;

	if (lpSwitches->grayscale == true)
		jpeg_set_colorspace(&cinfo, JCS_GRAYSCALE); // Force a monochrome JPEG file to be generated.

	if (lpSwitches->optimize == true)
#ifdef ENTROPY_OPT_SUPPORTED
		cinfo.optimize_coding = TRUE; // Enable entropy parm optimization.
#else
		warnMessage("%s: sorry, entropy optimization was not compiled", progname);
#endif

	if (lpSwitches->progressive == true)
#ifdef C_PROGRESSIVE_SUPPORTED
		simple_progressive = TRUE; // Select simple progressive mode
#else
		warnMessage("%s: sorry, progressive output was not compiled", progname);
#endif

	if ((lpSwitches->quality > 0) && (lpSwitches->quality <= 100)) // default would be '0'
		quality = lpSwitches->quality;
	q_scale_factor = jpeg_quality_scaling(quality); // Quality factor (quantization table scaling factor)

	if ((lpSwitches->smooth > 0) && (lpSwitches->smooth <= 100)) // default would be '0'
		cinfo.smoothing_factor = lpSwitches->smooth; // Set input smoothing factor

	is_targa = (lpSwitches->targa == true); // Input file is Targa format

	if (isReal)
	{
		/* Post-switch-scanning cleanup */

		/* Set quantization tables for selected quality. */
		/* Some or all may be overridden if -qtables is present. */
		jpeg_set_quality(&cinfo, quality, force_baseline);

#ifdef C_PROGRESSIVE_SUPPORTED
		if (simple_progressive)	/* process -progressive; -scans can override */
			jpeg_simple_progression(&cinfo);
#endif
	}

	return 0;			/* OK */
	// return -1	= input/output file not defined in param-struct
	// return -2	= input does not exist
	// return -3	= output-path (disk+dir) does not exist
	// return -4	= output exist and may not be overwritten
	// return -5	= output protection-error : ReadOnly or Access Denied
}

int Ccjpeg::Initialize(lpcjpeg_switches lpSwitches)
{
	if (lpSwitches == NULL) return -1;
	/* Initialize the JPEG compression object with default error handling. */
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
		jpeg_create_compress(&cinfo);
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
		enable_signal_catcher((j_common_ptr) &cinfo);
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
	cinfo.in_color_space = JCS_RGB; /* arbitrary guess */

	__try
	{
		jpeg_set_defaults(&cinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		return -5;
	}
	
	/* Scan command line to find file names.
	 * It is convenient to use just one switch-parsing routine, but the switch
	 * values read here are ignored; we will rescan the switches after opening
	 * the input file.
	 */
	int paramError;
	if ((paramError=parse_switches(lpSwitches, FALSE)) != 0)
		return (paramError * -1); // some error occured

	/* Open the input file. */
	if ((input_file = fopen(infilename, READ_BINARY)) == NULL)
	{
		errMessage(_T("%s: can't open [%s] for READING"), progname, infilename);
		return -7;
    }

	/* Open the output file. */
	if ((output_file = fopen(outfilename, WRITE_BINARY)) == NULL)
	{
		errMessage(_T("%s: can't open [%s] for WRITING"), progname, outfilename);
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		return -8;
	}

	/* Figure out the input file format, and set up to read it. */
	__try
	{
		select_file_type();
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -9;
	}
	if (! src_mgr)
	{// == NULL
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -10;
	}

	src_mgr->input_file = input_file;
	/* Read the input file header to obtain file size & colorspace. */
	__try
	{
		(*src_mgr->start_input) (&cinfo, src_mgr);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -11;
	}

	/* Now that we know input colorspace, fix colorspace-dependent defaults */
	__try
	{
		jpeg_default_colorspace(&cinfo);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -12;
	}
	/* Adjust default compression parameters by re-parsing the options */
	parse_switches(lpSwitches, TRUE); // must assume all wend well, while first time wend well too

	/* Specify data destination for compression */
	__try
	{
		jpeg_stdio_dest(&cinfo, output_file);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -13;
	}

	return 0;
}

int Ccjpeg::Processor()
{
	/* Start compressor */
	__try
	{
		jpeg_start_compress(&cinfo, TRUE);
	}
	__except( GetExceptionCode() != 0 )
	{
		// something wend wrong, exiting
		fclose(input_file); input_file=NULL; // be sure it's closed on exit
		fclose(output_file); output_file=NULL; // be sure it's closed on exit
		DeleteFile(outfilename); // remove the outfile
		return -1;
	}

	/* Process data */
	while (cinfo.next_scanline < cinfo.image_height)
	{
		num_scanlines = (*src_mgr->get_pixel_rows) (&cinfo, src_mgr);
		__try
		{
			(void) jpeg_write_scanlines(&cinfo, src_mgr->buffer, num_scanlines);
		}
		__except( GetExceptionCode() != 0 )
		{
			// something wend wrong, exiting
			fclose(input_file); input_file=NULL; // be sure it's closed on exit
			fclose(output_file); output_file=NULL; // be sure it's closed on exit
			DeleteFile(outfilename); // remove the outfile
			return -2;
		}
	}

	return 0;
}

int Ccjpeg::Finalize()
{
	/* Finish compression and release memory */
	__try
	{
		(*src_mgr->finish_input) (&cinfo, src_mgr);
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
		jpeg_finish_compress(&cinfo);
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
		jpeg_destroy_compress(&cinfo);
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

int Ccjpeg::Run(lpcjpeg_switches lpSwitches)
{
	// Initialize
	int initError;
	if ((initError=Initialize(lpSwitches)) != 0) return (initError > 0) ? (initError+20) : -1;
	// return 21	= input/output file not defined in param-struct
	// return 22	= input does not exist
	// return 23	= output-path (disk+dir) does not exist
	// return 24	= output exist and may not be overwritten
	// return 25	= output protection-error : ReadOnly or Access Denied

	// Processor
	if (Processor() != 0) return -2;

	// Finalize
	if (Finalize() != 0) return -3;

	/* All done. */
	return (jerr.num_warnings ? EXIT_WARNING : EXIT_SUCCESS);	/* suppress no-return-value warnings */
	// finalize_failure		= -3
	// process_failure		= -2
	// init_failure			= -1
	// EXIT_SUCCESS			= 0
	// EXIT_FAILURE			= 1
	// EXIT_WARNING			= 2
}

// ///////////////////////////////////////////////////////////////////
// The Thread that let "cjpeg" work
// ///////////////////////////////////////////////////////////////////
void cjpeg_Thread( void* arg )
{
	// added since 1.6.0.0
	lpcjpeg_switches lpSwitches = (arg) ? (lpcjpeg_switches)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (lpSwitches == NULL)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	Ccjpeg cjpegClass = Ccjpeg();
	int retVal = EXIT_FAILURE; // 1 : General Failure, possible in JPeG-lib
	retVal = cjpegClass.Run(lpSwitches); // if Run don't exceeds normaly, will have an error in JPeG-lib
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}