// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// MpegEnc.cpp: implementation of the MpegEnc class, part of the mjpgTools Library										 //
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
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include "types.h"
#define GLOBAL // used by global.h
#include "global.h"
#include "motionsearch.h"
#include "fastintfns.h"
#include "..\mjpeglib\config.h"
#include "..\mjpeglib\mpegconsts.h"

#include "MpegEnc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MpegEnc::MpegEnc(const void* _hWnd, const unsigned _uWnd)
{
	m_hWnd								= _hWnd;
	m_uWnd								= _uWnd;

	Init_Data();

	mpg2enc_defTitle = "MpegEnc";
	istrm_fd							= -1;
	outfile								= NULL;
#ifdef OUTPUT_STAT
	statfile							= NULL;
#endif

	param_inFile[0]						= 0;
	outfilename[0]						= 0;
	// Command Parameter values.  These are checked and processed for defaults etc after parsing.
	// The resulting values then set the opt_ variables that control the actual encoder.
	param_format						= MPEG_FORMAT_MPEG1;
	param_bitrate						= 0;
	param_nonvid_bitrate				= 0;
	param_quant							= 0;
	param_searchrad						= 16;
	param_mpeg							= 1;
	param_aspect_ratio					= 0;
	param_frame_rate					= 0;
	param_fieldenc						= -1;	// 0 = progressive, 
												// 1 = frame pictures, interlace frames with field MC and DCT in picture 
												// 2 = field pictures
	param_norm							= 0;	// 'n': NTSC, 'p': PAL, 's': SECAM, else unspecified
	param_44_red						= 2;
	param_22_red						= 3;	
	param_hf_quant						= 0;
	param_act_boost						= 0.0;
	param_video_buffer_size				= 0;
	param_seq_length_limit				= 0;
	param_min_GOP_size					= -1;
	param_max_GOP_size					= -1;
	param_preserve_B					= 0;
	param_Bgrp_size						= 3;
	param_num_cpus						= 1;
	param_32_pulldown					= 0;
	param_svcd_scan_data				= -1;
	param_seq_hdr_every_gop				= 0;
	param_seq_end_every_gop				= 0;
	param_still_size					= 0;
	param_pad_stills_to_vbv_buffer_size	= 0;
	param_vbv_buffer_still_size			= 0;
	param_force_interlacing				= Y4M_UNKNOWN;
	param_input_interlacing				= 0;
	param_hack_svcd_hds_bug				= 1;
	param_hack_altscan_bug				= 0;
	param_mpeg2_dc_prec					= 1;

	// Input Stream parameter values that have to be further processed to set encoding options
	memset(&strm_aspect_ratio, 0, sizeof(mpeg_aspect_code_t));
	strm_frame_rate_code				= 0;

	// reserved: for later use
	param_422							= 0;
}

MpegEnc::~MpegEnc()
{
	if (outfile != NULL) { fclose(outfile); outfile=NULL; }
	if (istrm_fd != -1) { _close(istrm_fd); istrm_fd=-1; }
#ifdef OUTPUT_STAT
	if( statfile != NULL )
	{
		fclose(statfile);
		statfile=NULL;
	}
#endif

	Cleanup_lum_mean();
}

//////////////////////////////////////////////////////////////////////
// Protected Routines
//////////////////////////////////////////////////////////////////////

void MpegEnc::DisplayFrameRates()
{
	mpg2enc_msgTitle = "DisplayFrameRates";
 	uint32_t i;
	char s[1024];
	strcpy(s, "Frame-rate codes:");
	for( i = 0; i < mpeg_num_framerates; ++i )
	{
		sprintf(&s[strlen(s)], "\n%2d - %s", i, mpeg_framerate_code_definition(i));
	}

	int verb = mpg2enc_verbose;
	mpg2enc_verbose = 1;
	info_message(s);
	mpg2enc_verbose = verb;
}

void MpegEnc::DisplayAspectRatios()
{
	mpg2enc_msgTitle = "DisplayAspectRatios";
 	uint32_t i;
	char s[1024];
	strcpy(s, "Display aspect ratio codes:");
	for( i = 1; i <= mpeg_num_aspect_ratios[1]; ++i )
	{
		sprintf(&s[strlen(s)], "\n%2d - %s", i, mpeg_aspect_code_definition(2,i));
	}
	int verb = mpg2enc_verbose;
	mpg2enc_verbose = 1;
	info_message(s);
	mpg2enc_verbose = verb;

}

