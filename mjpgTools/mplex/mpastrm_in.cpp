// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// mpastrm_in.cpp: MPEG Audio strem class members handling scanning and buffering raw input stream,						 //
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


static const char *mpa_audio_version[4] =
{
	"2.5",
	"2.0",
	"reserved",
	"1.0"
};

static const unsigned int mpa_bitrates_kbps [4][3][16] =
{
	{ /* MPEG audio V2.5 */
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}
	},
	{ /*RESERVED*/
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	},
	{ /* MPEG audio V2 */
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}
	},
	{ /* MPEG audio V1 */
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},
		{0,32,48,56,64,80,96,112,128,160,192,224,256,320,384,0},
		{0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0}
	}

};


static const int mpa_freq_table [4][4] = 
{
	/* MPEG audio V2.5 */
	{11025,12000,8000,0},
	/* RESERVED */
	{ 0, 0, 0, 0 }, 
	/* MPEG audio V2 */
	{22050,24000, 16000,0},
	/* MPEG audio V1 */
	{44100, 48000, 32000, 0}
};

static const char mpa_stereo_mode [4][15] =
{ "stereo", "joint stereo", "dual channel", "single channel" };
static const char mpa_copyright_status [2][20] =
{ "no copyright","copyright protected" };
static const char mpa_original_bit [2][10] =
{ "copy","original" };
static const char mpa_emphasis_mode [4][20] =
{ "none", "50/15 microseconds", "reserved", "CCITT J.17" };
static const unsigned int mpa_slots [4] = {12, 144, 144, 0};
static const unsigned int mpa_samples [4] = {384, 1152, 1152, 0};



MPAStream::MPAStream(IBitStream &ibs, OutputStream &into) : 
	AudioStream( ibs, into )
{
}

bool MPAStream::Probe(IBitStream &bs )
{
    return bs.getbits(11) == AUDIO_SYNCWORD;
}


/*************************************************************************
 *
 * Reads initial stream parameters and displays feedback banner to users
 *
 *************************************************************************/


void MPAStream::Init ( const int stream_num )

{
	msgTitle = "MPAStream::Init";

//    unsigned int i;
	int padding_bit;
	debug_Message( "SETTING zero stuff to %d", muxinto.vcd_zero_stuffing );
	debug_Message( "SETTING audio buffer to %d", muxinto.audio_buffer_size );

	MuxStream::Init( AUDIO_STR_0 + stream_num, 
					 0,  // Buffer scale
					 muxinto.audio_buffer_size,
					 muxinto.vcd_zero_stuffing,
					 muxinto.buffers_in_audio,
					 muxinto.always_buffers_in_audio
		);
    info_Message ("Scanning for header info: Audio stream %02x (%s)",
                AUDIO_STR_0 + stream_num,
                bs.filename
                );

	InitAUbuffer();
	
	/* A.Stevens 2000 - update to be compatible up to  MPEG2.5
	 */
    if (bs.getbits(11)==AUDIO_SYNCWORD)
    {
		num_syncword++;
		version_id = bs.getbits( 2);
		layer 		= 3-bs.getbits( 2); /* 0..2 not 1..3!! */
		protection 		= bs.get1bit();
		bit_rate_code	= bs.getbits( 4);
		frequency 		= bs.getbits( 2);
		padding_bit     = bs.get1bit();
		bs.get1bit();
		mode 		= bs.getbits( 2);
		mode_extension 	= bs.getbits( 2);
		copyright 		= bs.get1bit();
		original_copy 	= bs.get1bit ();
		emphasis		= bs.getbits( 2);

		framesize =
			mpa_bitrates_kbps[version_id][layer][bit_rate_code]  * 
			mpa_slots[layer] *1000 /
			mpa_freq_table[version_id][frequency];

		size_frames[0] = framesize;
		size_frames[1] = framesize+( layer == 0 ? 4 : 1);
		num_frames[padding_bit]++;
	
		access_unit.length = size_frames[padding_bit];
	  
		samples_per_second = mpa_freq_table[version_id][frequency];

		/* Presentation time-stamping  */
		access_unit.PTS = static_cast<clockticks>(decoding_order) * 
			static_cast<clockticks>(mpa_samples [layer]) * 
			static_cast<clockticks>(CLOCKS)	/ samples_per_second;
		access_unit.DTS = access_unit.PTS;
		access_unit.dorder = decoding_order;
		++decoding_order;
		aunits.append( access_unit );

    } else
    {
		error_exit_msg ( "Invalid MPEG Audio stream header.");
    }


	OutputHdrInfo();
}

unsigned int MPAStream::NominalBitRate()
{ 
	return mpa_bitrates_kbps[version_id][layer][bit_rate_code]*128;
}


unsigned int MPAStream::SizeFrame( int rate_code, int padding )
{
	return mpa_bitrates_kbps[version_id][layer][rate_code]  * 
		mpa_slots [layer] *1000 /
		mpa_freq_table[version_id][frequency] + padding;
}

