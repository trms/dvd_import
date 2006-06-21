// This is the main DLL file.

#include "stdafx.h"

//------------------------------------------------------------------------------
// File: MemFile.cpp
//
// Desc: DirectShow sample code - application using async filter.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include <windows.h>
#include <atlstr.h>
#include <streams.h>
#include <stdio.h>
#include <tchar.h>

#include "asyncio.h"
#include "asyncrdr.h"
#include "DSUtils.h"

//#import "C:\\WINDOWS\\System32\\mpg2splt.ax"

#undef DEFINE_GUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
DEFINE_GUID(CLSID_AC3ParserFilter,
0x280A3020, 0x86CF, 0x11D1, 0xAB, 0xE6, 0x00, 0xA0, 0xC9, 0x05, 0xF3, 0x75);

IMediaControl *pMC = 0;
IMediaEvent   *pME = 0;
IVideoWindow  *pVW = 0;
IFilterGraph *pFG = 0;
CMemReader *rdr = 0;
CVOBStream *stream = 0;
bool m_paused = false;


IPin *GetPin(IBaseFilter *pFilter, PIN_DIRECTION dir)
{
	ULONG fetched = 0;
	IEnumPins *pPins;
	HRESULT hr = pFilter->EnumPins(&pPins);
	if (FAILED(hr))
		return 0;
	hr = pPins->Reset();
	if (FAILED(hr))
	{
		RELEASE(pPins);
		return 0;
	}
	IPin *pPin = 0;
	while(SUCCEEDED(pPins->Next(1, &pPin, &fetched)))
	{
		if(fetched == 0)
			break;
		PIN_DIRECTION pinDir;
		hr = pPin->QueryDirection(&pinDir);
		if (FAILED(hr))
		{
			RELEASE(pPins);
			return 0;
		}
		if(pinDir == dir)
		{
			RELEASE(pPins);
			return(pPin);
		}
		RELEASE(pPin);
	}
	RELEASE(pPins);
	return(0);
}


//  Select a filter into a graph and render its output pin,
//  returning the graph
HRESULT SelectAndRender(CMemReader *pReader, IFilterGraph **ppFG)
{
    CheckPointer(pReader,E_POINTER);
    CheckPointer(ppFG,E_POINTER);

    /*  Create filter graph */
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                                  IID_IFilterGraph, (void**) ppFG);
    if(FAILED(hr))
    {
        return hr;
    }

    /*  Add our filter */
    hr = (*ppFG)->AddFilter(pReader, 0);

    if(FAILED(hr))
    {
        return hr;
    }

    /*  Get a GraphBuilder interface from the filter graph */
    IGraphBuilder *pBuilder;

    hr = (*ppFG)->QueryInterface(IID_IGraphBuilder, (void **)&pBuilder);
    if(FAILED(hr))
    {
        return hr;
    }

	IPin *pPin = GetPin(pReader, PINDIR_OUTPUT);

	/*IBaseFilter *pDDParser = 0;
	hr = CoCreateInstance(CLSID_AC3ParserFilter, 0, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pDDParser);
    if (FAILED(hr))
	{
		//OutputDebugString(L"CoCreateInstance(CLSID_DecklinkVideoRenderFilter) failed");
        return hr;
	}
	hr = pBuilder->AddFilter(pDDParser, L"AC3 Parser Filter");
    if (FAILED(hr))
	{
		//OutputDebugString(L"AddFilter(pVideoRenderer) failed");
        return hr;
	}*/


    /*  Render our output pin */
    //hr = pBuilder->Render(pReader->GetPin(0));
	if(pPin != 0)
		hr = pBuilder->Render(pPin);

    /* Release interface and return */
    pBuilder->Release();
    return hr;
}


