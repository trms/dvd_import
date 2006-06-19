// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// audiostrm_out.cpp:  Members of audio stream classes related to muxing out into the output stream,					 //
//					   part of the mjpgTools Library																	 //
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
#include <assert.h>
#include "..\mpg2enc\fastintfns.h"
#include "audiostrm.h"
#include "outputstream.h"


AudioStream::AudioStream(IBitStream &ibs, OutputStream &into) : 
	ElementaryStream( ibs, into,  ElementaryStream::audio ),
	num_syncword(0)
{
    FRAME_CHUNK = 24;
	for( int i = 0; i <2 ; ++i )
		num_frames[i] = size_frames[i] = 0;
}

void AudioStream::InitAUbuffer()
{
	for (int i = 0; i < aunits.BUF_SIZE; ++i )
		aunits.init( new AAunit );
}



/*********************************
 * Signals when audio stream has completed mux run-out specified
 * in associated mux stream. 
 *********************************/

bool AudioStream::RunOutComplete()
{

	return (au_unsent == 0 || 
			( muxinto.running_out && RequiredPTS() >= muxinto.runout_PTS));
}

bool AudioStream::AUBufferNeedsRefill()
{
    return 
        !eoscan
        && ( aunits.current()+FRAME_CHUNK > last_buffered_AU
             || 
             bs.buffered_bytes() < muxinto.sector_size
            );
}

/******************************************************************
	Output_Audio
	generates Pack/Sys Header/Packet information from the
	audio stream and saves them into the sector
******************************************************************/

void AudioStream::OutputSector ( )

{
	clockticks   PTS;
	unsigned int max_packet_data; 	 
	unsigned int actual_payload;
	unsigned int bytes_sent;
//	AAunit *aau;
//	Pack_struc pack;
	unsigned int old_au_then_new_payload;

	PTS = RequiredDTS();
	old_au_then_new_payload = 
		muxinto.PacketPayload( *this, buffers_in_header, false, false );

	max_packet_data = 0;
	if( muxinto.running_out && 
        NextRequiredPTS() > muxinto.runout_PTS )
	{
		/* We're now in the last AU of a segment.  So we don't want to
		   go beyond it's end when writing sectors. Hence we limit
		   packet payload size to (remaining) AU length.
		*/
		max_packet_data = au_unsent;
	}
  
    
	/* CASE: packet starts with new access unit			*/
	
	if (new_au_next_sec)
    {
		actual_payload = 
			muxinto.WritePacket ( max_packet_data,
								  *this,
								  buffers_in_header, PTS, 0,
								  TIMESTAMPBITS_PTS);

		Muxed( actual_payload );
    }


	/* CASE: packet starts with old access unit, no new one	*/
	/*       starts in this very same packet			*/
	else if (!(new_au_next_sec) && 
			 (au_unsent >= old_au_then_new_payload))
    {
		actual_payload = 
			muxinto.WritePacket ( max_packet_data,
								  *this,
								  buffers_in_header, 0, 0,
								  TIMESTAMPBITS_NO );
		Muxed( actual_payload );
    }


	/* CASE: packet starts with old access unit, a new one	*/
	/*       starts in this very same packet			*/
	else /* !(new_au_next_sec) &&  (au_unsent < old_au_then_new_payload)) */
    {
		bytes_sent = au_unsent;
		Muxed(bytes_sent);
		/* gibt es ueberhaupt noch eine Access Unit ? */
		/* is there another access unit anyway ? */
		if( !MuxCompleted()  )
		{
			PTS = RequiredDTS();
			actual_payload = 
				muxinto.WritePacket ( max_packet_data,
									  *this,
									  buffers_in_header, PTS, 0,
									  TIMESTAMPBITS_PTS );

			Muxed( actual_payload - bytes_sent );
		} 
		else
		{
			muxinto.WritePacket ( 0,
								  *this,
								  buffers_in_header, 0, 0,
								  TIMESTAMPBITS_NO );
		};
		
    }

    ++nsec;

	buffers_in_header = always_buffers_in_header;
	
}
