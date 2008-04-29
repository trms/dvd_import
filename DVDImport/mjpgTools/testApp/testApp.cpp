// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// testApp.cpp:	part of the testApp program																				 //
//				Defines the entry point for the console application.													 //
//																														 //
//				This program was intended to test the development of the DLL, you don't need this app in order to		 //
//				properly using the DLL. If you're gonna distribute the binary of the DLL, excluded this file !!			 //
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
#include "..\mjpgTools.h"
// development testing was done via an import-lib, so we could debug the dll

#include <conio.h>
#include <stdio.h>
#include <tchar.h>


int main(int argc, char* argv[])
{
	// const char* filename = "D:\\temp\\Timber wolf%s.%s";
//	const char* filter="D:\\temp\\Vinca%s.%s";
	char inFile[MAX_PATH], outFile[MAX_PATH];
	char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME];
	int c=0, i, j;
	char** strArr=NULL, *str;
	int fType;
	LONG h, w;

	HINSTANCE hinstLib=NULL;
	//lp_create_still				create_still;
	lp_GetFileType					_GetFileType;
	lp_GetGraphicalInformation		GetGraphicalInformation;
	lp_createMxV					createMxV;

	BOOL							vcd, pal;

	const LPCTSTR files[] = { "C:\\TEMP\\-02_10th Kingdom.ppm",
		"C:\\TEMP\\-01_102 Dalmatiers.ppm",
		"C:\\TEMP\\-05_2010_2.ppm",
		"C:\\TEMP\\a11_Air Force One.ppm",
		"C:\\TEMP\\a30_As Good As It Gets.ppm",
		"C:\\TEMP\\a28_Arrival, The (1).ppm",
		"C:\\TEMP\\a29_Arrival, The (2).ppm",
		"C:\\TEMP\\a27_Armageddon.ppm",
		"C:\\TEMP\\a23_Analyze This.ppm",
		"C:\\TEMP\\a21_Amazons and Gladiators.ppm",
		0 };

//	long h, w; // testing 'GetGraphicalInformation'

//	if ((hinstLib=LoadLibrary(_T("D:\\Data\\mjpgTools\\Debug\\mjpgTools.dll"))) == NULL)
	if ((hinstLib=LoadLibrary(_T("mjpgTools"))) == NULL)
	{
		fprintf(stderr, "Unable to load the \"mjpgTools\" library");
		goto End_Here;
	}

	if ((_GetFileType = (lp_GetFileType)GetProcAddress(hinstLib, "_GetFileType")) == NULL)
	{
		fprintf(stderr, "Unable to load a handle to function \"_GetFileType\"");
		goto End_Here;
	}
/*
	if ((GetGraphicalInformation = (lp_GetGraphicalInformation)GetProcAddress(hinstLib, "GetGraphicalInformation")) == NULL)
	{
		fprintf(stderr, "Unable to load a handle to function \"GetGraphicalInformation\"");
		goto End_Here;
	}

	c = (GetGraphicalInformation)(file1, &h, &w);
	fprintf(stdout, "Width = %ld, Height = %ld", w, h);
	goto End_Here;
*/
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if ((createMxV = (lp_createMxV)GetProcAddress(hinstLib, "createMxV")) == NULL)
	{
		fprintf(stderr, "Unable to load a handle to function \"createMxV\"");
		goto End_Here;
	}

	j = 2; // take 3 files
	for (c=0, i=0; files[i]; i++)
	{
		if (i > j) break;
		c += strlen(files[i])+1;
	}
	str = new char[c + 1];
	for (c=0, i=0; files[i]; i++)
	{
		if (i > j) break;
		strcpy(&str[c], files[i]);
		c += strlen(files[i])+1;
	}
	str[c] =0;

	vcd = 1;
	pal = 1;
	strcpy(outFile, "TestAA.mpg");
	c = (createMxV)(str, outFile, vcd, pal, 2.0, FALSE, 0, 0, 0);
	delete str; str=NULL;
	if ( c == FALSE )
	{
		delete str; str=NULL;
		fprintf(stderr, "\nFailed to create a MxV.\n");
		goto End_Here;
	}
	else
	{
		fprintf(stderr, "\nSuccessfully created a MPG from");
		for (i=0; files[i]; i++)
		{
			if (i > j) break;
			fprintf(stderr, "\n\t<%s>%s", files[i], ((i==0) ? "," : (files[i+1]) ? ", and" : ""));
		}
		fprintf(stderr, "\nResult can be found at <%s>\n\n", outFile);
	}

	c = 0;
	fType = (_GetFileType)(outFile);

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	if ((create_still = (lp_create_still)GetProcAddress(hinstLib, "create_still")) == NULL)
	{
		fprintf(stderr, "Unable to load a handle to function \"create_still\"");
		goto End_Here;
	}

	strcpy(inFile, _T("D:\\Temp\\Image1.ppm"));
	if ( (create_still)(inFile, outFile, TRUE, TRUE) == false )//, TRUE
	{
		fprintf(stderr, "\nFailed to create a STILL.\n");
		goto End_Here;
	}
	else
		fprintf(stderr, "\nSuccessfully created a STILL from <%s>.\nResult can be found at <%s>\n\n", inFile, outFile);
