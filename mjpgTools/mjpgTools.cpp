// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// mjpgTools.cpp:	part of the mjpgTools Library																		 //
//					Defines the entry point for the DLL application.													 //
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

#include "stdafx.h"
#include <io.h>
#include <stdio.h>	// for 'fopen', 'EOF', 'getc', 'fclose'
#include <stdlib.h>	// for '_MAX_PATH', '_MAX_DRIVE', '_MAX_DIR', '_MAX_FNAME', '_MAX_EXT', '_splitpath' 
#include <sys\types.h>
#include <fcntl.h>
#include <limits.h>	// for 'ULONG_MAX'
//#include <shlobj.h>

#define MJPGTOOLS_INTERNALS
#include ".\mjpeglib\config.h"		// required for "mjpgTools.h", "ppmtoyuv.h", "yuvtoppm.h"
#include ".\mjpeglib\yuv4mpeg.h"	// required for "mjpgTools.h", "ppmtoyuv.h", "yuvtoppm.h"
#include "mjpgTools.h"
#include "cjpeg.h"				// for 'cjpeg_Thread'
#include "djpeg.h"				// for 'djpeg_Thread'
#include "ppmtoyuv.h"			// for 'ppm2yuv_Thread'
#include "yuvtoppm.h"			// for 'yuv2ppm_Thread'
#include "jpeg2yuv.h"			// for 'jpeg2yuv_Thread'
#include "_yuvscaler.h"			// for 'yuvscaler_Thread'
#include "Mpg2enc.h"			// for 'mpg2enc_Thread'
#include "Mplex.h"				// for 'mplex_Thread'
#include "StillPatcher.h"		// for 'stillPatcher_Thread'
#include "jpegInfo.h"			// for 'jpegInfo_Thread'
#include "./tools/tools.h"		// for progress-stuff

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   _GetFileType																										//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
unsigned int WINAPI _GetFileType(LPCTSTR inputFile)
{
	unsigned int fType = FILETYPE_UNKNOWN; // default value
	if ((inputFile == NULL) || (!inputFile[0])) return fType;

	int fF;
	if ((fF = _open(inputFile, _O_RDONLY | _O_BINARY)) != -1)
	{
		// is it a Y4M yuv-stream?
		mjpeg_default_handler_verbosity(0); // supress any popup-message
		y4m_stream_info_t y4m_str_info;
		// initialise this struct
		y4m_init_stream_info(&y4m_str_info);
		// read the header of the file
		if (y4m_read_stream_header(fF, &y4m_str_info) == Y4M_OK)
			fType = FILETYPE_YUV;
		// release
		y4m_fini_stream_info(&y4m_str_info);
		_close(fF);
	}

	if (fType != FILETYPE_UNKNOWN)
		return fType; // already known, no need for further testing

	// See wether the file could be a MPeG
	__try
	{
		fType = GetMpegType(inputFile);
	}
	__except( GetExceptionCode() != 0 )
	{
		fType = FILETYPE_UNKNOWN;
	}

	if (fType != FILETYPE_UNKNOWN)
		return fType; // already known, no need for further testing
	
	FILE* fIn = fopen(inputFile, "r");
	if (fIn == NULL) return fType; // error trying to open the file

	int c1, c2=EOF;

	if ((c1 = getc(fIn)) != EOF)
		c2 = getc(fIn);

	// no further need of the input file
	fclose(fIn); fIn=NULL;
	if ( (c1 == EOF) || (c2 == EOF) )
		return fType;

	switch (c1)
	{
		case 'B' :
		{
			fType = FILETYPE_BMP;
			break;
		}
		case 'G' :
		{
			fType = FILETYPE_GIF;
			break;
		}
		case 'P' :
		{
			fType = FILETYPE_PPM;
			break;
		}
		case 'R' :
		{
			fType = FILETYPE_RLE;
			break;
		}
		case 0x00 :
		{
			fType = FILETYPE_TGA;
			break;
		}
		case 255 :
		{ // possible JPeG, must test 2nd byte too
			if (c2 == 0xD8)
				fType = FILETYPE_JPG;
			break;
		}
		default:
		{
			// maybe it's targa, check on file-extension
			char fExt[_MAX_EXT];
			_splitpath(inputFile, NULL, NULL, NULL, fExt);
			if ((fExt[0]) && (strnicmp(fExt, _T(".tga"), 4) == 0))
				fType = FILETYPE_TGA;
			break;
		}
	}

	return fType;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   MakeJPeG																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI MakeJPeG( lpcjpeg_switches arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(cjpeg_Thread, 0, (void*)&ourParam) == -1) 
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   JPG_2_BMP																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI JPG_2_BMP( lpdjpeg_switches arg, unsigned int winBMP )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;
	// set thread-param
	void* lpArg[2];
	lpArg[0] = (void*)arg;
	lpArg[1] = (void*)((winBMP==0) ? FMT_OS2 : FMT_BMP);

	caller2thread_t ourParam;
	ourParam.arg	= (void*)lpArg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(djpeg_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   JPG_2_GIF																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI JPG_2_GIF( lpdjpeg_switches arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;
	// set thread-param
	void* lpArg[2];
	lpArg[0] = (void*)arg;
	lpArg[1] = (void*)FMT_GIF;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)lpArg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(djpeg_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   JPG_2_PPM																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI JPG_2_PPM( lpdjpeg_switches arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;
	// set thread-param
	void* lpArg[2];
	lpArg[0] = (void*)arg;
	lpArg[1] = (void*)FMT_PPM;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)lpArg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(djpeg_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   JPG_2_TGA																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI JPG_2_TGA( lpdjpeg_switches arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;
	// set thread-param
	void* lpArg[2];
	lpArg[0] = (void*)arg;
	lpArg[1] = (void*)FMT_TARGA;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)lpArg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(djpeg_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   FileExists																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
BOOL WINAPI FileExists(LPCTSTR pPath)
{
	if ((pPath==NULL) || (!pPath[0])) return FALSE;

	char buf[1024], *fBuf=NULL;
	
	// use c-routine to split the path
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME+_MAX_EXT+1];
	char ext[_MAX_EXT];
	_splitpath( pPath, drive, dir, fname, ext );
	sprintf(path_buffer, "%s%s", fname, ext);
	if (!path_buffer[0]) strcpy(path_buffer, ".");
	strcpy(fname, path_buffer);
	sprintf(path_buffer, _T("%s%s"), drive, dir); // make 1 path

	ZeroMemory(buf, sizeof(buf));

	BOOL result = (SearchPath(
					path_buffer,	// pointer to search path
					fname,			// pointer to filename
					NULL,			// pointer to extension
					1023,			// size, in characters, of buffer
					buf,			// pointer to buffer for found filename
					&fBuf			// pointer to pointer to file component
					) == 0) ? FALSE : TRUE;

	return result;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   PathExists																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
BOOL WINAPI PathExists(LPCTSTR pPath)
{
	if ((pPath==NULL) || (!pPath[0])) return FALSE;

	// use c-routine to split the path
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath( pPath, drive, dir, NULL, NULL );
	// be sure of last '\\'
	if (dir[strlen(dir)-1] != '\\') strcat(dir, "\\");
	sprintf(path_buffer, _T("%s%s"), drive, dir); // make 1 path

	return FileExists(path_buffer);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   GetCorrectPath																										//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
BOOL WINAPI GetCorrectPath(LPTSTR pPath)
{
	if ((pPath==NULL) || (!pPath[0])) return FALSE;

	char buf[1024], *fBuf=NULL;
	
	// use c-routine to split the path
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME+_MAX_EXT+1];
	char ext[_MAX_EXT];
	_splitpath( pPath, drive, dir, fname, ext );
	sprintf(path_buffer, _T("%s%s"), drive, dir); // make 1 path
	strcpy(fname, ".");
/*
	if (fname[0])
	{ // contains a filename
		// we only wants to have the dir+drive
		path_buffer[strlen(path_buffer)-1]=0;
		_splitpath( path_buffer, drive, dir, fname, ext );
		wsprintf(path_buffer, _T("%s%s"), drive, dir); // make 1 path
	}
*/
	ZeroMemory(buf, sizeof(buf));

	BOOL result = (SearchPath(
					path_buffer,	// pointer to search path
					fname,			// pointer to filename
					NULL,			// pointer to extension
					1023,			// size, in characters, of buffer
					buf,			// pointer to buffer for found filename
					&fBuf			// pointer to pointer to file component
					) == 0) ? FALSE : TRUE;

	if (result)
	{
		if (strchr(buf, '/') != NULL)
		{
			while ((fBuf=strchr(pPath, '\\')) != NULL)
			{
				pPath[fBuf-pPath] = '/';
			}
		}
		else if (strchr(buf, '\\') != NULL)
		{
			while ((fBuf=strchr(pPath, '/')) != NULL)
			{
				pPath[fBuf-pPath] = '\\';
			}
		}
	}
	return result;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   IsWindowsNT																										//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
BOOL WINAPI IsWindowsNT()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);
	return (osv.dwPlatformId == VER_PLATFORM_WIN32_NT);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   IsWindowsNT5																										//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
BOOL WINAPI IsWindowsNT5()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);
	return ((osv.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osv.dwMajorVersion  >= 5));
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   IsWindows98h																										//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
BOOL WINAPI IsWindows98h()
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(osv);
	GetVersionEx(&osv);
	return ((osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		( (osv.dwMajorVersion > 4) || ((osv.dwMajorVersion == 4) && (osv.dwMinorVersion > 0)) ));
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   PPM_2_YUV																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI PPM_2_YUV( lp_ppm2yuv_switches arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(ppm2yuv_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   YUV_2_PPM																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI YUV_2_PPM( lp_yuv2ppm_switches arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(yuv2ppm_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   JPG_2_YUV																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI JPG_2_YUV( lp_jpeg2yuv_switches arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(jpeg2yuv_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   YuvScaler																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI YuvScaler( lp_yuvscaler_struct arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(yuvscaler_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   mpg2enc																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI mpg2enc( lp_mpg2enc_struct arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(mpg2enc_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   mplex																												//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI mplex( lp_mplex_struct arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(mplex_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   stillPatcher																										//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
void WINAPI stillPatcher( lp_stillpatch_struct arg )
{
	if (arg == NULL) return;
	// preset the return-value, so we can scan on it for when the thread realy is finished
	arg->m_ReturnCode = 9999;

	caller2thread_t ourParam;
	ourParam.arg	= (void*)arg;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	if ( _beginthread(stillPatcher_Thread, 0, (void*)&ourParam) == -1)
		arg->m_ReturnCode = -1; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( arg->m_ReturnCode == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ourParam.event) CloseHandle(ourParam.event);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   retrieve particular information of a graphical image (currently only JPG, BMP, GIF, PPM and TGA are supported)		//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
int WINAPI GetGraphicalInformation( LPCTSTR lpSource, long* imgHeight, long* imgWidth )
{
	if (imgHeight)	*imgHeight	= 0;
	if (imgWidth)	*imgWidth	= 0;

	if (!lpSource) return 1;

	jpegInfoData_t infoData; ZeroMemory(&infoData, sizeof(jpegInfoData_t));
	// preset the return-value, so we can scan on it for when the thread realy is finished
	infoData.retVal = 9999;

	// pre-init
	infoData.lpSource = lpSource; // setting ptr to source-file

	caller2thread_t ourParam;
	ourParam.arg	= (void*)&infoData;
	ourParam.event	= CreateEvent(NULL, TRUE, FALSE, NULL);
	if (ourParam.event) ResetEvent(ourParam.event);

	int ret = 0;
	if ( _beginthread(jpegInfo_Thread, 0, (void*)&ourParam) == -1)
		ret = 2; // some error occured
	else
	{
		if (ourParam.event)
			WaitForSingleObject(ourParam.event, INFINITE);
		else
			while ( infoData.retVal == 9999 )
				Sleep(100); // wait for 100 mili-seconds
		// must wait some extra time to let the thread realy finish
		Sleep(100);
	}

	if (ret == 0)
	{
		if (imgHeight)	*imgHeight	= infoData.iHeight;
		if (imgWidth)	*imgWidth	= infoData.iWidth;
	}

	if (ourParam.event) CloseHandle(ourParam.event);

	return (ret==0) ? infoData.retVal : ret;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//	Attempts to create a STILL (MPeG-1/-2 from a BMP/GIF/TGA/PPM/JPG													//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// inStream		= full path of input file
// outStream	= on successfull completion, will receive the full path of the created STILL (must be of at least the same size as inStream)
// still1		= TRUE to create a 'MPeG-1 STILL', FALSE to create a 'MPeG-2 STILL'
// pal			= TRUE (default) to creat a PAL/SECAM STILL, FALSE to create a NTSC STILL
// csVerbose	= Verbosity : 0=Silent (default), 1=Informational, 2=Debug
// returns true when successfull
bool WINAPI create_still(const char* inStream, char* outStream, BOOL still1, BOOL pal, short csVerbose)
{ // try's to create a still of the file
	outStream[0]=0;
	if ((inStream==NULL) || (!inStream[0])) return false;
	if (!FileExists(inStream)) return false;
	// resulting stream will come into %TEMP%\inStream.mpg

	char tempPath[MAX_PATH]; tempPath[0]=0;
	char tempFile[MAX_PATH], strSource[MAX_PATH];
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME];
	ULONG u;

	GetTempPath(MAX_PATH, tempPath);
	if (!tempPath[0]) strcpy(tempPath, _T("."));

	strcpy(strSource, inStream);
	int fType = 0;
	for (fType = -1; fType != FILETYPE_UNKNOWN; )
	{
		fType = _GetFileType(strSource);

		GetTempFileName(tempPath, _T("lhk"), 0, tempFile);
		if (FileExists(tempFile)) DeleteFile(tempFile);
		_splitpath(tempFile, drive, dir, fname, NULL);
		wsprintf(tempFile, _T("%s%s%s."), drive, dir, fname);

		switch (fType)
		{
			case FILETYPE_BMP :
			case FILETYPE_GIF :
			case FILETYPE_TGA :
			{ // convert to JPeG first
				strcat(tempFile, _T("jpg"));

				cjpeg_switches	jpg_switches;
				ZeroMemory(&jpg_switches, sizeof(cjpeg_switches));
				// Create a JPG file
				strcpy(jpg_switches.infile, strSource);
				strcpy(jpg_switches.outfile, tempFile);
				// do it
				MakeJPeG( &jpg_switches );

				if (stricmp(inStream, strSource) != 0)
					DeleteFile(strSource); // delete original file first, unless it's the same as the input-stream

				if (jpg_switches.m_ReturnCode == 0)
				{ // all wend fine
					strcpy(strSource, tempFile);
					tempFile[0] = 0;
				}
				else
				{ // something wend wrong
					if (FileExists(tempFile)) DeleteFile(tempFile);
					fType = FILETYPE_UNKNOWN;
				}
				break;
			}
			case FILETYPE_JPG :
			{ // convert to PPM first
				strcat(tempFile, _T("ppm"));

				djpeg_switches	ppm_switches;
				ZeroMemory(&ppm_switches, sizeof(djpeg_switches));
				// create a PPM file
				strcpy(ppm_switches.infile, strSource);
				strcpy(ppm_switches.outfile, tempFile);
				JPG_2_PPM( &ppm_switches );

				if (stricmp(inStream, strSource) != 0)
					DeleteFile(strSource); // delete original file first, unless it's the same as the input-stream

				if (ppm_switches.m_ReturnCode == 0)
				{ // all wend fine
					strcpy(strSource, tempFile);
					tempFile[0]=0;
				}
				else
				{ // something wend wrong, stopping
					fType = FILETYPE_UNKNOWN;
					if (FileExists(tempFile)) DeleteFile(tempFile);
				}
				break;
			}
			case FILETYPE_PPM :
			{ // convert to YUV
				strcat(tempFile, _T("yuv"));

				ppm2yuv_switches yuv_switches;
				ZeroMemory(&yuv_switches, sizeof(ppm2yuv_switches));
				yuv_switches.frame_rate = y4m_fps_PAL;
				strcpy(yuv_switches.infile, strSource);
				strcpy(yuv_switches.outfile, tempFile);
				if (!pal)
				{ // so creating NTSC
					yuv_switches.frame_rate	= y4m_fps_NTSC;
/*
					yuv_switches.frame_rate.n = y4m_fps_NTSC.n;
					yuv_switches.frame_rate.d = y4m_fps_NTSC.d;
*/
				} // else if pal/secam, no need to preset the framerate - default=pal/secam
				yuv_switches.verbose = csVerbose+1; // silent please

				// do it
				PPM_2_YUV( &yuv_switches );

				if (stricmp(inStream, strSource) != 0)
					DeleteFile(strSource); // delete original file first, unless it's the same as the input-stream

				if (yuv_switches.m_ReturnCode != 0)
				{ // something wend wrong, stopping
					if (FileExists(tempFile)) DeleteFile(tempFile);
					fType = FILETYPE_UNKNOWN;
					break; // no need to continue this 'case'-procedure
				}

				// YUV Scaling
				yuvscaler_struct	scale_switches;
				char sLow[MAX_PATH], sHigh[MAX_PATH];
				sLow[0] = sHigh[0] = 0;

				if (still1)
				{ // create a MPeG-1 STILL, we need to have a low-res frame too
					// creating low-resolution yuv
					ZeroMemory(&scale_switches, sizeof(yuvscaler_struct));
					scale_switches.verbosity = csVerbose+1; // completely silent please
					// preparing switches
					_splitpath(tempFile, drive, dir, fname, NULL);
					strcpy(scale_switches.infile, tempFile);
					wsprintf(scale_switches.outfile, _T("%s%s%s_low.yuv"), drive, dir, fname);
					// OUTPUT frame characteristics
					scale_switches.Output = 3; // resize
					if (pal)
					{
						scale_switches.resize[0] = 288; // height
						scale_switches.resize[1] = 352; // width
						scale_switches.outputNORM = 0; // PAL
					}
					else
					{ // ntsc
						scale_switches.resize[0] = 240; // height
						scale_switches.resize[1] = 352; // width
						scale_switches.outputNORM = 2; // NTSC
					}
					// do it
					YuvScaler(&scale_switches);

					if (scale_switches.m_ReturnCode != 0)
					{ // something wend wrong
						if (FileExists(tempFile)) DeleteFile(tempFile);
						if (FileExists(scale_switches.outfile)) DeleteFile(scale_switches.outfile);

						fType = FILETYPE_UNKNOWN;
						break; // no need to continue this 'case'-procedure
					}
					// store the name of this resize YUV-file in 'sLow'
					strcpy(sLow, scale_switches.outfile);
				}

				// creating high-res yuv
				ZeroMemory(&scale_switches, sizeof(yuvscaler_struct));
				scale_switches.verbosity = csVerbose+1; // completely silent please
				// preparing switches
				_splitpath(tempFile, drive, dir, fname, NULL);
				strcpy(scale_switches.infile, tempFile);
				wsprintf(scale_switches.outfile, _T("%s%s%s_high.yuv"), drive, dir, fname);
				// OUTPUT frame characteristics
				scale_switches.Output = 3; // resize
				if (pal)
				{
					scale_switches.resize[0] = 576; // height
					scale_switches.resize[1] = 704; // width
					scale_switches.outputNORM = 0; // PAL
				}
				else
				{ // ntsc
					scale_switches.resize[0] = 480; // height
					scale_switches.resize[1] = 704; // width
					scale_switches.outputNORM = 2; // NTSC
				}
				// do it
				YuvScaler(&scale_switches);

				// no more need of the original yuv-file
				if (FileExists(tempFile)) DeleteFile(tempFile);

				if (scale_switches.m_ReturnCode != 0)
				{ // something wend wrong
					if (FileExists(scale_switches.outfile)) DeleteFile(scale_switches.outfile);
					if ((sLow[0]) && (FileExists(sLow))) DeleteFile(sLow);

					fType = FILETYPE_UNKNOWN;
					break; // no need to continue this 'case'-procedure
				}
				// store the name of this resize YUV-file in 'sHigh'
				strcpy(sHigh, scale_switches.outfile);


				// rescaling wend fine, so now creating an mpeg of it
				mpg2enc_struct mpg2enc_param;

				if (still1)
				{ // MPeG-1 : a low- and a high-res
					ZeroMemory(&mpg2enc_param, sizeof(mpg2enc_struct));
					mpg2enc_param.verbose = csVerbose+1; // silent please
					// preparing
					_splitpath(sLow, drive, dir, fname, NULL);
					strcpy(mpg2enc_param.infile, sLow);
					wsprintf(mpg2enc_param.outfile, _T("%s%s%s.m1v"), drive, dir, fname);
					// -f 6 -T 42
					mpg2enc_param.format		= 6;
					mpg2enc_param.vcd_ssize		= 42;
					mpg2enc_param.aspect_ratio	= 2;
					if (pal)
					{
						mpg2enc_param.norm			= 'p';
						mpg2enc_param.frame_rate	= 3;
					}
					else
					{
						mpg2enc_param.norm			= 'n';
						mpg2enc_param.frame_rate	= 4;
					}
					// do it
					mpg2enc(&mpg2enc_param);

					// no more need of the original yuv-file
					DeleteFile(sLow);

					if (mpg2enc_param.m_ReturnCode != 0)
					{ // something wend wrong
						if (FileExists(mpg2enc_param.outfile)) DeleteFile(mpg2enc_param.outfile);
						if (FileExists(sHigh)) DeleteFile(sHigh);

						fType = FILETYPE_UNKNOWN;
						break; // no need to continue this 'case'-procedure
					}
					// store a copy of the resulting output-name in 'sLow'
					strcpy(sLow, mpg2enc_param.outfile);
				}

				// create a high-res m1v now
				ZeroMemory(&mpg2enc_param, sizeof(mpg2enc_struct));
				mpg2enc_param.verbose = csVerbose+1; // silent please
				// preparing
				_splitpath(sHigh, drive, dir, fname, NULL);
				strcpy(mpg2enc_param.infile, sHigh);
				wsprintf(mpg2enc_param.outfile, _T("%s%s%s.m1v"), drive, dir, fname);
				// -T 120
				mpg2enc_param.vcd_ssize		= 120;
				mpg2enc_param.aspect_ratio	= 2;
				if (pal)
				{
					mpg2enc_param.norm			= 'p';
					mpg2enc_param.frame_rate	= 3;
				}
				else
				{
					mpg2enc_param.norm			= 'n';
					mpg2enc_param.frame_rate	= 4;
				}
				if (still1)
					mpg2enc_param.format	= 6; // VCD
				else
					mpg2enc_param.format	= 7; // SVCD

				// do it
				mpg2enc(&mpg2enc_param);

				// no more need of the original yuv-file
				DeleteFile(sHigh);

				if (mpg2enc_param.m_ReturnCode != 0)
				{ // something wend wrong
					if (FileExists(mpg2enc_param.outfile)) DeleteFile(mpg2enc_param.outfile);
					if ((sLow[0]) && (FileExists(sLow))) DeleteFile(sLow);

					fType = FILETYPE_UNKNOWN;
					break; // no need to continue this 'case'-procedure
				}
				// store a copy of the resulting output-name in 'sHigh'
				strcpy(sHigh, mpg2enc_param.outfile);

				// Multi-Plexing
				mplex_struct	mplex_switches;
				char** strArr=NULL;
				int arrSize=0;

				if (still1)
				{ // multiplexing a low-res and a high-res together
					strArr = (char**)(malloc(sizeof(char*) * 3));
					strArr[0] = sLow;
					strArr[1] = sHigh;
					strArr[2] = NULL;
				}
				else
				{
					strArr = (char**)(malloc(sizeof(char*) * 2));
					strArr[0] = sHigh;
					strArr[1] = NULL;
				}
				// preparing
				ZeroMemory(&mplex_switches, sizeof(mplex_struct));
				mplex_switches.verbose = csVerbose+1; // silent pleas
				mplex_switches.infiles = strArr;
				_splitpath(tempFile, drive, dir, NULL, NULL); // taking drie+dir of tempFile
				_splitpath(inStream, NULL, NULL, fname, NULL); // taking filename of original input-file
				wsprintf(mplex_switches.outfile, _T("%s%s%s.mpg"), drive, dir, fname);
				u = 0; // get us a unique filename of the output
				while (FileExists(mplex_switches.outfile))
				{
					if (u == ULONG_MAX) break;
					wsprintf(mplex_switches.outfile, _T("%s%s%s_%lX.mpg"), drive, dir, fname, u++);
								// using hexadecimal suffix to keep the outfilename a bit short (max = ..._FFFFFFFE)
				}
				if (u == ULONG_MAX)
				{ // we cannot get a unique filename (would it be possible that a directory has 4GB of files??)
					if ((sLow[0]) && (FileExists(sLow))) DeleteFile(sLow);
					if (FileExists(sHigh)) DeleteFile(sHigh);
					fType = FILETYPE_UNKNOWN;
					break;
				}
				if (still1)
					mplex_switches.format  = 6; // VCD
				else
					mplex_switches.format  = 7; // SVCD
				// do it
				mplex(&mplex_switches);

				free(strArr);

				// no more need of sLow and sHigh
				if ((sLow[0]) && (FileExists(sLow))) DeleteFile(sLow);
				if (FileExists(sHigh)) DeleteFile(sHigh);

				if (mplex_switches.m_ReturnCode == 0)
				{ // all wend fine
					strcpy(strSource, mplex_switches.outfile);
					tempFile[0] = 0;
				}
				else
				{ // something wend wrong
					if (FileExists(mplex_switches.outfile)) DeleteFile(mplex_switches.outfile);
					fType = FILETYPE_UNKNOWN;
				}
				break;
			}
			case FILETYPE_MPG1 :
			case FILETYPE_MPG2 :
			{
				if (((still1) && (fType == FILETYPE_MPG2)) || ((!still1) && (fType == FILETYPE_MPG1)))
				{ // requesting to create a still-1 from a MPeG-2, or a still-2 from a MPeG-1
					fType = FILETYPE_UNKNOWN;
					break;
				}
				// else, let's see wether we can patch it
				stillpatch_struct	patcher_switches;
				// preparing
				ZeroMemory(&patcher_switches, sizeof(stillpatch_struct));
				// verbose is set to false via 'ZeroMemory', so silent here too
				patcher_switches.verbose = (csVerbose > 0) ? true : false;
				strcpy(patcher_switches.infile, inStream);
				_splitpath(tempFile, drive, dir, NULL, NULL); // taking drie+dir of tempFile
				_splitpath(inStream, NULL, NULL, fname, NULL); // taking filename of original input-file
				// give me a realy unique file :
				wsprintf(patcher_switches.outfile, _T("%s%s%s.mpg"), drive, dir, fname);
				u = 0;
				while (FileExists(patcher_switches.outfile))
				{
					if (u == ULONG_MAX) break;
					wsprintf(patcher_switches.outfile, _T("%s%s%s_%lX.mpg"), drive, dir, fname, u++);
								// using hexadecimal suffix to keep the outfilename a bit short (max = ..._FFFFFFFE)
				}
				if (u == ULONG_MAX)
				{ // we cannot get a unique filename (would it be possible that a directory has 4GB of files??)
					fType = FILETYPE_UNKNOWN;
					break;
				}

				// do it
				stillPatcher(&patcher_switches);

				if (patcher_switches.m_ReturnCode != 0)
				{ // something wend wrong
					if (FileExists(patcher_switches.outfile)) DeleteFile(patcher_switches.outfile);
					fType = FILETYPE_UNKNOWN;
				}
				else
					strcpy(outStream, patcher_switches.outfile);

				break;
			}
			case FILETYPE_STILL1 :
			{
				if (still1)
					strcpy(outStream, strSource);
				break;
			}
			case FILETYPE_STILL2 :
			{
				if (!still1)
					strcpy(outStream, strSource);
				break;
			}
//			case FILETYPE_RLE :
//			case FILETYPE_YUV :
//			case FILETYPE_MPG :
//			case FILETYPE_UNKNOWN :
			default:
			{
				// RLE and YUV and unspecified MPeG's are not supported this routine
				fType = FILETYPE_UNKNOWN;
				break;
			}
		}

		if ((fType == FILETYPE_STILL1) || (fType == FILETYPE_STILL2) || (fType == FILETYPE_UNKNOWN))
			break;
	}
	
	return (fType != FILETYPE_UNKNOWN) ? true : false;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//	Attempts to create a MPeG of a given filelist of images (MPeG-1/-2 from a BMP/GIF/TGA/PPM/JPG)						//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// inStream			= FileList, like "file1\0file2\0...\0lastfile\0\0"
// outStream		= buffer to receive path of returning MPeG, this may already contain a full path of a MPeG
//					  in which case the output will be directed to
// vcd				= create a MPeG-1 if TRUE, create a SVCD if FALSE
// pal				= create PAL if TRUE, NTSC if FALSE
// len				= length (in seconds) of each stream of each images
// forceCompliance	= force to use a compliant bitrate (for vcd that is 1150, for SVCD 2520),
//					  instead of the optimized bitrates we use (for vcd that is 1150+384, for SVCD 2520+384)
// csVerbose		= 0: Silent, 1: Info, 2: Debug
BOOL WINAPI createMxV(LPCTSTR inStream, LPTSTR outStream, BOOL vcd, BOOL pal, float len,
					  BOOL forceCompliance, short csVerbose, HWND o_hWnd, UINT o_uMsg)
{ // try's to create a M1V or M2V of an image
	BOOL ret = 0;
	char** inFiles=NULL;
	char** strArr=NULL;
	unsigned u=0, i, curPos=0, fCount=0, bgrpSize=3, totalInFiles=0, gopSize;
	int fType;
	char tempPath[MAX_PATH]; tempPath[0]=0;
	char tempFile[MAX_PATH], strSource[MAX_PATH];
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	char *inFiles2=NULL;

	unsigned	frameCount=0; // set in ppm2yuv and used in all the encoder

	gopSize = (pal) ? (25*2/4) : (30*2/5);

	if (!outStream) return 0;
	if ((inStream) && (inStream[0]))
	{ // first count amount of strings in this array
		for (i=0; inStream[i]; )
		{
			i += strlen(&inStream[i]) + 1;
			u++;
		}
		inFiles = (char**)(malloc(sizeof(char*) * (u+1)));
		u = 0;
		for (i=0; inStream[i]; )
		{
			inFiles[u++] = strdup(&inStream[i]);
			i += strlen(&inStream[i]) + 1;
		}
		inFiles[u] = NULL;
	}
	if (!inFiles) return 0;
	// check for existance of each file
	ret = 1;
	for (u=0; inFiles[u]; u++)
		if (!FileExists(inFiles[u]))
		{
			ret = 0;
			break;
		}
		else
			totalInFiles++;

	if (!ret)
		goto Fail;


	// resulting stream will come into %TEMP%\inStream.mpg
	GetTempPath(MAX_PATH, tempPath);
	if (!tempPath[0]) strcpy(tempPath, _T(".\\"));

	for (curPos=0; inFiles[curPos]; curPos++)
	{
		strcpy(strSource, inFiles[curPos]);
		// whatever came in, be sure it's a PPM - convert if required
		for (fType = -1; fType != FILETYPE_UNKNOWN; )
		{
			fType = _GetFileType(strSource);

			GetTempFileName(tempPath, _T("lhk"), 0, tempFile);
			if (FileExists(tempFile)) DeleteFile(tempFile);
			_splitpath(tempFile, drive, dir, fname, NULL);
			sprintf(tempFile, _T("%s%s%s."), drive, dir, fname);

			switch (fType)
			{
				case FILETYPE_BMP :
				case FILETYPE_GIF :
				case FILETYPE_TGA :
				{ // convert to JPeG first
					strcat(tempFile, _T("jpg"));

					cjpeg_switches	jpg_switches;
					ZeroMemory(&jpg_switches, sizeof(cjpeg_switches));
					// Create a JPG file
					strcpy(jpg_switches.infile, strSource);
					strcpy(jpg_switches.outfile, tempFile);
					// do it
					MakeJPeG( &jpg_switches );

					if (stricmp(inStream, strSource) != 0)
						DeleteFile(strSource); // delete original file first, unless it's the same as the input-stream

					if (jpg_switches.m_ReturnCode == 0)
					{ // all wend fine
						strcpy(strSource, tempFile);
						tempFile[0] = 0;
					}
					else
					{ // something wend wrong
						if (FileExists(tempFile)) DeleteFile(tempFile);
						fType = FILETYPE_UNKNOWN;
					}
					break;
				}
				case FILETYPE_JPG :
				{ // convert to PPM first
					strcat(tempFile, _T("ppm"));

					djpeg_switches	ppm_switches;
					ZeroMemory(&ppm_switches, sizeof(djpeg_switches));
					// create a PPM file
					strcpy(ppm_switches.infile, strSource);
					strcpy(ppm_switches.outfile, tempFile);
					JPG_2_PPM( &ppm_switches );

					if (stricmp(inStream, strSource) != 0)
						DeleteFile(strSource); // delete original file first, unless it's the same as the input-stream

					if (ppm_switches.m_ReturnCode == 0)
					{ // all wend fine
						strcpy(strSource, tempFile);
						tempFile[0]=0;
					}
					else
					{ // something wend wrong, stopping
						fType = FILETYPE_UNKNOWN;
						if (FileExists(tempFile)) DeleteFile(tempFile);
					}
					break;
				}
				case FILETYPE_PPM :
				{
					free(inFiles[curPos]);
					inFiles[curPos] = strdup(strSource);
					break;
				}
//				case FILETYPE_MPG1 :
//				case FILETYPE_MPG2 :
//				case FILETYPE_RLE :
//				case FILETYPE_YUV :
//				case FILETYPE_MPG :
//				case FILETYPE_UNKNOWN :
				default:
				{
					// RLE and YUV and unspecified MPeG's are not supported this routine
					fType = FILETYPE_UNKNOWN;
					free(inFiles[curPos]);
					inFiles[curPos] = strdup("\0");
					break;
				}
			}

			if ((fType == FILETYPE_PPM) || (fType == FILETYPE_UNKNOWN))
				break;
		}
	}

	// build length of substring
	strSource[0]=0; // first file
	i = 0;
	for (curPos=0; inFiles[curPos]; curPos++)
	{
		if (inFiles[curPos][0])
			i += (strlen(inFiles[curPos]) + 1);
	}
	if (i)
	{
		inFiles2 = new char[i+1];
		ZeroMemory(inFiles2, i+1);
		u = 0;
		for (curPos=0; inFiles[curPos]; curPos++)
		{
			if (!strSource[0])
				strcpy(strSource, inFiles[curPos]);
			else
			{
				strcpy(&inFiles2[u], inFiles[curPos]);
				u += strlen(inFiles[curPos]) + 1;
			}
		}
		inFiles2[u] = 0;
	}
	else
		goto Fail; // should not happen


	{
		ret = 1;

		GetTempFileName(tempPath, _T("lhk"), 0, tempFile);
		if (FileExists(tempFile)) DeleteFile(tempFile);
		_splitpath(tempFile, drive, dir, fname, NULL);

		// convert PPM -> YUV
		sprintf(tempFile, _T("%s%s%s.yuv"), drive, dir, fname);
		fCount  = (unsigned)((pal) ? 25.0 * len : 30.0 * len);

		{
			frameCount = 0;
			ppm2yuv_switches yuv_switches;
			ZeroMemory(&yuv_switches, sizeof(ppm2yuv_switches));
			yuv_switches.version	= 1;
			yuv_switches.o_hWnd		= o_hWnd;
			yuv_switches.o_uMsg		= o_uMsg;
			yuv_switches.frames		= &frameCount;
			yuv_switches.infile2 = inFiles2;
			strcpy(yuv_switches.infile, strSource);
			strcpy(yuv_switches.outfile, tempFile);
			yuv_switches.frame_rate = (!pal) ? y4m_fps_NTSC : y4m_fps_PAL;
			yuv_switches.verbose = csVerbose+1; // silent please
			if (fCount > 0)
			{
				yuv_switches.repeat_last = true;
				yuv_switches.frame_count = fCount;
				// be sure it's at least 4seconds long
				yuv_switches.min_frame_count = (unsigned)((pal) ? (float)25.0 * 6.5 : (float)30.0 * 6.5);
				while (yuv_switches.min_frame_count % bgrpSize != 0) yuv_switches.min_frame_count++;
			}
			if (!vcd)
			{ // svcd:
				// interlacing
				yuv_switches.interlacing = 't'; // top-field first
				yuv_switches.interleave_2Pass = true;
			}

			// do it
			PPM_2_YUV( &yuv_switches );

			if (stricmp(inStream, strSource) != 0)
			DeleteFile(strSource); // delete original file first, unless it's the same as the input-stream

			if (yuv_switches.m_ReturnCode != 0)
			{ // something wend wrong, stopping
				if (FileExists(tempFile)) DeleteFile(tempFile);
				ret = 0;
			}
		}

		if (ret)
		{ // YUV Scaling
			yuvscaler_struct	scale_switches;
			char sYUV[MAX_PATH];
			sYUV[0] = 0;

			ZeroMemory(&scale_switches, sizeof(yuvscaler_struct));
			scale_switches.verbosity = csVerbose+1; // completely silent please
			// preparing switches
			_splitpath(tempFile, drive, dir, fname, NULL);
			strcpy(scale_switches.infile, tempFile);
			// OUTPUT frame characteristics
			scale_switches.Output = 3; // resize

			if (vcd)
			{ // create a MPeG-1 STILL, we need to have a low-res frame too
				sprintf(scale_switches.outfile, _T("%s%s%s_low.yuv"), drive, dir, fname);
				// OUTPUT frame characteristics
				if (pal)
				{
					scale_switches.resize[0] = 288; // height
					scale_switches.resize[1] = 352; // width
					scale_switches.outputNORM = 0; // PAL
				}
				else
				{ // ntsc
					scale_switches.resize[0] = 240; // height
					scale_switches.resize[1] = 352; // width
					scale_switches.outputNORM = 2; // NTSC
				}
			}
			else
			{ // MPeG-2
				sprintf(scale_switches.outfile, _T("%s%s%s_high.yuv"), drive, dir, fname);
				// OUTPUT frame characteristics
				if (pal)
				{
					scale_switches.resize[0] = 576; // height
					scale_switches.resize[1] = 480; // width
					scale_switches.outputNORM = 0; // PAL
				}
				else
				{ // ntsc
					scale_switches.resize[0] = 480; // height
					scale_switches.resize[1] = 480; // width
					scale_switches.outputNORM = 2; // NTSC
				}
			}

			scale_switches.version			= 1;
			scale_switches.m_hOwner			= o_hWnd;
			scale_switches.m_uOwner			= o_uMsg;
			scale_switches.m_TotalFrames	= frameCount;

			// do it
			YuvScaler(&scale_switches);

			// no more need of the original yuv-file
			if (FileExists(tempFile)) DeleteFile(tempFile);

			if (scale_switches.m_ReturnCode != 0)
			{ // something wend wrong
				if (FileExists(scale_switches.outfile)) DeleteFile(scale_switches.outfile);
				ret = 0;
			}
			else
				strcpy(tempFile, scale_switches.outfile); // store the name of this resize YUV-file in 'sHigh'
		}

		if (ret)
		{ // rescaling wend fine, so now creating an mpeg of it
			mpg2enc_struct mpg2enc_param;

			ZeroMemory(&mpg2enc_param, sizeof(mpg2enc_struct));
			mpg2enc_param.verbose			= csVerbose+1; // silent please
			mpg2enc_param.version			= 1;
			mpg2enc_param.m_hWnd			= o_hWnd;
			mpg2enc_param.m_uMsg			= o_uMsg;
			mpg2enc_param.m_framecount		= frameCount;
			// preparing
			strcpy(mpg2enc_param.infile, tempFile);

			mpg2enc_param.aspect_ratio	= 2;

			if (vcd)
			{ // vcd
				strcpy(ext, ".m1v");
				mpg2enc_param.format		= 2;
				mpg2enc_param.v_bitrate		= 1150;
			}
			else
			{ // svcd
				strcpy(ext, ".m2v");
				mpg2enc_param.format		= 5;
				mpg2enc_param.interlace		= 1; // top field first
				mpg2enc_param.v_bitrate		= 2520;
			}
			if (!forceCompliance)
				mpg2enc_param.v_bitrate += 384; // not using any audio
			// store a copy of the resulting output-name in 'outStream'
			_splitpath(tempFile, drive, dir, fname, NULL); // taking drie+dir of tempFile
			_splitpath(strSource, NULL, NULL, fname, NULL); // taking filename of original input-file
			sprintf(mpg2enc_param.outfile, _T("%s%s%s%s"), drive, dir, fname, ext);
			u = 0; // get us a unique filename of the output
			while (FileExists(mpg2enc_param.outfile))
			{
				if (u == ULONG_MAX) break;
				sprintf(mpg2enc_param.outfile, _T("%s%s%s_%lX%s"), drive, dir, fname, u++, ext);
					// using hexadecimal suffix to keep the outfilename a bit short (max = ..._FFFFFFFE)
			}
			if (u == ULONG_MAX)
			{ // we cannot get a unique filename (would it be possible that a directory has 4GB of files??)
				if ((tempFile[0]) && (FileExists(tempFile))) DeleteFile(tempFile);
				ret = 0;
			}

//			if ((mpg2enc_param.max_gop = fCount) > 0)
			if (fCount > 0)
			{
				mpg2enc_param.bgrp_size	= bgrpSize;
				mpg2enc_param.min_gop	= bgrpSize;
				mpg2enc_param.force_bbp	= true;

				mpg2enc_param.max_gop   = gopSize; // about 12
				while ((mpg2enc_param.max_gop) && (mpg2enc_param.max_gop % bgrpSize != 0)) mpg2enc_param.max_gop--;
				if (!mpg2enc_param.max_gop) mpg2enc_param.max_gop = bgrpSize; // max_gop must be devizeble by 'bgrpSize'
				mpg2enc_param.seqhead_egop = true;
			}

			if (pal)
			{
				mpg2enc_param.norm			= 'p';
				mpg2enc_param.frame_rate	= 3;
			}
			else
			{
				mpg2enc_param.norm			= 'n';
				mpg2enc_param.frame_rate	= 4;
			}

			if (ret)
			{
				// do it
				mpg2enc(&mpg2enc_param);

				if (mpg2enc_param.m_ReturnCode != 0)
				{ // something wend wrong
					tempFile[0]=0;
					if (FileExists(mpg2enc_param.outfile)) DeleteFile(mpg2enc_param.outfile);
					ret = 0;
				}
				else
					strcpy(strSource, mpg2enc_param.outfile);
			}
			// no more need of the original yuv-file
			DeleteFile(tempFile);
		}

		if (!ret)
			strSource[0] = 0;
	}

	if (ret)
	{ // MULTI-PLEXING
		{
			tempFile[0]=0;
			// store a copy of the resulting output-name in 'outStream'
			if (outStream[0])
			{
				_splitpath(outStream, drive, dir, fname, NULL); // taking drie+dir of tempFile
				sprintf(tempFile, _T("%s%s"), drive, dir);
				if (PathExists(tempFile)) // be sure the path exists
					sprintf(tempFile, _T("%s%s%s%s"), drive, dir, fname, ".mpg");
				else
				{
					if (fname[0])
					{
						_splitpath(strSource, drive, dir, NULL, NULL);
						sprintf(tempFile, _T("%s%s%s%s"), drive, dir, fname, ".mpg");
					}
					else
						tempFile[0]=0;
				}
			}
			if (!tempFile[0])
			{
				_splitpath(strSource, drive, dir, fname, NULL); // taking drie+dir of tempFile
				sprintf(tempFile, _T("%s%s%s%s"), drive, dir, fname, ".mpg");
			}
			// get us a unique filename of the output
			u = 0;
			while (FileExists(tempFile))
			{
				if (u == ULONG_MAX) break;
				sprintf(tempFile, _T("%s%s%s_%lX%s"), drive, dir, fname, u++, ".mpg");
					// using hexadecimal suffix to keep the outfilename a bit short (max = ..._FFFFFFFE)
			}
			if (u == ULONG_MAX)
			{ // we cannot get a unique filename (would it be possible that a directory has 4GB of files??)
				ret = 0;
			}
		}
		if (ret)
		{
			strArr = (char**)(malloc(sizeof(char*) * 2));
			strArr[0] = strSource;
			strArr[1] = NULL;

			if ((o_hWnd) && (o_uMsg))
			{
				lhkProgress_Init((const void*)o_hWnd, o_uMsg, "Multi-Plexing MPeG", 1);
				lhkProgress_Step((const void*)o_hWnd, o_uMsg, "processing encoded video-stream", 0);
			}

			mplex_struct	mplex_switches;
			// preparing
			ZeroMemory(&mplex_switches, sizeof(mplex_struct));
			mplex_switches.verbose = csVerbose+1; // silent pleas
			mplex_switches.infiles = strArr;
			strcpy(mplex_switches.outfile, tempFile);

			if (vcd)
				mplex_switches.format  = 2; // VCD
			else
				mplex_switches.format  = 5; // SVCD
			// do it
			mplex(&mplex_switches);

			if (mplex_switches.m_ReturnCode == 0)
			{ // all wend fine
				strcpy(outStream, mplex_switches.outfile);
			}
			else
			{
				if (FileExists(mplex_switches.outfile))
					DeleteFile(mplex_switches.outfile);
				ret = 0;
			}

			if ((o_hWnd) && (o_uMsg))
			{
				lhkProgress_Step((const void*)o_hWnd, o_uMsg, "Multi-Plexing processed", 1);
				lhkProgress_End((const void*)o_hWnd, o_uMsg);
			}

			free(strArr); strArr=NULL;
		}
		DeleteFile(strSource);
	}

	if (!ret)
		outStream[0] = 0;

Fail:
	if (inFiles)
	{
		for (u=0; inFiles[u]; u++)
			free(inFiles[u]);
		free(inFiles);
		inFiles=NULL;
	}
	if (inFiles2) delete inFiles2; inFiles2=NULL;
	return ret;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
//   DLL entry																											//
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