void MPAStream::FillAUbuffer(unsigned int frames_to_buffer )
{
	msgTitle = "MPAStream::FillAUbuffer";

	unsigned int i;
	unsigned int padding_bit;
	last_buffered_AU += frames_to_buffer;

	debug_Message("Scanning %d MPEG audio frames to frame %d", frames_to_buffer, last_buffered_AU );

	while (!bs.eos() && 
		   decoding_order < last_buffered_AU && 
		   (!opt_max_PTS || access_unit.PTS < opt_max_PTS))
	{
		skip=access_unit.length-4;
		if (skip & 0x1) bs.getbits( 8);
		if (skip & 0x2) bs.getbits( 16);
		skip=skip>>2;

		for (i=0;i<skip;i++)
		{
			bs.getbits( 32);
		}
		prev_offset = AU_start;
		AU_start = bs.bitcount();

		/* Check we have reached the end of have  another catenated 
		   stream to process before finishing ... */
		if ( (syncword = bs.getbits( 11))!=AUDIO_SYNCWORD )
		{
			if( !bs.eobs   )
			{
				/* There appears to be another catenated stream... */
				int next;
				warn_Message( "End of component bit-stream ... seeking next" );
				/* Catenated stream must start on byte boundary */
				syncword = (syncword<<(8-AU_start % 8));
				next = bs.getbits(8-(AU_start % 8) );
				AU_start = bs.bitcount()-11;
				syncword = syncword | next;
				if( syncword != AUDIO_SYNCWORD )
				{
					warn_Message("Failed to find start of next stream at %ld prev %ld !", AU_start/8, prev_offset/8 );
					break;
				}
			}
			else
				/* No catenated stream... finished! */
				break;
		}
		// Skip version_id:2, layer:2, protection:1
		(void) bs.getbits( 5);
		int rate_code	= bs.getbits( 4);
		// Skip frequency
		(void) bs.getbits( 2);

		padding_bit=bs.get1bit();
		access_unit.start = AU_start;
		access_unit.length = SizeFrame( rate_code, padding_bit );
		access_unit.PTS = static_cast<clockticks>(decoding_order) * static_cast<clockticks>(mpa_samples[layer]) * static_cast<clockticks>(CLOCKS)
			/ samples_per_second;
		access_unit.DTS = access_unit.PTS;
		access_unit.dorder = decoding_order;
		decoding_order++;
		aunits.append( access_unit );
		num_frames[padding_bit]++;

		bs.getbits( 9);
		
		num_syncword++;

		if (num_syncword >= old_frames+10 )
		{
			debug_Message ("Got %d frame headers.", num_syncword);
			old_frames=num_syncword;
		
		}
	


    }
	last_buffered_AU = decoding_order;
	eoscan = bs.eos() || (opt_max_PTS && access_unit.PTS >= opt_max_PTS);

}



void MPAStream::Close()
{
	msgTitle = "MPAStream::Close";

    stream_length = AU_start >> 3;
	info_Message("AUDIO_STATISTICS: %02x\nAudio stream length %ld bytes.\nSyncwords      : %8u\nFrames         : %8u padded\nFrames         : %8u unpadded",
		stream_id, stream_length, num_syncword, num_frames[0], num_frames[1]); 
	
    bs.close();
}

/*************************************************************************
	OutputAudioInfo
	gibt gesammelte Informationen zu den Audio Access Units aus.

	Prints information on audio access units
*************************************************************************/

void MPAStream::OutputHdrInfo ()
{
	msgTitle = "MPAStream::OutputHdrInfo";

	char s[1024];
    unsigned int bitrate;
    bitrate = mpa_bitrates_kbps[version_id][layer][bit_rate_code];

	sprintf(s, "AUDIO STREAM:\nAudio version  : %s\nLayer          : %8u", mpa_audio_version[version_id], 4-layer);

    if (protection == 0) sprintf(&s[strlen(s)], "\nCRC checksums  :      yes");
    else  sprintf(&s[strlen(s)], "\nCRC checksums  :       no");

    if (bit_rate_code == 0)
		sprintf(&s[strlen(s)], "\nBit rate       :     free");
    else if (bit_rate_code == 0xf)
		sprintf(&s[strlen(s)], "\nBit rate       : reserved");
    else
		sprintf(&s[strlen(s)], "\nBit rate       : %8u bytes/sec (%3u kbit/sec)", bitrate*128, bitrate);

    if (frequency == 3)
		sprintf(&s[strlen(s)], "\nFrequency      : reserved");
    else
		sprintf(&s[strlen(s)], "\nFrequency      :     %d Hz", mpa_freq_table[version_id][frequency]);

    sprintf(&s[strlen(s)], "\nMode           : %8u %s", mode,mpa_stereo_mode[mode]);
    sprintf(&s[strlen(s)], "\nMode extension : %8u",mode_extension);
    sprintf(&s[strlen(s)], "\nCopyright bit  : %8u %s", copyright,mpa_copyright_status[copyright]);
    sprintf(&s[strlen(s)], "\nOriginal/Copy  : %8u %s", original_copy,mpa_original_bit[original_copy]);
    sprintf(&s[strlen(s)], "\nEmphasis       : %8u %s", emphasis,mpa_emphasis_mode[emphasis]);

	info_Message(s);
}
