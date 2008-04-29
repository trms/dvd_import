// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// MpegEnc.h: interface for the MpegEnc class, part of the mjpgTools Library											 //
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
#if !defined(AFX_MPEGENC_H__D7CFC1AD_9887_11D6_811E_0050BF61B407__INCLUDED_)
#define AFX_MPEGENC_H__D7CFC1AD_9887_11D6_811E_0050BF61B407__INCLUDED_

#include "types.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define MIN(a,b) ( (a)<(b) ? (a) : (b) )

class MpegEnc  
{
public:
	MpegEnc(const void* _hWnd=0, const unsigned _uWnd=0);
	virtual ~MpegEnc();

	virtual void Init_MainApp(void) = 0; // must be overriden in inherited class, which fills the param_ vars.

protected:
	// private prototypes
	void init_encoder(void);
	void init_mpeg_parms(void);
	void init_quantmat(void);

protected:
	// local prototypes
	void DisplayFrameRates(void);
	void DisplayAspectRatios(void);
	void set_format_presets(void);
	int infer_mpeg1_aspect_code( char norm, mpeg_aspect_code_t mpeg2_code );
	int infer_default_params(void);
	int check_param_constraints(void);

	void border_mark( uint8_t* frame, int w1, int h1, int w2, int h2);
	int f_code( int max_radius );
	int quant_hfnoise_filt(int orgquant, int qmat_pos );

	void Run_Main(void); // does the real work !

protected:
	char param_inFile[_MAX_PATH];
	char outfilename[_MAX_PATH];
	// Command Parameter values.  These are checked and processed for defaults etc after parsing.
	// The resulting values then set the opt_ variables that control the actual encoder.
	int param_format;
	int param_bitrate;
	int param_nonvid_bitrate;
	int param_quant;
	int param_searchrad;
	int param_mpeg;
	int param_aspect_ratio;
	int param_frame_rate;
	int param_fieldenc;	// 0 = progressive, 
						// 1 = frame pictures, interlace frames with field MC and DCT in picture 
						// 2 = field pictures
	char param_norm;	// 'n': NTSC, 'p': PAL, 's': SECAM, else unspecified
	int param_44_red;
	int param_22_red;	
	int param_hf_quant;
	double param_act_boost;
	int param_video_buffer_size;
	int param_seq_length_limit;
	int param_min_GOP_size;
	int param_max_GOP_size;
	int param_preserve_B;
	int param_Bgrp_size;
	int param_num_cpus;
	int param_32_pulldown;
	int param_svcd_scan_data;
	int param_seq_hdr_every_gop;
	int param_seq_end_every_gop;
	int param_still_size;
	int param_pad_stills_to_vbv_buffer_size;
	int param_vbv_buffer_still_size;
	int param_force_interlacing;
	int param_input_interlacing;
	int param_hack_svcd_hds_bug;
	int param_hack_altscan_bug;
	int param_mpeg2_dc_prec;

	// Input Stream parameter values that have to be further processed to set encoding options
	mpeg_aspect_code_t strm_aspect_ratio;
	int strm_frame_rate_code;

	// reserved: for later use
	int param_422;

	const void*		m_hWnd;
	unsigned		m_uWnd;
};

#endif // !defined(AFX_MPEGENC_H__D7CFC1AD_9887_11D6_811E_0050BF61B407__INCLUDED_)
