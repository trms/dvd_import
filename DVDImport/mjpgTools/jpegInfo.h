// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// jpegInfo.h:		part of the mjpgTools Library																		 //
//					Declaration of the function that launches the thread which does the 'real' work						 //
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
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
#ifndef __JPEGINFO_H__
#define __JPEGINFO_H__

typedef struct _jpegInfo_s {
	LPCTSTR		lpSource;		// [IN]  source path to file to query
	long		iWidth;			// [OUT] width of image
	long		iHeight;		// [OUT] heigth of image
	int			retVal;			// [OUT] '0' at success, !0 otherwise
} jpegInfoData_t;

void jpegInfo_Thread( void* arg ); // 'arg' should be of type 'jpegInfoData_t*'

#endif // __JPEGINFO_H__