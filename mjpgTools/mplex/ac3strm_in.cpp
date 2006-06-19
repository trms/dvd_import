// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// ac3strm_in.cpp: AC3 Audio strem class members handling scanning and buffering raw input stream,						 //
//				   part of the mjpgTools Library																		 //
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
//#include <config.h>
#include <math.h>
#include <stdlib.h>

#include "audiostrm.h"
#include "interact.h"
#include "outputstream.h"


#define AC3_SYNCWORD            0x0b77
#define AC3_PACKET_SAMPLES      1536

static const unsigned int ac3_bitrate_index[32] =
{ 32,40,48,56,64,80,96,112,128,160,192,
  224,256,320,384,448,512,576,640,
  0,0,0,0,0,0,0,0,0,0,0,0,0
};

static const unsigned int ac3_frame_size[3][32] =
{
    { 64,80,96,112,128,160,192,224,256,320,384,
      448,512,640,768,896,1024, 1152,1280,
      0,0,0,0,0,0,0,0,0,0,0,0,0
    },
    { 69,87,104,121,139,174,208,243,278,348,417,
      487,557,696,835,975,1114, 1253,1393,
          0,0,0,0,0,0,0,0,0,0,0,0,0
    },
    { 96,120,144,168,192,240,288,336,384,480,576,
      672,768,960,1152,1344, 1536,1728,1920,
      0,0,0,0,0,0,0,0,0,0,0,0,0
    }
}; 

static const unsigned int ac3_frequency[4] = 
{ 48000, 44100, 32000, 0};


AC3Stream::AC3Stream(IBitStream &ibs, OutputStream &into) : 
	AudioStream( ibs, into )
{
}

const unsigned int AC3Stream::default_buffer_size = 16*1024;

bool AC3Stream::Probe(IBitStream &bs )
{
    return bs.getbits(16) == AC3_SYNCWORD;
}


/*************************************************************************
 *
 * Reads initial stream parameters and displays feedback banner to users
 *
 *************************************************************************/


void AC3Stream::Init ( const int stream_num)

{
	msgTitle = "AC3Stream::Init";
//    unsigned int i;
    unsigned int framesize_code;

	debug_Message( "SETTING zero stuff to %d", muxinto.vcd_zero_stuffing );
	debug_Message( "SETTING audio buffer to %d", default_buffer_size );

	MuxStream::Init( PRIVATE_STR_1, // TODO Currently hard-wired....
					 1,  // Buffer scale
					 default_buffer_size,
					 muxinto.vcd_zero_stuffing,
					 muxinto.buffers_in_audio,
					 muxinto.always_buffers_in_audio
		);
    info_Message("Scanning for header info: Audio stream %02x (%s)",
                AUDIO_STR_0 + stream_num,
                bs.filename
                );

	InitAUbuffer();
	
    if (bs.getbits(16)==AC3_SYNCWORD)
    {
		num_syncword++;
        bs.getbits(16);         // CRC field
        frequency = bs.getbits(2);  // Sample rate code
        framesize_code = bs.getbits(6); // Frame size code
        framesize = ac3_frame_size[frequency][framesize_code>>1];
        framesize = 
            (framesize_code&1) && frequency == 1 ?
            (framesize + 1) << 1:
            (framesize <<1);
            

		size_frames[0] = framesize;
		size_frames[1] = framesize;
		num_frames[0]++;
		access_unit.length = framesize;
        bit_rate = ac3_bitrate_index[framesize_code>>1];
		samples_per_second = ac3_frequency[frequency];

		/* Presentation time-stamping  */
		access_unit.PTS = static_cast<clockticks>(decoding_order) * 
			static_cast<clockticks>(AC3_PACKET_SAMPLES) * 
			static_cast<clockticks>(CLOCKS)	/ samples_per_second;
		access_unit.DTS = access_unit.PTS;
		access_unit.dorder = decoding_order;
		++decoding_order;
		aunits.append( access_unit );

    } else
    {
		error_exit_msg( "Invalid AC3 Audio stream header.");
    }


	OutputHdrInfo();
}