void MpegEnc::set_format_presets()
{
	mpg2enc_msgTitle = "MpegEnc::set_format_presets";
	switch (param_format)
	{
		case MPEG_FORMAT_MPEG1 :
		{ // Generic MPEG1
			info_message("Selecting generic MPEG1 output profile");
			if (param_video_buffer_size == 0)
				param_video_buffer_size = 46;
			if (param_bitrate == 0)
				param_bitrate = 1151929;
			break;
		}
		case MPEG_FORMAT_VCD :
		{
			param_mpeg = 1;
			param_bitrate = 1151929;
			param_video_buffer_size = 46;
			param_preserve_B = true;
			param_min_GOP_size = 9;
			param_max_GOP_size = ((param_norm == 'n') || (param_norm == 'N')) ? 18 : 15;
			info_message("VCD default options selected");
		} // continue with next case-block
		case MPEG_FORMAT_VCD_NSR :
		{ // VCD format, non-standard rate
			info_message( "Selecting VCD output profile");
			param_mpeg = 1;
			param_svcd_scan_data = 0;
			param_seq_hdr_every_gop = 1;
			if (param_bitrate == 0)
				param_bitrate = 1151929;
			if (param_video_buffer_size == 0)
				param_video_buffer_size = 46 * param_bitrate / 1151929;
			if (param_seq_length_limit == 0)
				param_seq_length_limit = 700;
			if (param_nonvid_bitrate == 0)
				param_nonvid_bitrate = 230;
			break;
		}
		case MPEG_FORMAT_MPEG2 :
		{
			param_mpeg = 2;
			info_message("Selecting generic MPEG2 output profile");
			if (param_fieldenc == -1)
				param_fieldenc = 1;
			if (param_video_buffer_size == 0)
				param_video_buffer_size = 46 * param_bitrate / 1151929;
			break;
		}
		case MPEG_FORMAT_SVCD :
		{
			info_message("SVCD standard settings selected");
			if (param_bitrate == 0)
				param_bitrate = 2520000;
			param_max_GOP_size = ((param_norm == 'n') || (param_norm == 'N')) ? 18 : 15;
			param_video_buffer_size = 230;
		} // continuing to next block
		case  MPEG_FORMAT_SVCD_NSR :
		{ // Non-standard data-rate
			info_message( "Selecting SVCD output profile");
			param_mpeg = 2;
			if (param_bitrate == 0)
				param_bitrate = 2520000;
			if (param_fieldenc == -1)
				param_fieldenc = 1;
			if (param_quant == 0)
				param_quant = 8;
			if (param_svcd_scan_data == -1)
				param_svcd_scan_data = 1;
			if (param_min_GOP_size == -1)
				param_min_GOP_size = 9;
			param_seq_hdr_every_gop = 1;
			if (param_seq_length_limit == 0)
				param_seq_length_limit = 700;
			if (param_nonvid_bitrate == 0)
				param_nonvid_bitrate = 230;
			if (param_video_buffer_size == 0)
				param_video_buffer_size = 230;
			break;
		}
		case MPEG_FORMAT_VCD_STILL :
		{
			info_message("Selecting VCD Stills output profile");
			param_mpeg = 1;
			// We choose a generous nominal bit-rate as its VBR anyway
			// there's only one frame per sequence ;-). It *is* too small
			// to fill the frame-buffer in less than one PAL/NTSC frame period though...
			param_bitrate = 8000000;

			// Now we select normal/hi-resolution based on the input stream resolution. 
			if ((opt_horizontal_size == 352) && ((opt_vertical_size == 240) || (opt_vertical_size == 288)))
			{
				// VCD normal resolution still
				if (param_still_size == 0)
					param_still_size = 30*1024;
				if ((param_still_size < 20*1024) || (param_still_size > 42*1024))
					errorExit_msg("VCD normal-resolution stills must be >= 20KB and <= 42KB each");
				// VBV delay encoded normally
				param_vbv_buffer_still_size = 46*1024;
				param_video_buffer_size = 46;
				param_pad_stills_to_vbv_buffer_size = 0;
			}
			else if ((opt_horizontal_size == 704) && ((opt_vertical_size == 480) || (opt_vertical_size == 576)))
			{
				// VCD high-resolution stills: only these use vbv_delay to encode picture size...
				if (param_still_size == 0)
					param_still_size = 125*1024;
				if ((param_still_size < 46*1024) || (param_still_size > 220*1024))
					errorExit_msg("VCD normal-resolution stills should be >= 46KB and <= 220KB each");
				param_vbv_buffer_still_size = param_still_size;
				param_video_buffer_size = 224;
				param_pad_stills_to_vbv_buffer_size = 1;			
			}
			else
				errorExit_msg("VCD normal resolution stills must be 352x288 (PAL) or 352x240 (NTSC)\nVCD high resolution stills must be 704x576 (PAL) or 704x480 (NTSC)");

			param_quant = 0;		// We want to try and hit our size target
			param_seq_hdr_every_gop = 1;
			param_seq_end_every_gop = 1;
			param_min_GOP_size = 1;
			param_max_GOP_size = 1;
			break;
		}
		case MPEG_FORMAT_SVCD_STILL :
		{
			info_message("Selecting SVCD Stills output profile");
			param_mpeg = 2;
			if (param_fieldenc == -1)
				param_fieldenc = 1;

			// We choose a generous nominal bit-rate as its VBR anyway
			// there's only one frame per sequence ;-). It *is* too small
			// to fill the frame-buffer in less than one PAL/NTSC frame period though...
			param_bitrate = 2500000;
			param_video_buffer_size = 230;
			param_vbv_buffer_still_size = 220*1024;
			param_pad_stills_to_vbv_buffer_size = 0;

			// Now we select normal/hi-resolution based on the input stream resolution. 
			if ((opt_horizontal_size == 480) && ((opt_vertical_size == 480) || (opt_vertical_size == 576)))
				info_message("SVCD normal-resolution stills selected.");
			else if ((opt_horizontal_size == 704) && ((opt_vertical_size == 480) || (opt_vertical_size == 576)))
				info_message("SVCD high-resolution stills selected.");
			else
				errorExit_msg("SVCD normal resolution stills must be 480x576 (PAL) or 480x480 (NTSC)\nSVCD high resolution stills must be 704x576 (PAL) or 704x480 (NTSC)");

			if ((param_still_size < 30*1024) || (param_still_size > 200*1024))
				errorExit_msg("SVCD resolution stills must be >= 30KB and <= 200KB each");

			param_seq_hdr_every_gop = 1;
			param_seq_end_every_gop = 1;
			param_min_GOP_size = 1;
			param_max_GOP_size = 1;
			break;
		}
		case MPEG_FORMAT_DVD :
		{
			info_message("Selecting DVD output profile");
			if (param_bitrate == 0)
				param_bitrate = 7500000;
			if (param_fieldenc == -1)
				param_fieldenc = 1;
			param_video_buffer_size = 230;
			param_mpeg = 2;
			if (param_quant == 0)
				param_quant = 8;
			param_seq_hdr_every_gop = 1;
			break;
		}
	}

    switch (param_mpeg)
    {
		case 1 :
		{
			if (param_min_GOP_size == -1)
				param_min_GOP_size = 12;
			if (param_max_GOP_size == -1)
				param_max_GOP_size = 12;
			break;
		}
		case 2 :
		{
			if (param_min_GOP_size == -1)
				param_min_GOP_size = 9;
			if (param_max_GOP_size == -1)
				param_max_GOP_size = ((param_norm == 'n') || (param_norm == 'N')) ? 18 : 15;
			break;
		}
    }
	if (param_svcd_scan_data == -1)
		param_svcd_scan_data = 0;
}

