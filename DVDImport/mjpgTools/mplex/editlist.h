// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// editlist.h:	part of the mjpgTools Library																			 //
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
//	This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or		 //
//	(at your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#ifndef EDITLIST_H
#define EDITLIST_H

#include "lav_io.h"


/* If changing MAX_EDIT_LIST_FILES, the macros below
   also have to be adapted. */

#define MAX_EDIT_LIST_FILES 256

#define N_EL_FRAME(x)  ( (x)&0xffffff )
#define N_EL_FILE(x)   ( ((x)>>24)&0xff )
#define EL_ENTRY(file,frame) ( ((file)<<24) | ((frame)&0xffffff) )

typedef struct
{
	INT32		video_frames;
	INT32		video_width;
	INT32		video_height;
	INT32		video_inter;
	INT32		video_norm;
	double		video_fps;
	INT32		video_sar_width; /* sample aspect ratio */
	INT32		video_sar_height;

	INT32		max_frame_size;
	INT32		MJPG_chroma;
	/* TODO: Need to flag mixed chroma model files? */

	INT32		has_audio;
	INT32		audio_rate;
	INT32		audio_chans;
	INT32		audio_bits;
	INT32		audio_bps;

	INT32		num_video_files;
	char*		(video_file_list[MAX_EDIT_LIST_FILES]);
	lav_file_t*	(lav_fd[MAX_EDIT_LIST_FILES]);
	INT32		num_frames[MAX_EDIT_LIST_FILES];
	INT32*		frame_list;

	INT32		last_afile;
	INT32		last_apos;
} EditList;

#ifdef _cplusplus
extern "C" {
#endif

INT32	el_get_video_frame(UINT8* vbuff, INT32 nframe, EditList* el);  
INT32	el_get_audio_data(UINT8* abuff, INT32 nframe, EditList* el, INT32 mute);
void	read_video_files(char** filename, INT32 num_files, EditList* el, INT32 preserve_pathnames);
INT32	write_edit_list(char* name, INT32 n1, INT32 n2, EditList* el);
INT32	open_video_file(char* filename, EditList* el, INT32 preserve_pathname);
INT32	el_video_frame_data_format(INT32 nframe, EditList* el);

#ifdef _cplusplus
}
#endif

#endif /* ifndef EDITLIST_H */
