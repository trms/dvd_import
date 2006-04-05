// NativeMuxTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Main.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Muxer.h"


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		CoInitialize(NULL);

		if(argc > 4)
		{
			Muxer *m = new Muxer();
			long offset = wcstol(argv[3], 0, 10);
			printf("Audio:	%ls\n", argv[1]);
			printf("Video:	%ls\n", argv[2]);
			printf("Offset: %ldms\n", offset);
			printf("Output: %ls\n", argv[4]);
			m->Multiplex(argv[1], argv[2], argv[4], offset);
		}
		else
			printf("usage: %ls <audio stream> <video stream> <video offset in ms> <output file>\n", argv[0]);

		CoUninitialize();
	}

	return nRetCode;
}