HRESULT PlayFile(IFilterGraph *pFG, HWND parent)
{
    CheckPointer(pFG,E_POINTER);

    HRESULT hr;

    hr = pFG->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if(FAILED(hr))
    {
		pMC = 0;
        return hr;
    }

    hr = pFG->QueryInterface(IID_IMediaEvent, (void **)&pME);
    if(FAILED(hr))
    {
        pMC->Release();
		pMC = 0;
		pME = 0;
        return hr;
    }

	hr = pFG->QueryInterface(IID_IVideoWindow, (void **)&pVW);
    if(FAILED(hr))
    {
        pME->Release();
        pMC->Release();
 		pMC = 0;
		pME = 0;
		pVW = 0;
       return hr;
    }

	pVW->put_Owner((LONG_PTR)parent);
	pVW->HideCursor(OATRUE);
	pVW->put_Top(0);
	pVW->put_Left(0);
	WINDOWINFO wi;
	GetWindowInfo(parent, &wi);
	pVW->put_Width(wi.rcClient.right - wi.rcClient.left);
	pVW->put_Height(wi.rcClient.bottom - wi.rcClient.top);
	pVW->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

    //OAEVENT oEvent;
    //hr = pME->GetEventHandle(&oEvent);
    //if(SUCCEEDED(hr))
    //{
		//_tprintf(_T("playing\n"));
        hr = pMC->Run();
		m_paused = false;
	//}
    return hr;
}


void Utilities::DVDImport::DSUtils::Test(String ^stream)
{
/*  Read a file into memory, play it (or part of it), then exit */

    DWORD dwKBPerSec = INFINITE;
    LPTSTR lpType;

    CMediaType mt;
    mt.majortype = MEDIATYPE_Stream;

	IntPtr str = System::Runtime::InteropServices::Marshal::StringToHGlobalAuto(stream);
	LPTSTR file = (LPTSTR)str.ToPointer();

    /*  Find the extension */
    int len = lstrlen(file);
    if(len >= 4 && file[len - 4] == TEXT('.'))
    {
        lpType = file + len - 3;
    }
    else
    {
        _tprintf(_T("Invalid file extension\n"));
        return;
    }

    /* Set subtype based on file extension */
    if(lstrcmpi(lpType, TEXT("mpg")) == 0) {
        mt.subtype = MEDIASUBTYPE_MPEG1System;
    }
    else if(lstrcmpi(lpType, TEXT("vob")) == 0) {
        mt.subtype = MEDIASUBTYPE_MPEG1System;
    }
	else if(lstrcmpi(lpType, TEXT("mpa")) == 0) {
        mt.subtype = MEDIASUBTYPE_MPEG1Audio;
    }
    else if(lstrcmpi(lpType, TEXT("m2v")) == 0) {
        mt.subtype = MEDIASUBTYPE_MPEG1Video;
    }
    else if(lstrcmpi(lpType, TEXT("wav")) == 0) {
        mt.subtype = MEDIASUBTYPE_WAVE;
    }
    else if(lstrcmpi(lpType, TEXT("ac3")) == 0) {
        mt.subtype = MEDIASUBTYPE_DOLBY_AC3_SPDIF;
    }
    else if(lstrcmpi(lpType, TEXT("mpv")) == 0) {
        mt.subtype = MEDIASUBTYPE_MPEG1Video;
    }
    else if(lstrcmpi(lpType, TEXT("dat")) == 0) {
        mt.subtype = MEDIASUBTYPE_MPEG1VideoCD;
    }
    else if(lstrcmpi(lpType, TEXT("avi")) == 0) {
        mt.subtype = MEDIASUBTYPE_Avi;
    }
    else if(lstrcmpi(lpType, TEXT("mov")) == 0) {
        mt.subtype = MEDIASUBTYPE_QTMovie;
    }
    else if(lstrcmpi(lpType, TEXT("wav")) == 0) {
        mt.subtype = MEDIASUBTYPE_WAVE;
    }
    else
    {
        _tprintf(_T("Unknown file type: %s\n"), lpType);
        return;
    }

    /*  Open the file */
    _tprintf(_T("Reading: %s\n"), file);
    HANDLE hFile = CreateFile(file,
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              0,
                              NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Could not open %s\n"), file);
        return;
    }

	HANDLE hMap = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, 0, 0);
    if(hMap == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("Could not map %s\n"), file);
        return;
    }

	PBYTE pbMem = (PBYTE)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

    /* Determine the file size */
    ULARGE_INTEGER uliSize;
    uliSize.LowPart = GetFileSize(hFile, &uliSize.HighPart);
    _tprintf(_T("Length is: %u\n"), uliSize.LowPart);

    /* Allocate a buffer to hold the file's data */
    //PBYTE pbMem = new BYTE[uliSize.LowPart];
    if(pbMem == NULL)
    {
        _tprintf(_T("Could not allocate %d bytes\n"), uliSize.LowPart);
        return;
    }
    _tprintf(_T("allocated mem\n"));

    DWORD dwBytesRead;

    //if(!ReadFile(hFile,
   //             (LPVOID)pbMem,
   //             uliSize.LowPart,
    //            &dwBytesRead,
   //             NULL) ||
   //     (dwBytesRead != uliSize.LowPart))
  //  {
   //     _tprintf(_T("Could not read file\n"));
   //     CloseHandle(hFile);
  //      return;
   // }
   // _tprintf(_T("read file\n"));


    HRESULT hr = S_OK;

	CoInitialize(NULL);
    //CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    /*CMemStream Stream(pbMem, (LONGLONG)uliSize.QuadPart, dwKBPerSec);

    CMemReader *rdr = new CMemReader(&Stream, &mt, &hr);
    if(FAILED(hr) || rdr == NULL)
    {
        delete rdr;
        _tprintf(_T("Could not create filter - HRESULT 0x%8.8X\n"), hr);
        CoUninitialize();
        return;
    }

    _tprintf(_T("created reader\n"));
    //  Make sure we don't accidentally go away!
    rdr->AddRef();

    IFilterGraph *pFG = NULL;
    hr = SelectAndRender(rdr, &pFG);

    _tprintf(_T("rendered file\n"));
    if(FAILED(hr))
    {
        _tprintf(_T("Failed to create graph and render file - HRESULT 0x%8.8X"), hr);
    }
    else
    {
	  _tprintf(_T("playing file\n"));
        //  Play the file
        HRESULT hr = PlayFileWait(pFG);
        if(FAILED(hr))
        {
            _tprintf(_T("Failed to play graph - HRESULT 0x%8.8X"), hr);
        }
    }

	rdr->Release();

    if(pFG)
    {
        ULONG ulRelease = pFG->Release();
        if(ulRelease != 0)
        {
            _tprintf(_T("Filter graph count not 0! (was %d)"), ulRelease);
        }
    }
	UnmapViewOfFile(pbMem);
    CloseHandle(hMap);
    CloseHandle(hFile);