unsigned int AC3Stream::NominalBitRate()
{ 
	return bit_rate;
}



void AC3Stream::FillAUbuffer(unsigned int frames_to_buffer )
{
	msgTitle = "AC3Stream::FillAUbuffer";

	unsigned int framesize_code;
	last_buffered_AU += frames_to_buffer;

	debug_Message("Scanning %d MPEG audio frames to frame %d", 
				 frames_to_buffer, last_buffered_AU );

	while (!bs.eos() && 
		   decoding_order < last_buffered_AU && 
		   (!opt_max_PTS || access_unit.PTS < opt_max_PTS))
	{

		skip=access_unit.length-5; // 5 bytes of header read already...
		if (skip & 0x1) bs.getbits( 8);
		if (skip & 0x2) bs.getbits( 16);
		skip=skip>>2;

		for (int i=0;i<skip;i++)
		{
			bs.getbits( 32);
		}

		prev_offset = AU_start;
		AU_start = bs.bitcount();

		/* Check we have reached the end of have  another catenated 
		   stream to process before finishing ... */
		if ( (syncword = bs.getbits(16))!=AC3_SYNCWORD )
		{
			if( !bs.eobs   )
			{
				warn_Message( "Can't find next AC3 frame - broken bit-stream?" );
            }
            break;
		}

        bs.getbits(16);         // CRC field
        bs.getbits(2);          // Sample rate code TOOD: check for change!
        framesize_code = bs.getbits(6);
        if( bs.eobs )
            break;
        framesize = ac3_frame_size[frequency][framesize_code>>1];
        framesize = 
            (framesize_code&1) && frequency == 1 ?
            (framesize + 1) << 1:
            (framesize <<1);
        
		access_unit.start = AU_start;
		access_unit.length = framesize;
		access_unit.PTS = static_cast<clockticks>(decoding_order) * 
			static_cast<clockticks>(AC3_PACKET_SAMPLES) * 
			static_cast<clockticks>(CLOCKS)	/ samples_per_second;;
		access_unit.DTS = access_unit.PTS;
		access_unit.dorder = decoding_order;
		decoding_order++;
		aunits.append( access_unit );
		num_frames[0]++;
		
		num_syncword++;

		if (num_syncword >= old_frames+10 )
		{
			debug_Message("Got %d frame headers.", num_syncword);
			old_frames=num_syncword;
		
		}


    }
	last_buffered_AU = decoding_order;
	eoscan = bs.eos() || (opt_max_PTS && access_unit.PTS >= opt_max_PTS);

}



void AC3Stream::Close()
{
	msgTitle = "AC3Stream::Close";
    stream_length = AU_start >> 3;
	info_Message("AUDIO_STATISTICS: %02x\nAudio stream length %ld bytes.\nSyncwords      : %8u\nFrames         : %8u padded\nFrames         : %8u unpadded",
		stream_id, num_syncword, stream_length, num_frames[0], num_frames[1]); 
	
    bs.close();
}

/*************************************************************************
	OutputAudioInfo
	gibt gesammelte Informationen zu den Audio Access Units aus.

	Prints information on audio access units
*************************************************************************/

void AC3Stream::OutputHdrInfo ()
{
	msgTitle = "AC3Stream::OutputHdrInfo";

	char s[1024];
	sprintf(s, "AC3 AUDIO STREAM :");
    sprintf(&s[strlen(s)], "\nBit rate       : %8u bytes/sec (%3u kbit/sec)", bit_rate*128, bit_rate);

    if (frequency == 3)
		sprintf(&s[strlen(s)], "\nFrequency      : reserved");
    else
		sprintf(&s[strlen(s)], "\nFrequency      :     %d Hz", ac3_frequency[frequency]);

	info_Message(s);
}