*/
	// test our '_GetFileType' function
	/*
	for (i=0; i >= 0 ; i++)
	{
		switch (i)
		{
		case 0 :
			{
				wsprintf(buf, filter, "", "jpg");
				break;
			}
		case 1 :
			{
				wsprintf(buf, filter, "1", "ppm");
				break;
			}
		case 2 :
			{
				wsprintf(buf, filter, "_high", "m1v");
				break;
			}
		case 3 :
			{
				wsprintf(buf, filter, "_high", "yuv");
				break;
			}
		case 4 :
			{
				wsprintf(buf, "d:\\data\\simp_short.mpg");
				break;
			}
		case 5 :
			{
				wsprintf(buf, filter, "_", "yuv");
				break;
			}
		case 6 :
			{
				wsprintf(buf, filter, "_", "mpg");
				break;
			}
		case 7 :
			{
				wsprintf(buf, "d:\\temp\\test.txt");
				break;
			}
		default:
			{
				i = -10;
				continue;
			}
		}
		
		fType = (_GetFileType)(outFile);
*/
		switch (fType)
		{
			case FILETYPE_BMP : { str = "BMP"; break; }
			case FILETYPE_GIF : { str = "GIF"; break; }
			case FILETYPE_PPM : { str = "PPM"; break; }
			case FILETYPE_RLE : { str = "RLE"; break; }
			case FILETYPE_TGA : { str = "TARGA"; break; }
			case FILETYPE_JPG : { str = "JPeG"; break; }
			case FILETYPE_YUV : { str = "\"Y4M yuv\""; break; }
			case FILETYPE_MPG : { str = "\"MPeG Unspecified\""; break; }
			case FILETYPE_MPG1 : { str = "\"MPeG 1\""; break; }
			case FILETYPE_MPG2 : { str = "\"MPeG 2\""; break; }
			case FILETYPE_STILL1 : { str = "\"MPeG 1 STILL\""; break; }
			case FILETYPE_STILL2 : { str = "\"MPeG 2 STILL\""; break; }
			case FILETYPE_UNKNOWN :
			default:
			{
				str = "UNKNOWN";
				break;
			}
		}
		fprintf(stderr, "\nFile <%s> is of type %s.\n", outFile, str);
	//}
			
