// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// privateMessages.h:	part of the mjpgTools Library																	 //
//						private strings																					 //
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

#ifndef __PRIVATEMESSAGES_H__
#define __PRIVATEMESSAGES_H__

#define lk_NO_INPUT_MESSAGE					"%s: No input-file has been entered"
#define lk_NO_OUTPUT_MESSAGE				"%s: No output-file has been entered"
#define lk_INPUT_NOT_FOUND					"[%s]\nFile for INPUT couldn't been found.\nCorrect the problem and retry."
#define lk_OUTPUT_PATH_DOES_NOT_EXIST		"[%s%s]\nPath for OUTPUT doesn't exist,\nCorrect the problem and retry."
#define lk_FILE_EXISTS_MESSAGE				"File [%s] already exisits.\nIs it okay to overwrite?"
#define lk_FILE_EXISTS_TITLE				"File exists"
#define lk_OVERWRITE_ERROR					"[%s] :\nThe library was unable to open the file in WRITE mode.\nProperly the file is Read-Only or you don't have access to this file.\n\nCorrect the problem and retry."
#define lk_CANNOT_OPEN_FOR_READ				"[%s]\nUnable to open \"%s\"\nfor reading.\n\nCorrect the problem and retry."
#define lk_INVALID_SIZE						"[%s]\nInvalid '%s' entered for resizing.\nIt's '0' or not a multiple of '2'."
#define lk_PROCESS_ERROR					"exiting at error, after calling '%s'"

#endif // __PRIVATEMESSAGES_H__