int MpegEnc::infer_mpeg1_aspect_code( char norm, mpeg_aspect_code_t mpeg2_code )
{
	int retVal = 0;
	switch (mpeg2_code)
	{
		case 1 :
		{ // 1:1
			retVal = 1;
			break;
		}
		case 2 :
		{ // 4:3
			if (((param_norm == 'p') || (param_norm == 'P')) || ((param_norm == 's') || (param_norm == 'S')))
				retVal = 8;
			else if ((param_norm == 'n') || (param_norm == 'N'))
				retVal = 12;
			break;
		}
		case 3 :
		{ // 16:9
			if (((param_norm == 'p') || (param_norm == 'P')) || ((param_norm == 's') || (param_norm == 'S')))
				retVal = 3;
			else if ((param_norm == 'n') || (param_norm == 'N'))
				retVal = 6;
			break;
		}
		default :
		{
			break;	// Unknown
		}
	}

	return retVal;
}

int MpegEnc::infer_default_params()
{
	mpg2enc_msgTitle = "MpegEnc::infer_default_params";
	int nerr = 0;

	// Infer norm, aspect ratios and frame_rate if not specified
	if (param_frame_rate == 0)
	{
		if ((strm_frame_rate_code < 1) || (strm_frame_rate_code > 8))
		{
			err_message("Input stream with unknown frame-rate and no frame-rate specified with /*a*/!");
			nerr++;
		}
		else
			param_frame_rate = strm_frame_rate_code;
	}

	if ((param_norm == 0) && ((strm_frame_rate_code == 3) || (strm_frame_rate_code == 2)))
	{
		info_message("Assuming norm PAL");
		param_norm = 'p';
	}
	if ((param_norm == 0) && ((strm_frame_rate_code == 4) || (strm_frame_rate_code == 1)))
	{
		info_message("Assuming norm NTSC");
		param_norm = 'n';
	}

	if (param_frame_rate != 0)
	{
		if ((strm_frame_rate_code != param_frame_rate) && (strm_frame_rate_code > 0) &&
			(strm_frame_rate_code < (int)mpeg_num_framerates))
		{
			warn_message("Specified display frame-rate %d/%d will over-ride\n(different!) frame-rate %d/%d of the input stream",
				mpeg_framerate(param_frame_rate).n, mpeg_framerate(param_frame_rate).d,
				mpeg_framerate(strm_frame_rate_code).n, mpeg_framerate(strm_frame_rate_code).d );
		}
		opt_frame_rate_code = param_frame_rate;
	}

	if (param_aspect_ratio == 0)
		param_aspect_ratio = strm_aspect_ratio;

	if (param_aspect_ratio == 0)
	{
		warn_message("No aspect ratio specifed and no guess possible: assuming 4:3 display aspect!");
		param_aspect_ratio = 2;
	}

	// Convert to MPEG1 coding if we're generating MPEG1
	if (param_mpeg == 1)
		param_aspect_ratio = infer_mpeg1_aspect_code(param_norm, param_aspect_ratio);

	return nerr;
}

int MpegEnc::check_param_constraints()
{
	mpg2enc_msgTitle = "MpegEnc::check_param_constraints";
	int nerr = 0;
	if (param_32_pulldown)
	{
		if (param_mpeg == 1)
			errorExit_msg("MPEG-1 cannot encode 3:2 pulldown (for transcoding to VCD set 24fps)!");

		if ((param_frame_rate != 4) && (param_frame_rate != 5))
		{
			if ((param_frame_rate == 1) || (param_frame_rate == 2))
			{
				param_frame_rate += 3;
				warn_message("3:2 movie pulldown with frame rate set to decode rate not display rate\n3:2 Setting frame rate code to display rate = %d (%d/%d fps)", 
						   param_frame_rate, mpeg_framerate(param_frame_rate).n, mpeg_framerate(param_frame_rate).d);
			}
			else
			{
				err_message("3:2 movie pulldown not sensible for %d/%d fps dispay rate",
							mpeg_framerate(param_frame_rate).n, mpeg_framerate(param_frame_rate).d);
				++nerr;
			}
		}
		if (param_fieldenc != 0)
		{
			err_message("3:2 pulldown only possible for frame pictures (-I 0)");
			++nerr;
		}
	}

	if ((unsigned)param_aspect_ratio > mpeg_num_aspect_ratios[param_mpeg-1]) 
	{
		err_message("For MPEG-%d aspect ratio code  %d > %d illegal", 
					param_mpeg, param_aspect_ratio, mpeg_num_aspect_ratios[param_mpeg-1]);
		++nerr;
	}

	if (param_min_GOP_size > param_max_GOP_size)
	{
		err_message("Min GOP size must be <= Max GOP size");
		++nerr;
	}

	if ((param_preserve_B) && ((param_min_GOP_size % param_Bgrp_size != 0) ||
		(param_max_GOP_size % param_Bgrp_size != 0)))
		errorExit_msg("Preserving I/P frame spacing is impossible if min and max GOP sizes are\nnot both divisible by %d", param_Bgrp_size);

	switch (param_format)
	{
		case MPEG_FORMAT_SVCD_STILL :
		case MPEG_FORMAT_SVCD_NSR :
		case MPEG_FORMAT_SVCD :
		{
			if ((param_aspect_ratio != 2) && (param_aspect_ratio != 3))
				errorExit_msg("SVCD only supports 4:3 and 16:9 aspect ratios");
			if (param_svcd_scan_data)
				info_message("Generating dummy SVCD scan-data offsets to be filled in by \"vcdimager\"\nIf you're not using vcdimager you may wish to turn this off using /*d*/");
			break;
		}
	}
	return nerr;
}

