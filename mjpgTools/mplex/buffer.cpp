// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// buffer.cpp:  implementation for the BufferModel class, part of the mjpgTools Library									 //
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
#include "buffer.h"
#include <stdlib.h>

/******************************************************************
 *	Remove entries from FIFO buffer list, if their DTS is less than
 *	actual SCR. These packet data have been already decoded and have
 *	been removed from the system target decoder's elementary stream
 *	buffer.
 *****************************************************************/

void BufferModel::Cleaned(clockticks SCR)
{
    BufferQueue *pointer;

    while ((first != NULL) && first->DTS < SCR)
    {
	  pointer = first;
	  first = first->next;
	  delete pointer;	
    }
}

/******************************************************************
 * Return the SCR when there will next be some change in the
 * buffer.
 * If the buffer is empty return a zero timestamp.
 *****************************************************************/

clockticks BufferModel::NextChange()
{
	if( first == NULL )
		return static_cast<clockticks>(0);
	else
		return first->DTS;
}


/******************************************************************
 *
 *	Remove all entries from FIFO buffer list, if their DTS is less
 *	than actual SCR. These packet data have been already decoded and
 *	have been removed from the system target decoder's elementary
 *	stream buffer.
 *****************************************************************/

void BufferModel::Flushed ()
{
    BufferQueue *pointer;

    while (first != NULL)
    {
	  pointer = first;
	  first = first->next;
	  delete pointer;	
    }
}

/******************************************************************
	BufferModel::Space

	returns free space in the buffer
******************************************************************/

unsigned int BufferModel::Space ()
{
    unsigned int used_bytes;
    BufferQueue *pointer;

    pointer=first;
    used_bytes=0;

    while (pointer != NULL)
    {
		used_bytes += pointer->size;
		pointer = pointer->next;
    }

    return (max_size - used_bytes);

}

/******************************************************************
	Queue_Buffer

	adds entry into the buffer FIFO queue
******************************************************************/

void BufferModel::Queued (unsigned int bytes,
					clockticks TS)
{
    BufferQueue *pointer;

    pointer=first;
    if (pointer==NULL)
    {
		first = new BufferQueue;
		first->size = bytes;
		first->next=NULL;
		first->DTS = TS;
    } 
	else
    {
		while ((pointer->next)!=NULL)
		{
			pointer = pointer->next;
		}

		pointer->next = (BufferQueue*) malloc (sizeof (BufferQueue));
		pointer->next->size = bytes;
		pointer->next->next = NULL;
		pointer->next->DTS = TS;
    }
}


void BufferModel::Init ( unsigned int size)
{
    max_size = size;
    first = 0;
}
