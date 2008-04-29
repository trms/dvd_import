// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// avilib.h:	part of the mjpgTools Library																			 //
//				(see avilib.c for specifications)																		 //
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
//   MJPEGtools v1.6.0 (of the linux group)																				 //
//																														 //
//	Copyright (C) 1999 Rainer Johanni <Rainer@Johanni.de>																 //
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#ifndef AVILIB_H
#define AVILIB_H

#include "mjpeg_types.h"

typedef struct
{
	INT32				pos;
	INT32				len;
} video_index_entry;

typedef struct
{
	INT32				pos;
	INT32				len;
	INT32				tot;
} audio_index_entry;

typedef struct
{
	INT32				fdes;				// File descriptor of AVI file
	INT32				mode;				// 0 for reading, 1 for writing

	INT32				width;				// Width  of a video frame
	INT32				height;				// Height of a video frame
	double				fps;				// Frames per second
	UINT8				compressor[8];		// Type of compressor, 4 bytes + padding for 0 byte
	INT32				video_strn;			// Video stream number
	INT32				video_frames;		// Number of video frames
	UINT8				video_tag[4];		// Tag of video data
	INT32				video_pos;			// Number of next frame to be read (if index present)

	INT32				a_fmt;				// Audio format, see #defines below
	INT32				a_chans;			// Audio channels, 0 for no audio
	INT32				a_rate;				// Rate in Hz
	INT32				a_bits;				// bits per audio sample
	INT32				audio_strn;			// Audio stream number
	INT32				audio_bytes;		// Total number of bytes of audio data
	INT32				audio_chunks;		// Chunks of audio data in the file
	UINT8				audio_tag[4];		// Tag of audio data
	INT32				audio_posc;			// Audio position: chunk
	INT32				audio_posb;			// Audio position: byte within chunk

	INT32				pos;				// position in file
	INT32				n_idx;				// number of index entries actually filled
	INT32				max_idx;			// number of index entries actually allocated
	UINT8				(*idx)[16];			// index entries (AVI idx1 tag)
	video_index_entry*	video_index;
	audio_index_entry*	audio_index;
	INT32				last_pos;			// Position of last frame written
	INT32				last_len;			// Length of last frame written
	INT32				must_use_index;		// Flag if frames are duplicated
	INT32				movi_start;
} avi_t;

#define AVI_MODE_WRITE				0
#define AVI_MODE_READ				1

/* The error codes delivered by avi_open_input_file */

#define AVI_ERR_SIZELIM				1		// The write of the data would exceed the maximum size of the AVI file.
											// This is more a warning than an error since the file may be closed safely

#define AVI_ERR_OPEN				2		// Error opening the AVI file - wrong path name or file nor readable/writable

#define AVI_ERR_READ				3		// Error reading from AVI File

#define AVI_ERR_WRITE				4		// Error writing to AVI File, disk full ???

#define AVI_ERR_WRITE_INDEX			5		// Could not write index to AVI file during close, file may still be usable

#define AVI_ERR_CLOSE				6		// Could not write header to AVI file or not truncate the file during close,
											// file is most probably corrupted

#define AVI_ERR_NOT_PERM			7		// Operation not permitted: trying to read from a file open
											// for writing or vice versa

#define AVI_ERR_NO_MEM				8		// malloc failed

#define AVI_ERR_NO_AVI				9		// Not an AVI file

#define AVI_ERR_NO_HDRL				10		// AVI file has no has no header list, corrupted ???

#define AVI_ERR_NO_MOVI				11		// AVI file has no has no MOVI list, corrupted ???

#define AVI_ERR_NO_VIDS				12		// AVI file contains no video data

#define AVI_ERR_NO_IDX				13		// The file has been opened with getIndex==0, but an operation has been
											// performed that needs an index

/* Possible Audio formats */

#define WAVE_FORMAT_UNKNOWN			(0x0000)
#ifdef WAVE_FORMAT_PCM
	#undef WAVE_FORMAT_PCM
#endif
#define WAVE_FORMAT_PCM				(0x0001)
#define WAVE_FORMAT_ADPCM			(0x0002)
#define WAVE_FORMAT_IBM_CVSD		(0x0005)
#define WAVE_FORMAT_ALAW			(0x0006)
#define WAVE_FORMAT_MULAW			(0x0007)
#define WAVE_FORMAT_OKI_ADPCM		(0x0010)
#define WAVE_FORMAT_DVI_ADPCM		(0x0011)
#define WAVE_FORMAT_DIGISTD			(0x0015)
#define WAVE_FORMAT_DIGIFIX			(0x0016)
#define WAVE_FORMAT_YAMAHA_ADPCM	(0x0020)
#define WAVE_FORMAT_DSP_TRUESPEECH	(0x0022)
#define WAVE_FORMAT_GSM610			(0x0031)
#define IBM_FORMAT_MULAW			(0x0101)
#define IBM_FORMAT_ALAW				(0x0102)
#define IBM_FORMAT_ADPCM			(0x0103)

#ifdef __cplusplus
extern "C" {
#endif

avi_t*			AVI_open_output_file(char* filename);
void			AVI_set_video(avi_t* AVI, INT32 width, INT32 height, double fps, const char* compressor);
void			AVI_set_audio(avi_t* AVI, INT32 channels, INT32 rate, INT32 bits, INT32 format);
INT32			AVI_write_frame(avi_t* AVI, UINT8* data, INT32 bytes);
INT32			AVI_dup_frame(avi_t* AVI);
INT32			AVI_write_audio(avi_t* AVI, UINT8* data, INT32 bytes);
INT32			AVI_bytes_remain(avi_t* AVI);
INT32			AVI_close(avi_t* AVI);
INT32			AVI_fileno(avi_t* AVI);

avi_t*			AVI_open_input_file(char* filename, INT32 getIndex);

INT32			AVI_video_frames(avi_t* AVI);
INT32			AVI_video_width(avi_t* AVI);
INT32			AVI_video_height(avi_t* AVI);
double			AVI_frame_rate(avi_t* AVI);
char*			AVI_video_compressor(avi_t* AVI);

INT32			AVI_audio_channels(avi_t* AVI);
INT32			AVI_audio_bits(avi_t* AVI);
INT32			AVI_audio_format(avi_t* AVI);
INT32			AVI_audio_rate(avi_t* AVI);
INT32			AVI_audio_bytes(avi_t* AVI);

INT32			AVI_frame_size(avi_t* AVI, INT32 frame);
INT32			AVI_seek_start(avi_t* AVI);
INT32			AVI_set_video_position(avi_t* AVI, INT32 frame);
INT32			AVI_read_frame(avi_t* AVI, UINT8* vidbuf);
INT32			AVI_set_audio_position(avi_t* AVI, INT32 byte);
INT32			AVI_read_audio(avi_t* AVI, UINT8* audbuf, INT32 bytes);

INT32			AVI_read_data(avi_t* AVI, UINT8* vidbuf, INT32 max_vidbuf, UINT8* audbuf, INT32 max_audbuf, INT32* len);

void			AVI_print_error(const char* str);
const char*		AVI_strerror(void);
const char*		AVI_syserror(void);

#ifdef __cplusplus
}
#endif

#endif // AVILIB_H
