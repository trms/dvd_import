// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// padstrm.h:  Padding stream pseudo input-streamsin, part of the mjpgTools Library										 //
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
#ifndef __PADSTRM_H__
#define __PADSTRM_H__

#include "inputstrm.h"

class PaddingStream : public MuxStream
{
public:
	PaddingStream()
		{
			MuxStream::Init( PADDING_STR, 0, 0,  0, false,false );
		}

	unsigned int ReadStrm(uint8_t *dst, unsigned int to_read);
};

class VCDAPadStream : public MuxStream
{
public:
	VCDAPadStream()
		{
			Init( PADDING_STR, 0, 0, 20, false, false );

		}

	unsigned int ReadStrm(uint8_t *dst, unsigned int to_read);
};

class DVDPriv2Stream : public MuxStream
{
public:
	DVDPriv2Stream()
		{
			Init( PRIVATE_STR_2, 0, 0, 0, false, false );

		}

	unsigned int ReadStrm(uint8_t *dst, unsigned int to_read);
};


#endif // __PADSTRM_H__