/***********************************************************
 * Mark the border so that blocks in the frame are unlikely
 * to match blocks outside the frame.  This is useful for
 * motion estimation routines that, for speed, are a little
 * sloppy about some of the candidates they consider.
 ***********************************************************/

void MpegEnc::border_mark( uint8_t* frame, int w1, int h1, int w2, int h2)
{
	int			i, j;
	uint8_t*	fp;
	uint8_t		mask = 0xff;

	// horizontal pixel replication (right border)
	for (j=0; j<h1; j++)
	{
		fp = frame + j*w2;
		for (i=w1; i<w2; i++)
		{
			fp[i] = mask;
			mask ^= 0xff;
		}
	}
 
	// vertical pixel replication (bottom border)
	for (j=h1; j<h2; j++)
	{
		fp = frame + j*w2;
		for (i=0; i<w2; i++)
		{
			fp[i] = mask;
			mask ^= 0xff;
		}
	}
}

void MpegEnc::init_encoder()
{
	mpg2enc_msgTitle = "MpegEnc::init_encoder";
	int i;
	unsigned n;
	static int block_count_tab[3] = {6,8,12};
	int enc_chrom_width, enc_chrom_height;
	initbits(); 
	init_fdct();
	init_idct();

	// Tune threading and motion compensation for specified number of CPU's and specified speed parameters.
    
	ctl_act_boost = (param_act_boost >= 0.0) ? (param_act_boost+1.0) : (param_act_boost-1.0);
	switch ( param_num_cpus )
	{
		case 0 :
		default :
		{ // Special case for debugging... turns of multi-threading
			// because this version only supports SINGLE-THREADING, this one also will be the default !
			ctl_max_encoding_frames = 1;
			ctl_refine_from_rec = true;
			ctl_parallel_read = false;
			break;
		}
		/*
		case 1 :
		{
			ctl_max_encoding_frames = 1;
			ctl_refine_from_rec = true;
			ctl_parallel_read = true;
			break;
		}
		case 2:
			ctl_max_encoding_frames = 2;
			ctl_refine_from_rec = true;
			ctl_parallel_read = true;
			break;
		}
		default :
		{
			ctl_max_encoding_frames = (param_num_cpus > MAX_WORKER_THREADS-1) ? MAX_WORKER_THREADS-1 : param_num_cpus;
			ctl_refine_from_rec = false;
			ctl_parallel_read = true;
			break;
		}
		*/
	}

	ctl_44_red				= param_44_red;
	ctl_22_red				= param_22_red;

	// round picture dimensions to nearest multiple of 16 or 32
	mb_width				= (opt_horizontal_size+15)/16;
	mb_height				= (opt_prog_seq) ? (opt_vertical_size+15)/16 : 2*((opt_vertical_size+31)/32);
	mb_height2				= (opt_fieldpic) ? mb_height>>1 : mb_height; // for field pictures
	opt_enc_width			= 16*mb_width;
	opt_enc_height			= 16*mb_height;
	// James: here's the place to do that padding!!
    // The values here are just for testing.
	opt_phy_width			= opt_enc_width+16;
	opt_phy_height			= opt_enc_height+8;

	/* Calculate the sizes and offsets in to luminance and chrominance
	   buffers.  A.Stevens 2000 for luminance data we allow space for
	   fast motion estimation data.  This is actually 2*2 pixel
	   sub-sampled uint8_t followed by 4*4 sub-sampled.  We add an
	   extra row to act as a margin to allow us to neglect / postpone
	   edge condition checking in time-critical loops...  */

	opt_phy_chrom_width		= (opt_chroma_format == CHROMA444) ? opt_phy_width : opt_phy_width>>1;
	opt_phy_chrom_height	= (opt_chroma_format != CHROMA420) ? opt_phy_height : opt_phy_height>>1;
	enc_chrom_width			= (opt_chroma_format == CHROMA444) ? opt_enc_width : opt_enc_width>>1;
	enc_chrom_height		= (opt_chroma_format != CHROMA420) ? opt_enc_height : opt_enc_height>>1;

	opt_phy_height2			= (opt_fieldpic) ? opt_phy_height>>1 : opt_phy_height;
	opt_enc_height2			= (opt_fieldpic) ? opt_enc_height>>1 : opt_enc_height;
	opt_phy_width2			= (opt_fieldpic) ? opt_phy_width<<1 : opt_phy_width;
	opt_phy_chrom_width2	= (opt_fieldpic) ? opt_phy_chrom_width<<1 : opt_phy_chrom_width;

	block_count				= block_count_tab[opt_chroma_format-1];
	lum_buffer_size			= (opt_phy_width*opt_phy_height) + sizeof(uint8_t) *(opt_phy_width/2)*(opt_phy_height/2) +
		sizeof(uint8_t) *(opt_phy_width/4)*(opt_phy_height/4+1);
	chrom_buffer_size		= opt_phy_chrom_width*opt_phy_chrom_height;

	fsubsample_offset		= (opt_phy_width)*(opt_phy_height) * sizeof(uint8_t);
	qsubsample_offset		=  fsubsample_offset + (opt_phy_width/2)*(opt_phy_height/2)*sizeof(uint8_t);

	mb_per_pict				= mb_width*mb_height2;

	// clip table
	if (!(clp_0_255 = (uint8_t *)malloc(1024)))
		errorExit_msg("malloc failed");
	clp_0_255				+= 384;
	for (i=-384; i<640; i++)
		clp_0_255[i] = (i<0) ? 0 : ((i>255) ? 255 : i);
	
	// Allocate the frame data buffers
	frame_buffer_size		= 2*param_max_GOP_size+param_Bgrp_size+READ_CHUNK_SIZE+1;
	info_message("Buffering %d frames", frame_buffer_size);
	frame_buffers = (uint8_t ***)bufalloc(frame_buffer_size*sizeof(uint8_t**));

	for (n=0;n<frame_buffer_size;n++)
	{
		frame_buffers[n] = (uint8_t **) bufalloc(3*sizeof(uint8_t*));
		for (i=0; i<3; i++)
		{
			frame_buffers[n][i] = bufalloc( (i==0) ? lum_buffer_size : chrom_buffer_size );
		}

		border_mark(frame_buffers[n][0], opt_enc_width, opt_enc_height, opt_phy_width, opt_phy_height);
		border_mark(frame_buffers[n][1], enc_chrom_width, enc_chrom_height, opt_phy_chrom_width, opt_phy_chrom_height);
		border_mark(frame_buffers[n][2], enc_chrom_width, enc_chrom_height, opt_phy_chrom_width, opt_phy_chrom_height);
	}
#ifdef OUTPUT_STAT
	// open statistics output file
	if (!(statfile = fopen(statname,"w")))
	{
		errorExit_msg( "Couldn't create statistics output file %s", statname);
	}
#endif
}

