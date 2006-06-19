// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// ppmtoyuv.cpp:	implementation of the Cppm2yuv class, part of the mjpgTools Library									 //
//					Converts a PPM stream into a (MJPEG-Tools-)YUV stream												 //
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
//	<ppmtoy4m.c> of the MJPEGtools v1.6.0 (of the linux group)															 //
//																														 //
//	Generate a YUV4MPEG2 stream from one or more PPM images																 //
//	Converts R'G'B' to ITU-Rec.601 Y'CbCr colorspace and performs 4:2:0 chroma subsampling.								 //
//																														 //
//	Copyright (C) 2001 Matthew J. Marjanovic <maddog@mir.com>															 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include "stdafx.h"

#include <io.h>						// for low-level file-routines
#include <sys\stat.h>				// for low-level file-routines
#include <fcntl.h>					// for low-level file-routines
#include <stdio.h>

#include ".\mjpeglib\config.h"
#include ".\mjpeglib\yuv4mpeg.h"
#include ".\mjpeglib\mpegconsts.h"
#include ".\mjpeglib\subsample.h"
#include ".\mjpeglib\colorspace.h"

#define MJPGTOOLS_INTERNALS
#include "mjpgTools.h"
#include "privateMessages.h"		// for 'lk_...' strings
#include ".\tools\tools.h"			// for 'Message' and 'AskMessage'
#include "ppmtoyuv.h"


/* command-line parameters */
typedef struct _cl_info_ppm2yuv {
	y4m_ratio_t aspect;    
	y4m_ratio_t framerate; 
	int			interlace;         
	int			interleave;
	int			offset;
	unsigned	framecount;
	unsigned	min_frame_count;
	int			repeatlast;
	int			ss_mode;
	int			verbosity;
	int			fdin;
	int			fdout;
} ppm2yuv_info_t;


/* PPM image information */
typedef struct _ppm_info {
	int width;
	int height;
} ppm_info_t;


class Cppm2yuv
{
public:
	Cppm2yuv(HWND _hOwner=0, UINT _oMsg=0);
	~Cppm2yuv();

	virtual int Run( lp_ppm2yuv_switches lpSwitches );

protected:
	LPTSTR*				inFilename;				// filename(s) of input
	char				outfilename[MAX_PATH];	// filename of output
	LPCTSTR				_internal_progname;
	LPCTSTR				_default_extension;		// ".yuv" (with the leading dot)

	ppm2yuv_info_t		cl;

	HWND				m_hOwner;
	UINT				m_ownerMsg;
	BOOL				m_ProgressHasInit;		// TRUE if (on failure/destruction) an End-message still must be send
	unsigned*			m_totalFrames;

private:
	int ratio_parser( const y4m_ratio_t ratio_src, y4m_ratio_t* ratio_dst );
	int parse_switches( lp_ppm2yuv_switches lpSwitches, BOOL forReal );
	int read_ppm_header(int* width, int* height);
	void alloc_buffers(UINT8* buffers[], int width, int height);
	void read_ppm_into_two_buffers(UINT8* buffers[], UINT8* buffers2[], UINT8* rowbuffer, int width, int height);
	void read_ppm_into_one_buffer(UINT8* buffers[], UINT8* rowbuffer, int width, int height);
	int read_ppm_frame(ppm_info_t* ppm, UINT8* buffers[], UINT8* buffers2[], int ilace, int ileave);
	int setup_output_stream(y4m_stream_info_t* sinfo, ppm_info_t* ppm, int* field_height);

	int Initialize( lp_ppm2yuv_switches lpSwitches );
//	void Process(void);
	void ProcessMulti(void);
};


