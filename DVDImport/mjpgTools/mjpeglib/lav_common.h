// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// lav_common.h:	part of the mjpgTools Library																		 //
//					Some general utility functionality used by multiple lavtool utilities.								 //
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
// This sub-project is a corrected version of :																			 //
//	MJPEGtools v1.6.0 (of the linux group)																				 //
//																														 //
//	Copyright (C) 2000, Rainer Johanni, Andrew Stevens																	 //
//		2001, pHilipp Zabel, July 2001 - Shawn Sulma <lavtools@athos.cx>, 2002 - Matthew Marjanovic <maddog@mir.com>	 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#define MAX_EDIT_LIST_FILES 256
#define MAX_JPEG_LEN (3*576*768/2)

#define BUFFER_ALIGN 16

/**
 * (SS 2001-July-13)
 * The split of the globals into three structs is somewhat arbitrary, but
 * I've tried to do them based on role as used in lav2yuv and (my own)
 * lav2divx.  
 * - LavParam handles data that is generally per-run dependent
 *   (e.g. from the command line).
 * - LavBounds contains data about bounds used in processing.  It is generally
 *   not dependent on command line alteration.
 * - LavBuffer contains buffers used to perform various tasks.
 *
 **/

typedef struct {
   INT32 offset;
   INT32 frames;
   INT32 mono;
   char *scenefile;
   INT32 delta_lum_threshold; /* = 4; */
   UINT32 scene_detection_decimation; /* = 2; */
   INT32 output_width;
   INT32 output_height;
   INT32 interlace;
   y4m_ratio_t sar; /* sample aspect ratio (default 0:0 == unspecified) */
   y4m_ratio_t dar; /* 'suggested' display aspect ratio */
  
  INT32 chroma_width;
  INT32 chroma_height;
  INT32 luma_size;
  INT32 chroma_size;
} LavParam;


#ifdef __cplusplus
extern "C" {
#endif

INT32 luminance_mean(UINT8 *frame[], INT32 w, INT32 h);

INT32 readframe(INT32 numframe, UINT8 *frame[],
	      LavParam *param, EditList el);

void writeoutYUV4MPEGheader(INT32 out_fd, LavParam *param, EditList el,
			    y4m_stream_info_t *streaminfo);

void init(LavParam *param, UINT8 *frame[]);


#ifdef __cplusplus
}
#endif

#ifdef SUPPORT_READ_DV2
#include <libdv/dv.h>

#ifdef __cplusplus
extern "C" {
#endif

void frame_YUV422_to_YUV420P(UINT8 **output, UINT8 *input,
			     INT32 width, INT32 height);
void lav_init_dv_decoder(void);

#ifdef __cplusplus
}
#endif

#endif /* SUPPORT_READ_DV2 */

