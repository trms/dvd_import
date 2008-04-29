// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// mplexconsts.h:  Constants, part of the mjpgTools Library																 //
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
#ifndef __MPLEXCONSTS_H__
#define __MPLEXCONSTS_H__


#define SEQUENCE_HEADER					0x000001b3
#define SEQUENCE_END					0x000001b7
#define PICTURE_START					0x00000100
#define EXT_START_CODE					0x000001b5
#define GROUP_START						0x000001b8
#define SYNCWORD_START					0x000001

#define IFRAME							1
#define PFRAME							2
#define BFRAME							3
#define DFRAME							4
#define NOFRAME							5

#define PIC_TOP_FIELD					1
#define PIC_BOT_FIELD					2
#define PIC_FRAME						3

#define CODING_EXT_ID					8
#define AUDIO_SYNCWORD					0x7ff


#define PACK_START						0x000001ba
#define SYS_HEADER_START				0x000001bb
#define ISO11172_END					0x000001b9
#define PACKET_START					0x000001

#define MAX_FFFFFFFF					4294967295.0 	/* = 0xffffffff in dec.	*/

#define CLOCKS							(300 *90000)		/* MPEG-2 System Clock Hertz - we divide down by 300.0 for MPEG-1*/

/* Range of sizes of the fields following the packet length field in packet header:
	used to calculate if recieve buffers will have enough space... */

#define MPEG2_BUFFERINFO_LENGTH			3
#define MPEG1_BUFFERINFO_LENGTH			2
#define DTS_PTS_TIMESTAMP_LENGTH		5
#define MPEG2_AFTER_PACKET_LENGTH_MIN	3
#define MPEG1_AFTER_PACKET_LENGTH_MIN	(0+1)

	/* Sector under-size below which header stuffing rather than padding packets
		or post-packet zero stuffing is used.  *Must* be less than 20 for VCD
		multiplexing to work correctly!
	 */
	 
#define MINIMUM_PADDING_PACKET_SIZE		10

#define PACKET_HEADER_SIZE				6

#define AUDIO_STREAMS					0xb8		/* Marker Audio Streams	*/
#define VIDEO_STREAMS					0xb9		/* Marker Video Streams	*/
#define AUDIO_STR_0						0xc0		/* Marker Audio Stream0	*/
#define VIDEO_STR_0						0xe0		/* Marker Video Stream0	*/
#define PADDING_STR						0xbe		/* Marker Padding Stream*/
#define PRIVATE_STR_1					0xbd            /* private stream 1 */
#define PRIVATE_STR_2					0xbf            /* private stream 2 */
#define AC3_SUB_STR_1					0x80            /* AC3 substream id 1 */
#define AC3_SUB_STR_2					0x81            /* AC3 substream id 2 */

#define ZERO_STUFFING_BYTE				0
#define STUFFING_BYTE					0xff
#define RESERVED_BYTE					0xff
#define TIMESTAMPBITS_NO				0		/* Flag NO timestamps	*/
#define TIMESTAMPBITS_PTS				2		/* Flag PTS timestamp	*/
#define TIMESTAMPBITS_DTS				1		/* Flag PTS timestamp	*/
#define TIMESTAMPBITS_PTS_DTS			(TIMESTAMPBITS_DTS|TIMESTAMPBITS_PTS)		/* Flag BOTH timestamps	*/

#define MARKER_MPEG1_SCR				2		/* Marker SCR		*/
#define MARKER_MPEG2_SCR				1		/* These don't need to be distinct! */
#define MARKER_JUST_PTS					2		/* Marker only PTS	*/
#define MARKER_PTS						3		/* Marker PTS		*/
#define MARKER_DTS						1		/* Marker DTS		*/
#define MARKER_NO_TIMESTAMPS			0x0f	/* Marker NO timestamps	*/


#endif // __MPLEXCONSTS_H__
