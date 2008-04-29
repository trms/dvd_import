// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// vector.cpp: implementation of the AUStream class, part of the mjpgTools Library										 //
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "mjpeg_types.h"
#include "vector.h"


AUStream::AUStream() : 
	cur_rd( 0 ), cur_wr(0), totalctr(0), size(0), buf(0)
{
	//buf = new (Aunit*)[AUStream::BUF_SIZE];
	buf = (Aunit**)(malloc(sizeof(Aunit*)*AUStream::BUF_SIZE));
}

const int AUStream::BUF_SIZE = 128;

void AUStream::init( Aunit *rec )
{
	buf[cur_wr] = rec;
	++cur_wr;
	cur_wr = cur_wr >= AUStream::BUF_SIZE ? 0 : cur_wr;
	cur_rd = cur_wr;
}
