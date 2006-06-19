// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// audiostrm.h:  Input stream classes for MPEG multiplexing, part of the mjpgTools Library								 //
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
#ifndef __AUDIOSTRM_H__
#define __AUDIOSTRM_H__

#include "types.h"
#include "inputstrm.h"

class AudioStream : public ElementaryStream
{
public:   
    AudioStream(IBitStream &ibs, OutputStream &into );
    virtual void Init(const int stream_num) = 0;
    virtual void Close() = 0;

    void OutputSector();
    bool RunOutComplete();
    virtual unsigned int NominalBitRate() = 0;

    unsigned int num_syncword	;
    unsigned int num_frames [2];
    unsigned int size_frames[2];
	unsigned int version_id ;
    unsigned int layer		;
    unsigned int protection	;
    unsigned int bit_rate_code;
    unsigned int frequency	;
    unsigned int mode		;
    unsigned int mode_extension ;
    unsigned int copyright      ;
    unsigned int original_copy  ;
    unsigned int emphasis	;

protected:
    virtual bool AUBufferNeedsRefill();
	virtual void FillAUbuffer(unsigned int frames_to_buffer) = 0;
	void InitAUbuffer();
    
	/* State variables for scanning source bit-stream */
    unsigned int framesize;
    unsigned int skip;
    unsigned int samples_per_second;
	uint64_t length_sum;
    AAunit access_unit;
}; 	

class MPAStream : public AudioStream
{
public:   
    MPAStream(IBitStream &ibs, OutputStream &into );
    virtual void Init(const int stream_num);
    static bool Probe(IBitStream &bs);
    virtual void Close();
    virtual unsigned int NominalBitRate();


private:
	void OutputHdrInfo();
	unsigned int SizeFrame( int bit_rate, int padding_bit );
	virtual void FillAUbuffer(unsigned int frames_to_buffer);

    
	/* State variables for scanning source bit-stream */
    unsigned int framesize;
    unsigned int skip;
    unsigned int samples_per_second;
}; 	



class AC3Stream : public AudioStream
{
public:   
    AC3Stream(IBitStream &ibs,OutputStream &into );
    virtual void Init(const int stream_num);
    static bool Probe(IBitStream &bs);
    virtual void Close();
    virtual unsigned int NominalBitRate();


private:
	void OutputHdrInfo();
	virtual void FillAUbuffer(unsigned int frames_to_buffer);

    static const unsigned int default_buffer_size;
	/* State variables for scanning source bit-stream */
    unsigned int framesize;
    unsigned int samples_per_second;
    unsigned int bit_rate;
}; 	

#endif // __AUDIOSTRM_H__


/* 
 * Local variables:
 *  c-file-style: "stroustrup"
 *  tab-width: 4
 *  indent-tabs-mode: nil
 * End:
 */
