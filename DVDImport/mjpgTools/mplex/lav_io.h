// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// lav_io.h:	part of the mjpgTools Library																			 //
//				(see <lav_io.c> for specifications)																		 //
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
//	Copyright (C) 2000 Rainer Johanni <Rainer@Johanni.de>																 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#ifndef LAV_IO_H
#define LAV_IO_H

#ifdef COMPILE_LAV_IO_C
#include "avilib.h"

#ifdef HAVE_LIBQUICKTIME
#ifdef HAVE_OPENQUICKTIME
#include <openquicktime.h>
#else
#include <quicktime.h>
#endif
#endif

#ifdef HAVE_LIBMOVTAR
#include <movtar.h>
#endif
#else
typedef void avi_t;
typedef void quicktime_t;
typedef void movtar_t;
#endif

#include "yuv4mpeg.h"

#define LAV_INTER_UNKNOWN       Y4M_UNKNOWN
#define LAV_NOT_INTERLACED      Y4M_ILACE_NONE
#define LAV_INTER_TOP_FIRST     Y4M_ILACE_TOP_FIRST
#define LAV_INTER_BOTTOM_FIRST  Y4M_ILACE_BOTTOM_FIRST


/* chroma_format */
#define CHROMAUNKNOWN 0
#define CHROMA420 1
#define CHROMA422 2
#define CHROMA444 3

/* raw data format of a single frame */
#define DATAFORMAT_MJPG     0
#define DATAFORMAT_DV2      1
#define DATAFORMAT_YUV420   2
#define DATAFORMAT_YUV422   3

typedef struct
{
	avi_t*			avi_fd;
	INT32			jpeg_fd;
	char*			jpeg_filename;
#ifdef HAVE_LIBQUICKTIME
	quicktime_t*	qt_fd;
#endif
#ifdef HAVE_LIBMOVTAR
	movtar_t*		movtar_fd;
#endif
	INT32			format;
	INT32			interlacing;
	INT32			sar_w;  /* "sample aspect ratio" width  */
	INT32			sar_h;  /* "sample aspect ratio" height */
	INT32			has_audio;
	INT32			bps;
	INT32			is_MJPG;
	INT32			MJPG_chroma;
} lav_file_t;

#ifdef __cplusplus
extern "C" {
#endif

INT32		lav_query_APP_marker(char format);
INT32		lav_query_APP_length(char format);
INT32		lav_query_polarity(char format);
lav_file_t*	lav_open_output_file(char* filename, char format, INT32 width, INT32 height, INT32 interlaced, double fps,
								 INT32 asize, INT32 achans, INT32 arate);
INT32		lav_close(lav_file_t* lav_file);
INT32		lav_write_frame(lav_file_t* lav_file, UINT8* buff, INT32 size, INT32 count);
INT32		lav_write_audio(lav_file_t* lav_file, UINT8* buff, INT32 samps);
INT32		lav_video_frames(lav_file_t* lav_file);
INT32		lav_video_width(lav_file_t* lav_file);
INT32		lav_video_height(lav_file_t* lav_file);
double		lav_frame_rate(lav_file_t* lav_file);
INT32		lav_video_interlacing(lav_file_t* lav_file);
void		lav_video_sampleaspect(lav_file_t* lav_file, INT32* sar_w, INT32* sar_h);
INT32		lav_video_is_MJPG(lav_file_t* lav_file);
INT32		lav_video_MJPG_chroma(lav_file_t* lav_file);
const char*	lav_video_compressor(lav_file_t* lav_file);
INT32		lav_audio_channels(lav_file_t* lav_file);
INT32		lav_audio_bits(lav_file_t* lav_file);
INT32		lav_audio_rate(lav_file_t* lav_file);
INT32		lav_audio_samples(lav_file_t* lav_file);
INT32		lav_frame_size(lav_file_t* lav_file, INT32 frame);
INT32		lav_seek_start(lav_file_t* lav_file);
INT32		lav_set_video_position(lav_file_t* lav_file, INT32 frame);
INT32		lav_read_frame(lav_file_t* lav_file, UINT8* vidbuf);
INT32		lav_set_audio_position(lav_file_t* lav_file, INT32 sample);
INT32		lav_read_audio(lav_file_t* lav_file, UINT8* audbuf, INT32 samps);
INT32		lav_filetype(lav_file_t* lav_file);
lav_file_t*	lav_open_input_file(char* filename);
INT32		lav_get_field_size(UINT8* jpegdata, INT32 jpeglen);
const char*	lav_strerror(void);
INT32		lav_fileno(lav_file_t* lav_file);

#ifdef __cplusplus
}
#endif

#endif
