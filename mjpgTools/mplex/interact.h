// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// interact.h:  Data and Check Inputfiles, part of the mjpgTools Library												 //
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
//   mplex - MPEG1/SYSTEMS multiplexer - Copyright (C) 1994 1995 Christoph Moar	<moar@informatik.tu-muenchen.de>		 //
//	 Additional portions and modifications - Copyright (C) 2000 Andrew Stevens	<as@comlab.ox.ac.uk>					 //
//																														 //
//	 Research at Siemens ZFE ST SN 11 / T SN 6																			 //
//	 TECHNICAL UNIVERSITY OF MUNICH, GERMANY																			 //
//																														 //
//	 This program is free software; you can redistribute it and/or modify it under the terms of the						 //
//	 GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at	 //
//	 your option) any later version.																					 //
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __INTERACT_H__
#define __INTERACT_H__

#include "types.h"
//#include <config.h>
//#include <unistd.h>
#include <vector>
//#include "mjpeg_types.h"

#include "aunit.h"

/*************************************************************************
    Entry points...
*************************************************************************/

using std::vector;
    
//int intro_and_options( int, char **, char**);

void check_files (char** argv,
                  vector<IBitStream *> &mpa_file,
                  vector<IBitStream *> &ac3_file,                  
                  vector<IBitStream *> &video_file
	);
//bool open_file(const char *name);


/*************************************************************************
    Command line options and derived parameters
*************************************************************************/



extern int opt_quiet_mode;
extern int opt_interactive_mode;
extern int opt_buffer_size;
extern int opt_data_rate;
extern int opt_video_offset;
extern int opt_audio_offset;
extern int opt_sector_size;
extern int opt_VBR;
extern int opt_mpeg;
extern int opt_mux_format;
extern int opt_multifile_segment;
extern int opt_always_system_headers;
extern int opt_packets_per_pack;
extern clockticks opt_max_PTS;
extern int opt_emul_vcdmplex;
extern bool opt_stills;
extern bool opt_ignore_underrun;
// extern int verbose;

extern off_t opt_max_segment_size;

/*************************************************************************
    Program ID
*************************************************************************/
 
#define MPLEX_VER    "2.2.1"
#define MPLEX_DATE   "$Date$"


#endif // __INTERACT_H__