//////////////////////////////////////////////////////////////////////
// Cppm2yuv : Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cppm2yuv::Cppm2yuv(HWND _hOwner, UINT _oMsg)
{
	m_hOwner			= _hOwner;
	m_ownerMsg			= _oMsg;
	m_totalFrames		= NULL;
	m_ProgressHasInit	= FALSE;

	_internal_progname	= _T("PPMtoYUV");
	_default_extension	= _T(".yuv");
	// fill with defaults
	ZeroMemory(&cl, sizeof(ppm_info_t));
	cl.aspect			= y4m_sar_SQUARE;
	cl.fdin				= -1; // default error : use '_open()' to get a handle to the input-file
	cl.fdout			= -1; // default error : use '_create()' to get a handle to the output-file
	cl.framecount		= 0;
	cl.framerate		= y4m_fps_PAL;
	cl.interlace		= Y4M_ILACE_NONE;
	cl.interleave		= 0;
	cl.offset			= 0;
	cl.repeatlast		= 0;
	cl.ss_mode			= SSM_420_JPEG;
#ifdef _DEBUG
	cl.verbosity		= 2; // verbose
#else
	cl.verbosity		= 0; // no verbosity
#endif
	inFilename			= NULL;
}

Cppm2yuv::~Cppm2yuv()
{
	if ((m_hOwner) && (m_ownerMsg) && (m_ProgressHasInit))
		lhkProgress_End((const void*)m_hOwner, (const unsigned)m_ownerMsg);
	if (inFilename)
	{
		for (unsigned u=0; inFilename[u]; u++)
			free(inFilename[u]);
		free(inFilename);
		inFilename=NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// some MACRO's, used in 'read_ppm_header'
//////////////////////////////////////////////////////////////////////

#define DO_READ_NUMBER(var)	\
	do \
	{ \
		if (!isdigit(s[0])) \
			mjpeg_error_exit1("PPM read error:  bad char"); \
		(var) = ((var) * 10) + (s[0] - '0'); \
	} while (((n = _read(cl.fdin, s, 1)) == 1) && (!isspace(s[0]))); \
	if (n <= 0) return -1;                                        

#define DO_SKIP_WHITESPACE() \
	incomment = 0; \
	while ( ((n = _read(cl.fdin, s, 1)) == 1) && ((isspace(s[0])) || (s[0] == '#') || (incomment)) ) \
	{ \
		if (s[0] == '#') incomment = 1; \
		if (s[0] == '\n') incomment = 0; \
	} \
	if (n <= 0) return -1;                                         

//////////////////////////////////////////////////////////////////////
// Routines
//////////////////////////////////////////////////////////////////////

int Cppm2yuv::ratio_parser( const y4m_ratio_t ratio_src, y4m_ratio_t* ratio_dst )
{
	if (ratio_dst == NULL) return Y4M_ERR_RANGE;

	ratio_dst->n	= ratio_src.n;
	ratio_dst->d	= ratio_src.d;
	if (ratio_dst->d < 0) return Y4M_ERR_RANGE;
	/* 0:0 == unknown, so that is ok, otherwise zero denominator is bad */
	if ((ratio_dst->d == 0) && (ratio_dst->n != 0)) return Y4M_ERR_RANGE;
	y4m_ratio_reduce(ratio_dst);
	return Y4M_OK;
}

int Cppm2yuv::parse_switches( lp_ppm2yuv_switches lpSwitches, BOOL forReal )
{
	if (lpSwitches==NULL) return -1;

	if (!forReal)
	{
		char fdisk[_MAX_DRIVE], fdir[_MAX_DIR], fname[_MAX_FNAME];
		unsigned u, i;

		if ((lpSwitches->infile) && (lpSwitches->infile[0]))
		{
			u = 1;
			if ((lpSwitches->infile2) && (lpSwitches->infile2[0]))
			{ // first count amount of strings in this array
				for (i=0; lpSwitches->infile2[i]; )
				{
					i += strlen(&lpSwitches->infile2[i]) + 1;
					u++;
				}
			}
			inFilename = (char**)(malloc(sizeof(char*) * (u+1)));
			u = 0;
			inFilename[u++] = strdup(lpSwitches->infile);
			if ((lpSwitches->infile2) && (lpSwitches->infile2[0]))
			{
				for (i=0; lpSwitches->infile2[i];)
				{
					inFilename[u++] = strdup(&lpSwitches->infile2[i]);
					i += strlen(&lpSwitches->infile2[i]) + 1;
				}
			}
			inFilename[u] = NULL;
//			strcpy(infilename, lpSwitches->infile); // Set input file name.
		}
		else
		{
			errMessage(lk_NO_INPUT_MESSAGE, _internal_progname);
			return 21;
		}
		for (u=0; inFilename[u]; u++)
		{
			// See if the input really exists
			if (! FileExists(inFilename[u]))
			{
				errMessage(lk_INPUT_NOT_FOUND, inFilename[u]);
				return 22;
			}
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

		return 0; // no need to continue, while in first pass
	} // else we've got these already

	// verbosity
	cl.verbosity = ((lpSwitches->verbose >= 1) && (lpSwitches->verbose <= 3)) ? lpSwitches->verbose -1 : cl.verbosity;

	// pixel_aspect_ratio
	if (!Y4M_RATIO_EQL(lpSwitches->aspect_ratio, y4m_sar_UNKNOWN))
		if (ratio_parser(lpSwitches->aspect_ratio, &(cl.aspect)) != Y4M_OK)
		{
			errMessage("[%s]\nCould not parse aspect ratio :  '%ld:%ld'", _internal_progname, lpSwitches->aspect_ratio.n, lpSwitches->aspect_ratio.d);
			return -2;
		}
	// else take default

	// frame_rate
	if (!Y4M_RATIO_EQL(lpSwitches->frame_rate, y4m_fps_UNKNOWN))
		if (ratio_parser(lpSwitches->frame_rate, &(cl.framerate)) != Y4M_OK)
		{
			errMessage("[%s]\nCould not parse frame rate :  '%ld:%ld'", _internal_progname, lpSwitches->frame_rate.n, lpSwitches->frame_rate.d);
			return -3;
		}
	// else take default

	// interlacing
	switch (lpSwitches->interlacing)
	{
		case 'T' :
		case 't' :
		{ // top-field-first
			cl.interlace = Y4M_ILACE_TOP_FIRST;
			break;
		}
		case 'B' :
		case 'b' :
		{ // bottom-field-first
			cl.interlace = Y4M_ILACE_BOTTOM_FIRST;
			break;
		}
//		case 0 :
//		case 'P' :
//		case 'p' :
		default :
		{ // none/progressive
			cl.interlace = Y4M_ILACE_NONE;
			break;
		}
	}

	// interleave_2Pass
	cl.interleave	= (lpSwitches->interleave_2Pass == true) ? 1 : cl.interleave;

	// skip_frame_offset
	cl.offset		= lpSwitches->skip_frame_offset;

	// frame_count
	cl.framecount	= lpSwitches->frame_count;

	// min_frame_count
	cl.min_frame_count = lpSwitches->min_frame_count;

	// repeat_last
	cl.repeatlast	= (lpSwitches->repeat_last == true) ? 1 : cl.repeatlast;

	// chroma
	cl.ss_mode		= (lpSwitches->chroma == 1) ? SSM_UNKNOWN : ((lpSwitches->chroma == 2) ? SSM_420_MPEG2 : SSM_420_JPEG);

	if (lpSwitches->version >= 1)
	{
		m_totalFrames = lpSwitches->frames;
	}

	// verbose : always off, except when debugging
	mjpeg_default_handler_verbosity(cl.verbosity);
	return 0;
}


int Cppm2yuv::read_ppm_header(int* width, int* height)
{
	char s[6];
	int incomment;
	int n;
	int maxval = 0;

	*width	= 0;
	*height	= 0;

	/* look for "P6" */
	if ((n = y4m_read(cl.fdin, s, 3)) > 0)
		return 1;  /* EOF */

	if ((n < 0) || (strncmp(s, "P6", 2)))
		mjpeg_error_exit1("Bad Raw PPM magic!");

	incomment = 0;
	DO_SKIP_WHITESPACE();
	DO_READ_NUMBER(*width);
	DO_SKIP_WHITESPACE();
	DO_READ_NUMBER(*height);
	DO_SKIP_WHITESPACE();
	DO_READ_NUMBER(maxval);

	if (maxval != 255)
		mjpeg_error_exit1("Expecting maxval == 255, not %d!", maxval);

	return 0;
}

void Cppm2yuv::alloc_buffers(UINT8* buffers[], int width, int height)
{
  mjpeg_debug("Alloc'ing buffers");
  buffers[0] = (UINT8*)(malloc(width * height * 2 * sizeof(buffers[0][0])));
  buffers[1] = (UINT8*)(malloc(width * height * 2 * sizeof(buffers[1][0])));
  buffers[2] = (UINT8*)(malloc(width * height * 2 * sizeof(buffers[2][0])));
}

void Cppm2yuv::read_ppm_into_two_buffers(UINT8* buffers[], UINT8* buffers2[], UINT8* rowbuffer, int width, int height)
{
	int x, y;
	UINT8* pixels;
	UINT8* R	= buffers[0];
	UINT8* G	= buffers[1];
	UINT8* B	= buffers[2];
	UINT8* R2	= buffers2[0];
	UINT8* G2	= buffers2[1];
	UINT8* B2	= buffers2[2];

	mjpeg_debug("read into two buffers, %dx%d", width, height);
	height /= 2;
	for (y = 0; y < height; y++)
	{
		pixels = rowbuffer;
		if (y4m_read(cl.fdin, pixels, width * 3))
			mjpeg_error_exit1("read error A  y=%d", y);
		for (x = 0; x < width; x++)
		{
			*(R++) = *(pixels++);
			*(G++) = *(pixels++);
			*(B++) = *(pixels++);
		}
		pixels = rowbuffer;
		if (y4m_read(cl.fdin, pixels, width * 3))
			mjpeg_error_exit1("read error B  y=%d", y);
		for (x = 0; x < width; x++)
		{
			*(R2++) = *(pixels++);
			*(G2++) = *(pixels++);
			*(B2++) = *(pixels++);
		}
	}
}

void Cppm2yuv::read_ppm_into_one_buffer(UINT8* buffers[], UINT8* rowbuffer, int width, int height)
{
	int x, y;
	UINT8* pixels;
	UINT8* R	= buffers[0];
	UINT8* G	= buffers[1];
	UINT8* B	= buffers[2];

	for (y = 0; y < height; y++)
	{
		pixels = rowbuffer;
		y4m_read(cl.fdin, pixels, width * 3);
		for (x = 0; x < width; x++)
		{
			*(R++) = *(pixels++);
			*(G++) = *(pixels++);
			*(B++) = *(pixels++);
		}
	}
}


// read one whole frame
// if field-sequential interlaced, this requires reading two PPM images
//
// if interlaced, fields are written into separate buffers
//
// ppm.width/height refer to image dimensions
int Cppm2yuv::read_ppm_frame(ppm_info_t* ppm, UINT8* buffers[], UINT8* buffers2[], int ilace, int ileave)
{
	int width, height;
	static UINT8* rowbuffer = NULL;
	int err = read_ppm_header(&width, &height);

	if (err > 0) return 1;  /* EOF */
	if (err < 0) return -1; /* error */
	mjpeg_debug("Got PPM header:  %dx%d", width, height);

	if (ppm->width == 0)
	{ // first time
		mjpeg_debug("Initializing PPM read_frame");
		ppm->width	= width;
		ppm->height	= height;
		rowbuffer = (UINT8*)(malloc(width * 3 * sizeof(rowbuffer[0])));
	}
	else
	{ // make sure everything matches
		if ( (ppm->width != width) || (ppm->height != height) )
			mjpeg_error_exit1("One of these frames is not like the others!");
	}
	if (buffers[0] == NULL) 
		alloc_buffers(buffers, width, height);
	if ((buffers2[0] == NULL) && (ilace != Y4M_ILACE_NONE))
		alloc_buffers(buffers2, width, height);

	mjpeg_debug("Reading rows");

	if ((ilace != Y4M_ILACE_NONE) && (ileave))
	{ // Interlaced and Interleaved: --> read image and deinterleave fields at same time
		if (ilace == Y4M_ILACE_TOP_FIRST)
		{ // 1st buff arg == top field == temporally first == "buffers"
			read_ppm_into_two_buffers(buffers, buffers2, rowbuffer, width, height);
		}
		else
		{ // bottom-field-first --> 1st buff art == top field == temporally second == "buffers2"
			read_ppm_into_two_buffers(buffers2, buffers, rowbuffer, width, height);
		}      
	}
	else if ((ilace == Y4M_ILACE_NONE) || (!ileave))
	{ // Not Interlaced, or Not Interleaved: --> read image into first buffer...
		read_ppm_into_one_buffer(buffers, rowbuffer, width, height);
		if ((ilace != Y4M_ILACE_NONE) && (!ileave))
		{ // ...Actually Interlaced: --> read the second image/field into second buffer
			if ((err = read_ppm_header(&width, &height)) != 0) return 1;  /* EOF */
			mjpeg_debug("Got PPM header:  %dx%d", width, height);

			// make sure everything matches
			if ( (ppm->width != width) || (ppm->height != height) )
				mjpeg_error_exit1("One of these frames is not like the others!");
			read_ppm_into_one_buffer(buffers2, rowbuffer, width, height);
		}
	}
	return 0;
}

int Cppm2yuv::setup_output_stream(y4m_stream_info_t* sinfo, ppm_info_t* ppm, int* field_height)
{
	if ((ppm->width % 2) != 0)
	{
		mjpeg_error("PPM width (%d) is not a multiple of 2!", ppm->width);
		return -1;
	}

	if (((ppm->height % 2) != 0) && (cl.interlace == Y4M_ILACE_NONE))
	{
		mjpeg_error("PPM height (%d) is not a multiple of 2!", ppm->height);
		return -2;
	}
	else if (((ppm->height % 4) != 0) && (cl.interlace != Y4M_ILACE_NONE))
	{
		mjpeg_error("PPM height (%d) is not a multiple of 4!", ppm->height);
		return -3;
	}

	y4m_si_set_width(sinfo, ppm->width);
	if (cl.interlace == Y4M_ILACE_NONE)
	{
		y4m_si_set_height(sinfo, ppm->height);
		*field_height = ppm->height;
	}
	else if (cl.interleave)
	{
		y4m_si_set_height(sinfo, ppm->height);
		*field_height = ppm->height / 2;
	}
	else
	{
		y4m_si_set_height(sinfo, ppm->height * 2);
		*field_height = ppm->height;
	}
	y4m_si_set_sampleaspect(sinfo, cl.aspect);
	y4m_si_set_interlace(sinfo, cl.interlace);
	y4m_si_set_framerate(sinfo, cl.framerate);

	y4m_write_stream_header(cl.fdout, sinfo);

	mjpeg_info("Output Stream parameters:");
	y4m_log_stream_info(LOG_INFO, "  ", sinfo);

	return 0;
}

//////////////////////////////////////////////////////////////////////
// MAIN Routines
//////////////////////////////////////////////////////////////////////

int Cppm2yuv::Initialize( lp_ppm2yuv_switches lpSwitches )
{
	// first step of initialisation : get the in-/out- filenames
	int InitError;
	if ( (InitError=parse_switches(lpSwitches, FALSE)) != 0) return (InitError > 0) ? InitError : -1;

	if (!inFilename) return -2;
	// open the input-file
	if ((cl.fdin=_open(inFilename[0], _O_BINARY | _O_RDONLY)) == -1) return -2; // seems not to be able to open the input
	// create the output-file
	if ((cl.fdout=_open(outfilename, _O_BINARY | _O_CREAT | _O_RDWR | _O_TRUNC, _S_IREAD | _S_IWRITE)) == -1)
	{
		_close(cl.fdin); // close input-file
		cl.fdin=-1;
		return -3; // seems we're not able to create this file
	}

	// now parse the switches for the 'REAL' time
	parse_switches(lpSwitches, TRUE);


	return 0;
}
/*
void Cppm2yuv::Process()
{
	y4m_stream_info_t	sinfo;
	y4m_frame_info_t	finfo;
	UINT8*			buffers[3];				// R'G'B' or Y'CbCr
	UINT8*			buffers2[3];			// R'G'B' or Y'CbCr
	ppm_info_t			ppm;
	int					field_height;

	int err, i, count, repeating_last;

	y4m_init_stream_info(&sinfo);
	y4m_init_frame_info(&finfo);

	ppm.width = 0;
	ppm.height = 0;
	for (i = 0; i < 3; i++)
	{
		buffers[i] = NULL;
		buffers2[i] = NULL;
	}

	// Read first PPM frame/field-pair, to get dimensions
	if (read_ppm_frame(&ppm, buffers, buffers2, cl.interlace, cl.interleave))
	{
		Cleanup();
		mjpeg_error_exit1("Failed to read first frame.");
	}

	// Setup streaminfo and write output header
	setup_output_stream(&sinfo, &ppm, &field_height);

	// Loop 'framecount' times, or possibly forever...
	for (count = 0, repeating_last = 0; (count < (cl.offset + cl.framecount)) || (cl.framecount == 0); count++)
	{
		if (repeating_last) goto WRITE_FRAME;

		// Read PPM frame/field
		// ...but skip reading very first frame, already read prior to loop
		if (count > 0)
		{
			if ((err = read_ppm_frame(&ppm, buffers, buffers2,  cl.interlace, cl.interleave)) == 1)
			{ // clean input EOF
				if (cl.repeatlast)
				{
					repeating_last = 1;
					goto WRITE_FRAME;
				}
				else if (cl.framecount != 0)
				{
					Cleanup();
					mjpeg_error_exit1("Input frame shortfall (only %d converted).", count - cl.offset);
				}
				else
				{
					break;  // input is exhausted; we are done!  time to go home!
				}
			}
			else if (err)
			{
				Cleanup();
				mjpeg_error_exit1("Error reading ppm frame");
			}
		}
    
		// * ...skip transforms if we are just going to skip this frame anyway.
			BUT, if 'cl.repeatlast' is on, we must process/buffer every frame,
			because we don't know when we will see the last one. * //
		if ((count >= cl.offset) || (cl.repeatlast))
		{ // Transform colorspace, then subsample (in place)
			convert_RGB_to_YCbCr(buffers, ppm.width * field_height);
			chroma_subsample((subsample_mode_t)cl.ss_mode, buffers, ppm.width, field_height);
			if (cl.interlace != Y4M_ILACE_NONE)
			{
				convert_RGB_to_YCbCr(buffers2, ppm.width * field_height);
				chroma_subsample((subsample_mode_t)cl.ss_mode, buffers2, ppm.width, field_height);
			}
		}

WRITE_FRAME:
		// Write converted frame to output
		if (count >= cl.offset)
		{
			switch (cl.interlace)
			{
				case Y4M_ILACE_NONE :
				{
					y4m_write_frame(cl.fdout, &sinfo, &finfo, buffers);
					break;
				}
				case Y4M_ILACE_TOP_FIRST :
				{
					y4m_write_fields(cl.fdout, &sinfo, &finfo, buffers, buffers2);
					break;
				}
				case Y4M_ILACE_BOTTOM_FIRST :
				{
					y4m_write_fields(cl.fdout, &sinfo, &finfo, buffers2, buffers);
					break;
				}
				default:
				{
					Cleanup();
					mjpeg_error_exit1("Unknown ilace type!   %d", cl.interlace);
					break;
				}
			}
		} 
	}

	for (i = 0; i < 3; i++)
	{
		free(buffers[i]);
		free(buffers2[i]);
	}
	y4m_fini_stream_info(&sinfo);
	y4m_fini_frame_info(&finfo);

	mjpeg_debug("Done.");
}
*/
void Cppm2yuv::ProcessMulti()
{
	y4m_stream_info_t	sinfo;
	y4m_frame_info_t	finfo;
	UINT8*				buffers[3];				// R'G'B' or Y'CbCr
	UINT8*				buffers2[3];			// R'G'B' or Y'CbCr
	ppm_info_t			ppm;
	int					field_height;

	unsigned			curFile;
	char				prgText[256], prgFname[_MAX_FNAME], prgFext[_MAX_EXT];

	int err, i, repeating_last;
	unsigned count;

	y4m_init_stream_info(&sinfo);
	y4m_init_frame_info(&finfo);

	ppm.width = 0;
	ppm.height = 0;
	for (i = 0; i < 3; i++)
	{
		buffers[i] = NULL;
		buffers2[i] = NULL;
	}

	// Read first PPM frame/field-pair, to get dimensions
	if (read_ppm_frame(&ppm, buffers, buffers2, cl.interlace, cl.interleave))
		mjpeg_error_exit1("Failed to read first frame.");

	// Setup streaminfo and write output header
	setup_output_stream(&sinfo, &ppm, &field_height);

	if ((m_hOwner) && (m_ownerMsg))
	{
		// count amount of files to do, if we're supposed to transmit progress-info
		for (curFile = 0; inFilename[curFile]; curFile++) ;
		// notify owner we're about to start
		lhkProgress_Init((const void*)m_hOwner, (const unsigned)m_ownerMsg, "Creating YUV", (const long)curFile);
		m_ProgressHasInit = TRUE;
	}

	unsigned frames=0;
	for (curFile = 0; inFilename[curFile]; curFile++)
	{
		if ((m_hOwner) && (m_ownerMsg))
		{
			_splitpath(inFilename[curFile], NULL, NULL, prgFname, prgFext);
			sprintf(prgText, "Processing item #%d, <%s%s>", curFile+1, prgFname, prgFext);
			lhkProgress_Step((const void*)m_hOwner, (const unsigned)m_ownerMsg, prgText, (const long)curFile);
		}

		if (curFile > 0)
		{
			_close(cl.fdin);
			// open the input-file
			if ((cl.fdin=_open(inFilename[curFile], _O_BINARY | _O_RDONLY)) == -1) break; // seems not to be able to open the input
			if (!inFilename[curFile+1])
				Sleep(0);
		}

		// Loop 'framecount' times, or possibly forever...
		for (count = 0, repeating_last = 0; (count < (cl.offset + cl.framecount)) || (cl.framecount == 0) || ((!inFilename[curFile+1]) && (frames < cl.min_frame_count)); count++, frames++)
		{
			if (repeating_last) goto WRITE_FRAME;

			// Read PPM frame/field
			// ...but skip reading very first frame, already read prior to loop
			if ((count > 0) || (curFile > 0))
			{
				if ((err = read_ppm_frame(&ppm, buffers, buffers2,  cl.interlace, cl.interleave)) == 1)
				{ // clean input EOF
					if (cl.repeatlast)
					{
						repeating_last = 1;
						goto WRITE_FRAME;
					}
					else if (cl.framecount != 0)
						mjpeg_error_exit1("Input frame shortfall (only %d converted).", count - cl.offset);
					else
					{
						break;  // input is exhausted; we are done!  time to go home!
					}
				}
				else if (err)
					mjpeg_error_exit1("Error reading ppm frame");
			}
    
			/*	...skip transforms if we are just going to skip this frame anyway.
				BUT, if 'cl.repeatlast' is on, we must process/buffer every frame,
				because we don't know when we will see the last one. */
			if ((count >= (unsigned)cl.offset) || (cl.repeatlast))
			{ // Transform colorspace, then subsample (in place)
				convert_RGB_to_YCbCr(buffers, ppm.width * field_height);
				chroma_subsample((subsample_mode_t)cl.ss_mode, buffers, ppm.width, field_height);
				if (cl.interlace != Y4M_ILACE_NONE)
				{
					convert_RGB_to_YCbCr(buffers2, ppm.width * field_height);
					chroma_subsample((subsample_mode_t)cl.ss_mode, buffers2, ppm.width, field_height);
				}
			}

WRITE_FRAME:
			// Write converted frame to output
			if (count >= (unsigned)cl.offset)
			{
				switch (cl.interlace)
				{
					case Y4M_ILACE_NONE :
					{
						y4m_write_frame(cl.fdout, &sinfo, &finfo, buffers);
						break;
					}
					case Y4M_ILACE_TOP_FIRST :
					{
						y4m_write_fields(cl.fdout, &sinfo, &finfo, buffers, buffers2);
						break;
					}
					case Y4M_ILACE_BOTTOM_FIRST :
					{
						y4m_write_fields(cl.fdout, &sinfo, &finfo, buffers2, buffers);
						break;
					}
					default:
					{
						mjpeg_error_exit1("Unknown ilace type!   %d", cl.interlace);
						break;
					}
				}
			} 
		}

		if ((m_hOwner) && (m_ownerMsg))
			lhkProgress_Step((const void*)m_hOwner, (const unsigned)m_ownerMsg, prgText, (const long)curFile+1);
	}
	if (m_totalFrames)
		*m_totalFrames = frames;

	for (i = 0; i < 3; i++)
	{
		free(buffers[i]);
		free(buffers2[i]);
	}
	y4m_fini_stream_info(&sinfo);
	y4m_fini_frame_info(&finfo);

	mjpeg_debug("Done.");
	if ((m_hOwner) && (m_ownerMsg))
	{
		lhkProgress_End((const void*)m_hOwner, (const unsigned)m_ownerMsg);
		m_ProgressHasInit = FALSE;
	}
}

int Cppm2yuv::Run( lp_ppm2yuv_switches lpSwitches )
{
	// initialize
	int InitError;
	__try
	{
		InitError = Initialize(lpSwitches);
	}
	__except( GetExceptionCode() != 0 )
	{
		if (cl.fdin != -1) _close(cl.fdin); // close input-file
		if (cl.fdout != -1) _close(cl.fdout); // and close output-file
		// something wend wrong, exiting
		return -1;
	}
	if (InitError != 0) return (InitError > 0) ? InitError : -1;

	// Processing
	__try
	{
		ProcessMulti();
	}
	__except( GetExceptionCode() != 0 )
	{
		if (cl.fdin != -1) _close(cl.fdin); // close input-file
		if (cl.fdout != -1) _close(cl.fdout); // and close output-file
		// something wend wrong, exiting
		return -2;
	}

	if (cl.fdin != -1) _close(cl.fdin); // close input-file
	if (cl.fdout != -1) _close(cl.fdout); // and close output-file

	return 0;
}


// ///////////////////////////////////////////////////////////////////
// The Thread that let "ppm2yuv" work
// ///////////////////////////////////////////////////////////////////
void ppm2yuv_Thread( void* arg )
{
	lp_ppm2yuv_switches lpSwitches = (arg) ? (lp_ppm2yuv_switches)((caller2thread_p)arg)->arg : NULL;
	HANDLE* event = ((arg) && (((caller2thread_p)arg)->event)) ? &((caller2thread_p)arg)->event : NULL;

	if (!lpSwitches)
	{
		if (event) SetEvent(*event);
		_endthread(); // wrong param -- shouldn't occure!
		return;
	}

	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

	Cppm2yuv* ppm2yuvClass;
	if (lpSwitches->version >= 1)
		ppm2yuvClass = new Cppm2yuv(lpSwitches->o_hWnd, lpSwitches->o_uMsg);
	else
		ppm2yuvClass = new Cppm2yuv();

	int retVal = 1; // 1 : General Failure, possible in mjpeglib-lib
	retVal = ppm2yuvClass->Run(lpSwitches); // if Run don't exceeds normaly, will have an error in MJPeG-lib
	// set the return value
	lpSwitches->m_ReturnCode = retVal;
	delete ppm2yuvClass;
	if (event) SetEvent(*event); // notifying caller-process we're done
	// _endthread(); // not required
}