// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// outputstream.h, part of the mjpgTools Library																		 //
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
#ifndef __OUTPUTSTREAM_H__
#define __OUTPUTSTREAM_H__

#include "types.h"
//#include <config.h>
#include <stdio.h>
//#include "mjpeg_types.h"

#include "inputstrm.h"
#include "padstrm.h"
#include "systems.h"

#ifndef _HAS_LOG_LEVEL_
#define _HAS_LOG_LEVEL_
typedef enum {
  LOG_DEBUG = 1,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR
} log_level_t;
#endif // _HAS_LOG_LEVEL_

class OutputStream
{
public:
	OutputStream()
		{
			underrun_ignore = 0;
			underruns = 0;
		}
	void OutputMultiplex ( vector<ElementaryStream *> *strms,
						   char *multi_file);


	void InitSyntaxParameters();
	void ByteposTimecode( bitcount_t bytepos, clockticks &ts );
	
	inline Sys_header_struc *SystemHeader() { return &sys_header; }

	unsigned int PacketPayload(	MuxStream &strm,
								bool buffers, bool PTSstamp, bool DTSstamp );
	unsigned int WritePacket( unsigned int     max_packet_data_size,
							  MuxStream        &strm,
							  bool 	 buffers,
							  clockticks   	 PTS,
							  clockticks   	 DTS,
							  uint8_t 	 timestamps
		);

	/* Special "unusual" sector types needed for particular formats 
	 */
	  
	void OutputDVDPriv2 ();

	/* Syntax control parameters, public becaus they're partly referenced
	   by the input stream objects.
	 */

	bool always_sys_header_in_pack;
	bool dtspts_for_all_vau;
	bool sys_header_in_pack1;
	bool buffers_in_video;
	bool always_buffers_in_video;	
	bool buffers_in_audio;
	bool always_buffers_in_audio;
	bool sector_align_iframeAUs;
	bool split_at_seq_end;
	bool seg_starts_with_video;
	bool timestamp_iframe_only;
	bool video_buffers_iframe_only;
	unsigned int audio_buffer_size;
	unsigned int video_buffer_size;


	/* Sequence run-out control */
	bool running_out;
	clockticks runout_PTS;


/* In some situations the system/PES packets are embedded with
   external transport data which has to be taken into account for SCR
   calculations to be correct.  E.g. VCD streams. Where each 2324 byte
   system packet is embedded in a 2352 byte CD sector and the actual
   MPEG data is preceded by 30 empty sectors.
*/

	unsigned int	sector_transport_size;
	unsigned int    transport_prefix_sectors; 
	unsigned int 	sector_size;
	unsigned int	vcd_zero_stuffing;	/* VCD audio sectors have 20 0 bytes :-( */

	int 		dmux_rate;	/* Actual data mux-rate for calculations always a multiple of 50  */
	int 		mux_rate;	/* MPEG mux rate (50 byte/sec units      */
	unsigned int packets_per_pack;
	
private:	
	
    /* Stream packet component buffers */
	
	Sys_header_struc 	sys_header;
	Pack_struc          pack_header;
	Pack_struc *pack_header_ptr;
	Sys_header_struc *sys_header_ptr;
	bool start_of_new_pack;
	bool include_sys_header;

	/* Under-run error messages */
	unsigned int underruns;
	unsigned int underrun_ignore;

	/* Output data stream... */
	PS_Stream *psstrm;
	bitcount_t bytes_output;
    clockticks ticks_per_sector;

public:
	clockticks current_SCR;
private:
	clockticks audio_delay;
	clockticks video_delay;
	bool vbr;
	/* Source data streams */
	/* Note: 1st video stream is regarded as the "master" stream for
	   the purpose of splitting sequences etc...
	*/
	vector<ElementaryStream *> *estreams; // Complete set
	vector<ElementaryStream *> vstreams; // Video streams in estreams
	vector<ElementaryStream *> astreams; // Audio streams in estreams
	
	PaddingStream pstrm;
	VCDAPadStream vcdapstrm;
	DVDPriv2Stream dvdpriv2strm;

private:
	unsigned int RunInSectors();
	void Init( char *multi_file );
	

	void NextPosAndSCR();
	void SetPosAndSCR( bitcount_t bytepos );

	void OutputPrefix( );

	void OutputSuffix();
	void OutputPadding ( bool vcd_audio_pad );
	void MuxStatus( log_level_t level );

	void WriteRawSector( uint8_t *rawpackets,
						 unsigned int     length
		);
	void AppendMuxStreamsOf( vector<ElementaryStream *> &elem, 
							 vector<MuxStream *> &mux );
};


#endif //__OUTPUTSTREAM_H__