int MpegEnc::f_code( int max_radius )
{
	int c=5;
	if( max_radius < 64) c = 4;
	if( max_radius < 32) c = 3;
	if( max_radius < 16) c = 2;
	if( max_radius < 8) c = 1;
	return c;
}

void MpegEnc::init_mpeg_parms()
{
	int i;
	const char *msg;
	mpg2enc_msgTitle = "MpegEnc::init_mpeg_parms";

	inputtype = 0; // doesnt matter
	istrm_nframes = 999999999; // determined by EOF of stdin

	ctl_N_min = param_min_GOP_size;      // I frame distance
	ctl_N_max = param_max_GOP_size;
	info_message("GOP SIZE RANGE %d TO %d", ctl_N_min, ctl_N_max);
	ctl_M = param_Bgrp_size;             // I or P frame distance
	ctl_M_min = (param_preserve_B) ? ctl_M : 1;
	if (ctl_M >= ctl_N_min) ctl_M = ctl_N_min-1;
	opt_mpeg1			= (param_mpeg == 1);
	opt_fieldpic		= (param_fieldenc == 2);

	// SVCD and probably DVD? mandate progressive_sequence = 0 
	switch (param_format)
	{
		case MPEG_FORMAT_SVCD :
		case MPEG_FORMAT_SVCD_NSR :
		case MPEG_FORMAT_SVCD_STILL :
		case MPEG_FORMAT_DVD :
		{
			opt_prog_seq = 0;
			break;
		}
		default :
		{
			opt_prog_seq		= (param_mpeg == 1 || param_fieldenc == 0);
			break;
		}
	}

	opt_pulldown_32			= param_32_pulldown;

	opt_aspectratio			= param_aspect_ratio;
	opt_frame_rate_code		= param_frame_rate;
	opt_dctsatlim			= (opt_mpeg1) ? 255 : 2047;

	// If we're using a non standard (VCD?) profile bit-rate adjust	the vbv buffer accordingly...

	if (param_bitrate == 0)
		errorExit_msg( "Generic format - must specify bit-rate!" );

	opt_still_size			= 0;
	if (MPEG_STILLS_FORMAT(param_format))
	{
		opt_vbv_buffer_code	= param_vbv_buffer_still_size / 2048;
		opt_vbv_buffer_still_size = param_pad_stills_to_vbv_buffer_size;
		opt_bit_rate		= param_bitrate;
		opt_still_size		= param_still_size;
	}
	else if (param_mpeg == 1)
	{
		// Scale VBV relative to VCD
		opt_bit_rate		= MAX(10000, param_bitrate);
		opt_vbv_buffer_code	= (20 * param_bitrate  / 1151929);
	}
	else
	{
		opt_bit_rate		= MAX(10000, param_bitrate);
		opt_vbv_buffer_code	= MIN(112,param_video_buffer_size / 2);
	}
	opt_vbv_buffer_size		= opt_vbv_buffer_code*16384;

	if (param_quant)
		ctl_quant_floor		= (double)param_quant;
	else
		ctl_quant_floor		= 0.0;		// Larger than max quantisation

	ctl_video_buffer_size	= param_video_buffer_size * 1024 * 8;

	opt_seq_hdr_every_gop	= param_seq_hdr_every_gop;
	opt_seq_end_every_gop	= param_seq_end_every_gop;
	opt_svcd_scan_data		= param_svcd_scan_data;
	ctl_seq_length_limit	= param_seq_length_limit;
	ctl_nonvid_bit_rate		= param_nonvid_bitrate * 1000;
	opt_low_delay			= 0;
	opt_constrparms			= ((param_mpeg == 1) && (!MPEG_STILLS_FORMAT(param_format)));
	opt_profile				= (param_422) ? 1 : 4; // High or Main profile resp.
	opt_level				= 8;				   // Main Level      CCIR 601 rates
	opt_chroma_format		= (param_422) ? CHROMA422 : CHROMA420;

	switch (param_norm)
	{
		case 'p' :
		case 'P' :
		{ // PAL
			opt_video_format = 1;
			break;
		}
		case 'n' :
		case 'N' :
		{ // NTSC
			opt_video_format = 2;
			break;
		}
		case 's' :
		case 'S' :
		{ // SECAM
			opt_video_format = 3;
			break;
		}
		default :
		{ // unspecified
			opt_video_format = 5;
			break;
		}
	}

	switch (param_norm)
	{
		case 'S' :
		case 's' :
		case 'P' :
		case 'p' :
		{ // ITU BT.470  B,G
			opt_color_primaries = 5;
			opt_transfer_characteristics = 5; /* Gamma = 2.8 (!!) */
			opt_matrix_coefficients = 5; 
			msg = "PAL B/G";
			break;
		}
		case 'N' :
		case 'n' :
		{ // SMPTPE 170M "modern NTSC"
			opt_color_primaries = 6;
			opt_matrix_coefficients = 6; 
			opt_transfer_characteristics = 6;
			msg = "NTSC";
			break;
		}
		default :
		{ // unspec.
			opt_color_primaries = 2;
			opt_matrix_coefficients = 2; 
			opt_transfer_characteristics = 2;
			msg = "unspecified";
			break;
		}
	}
	info_message("Setting colour/gamma parameters to \"%s\"", msg);

	switch (param_format)
	{
		case MPEG_FORMAT_SVCD_STILL :
		case MPEG_FORMAT_SVCD_NSR :
		case MPEG_FORMAT_SVCD :
		case MPEG_FORMAT_DVD :
		{
			// It would seem DVD and perhaps SVCD demand a 540 pixel display size
			// for 4:3 aspect video. However, many players expect 480 and go weird
			// if this isn't set...
			if (param_hack_svcd_hds_bug)
			{
				opt_display_horizontal_size  = opt_horizontal_size;
				opt_display_vertical_size    = opt_vertical_size;
			}
			else
			{
				opt_display_horizontal_size  = opt_aspectratio == 2 ? 540 : 720;
				opt_display_vertical_size    = opt_vertical_size;
			}
			break;
		}
		default :
		{
			opt_display_horizontal_size  = opt_horizontal_size;
			opt_display_vertical_size    = opt_vertical_size;
			break;
		}
	}

	opt_dc_prec			= param_mpeg2_dc_prec;  /* 9 bits */
	opt_topfirst		= 0;
	if (!opt_prog_seq)
	{
		int fieldorder;
		if (param_force_interlacing != Y4M_UNKNOWN) 
		{
			info_message("Forcing playback video to be: %s",
				mpeg_interlace_code_definition(param_force_interlacing));	
			fieldorder = param_force_interlacing;
		}
		else
			fieldorder = param_input_interlacing;

		opt_topfirst = ((fieldorder == Y4M_ILACE_TOP_FIRST) || (fieldorder == Y4M_ILACE_NONE));
	}
	else
		opt_topfirst = 0;

	opt_frame_pred_dct_tab[0] = opt_frame_pred_dct_tab[1] = opt_frame_pred_dct_tab[2] = 
		((param_mpeg == 1) || (param_fieldenc == 0)) ? 1 : 0;

	info_message("Progressive format frames = %d", opt_frame_pred_dct_tab[0]);
	opt_qscale_tab[0] = opt_qscale_tab[1] = opt_qscale_tab[2] = (param_mpeg == 1) ? 0 : 1;

	opt_intravlc_tab[0] = opt_intravlc_tab[1] = opt_intravlc_tab[2] = (param_mpeg == 1) ? 0 : 1;

	opt_altscan_tab[2] = opt_altscan_tab[1] = opt_altscan_tab[0] =
		((param_mpeg == 1) || (param_hack_altscan_bug)) ? 0 : 1;
	
	// A.Stevens 2000: The search radius *has* to be a multiple of 8 for the new fast motion compensation
	// search to work correctly. We simply round it up if needs be.
	if (param_searchrad*ctl_M>127)
	{
		param_searchrad = 127/ctl_M;
		warn_message("Search radius reduced to %d",param_searchrad);
	}

	int radius_x = param_searchrad;
	int radius_y = param_searchrad*opt_vertical_size/opt_horizontal_size;

	// TODO: These f-codes should really be adjusted for each picture type...

	if ((opt_motion_data = (struct motion_data *)malloc(ctl_M*sizeof(struct motion_data))) == NULL)
		errorExit_msg("malloc failed");

	for (i=0; i<ctl_M; i++)
	{
		if (i==0)
		{
			opt_motion_data[i].sxf = round_search_radius(radius_x*ctl_M);
			opt_motion_data[i].forw_hor_f_code  = f_code(opt_motion_data[i].sxf);
			opt_motion_data[i].syf = round_search_radius(radius_y*ctl_M);
			opt_motion_data[i].forw_vert_f_code  = f_code(opt_motion_data[i].syf);
		}
		else
		{
			opt_motion_data[i].sxf = round_search_radius(radius_x*i);
			opt_motion_data[i].forw_hor_f_code  = f_code(opt_motion_data[i].sxf);
			opt_motion_data[i].syf = round_search_radius(radius_y*i);
			opt_motion_data[i].forw_vert_f_code  = f_code(opt_motion_data[i].syf);
			opt_motion_data[i].sxb = round_search_radius(radius_x*(ctl_M-i));
			opt_motion_data[i].back_hor_f_code  = f_code(opt_motion_data[i].sxb);
			opt_motion_data[i].syb = round_search_radius(radius_y*(ctl_M-i));
			opt_motion_data[i].back_vert_f_code  = f_code(opt_motion_data[i].syb);
		}

		// MPEG-1 demands f-codes for vertical and horizontal axes are identical!!!!
		if (opt_mpeg1)
		{
			opt_motion_data[i].syf = opt_motion_data[i].sxf;
			opt_motion_data[i].syb  = opt_motion_data[i].sxb;
			opt_motion_data[i].forw_vert_f_code = opt_motion_data[i].forw_hor_f_code;
			opt_motion_data[i].back_vert_f_code = opt_motion_data[i].back_hor_f_code;
		}
	}
	
	// make sure MPEG specific parameters are valid
	range_checks();

	// Set the frame decode rate and frame display rates.
	// For 3:2 movie pulldown decode rate is != display rate due to the repeated field that appears every other frame
	opt_frame_rate = Y4M_RATIO_DBL(mpeg_framerate(opt_frame_rate_code));
	if (param_32_pulldown)
	{
		ctl_decode_frame_rate = opt_frame_rate * (2.0 + 2.0) / (3.0 + 2.0);
		info_message( "3:2 Pulldown selected frame decode rate = %d/%d fps", 
			mpeg_conform_framerate(ctl_decode_frame_rate).n, mpeg_conform_framerate(ctl_decode_frame_rate).d);
	}
	else
		ctl_decode_frame_rate = opt_frame_rate;

	if (!opt_mpeg1)
		profile_and_level_checks();
	else
	{ // MPEG-1
		if (opt_constrparms)
		{
			if ((opt_horizontal_size>768) || (opt_vertical_size>576) ||
				(((opt_horizontal_size+15)/16)*((opt_vertical_size+15)/16)>396) ||
				(((opt_horizontal_size+15)/16)*((opt_vertical_size+15)/16)*opt_frame_rate>396*25.0) ||
				(opt_frame_rate>30.0))
			{
				info_message("size - setting constrained_parameters_flag = 0");
				opt_constrparms = 0;
			}
		}

		if (opt_constrparms)
		{
			for (i=0; i<ctl_M; i++)
			{
				if (opt_motion_data[i].forw_hor_f_code > 4)
				{
					info_message("Hor. motion search forces constrained_parameters_flag = 0");
					opt_constrparms = 0;
					break;
				}

				if (opt_motion_data[i].forw_vert_f_code > 4)
				{
					info_message("Ver. motion search forces constrained_parameters_flag = 0");
					opt_constrparms = 0;
					break;
				}

				if (i!=0)
				{
					if (opt_motion_data[i].back_hor_f_code > 4)
					{
						info_message("Hor. motion search setting constrained_parameters_flag = 0");
						opt_constrparms = 0;
						break;
					}

					if (opt_motion_data[i].back_vert_f_code > 4)
					{
						info_message("Ver. motion search setting constrained_parameters_flag = 0");
						opt_constrparms = 0;
						break;
					}
				} // if (i!=0)
			} // for (i=0; i<ctl_M; i++)
		} // if (opt_constrparms)
	} // (opt_mpeg1)

	// relational checks
	if (opt_mpeg1)
	{
		if (!opt_prog_seq)
		{
			warn_message("opt_mpeg1 specified - setting progressive_sequence = 1");
			opt_prog_seq = 1;
		}

		if (opt_chroma_format != CHROMA420)
		{
			info_message("mpeg1 - forcing chroma_format = 1 (4:2:0) - others not supported");
			opt_chroma_format = CHROMA420;
		}

		if (opt_dc_prec != 0)
		{
			info_message("mpeg1 - setting intra_dc_precision = 0");
			opt_dc_prec = 0;
		}

		for (i=0; i<3; i++)
			if (opt_qscale_tab[i])
			{
				info_message("mpeg1 - setting qscale_tab[%d] = 0",i);
				opt_qscale_tab[i] = 0;
			}

		for (i=0; i<3; i++)
			if (opt_intravlc_tab[i])
			{
				info_message("mpeg1 - setting intravlc_tab[%d] = 0",i);
				opt_intravlc_tab[i] = 0;
			}

		for (i=0; i<3; i++)
			if (opt_altscan_tab[i])
			{
				info_message("mpeg1 - setting altscan_tab[%d] = 0",i);
				opt_altscan_tab[i] = 0;
			}
	}

	if ((!opt_mpeg1) && (opt_constrparms))
	{
		info_message("not mpeg1 - setting constrained_parameters_flag = 0");
		opt_constrparms = 0;
	}

	if (((!opt_prog_seq) || (opt_fieldpic != 0)) && (((opt_vertical_size+15) / 16)%2 != 0 ))
	{
		warn_message( "Frame height won't split into two equal field pictures...\nforcing encoding as progressive video");
		opt_prog_seq = 1;
		opt_fieldpic = 0;
	}

	if (opt_prog_seq && opt_fieldpic != 0)
	{
		info_message("prog sequence - forcing progressive frame encoding");
		opt_fieldpic = 0;
	}

	if ((opt_prog_seq) && (opt_topfirst))
	{
		info_message("prog sequence setting top_field_first = 0");
		opt_topfirst = 0;
	}

	// search windows
	for (i=0; i<ctl_M; i++)
	{
		if (opt_motion_data[i].sxf > (4<<opt_motion_data[i].forw_hor_f_code)-1)
		{
			info_message("reducing forward horizontal search width to %d", (4<<opt_motion_data[i].forw_hor_f_code)-1);
			opt_motion_data[i].sxf = (4<<opt_motion_data[i].forw_hor_f_code)-1;
		}

		if (opt_motion_data[i].syf > (4<<opt_motion_data[i].forw_vert_f_code)-1)
		{
			info_message("reducing forward vertical search width to %d", (4<<opt_motion_data[i].forw_vert_f_code)-1);
			opt_motion_data[i].syf = (4<<opt_motion_data[i].forw_vert_f_code)-1;
		}

		if (i!=0)
		{
			if (opt_motion_data[i].sxb > (4<<opt_motion_data[i].back_hor_f_code)-1)
			{
				info_message("reducing backward horizontal search width to %d", (4<<opt_motion_data[i].back_hor_f_code)-1);
				opt_motion_data[i].sxb = (4<<opt_motion_data[i].back_hor_f_code)-1;
			}

			if (opt_motion_data[i].syb > (4<<opt_motion_data[i].back_vert_f_code)-1)
			{
				info_message("reducing backward vertical search width to %d", (4<<opt_motion_data[i].back_vert_f_code)-1);
				opt_motion_data[i].syb = (4<<opt_motion_data[i].back_vert_f_code)-1;
			}
		}
	}
}