*/

	System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
    CoUninitialize();
    return;
}

long Utilities::DVDImport::DSUtils::Preview(System::Collections::ArrayList ^ranges, System::Collections::ArrayList ^vobs, System::IntPtr hwnd)
{
    HRESULT hr = S_OK;
	if(pMC != 0)
		return(hr);

	int vobCount = vobs->Count / 2;
	int cellCount = ranges->Count / 2;
	LPTSTR *vobNames = new LPWSTR[vobCount];
	int *vobSectors = new int[vobCount];
	int *cellStarts = new int[cellCount];
	int *cellEnds = new int[cellCount];
	System::Collections::ArrayList ^stringList = gcnew System::Collections::ArrayList();

	// load vob info
	for(int i = 0; i < vobCount; i++)
	{
		IntPtr str = System::Runtime::InteropServices::Marshal::StringToHGlobalAuto(vobs[i * 2]->ToString());
		LPTSTR file = (LPTSTR)str.ToPointer();
		stringList->Add(str);
		vobNames[i] = file;
		vobSectors[i] = System::Convert::ToInt32(vobs[(i * 2) + 1]);
	}

	// load cell info
	for(int i = 0; i < cellCount; i++)
	{
		cellStarts[i] = (int)ranges[i * 2];
		cellEnds[i] = (int)ranges[(i * 2) + 1];
	}

	// this type seems to work for a vob stream
    CMediaType mt;
    mt.majortype = MEDIATYPE_Stream;
    mt.subtype = MEDIASUBTYPE_MPEG1System;

	CoInitialize(NULL);

    stream = new CVOBStream(vobCount, vobNames, vobSectors, cellCount, cellStarts, cellEnds);
    rdr = new CMemReader(stream, &mt, &hr);
    if(FAILED(hr) || rdr == NULL)
		goto cleanup;

    //  Make sure we don't accidentally go away!
    rdr->AddRef();

    pFG = 0;
    hr = SelectAndRender(rdr, &pFG);

    if(SUCCEEDED(hr))
    {
        //  Play the file
		HRESULT hr = PlayFile(pFG, (HWND)hwnd.ToPointer());
        if(FAILED(hr))
			goto cleanup;
    }

//	if(rdr != 0)
//        delete rdr;
    //rdr->Release();
	//RELEASE(pFG);

cleanup:
	for(int i = 0; i < stringList->Count; i++)
		System::Runtime::InteropServices::Marshal::FreeHGlobal((IntPtr)stringList[i]);
	delete vobNames;
	delete vobSectors;
	delete cellStarts;
	delete cellEnds;

    return(hr);
}

