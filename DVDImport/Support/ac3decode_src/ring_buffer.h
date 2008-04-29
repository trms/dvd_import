/*
 *
 *  ring_buffer.h
 *
 *	Copyright (C) Aaron Holtzman - May 1999
 *
 *  This file is part of ac3dec, a free Dolby AC-3 stream decoder.
 *	
 *  ac3dec is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  ac3dec is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 *
 *  AC3DEC_WAVE
 *  -----------
 *  changes to the buffer-size and buffer-count, for greater compatibility
 *  with Windows' waveOut architecture.  The waveOut architecture does
 *  not deal very gracefully with numerous buffers of tiny size.  It's
 *  better to use fewer buffers of larger size.
 */

void rb_init(void);
sint_16* rb_begin_read(void);
void rb_end_read(void);
sint_16* rb_begin_write(void);
void rb_end_write(void);
sint_16 rb_read_get_index( void );  /* added for output_winmci.c */
sint_16 rb_write_get_index( void ); /* added for output_winmci.c */

/*#define BUFFER_SIZE 1024*/
#define BUFFER_SIZE 65536
/*#define NUM_BUFFERS 256*/
#define NUM_BUFFERS 8
/* Windows MCI doesn't seem to like >50 or so buffers...*/
/* buffer size of 1024 is *FAR TOO SMALL* for Windows MCI to manage.  The
   overhead of each WaveOutPrepareHeader() / WaveOutUnprepareHeader() pair
   brings even a >400MHz CPU to its knees!  Don't blame me, blame Windows...
   */


/* return codes for rb_read_get_index(), rb_write_get_index() */
#define RB_EMPTY -1
#define RB_FULL -2
