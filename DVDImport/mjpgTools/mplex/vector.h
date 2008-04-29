// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// vector.h: interface for the AUStream class, part of the mjpgTools Library											 //
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
#ifndef __AUSTREAM_H__
#define __AUSTREAM_H__

//#include <config.h>
#include "types.h"
#include <deque>
//#include "mjpeg_logging.h"
#include "aunit.h"

class AUStream
{
public:
	AUStream();

	void init( Aunit *rec );

	void append( Aunit &rec )
	{
		if( size == BUF_SIZE )
			error_exit_msg( "INTERNAL ERROR: AU buffer overflow" );
		*buf[cur_wr] = rec;
		++size;
		++cur_wr;
		cur_wr = cur_wr >= BUF_SIZE ? 0 : cur_wr;
	}

	inline Aunit *next( ) 
	{ 
		if( size==0 )
		{
			return 0;
		}
	    else
		{
			Aunit *ret;
			ret = buf[cur_rd];
			++cur_rd;
			++totalctr;
			--size;
			cur_rd = cur_rd >= BUF_SIZE ? 0 : cur_rd;
			return ret;
		}
	}

	inline Aunit *lookahead( )
	{
		return size == 0 ? 0 : buf[cur_rd];
    }

	static const int BUF_SIZE;

	inline unsigned int current() { return totalctr; }
//private:
	unsigned int cur_rd;
	unsigned int cur_wr;
	unsigned int totalctr;
	unsigned int size;
	Aunit **buf;
};




#endif // __AUSTREAM_H__