/************************************************************
 * If the use has selected suppression of hf noise via
 * quantisation then we boost quantisation of hf components
 * EXPERIMENTAL: currently a linear ramp from 0 at 4pel to 
 * 50% increased quantisation...
 ************************************************************/
int MpegEnc::quant_hfnoise_filt(int orgquant, int qmat_pos )
{
	int orgdist = intmax(qmat_pos % 8, qmat_pos/8);
	int qboost = 1024;

	if (param_hf_quant != 1)
		return orgquant;

	// Maximum 150% quantisation boost for HF components...
	qboost = 256+(384/8)*orgdist;

	return (orgquant * qboost)/ 256;
}

void MpegEnc::init_quantmat()
{
	int i, v, q;
	mpg2enc_msgTitle = "MpegEnc::init_quantmat";
	opt_load_iquant = 0;
	opt_load_niquant = 0;

	if (param_hf_quant == 2)
	{
		opt_load_iquant |= 1;
		info_message("Setting hi-res intra Quantisation matrix");
		for (i=0; i<64; i++)
			opt_intra_q[i] = hires_intra_quantizer_matrix[i];
	}
	else
	{
		// use default intra matrix
		opt_load_iquant = (param_hf_quant == 1);
		for (i=0; i<64; i++)
		{
			v = quant_hfnoise_filt( default_intra_quantizer_matrix[i], i);
			if ((v<1) || (v>255))
				errorExit_msg("value in intra quant matrix invalid (after noise filt adjust)");
			opt_intra_q[i] = v;
		} 
	}

	// MPEG default non-intra matrix is all 16's. For *our* default we use something more suitable for domestic analog
	// sources... which is non-standard...
	if (param_hf_quant == 2)
	{
		info_message( "Setting hi-res non-intra quantiser matrix" );
		for (i=0; i<64; i++)
			opt_inter_q[i] = hires_nonintra_quantizer_matrix[i];
	}
	else
	{
		opt_load_niquant |= (param_hf_quant == 1);
		for (i=0; i<64; i++)
		{
			v = quant_hfnoise_filt(default_nonintra_quantizer_matrix[i],i);
			if ((v<1) || (v>255))
				errorExit_msg("value in non-intra quant matrix invalid (after noise filt adjust)");
			opt_inter_q[i] = v;
		}
	}

	// TODO: Inv Quant matrix initialisation should check if the fraction fits in 16 bits!

	for (i=0; i<64; i++)
	{
		i_intra_q[i] = (int)(((double)IQUANT_SCALE) / ((double)opt_intra_q[i]));
		i_inter_q[i] = (int)(((double)IQUANT_SCALE) / ((double)opt_inter_q[i]));
	}

	for (q=1; q <= 112; ++q )
	{
		for (i=0; i<64; i++)
		{
			intra_q_tbl[q][i] = opt_intra_q[i] * q;
			inter_q_tbl[q][i] = opt_inter_q[i] * q;
			intra_q_tblf[q][i] = (float)intra_q_tbl[q][i];
			inter_q_tblf[q][i] = (float)inter_q_tbl[q][i];
			i_intra_q_tblf[q][i] = (float)(1.0f/ ( intra_q_tblf[q][i] * 0.98));
			i_intra_q_tbl[q][i] = (IQUANT_SCALE/intra_q_tbl[q][i]);
			i_inter_q_tblf[q][i] =  (float)(1.0f/ (inter_q_tblf[q][i] * 0.98));
			i_inter_q_tbl[q][i] = (IQUANT_SCALE/inter_q_tbl[q][i] );
		}
	}
}

