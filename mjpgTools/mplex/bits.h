// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// bits.h: interface for the BitStreamUndo class, part of the mjpgTools Library											 //
//		   interface for the BitStream class, part of the mjpgTools Library												 //
//		   interface for the IBitStream class, part of the mjpgTools Library											 //
//		   interface for the OBitStream class, part of the mjpgTools Library											 //
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
#ifndef __BITS_H__
#define __BITS_H__
// bit-level output
#include "types.h"
#include <stdio.h>

typedef uint64_t bitcount_t;

class BitStreamUndo
{
//protected:
public:
	uint8_t outbyte;
	unsigned int byteidx;
	int bitidx;
	unsigned int bufcount;
	fpos_t actpos;
	bitcount_t totbits;
	bitcount_t buffer_start;
	bitcount_t readpos;
	uint8_t *bfr;
	unsigned int bfr_size;
public:
	bool eobs;

};

class BitStream : public BitStreamUndo
{
public:
	FILE *fileh;
	static const unsigned int BUFFER_SIZE;
	char *filename;
public:
	BitStream();
	~BitStream();
	inline bitcount_t bitcount() { return totbits; }
	inline bool eos() { return eobs; }
};


//
// Input bit stream class.   Supports the "scanning" of a stream
// into a large buffer which is flushed once it has been read.
// N.b. if you scan ahead a long way and don't read its your
// responsibility to flush manually...
//

class IBitStream : public BitStream {
public:
	void open( char *bs_filename, unsigned int buf_size = BUFFER_SIZE);
	void SetBufSize( unsigned int buf_size);
	void close();
	uint32_t get1bit();
	uint32_t getbits(int N);
	void prepareundo(BitStreamUndo &undobuf);
	void undochanges(BitStreamUndo &undobuf);
	bool seek_sync(uint32_t sync, int N, int lim);
	void flush( bitcount_t bitposition );
	inline unsigned int buffered_bytes()
		{
			return (unsigned int)(buffer_start+bufcount-readpos);
		}
	unsigned int read_buffered_bytes( uint8_t *dst,
									  unsigned int length_bytes);
private:
	bool refill_buffer();
	static uint8_t masks[8];
};

class OBitStream : public BitStream {
public:
	void open( char *bs_filename, unsigned int buf_size = BUFFER_SIZE);
	void close();
	void putbits( int val, int n);
	void put1bit( int val);
	void alignbits();
private:
	void putbyte();
};

#endif  // __BITS_H__