void Utilities::DVDImport::DSUtils::Stop()
{
	if(pMC == 0)
		return;

	if(SUCCEEDED(pMC->Stop()))
	{
		rdr->Release();
		RELEASE(pVW);
		RELEASE(pMC);
		RELEASE(pME);
		RELEASE(pFG);
		delete stream;
		stream = 0;
		CoUninitialize();
		m_paused = false;
	}
}

void Utilities::DVDImport::DSUtils::Pause()
{
	if(pMC == 0)
		return;

	if(m_paused)
	{
		pMC->Run();
		m_paused = false;
	}
	else
	{
		pMC->Pause();
		m_paused = true;
	}
}

bool Utilities::DVDImport::DSUtils::IsPlaying()
{
	LONG levCode;
	if(pMC == 0)
		return(false);
	if(m_paused)
		return(true);
	HRESULT hr = pME->WaitForCompletion(0, &levCode);
	if(hr == E_ABORT || (SUCCEEDED(hr) && levCode == 0))
		return(true);
	return(false);
}

long Utilities::DVDImport::DSUtils::DemuxAudio(System::Collections::ArrayList ^ranges, System::Collections::ArrayList ^vobs)
{
    HRESULT hr = S_OK;
	CMemReader *rdr = 0;
	int vobCount = vobs->Count / 2;
	int cellCount = ranges->Count / 2;
	LPTSTR *vobNames = new LPWSTR[vobCount];
	int *vobSectors = new int[vobCount];
	int *cellStarts = new int[cellCount];
	int *cellEnds = new int[cellCount];
	System::Collections::ArrayList ^stringList = gcnew System::Collections::ArrayList();

	// load vob info
	for(int i = 0; i < vobCount; i++)
	{
		IntPtr str = System::Runtime::InteropServices::Marshal::StringToHGlobalAuto(vobs[i * 2]->ToString());
		LPTSTR file = (LPTSTR)str.ToPointer();
		stringList->Add(str);
		vobNames[i] = file;
		vobSectors[i] = System::Convert::ToInt32(vobs[(i * 2) + 1]);
	}

	// load cell info
	for(int i = 0; i < cellCount; i++)
	{
		cellStarts[i] = (int)ranges[i * 2];
		cellEnds[i] = (int)ranges[(i * 2) + 1];
	}

	// this type seems to work for a vob stream
    CMediaType mt;
    mt.majortype = MEDIATYPE_Stream;
    mt.subtype = MEDIASUBTYPE_MPEG1System;

	CoInitialize(NULL);

    CVOBStream Stream(vobCount, vobNames, vobSectors, cellCount, cellStarts, cellEnds);

    rdr = new CMemReader(&Stream, &mt, &hr);
    if(FAILED(hr) || rdr == NULL)
		goto cleanup;

    //  Make sure we don't accidentally go away!
    rdr->AddRef();

    IFilterGraph *pFG = NULL;
    hr = SelectAndRender(rdr, &pFG);

    if(SUCCEEDED(hr))
    {
        //  Play the file
        HRESULT hr = PlayFile(pFG, 0);
        if(FAILED(hr))
			goto cleanup;
    }

//	if(rdr != 0)
//        delete rdr;
    rdr->Release();
	RELEASE(pFG);

cleanup:
	for(int i = 0; i < stringList->Count; i++)
		System::Runtime::InteropServices::Marshal::FreeHGlobal((IntPtr)stringList[i]);
	delete vobNames;
	delete vobSectors;
	delete cellStarts;
	delete cellEnds;

    CoUninitialize();
    return(hr);
}
