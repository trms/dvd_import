// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// tools.h:	part of the mjpgTools Library																				 //
//			Header File for specific commands																			 //
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

#ifndef __TOOLS_H__
#define __TOOLS_H__

void infoMessage(const char* lpText, ...);							// displays a message onto stderr or in a messagebox
void warnMessage2(const char* lpTitle, const char* lpText, ...);	// displays a message onto stderr or in a messagebox and but uses a user-custom Title too
void infoMessage2(const char* lpTitle, const char* lpText, ...);
void errMessage2(const char* lpTitle, const char* lpText, ...);

#endif // __TOOLS_H__