void MpegEnc::Run_Main()
{
	// initialize first
	Init_MainApp();

	mpg2enc_msgTitle = "MpegEnc::Run_Main";
	int nerr=0;

	// Select input stream 
	if ((!param_inFile[0]) || ((istrm_fd = _open( param_inFile, O_RDONLY | O_BINARY )) == -1))
		errorExit_msg("Unable to open input-file !");

	// Read parameters inferred from input stream
	read_stream_params( &opt_horizontal_size, &opt_vertical_size, 
						&strm_frame_rate_code, &param_input_interlacing,
						(unsigned int*)&strm_aspect_ratio );
	
	if (opt_horizontal_size <= 0)
		errorExit_msg("Horizontal size from input stream illegal");

	if (opt_vertical_size <= 0)
		errorExit_msg("Vertical size from input stream illegal");
	
	// Check parameters that cannot be checked when parsed/read

	if (!outfilename[0])
		errorExit_msg("Output file name (/*o*/ option) is required!");

	set_format_presets();
	nerr += infer_default_params();
	nerr += check_param_constraints();

	if (nerr) 
		errorExit(); // just exit while there are errors

	char s[2048];
	sprintf(s, "Encoding MPEG-%d video to <%s>:", param_mpeg, outfilename);
	sprintf(&s[strlen(s)], "\nHorizontal size: %d pel", opt_horizontal_size);
	sprintf(&s[strlen(s)], "\nVertical size: %d pel", opt_vertical_size);
	sprintf(&s[strlen(s)], "\nAspect ratio code: %d = %s", param_aspect_ratio, mpeg_aspect_code_definition(param_mpeg,param_aspect_ratio));
	sprintf(&s[strlen(s)], "\nFrame rate code:   %d = %s", param_frame_rate, mpeg_framerate_code_definition(param_frame_rate));

	if (param_bitrate) 
		sprintf(&s[strlen(s)], "\nBitrate: %d KBit/s", param_bitrate/1000);
	else
		sprintf(&s[strlen(s)], "\nBitrate: VCD");
	if (param_quant) 
		sprintf(&s[strlen(s)], "\nQuality factor: %d (1=best, 31=worst)", param_quant);

	sprintf(&s[strlen(s)], "\nField order for input: %s", mpeg_interlace_code_definition(param_input_interlacing) );

	if (param_seq_length_limit)
	{
		sprintf(&s[strlen(s)], "\nNew Sequence every %d Mbytes", param_seq_length_limit);
		sprintf(&s[strlen(s)], "\nAssuming non-video stream of %d Kbps", param_nonvid_bitrate);
	}
	else
		sprintf(&s[strlen(s)], "\nSequence unlimited length");

	sprintf(&s[strlen(s)], "\nSearch radius: %d", param_searchrad);
	info_message(s);

	// set params
	init_mpeg_parms();

	// read quantization matrices
	init_quantmat();

	// open output file
	if (!(outfile=fopen(outfilename,"wb")))
		errorExit_msg("Couldn't create output file <%s>", outfilename);

	init_encoder();
	init_quantizer();
	init_motion();
	init_transform();
	init_predict();
//	putseq();
	putseq2(m_hWnd, m_uWnd);

	fclose(outfile); outfile=NULL;
	_close(istrm_fd); istrm_fd=-1;

#ifdef OUTPUT_STAT
	if( statfile != NULL )
	{
		fclose(statfile);
		statfile=NULL;
	}
#endif
}