//goto Begin_Here;
/*
	// convert JPG --> PPM
	djpeg_switches d_switches;
	ZeroMemory(&d_switches, sizeof(djpeg_switches));
	// create a PPM file
	wsprintf(d_switches.infile, filter, "", "jpg");
	wsprintf(d_switches.outfile, filter, "1", "ppm");
	JPG_2_PPM( &d_switches );

	// convert PPM --> YUV
	ppm2yuv_switches p_switches;
	ZeroMemory(&p_switches, sizeof(ppm2yuv_switches));
	p_switches.frame_rate = y4m_fps_PAL;
	wsprintf(p_switches.infile, filter, "1", "ppm");
	PPM_2_YUV( &p_switches );

	// Creating VCD-low
	yuvscaler_struct yr_switches;
	ZeroMemory(&yr_switches, sizeof(yuvscaler_struct));
	wsprintf(yr_switches.infile, filter, "1", "yuv");
	wsprintf(yr_switches.outfile, filter, "_low", "yuv");
	// OUTPUT frame characteristics
	yr_switches.Output = 3; // SVCD
	yr_switches.resize[0] = 288; // height
	yr_switches.resize[1] = 352; // width
	// OUTPUT norm for VCD/SVCD
	// yr_switches.outputNORM = 0;
	// DO it
	YuvScaler(&yr_switches);

	// Creating VCD-high
	ZeroMemory(&yr_switches, sizeof(yuvscaler_struct));
	wsprintf(yr_switches.infile, filter, "1", "yuv");
	wsprintf(yr_switches.outfile, filter, "_high", "yuv");
	// OUTPUT frame characteristics
	yr_switches.Output = 3; // SVCD
	yr_switches.resize[0] = 576; // height
	yr_switches.resize[1] = 704; // width
	// DO it
	YuvScaler(&yr_switches);

	// convert YUV --> PPM
	yuv2ppm_switches y_switches;
	ZeroMemory(&y_switches, sizeof(yuv2ppm_switches));
	wsprintf(y_switches.infile, filter, "_low", "yuv");
	// wsprintf(y_switches.outfile, filter, "3", "ppm");
	YUV_2_PPM( &y_switches );
	//
	ZeroMemory(&y_switches, sizeof(yuv2ppm_switches));
	wsprintf(y_switches.infile, filter, "_high", "yuv");
	// wsprintf(y_switches.outfile, filter, "3", "ppm");
	YUV_2_PPM( &y_switches );

	// try-out MPG2enc
	mpg2enc_struct mpg2enc_param;
	ZeroMemory(&mpg2enc_param, sizeof(mpg2enc_struct));
	wsprintf(mpg2enc_param.infile, filter, "_high", "yuv");
	// -f 6 -T 120
	mpg2enc_param.norm		= 'p';
	//mpg2enc_param.verbose	= 2;
	mpg2enc_param.format	= 6;
	mpg2enc_param.vcd_ssize	= 120;
	mpg2enc(&mpg2enc_param);

	ZeroMemory(&mpg2enc_param, sizeof(mpg2enc_struct));
	wsprintf(mpg2enc_param.infile, filter, "_low", "yuv");
	// -f 6 -T 42
	mpg2enc_param.norm		= 'p';
	//mpg2enc_param.verbose	= 2;
	mpg2enc_param.format	= 6;
	mpg2enc_param.vcd_ssize	= 42;
	mpg2enc(&mpg2enc_param);

	// mplex
	char a[MAX_PATH], b[MAX_PATH];
	mplex_struct mp_switches;

	wsprintf(a, filter, "_low", "m1v");
	wsprintf(b, filter, "_high", "m1v");
	strArr = (char**)(malloc(sizeof(char*) * (++m1v)));
	for (i=0; i<m1v; i++)
	{
		switch (i)
		{
			case 0 :
			{
				strArr[i] = a;
				break;
			}
			case 1 :
			{
				strArr[i] = b;
				break;
			}
			default :
			{
				strArr[i] = NULL;
				break;
			}
		}
	}
	ZeroMemory(&mp_switches, sizeof(mplex_struct));
	mp_switches.infiles = strArr;
	wsprintf(mp_switches.outfile, filter, "_", "mpg");
	mp_switches.format  = 6;
	//mp_switches.verbose = 2;
	// do it
	mplex(&mp_switches);
	free(strArr);

//Begin_Here:
	stillpatch_struct sp_switches;
	ZeroMemory(&sp_switches, sizeof(stillpatch_struct));
	wsprintf(sp_switches.infile, filter, "_", "mpg");
	//sp_switches.reverse = false;
	sp_switches.verbose = true;
	// do it
	stillPatcher(&sp_switches);
*/
End_Here:
	if (hinstLib) FreeLibrary(hinstLib);
	fprintf(stderr, "\nPress a key to exit...\n");
	c = _getch();
	if (c==0) _getch();	// in case a function-key was pressed, get the remainder of the key too

	return 0;
}
