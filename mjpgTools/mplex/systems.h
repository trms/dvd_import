// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// systems.h: interface for the PS_Stream class, part of the mjpgTools Library											 //
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
#ifndef __SYSTEMS_HH__
#define __SYSTEMS_HH__

//#include <sys/param.h>
#include "inputstrm.h"

#include <vector>
using std::vector;

/* Buffer size parameters */

#define MAX_SECTOR_SIZE			16384
#define MAX_PACK_HEADER_SIZE	255
#define MAX_SYS_HEADER_SIZE		255

typedef struct sector_struc	/* Ein Sektor, kann Pack, Sys Header	*/
/* und Packet enthalten.		*/
{
	uint8_t		buf[MAX_SECTOR_SIZE] ;
	unsigned int	length_of_packet_data ;
	//clockticks TS                ;
} Sector_struc;

typedef struct pack_struc	/* Pack Info				*/
{
	// unsigned char  buf[MAX_PACK_HEADER_SIZE];
	uint8_t		buf[MAX_PACK_HEADER_SIZE];
	int length;
	clockticks SCR;
} Pack_struc;

typedef struct sys_header_struc	/* System Header Info			*/
{
	uint8_t		buf [MAX_SYS_HEADER_SIZE];
	int length;
} Sys_header_struc;


class PS_Stream
{
public:
    PS_Stream( unsigned _mpeg,
               unsigned int _sector_size
        )
        : mpeg_version( _mpeg),
          sector_size( _sector_size ),
          segment_num( 1 ),
          max_segment_size( 2040 * 1024 * 1024 )
        {
            sector_buf = new uint8_t[sector_size];
        }

    void Init( const char *filename_pat,
               off_t max_segment_size // 0 = No Limit
        );
    bool FileLimReached( );
    void NextFile();
    unsigned int PacketPayload( MuxStream &strm,
                                Sys_header_struc *sys_header, 
                                Pack_struc *pack_header, 
                                int buffers, int PTSstamp, int DTSstamp );

    unsigned int CreateSector (Pack_struc	 	 *pack,
                               Sys_header_struc *sys_header,
                               unsigned int     max_packet_data_size,
                               MuxStream        &strm,
                               bool 	 buffers,
                               bool      end_marker,
                               clockticks   	 PTS,
                               clockticks   	 DTS,
                               uint8_t 	 timestamps
        );
    void RawWrite(uint8_t *data, unsigned int len);
    static void BufferSectorHeader( uint8_t *buf,
                             Pack_struc	 	 *pack,
                             Sys_header_struc *sys_header,
                             uint8_t *&header_end );
    static void BufferPacketHeader( uint8_t *buf,
                             uint8_t type,
                             unsigned int mpeg_version,
                             bool buffers,
                             unsigned int buffer_size,
                             uint8_t buffer_scale,
                             clockticks   	 PTS,
                             clockticks   	 DTS,
                             uint8_t 	 timestamps,
                             uint8_t     *&size_field,
                             uint8_t     *&header_end );
    
    static inline void 
    BufferPacketSize( uint8_t *size_field, uint8_t *packet_end )
        {
            unsigned int packet_size = packet_end-size_field-2;
            size_field[0] = static_cast<uint8_t>(packet_size>>8);
            size_field[1] = static_cast<uint8_t>(packet_size&0xff);

        }

    void CreatePack ( Pack_struc	 *pack,
                      clockticks   SCR,
                      unsigned int 	 mux_rate
        );
    void CreateSysHeader ( Sys_header_struc *sys_header,
                           unsigned int	 rate_bound,
                           bool	 fixed,
                           int	     CSPS,
                           bool	 audio_lock,
                           bool	 video_lock,
                           vector<MuxStream *> &streams
        );

    void Close() { fclose(strm); }

private:

    /* TODO: Replace **'s with *&'s */
    static void 
    BufferDtsPtsMpeg1ScrTimecode (clockticks    timecode,
                                  uint8_t  marker,
                                  uint8_t **buffer);
    static void BufferMpeg2ScrTimecode( clockticks    timecode,
                                        uint8_t **buffer);
    void BufferPaddingPacket( int padding,
                              uint8_t *&buffer );
private:
    unsigned int mpeg_version;
    unsigned int sector_size;
    int segment_num;
    off_t max_segment_size;
    FILE *strm;
    char filename_pat[_MAX_PATH];
    char cur_filename[_MAX_PATH];
    uint8_t *sector_buf;
};
#endif // __SYSTEMS_HH__
