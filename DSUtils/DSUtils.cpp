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

long Utilities::DVDImport::DSUtils::Preview(System::Collections::ArrayList ^ranges, System::Collections::ArrayList ^vobs, System::IntPtr hwnd, int level)
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

	//if(SUCCEEDED(pMC->Stop()))
	//{
	rdr->Release();
	RELEASE(pVW);
	RELEASE(pMC);
	RELEASE(pME);
	RELEASE(pFG);
	delete stream;
	stream = 0;
	CoUninitialize();
	m_paused = false;
	//}
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
	if(pME == 0)
		return(false);
	if(m_paused)
		return(true);
	HRESULT hr = pME->WaitForCompletion(0, &levCode);
	if(hr == E_ABORT || (SUCCEEDED(hr) && levCode == 0))
		return(true);
	return(false);
}
