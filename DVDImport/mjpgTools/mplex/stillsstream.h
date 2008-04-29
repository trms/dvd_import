// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// stillsstream.h:  Class for elemenary still video streams Most functionality is inherited from VideoStream,			 //
//					part of the mjpgTools Library																		 //
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
#include "types.h"
#include "videostrm.h"

class FrameIntervals
{
public:
	virtual int NextFrameInterval() = 0;
};

//
// Class of sequence of frame intervals.
//

class ConstantFrameIntervals : public FrameIntervals
{
public:
	ConstantFrameIntervals( int _frame_interval ) :
		frame_interval( _frame_interval )
		{
		}
	int NextFrameInterval() { return frame_interval; };
private:
	int frame_interval;
};


//
// Class for video stills sequence for (S)VCD non-mixed stills segment 
// item
//

class StillsStream : public VideoStream
{
public:
	StillsStream( IBitStream &ibs, 
                  OutputStream &into, FrameIntervals *frame_ints) :
		VideoStream( ibs, into ),
		current_PTS(0),
		current_DTS(0),
		intervals( frame_ints )
		{}
	void Init( );
private:
	virtual void NextDTSPTS( clockticks &DTS, clockticks &PTS );
	clockticks current_PTS;
	clockticks current_DTS;
	FrameIntervals *intervals;
};

//
// Class for video stills sequence for VCD mixed stills Segment item.
// 

class VCDStillsStream : public StillsStream
{
public:
	VCDStillsStream( IBitStream &ibs,  
                     OutputStream &into, FrameIntervals *frame_ints) :
		StillsStream( ibs, into, frame_ints ),
		sibling( 0 ),
        stream_mismatch_warned( false )
		{}
	
	void SetSibling( VCDStillsStream * );
	virtual bool MuxPossible();
private:
	bool LastSectorLastAU();
	VCDStillsStream *sibling;
    bool stream_mismatch_warned;
	
};
