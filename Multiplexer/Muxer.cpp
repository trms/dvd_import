#include ".\muxer.h"
#include "include\MuxerFltrIfaces.h"
#include "include\MuxerFltrGuids.h"
#include "include\SmartFileDumpFltrGuids.h"
#include <wmsstd.h>
#include <stdio.h>

Muxer::Muxer(void)
{
}

Muxer::~Muxer(void)
{
}

/*
 * It should be noted that GraphEdit files are intended only for testing and debugging.
 * They are not meant for use by end-user applications.
 */
HRESULT Muxer::LoadGraphFile(IGraphBuilder *pGraph, const WCHAR* wszName)
{
    IStorage *pStorage = 0;
    if (S_OK != StgIsStorageFile(wszName))
	  return(E_FAIL);

	printf("Loading %ls...\n", wszName);
    HRESULT hr = StgOpenStorage(wszName, 0, STGM_TRANSACTED | STGM_READ | STGM_SHARE_DENY_WRITE, 0, 0, &pStorage);
    if (FAILED(hr))
	  return(hr);

    IPersistStream *pPersistStream = 0;
    hr = pGraph->QueryInterface(IID_IPersistStream, reinterpret_cast<void**>(&pPersistStream));
    if (SUCCEEDED(hr))
    {
        IStream *pStream = 0;
        hr = pStorage->OpenStream(L"ActiveMovieGraph", 0, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStream);
        if(SUCCEEDED(hr))
        {
            hr = pPersistStream->Load(pStream);
			SAFE_RELEASE(pStream);
        }
		SAFE_RELEASE(pPersistStream);
    }
	SAFE_RELEASE(pStorage);
    return(hr);
}

/*
 * The Filter Graph Manager supports the IFilterGraph::EnumFilters method, which enumerates all
 * the filters in the filter graph. It returns a pointer to the IEnumFilters interface. The
 * IEnumFilters::Next method retrieves IBaseFilter interface pointers.
 *
 * The following example shows a function that enumerates the filters in a graph and displays
 * a message box with each filter's name. It uses the IBaseFilter::QueryFilterInfo method to
 * retrieve the name of the filter. Note the places where the function calls Release on an
 * interface to decrement the reference count.
*/
HRESULT Muxer::EnumFilters(IFilterGraph *pGraph) 
{
    IEnumFilters *pEnum = NULL;
    IBaseFilter *pFilter;
    ULONG cFetched;

    HRESULT hr = pGraph->EnumFilters(&pEnum);
    if(FAILED(hr))
		return(hr);

    while(pEnum->Next(1, &pFilter, &cFetched) == S_OK)
    {
        FILTER_INFO FilterInfo;
        hr = pFilter->QueryFilterInfo(&FilterInfo);
        if (FAILED(hr))
        {
			printf("Could not get the filter info: 0x%x\n", hr);
            continue;  // Maybe the next one will work.
        }

		printf("Filter: %ls\n", FilterInfo.achName);

        // The FILTER_INFO structure holds a pointer to the Filter Graph
        // Manager, with a reference count that must be released.
        if (FilterInfo.pGraph != NULL)
            FilterInfo.pGraph->Release();
        pFilter->Release();
    }

    pEnum->Release();
    return(S_OK);
}

/*
 * Filters support the IBaseFilter::EnumPins method, which enumerates the pins available on the filter.
 * It returns a pointer to the IEnumPins interface. The IEnumPins::Next method retrieves IPin interface
 * pointers.
 *
 * The following example shows a function that locates a pin with a given direction (input or output)
 * on a given filter. It uses the PIN_DIRECTION enumeration to specify the pin direction, and the
 * IPin::QueryDirection method to find the direction of each enumerated pin. If this function finds
 * a matching pin, it returns an IPin interface pointer with an outstanding reference count. The
 * caller is responsible for releasing the interface.
 */
HRESULT Muxer::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	return(GetPin(pFilter, PinDir, ppPin, 0));
}

HRESULT Muxer::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin, int index)
{
    IEnumPins	*pEnum = NULL;
    IPin		*pPin = NULL;
    HRESULT		hr;
	int			count = 0;

    if (ppPin == NULL)
    {
        return E_POINTER;
    }

    hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
        PIN_DIRECTION PinDirThis;
        hr = pPin->QueryDirection(&PinDirThis);
        if (FAILED(hr))
        {
            pPin->Release();
            pEnum->Release();
            return hr;
        }
        if (PinDir == PinDirThis)
        {
			if(count == index)
			{
				// Found a match. Return the IPin pointer to the caller.
				*ppPin = pPin;
				pEnum->Release();
				return S_OK;
			}
			count++;
        }
        // Release the pin for the next time through the loop.
        pPin->Release();
    }
    // No more pins. We did not find a match.
    pEnum->Release();
    return E_FAIL;  
}

bool Muxer::Multiplex(LPWSTR audio, LPWSTR video, LPWSTR output, long offset)
{
	HRESULT			hr				= S_OK;
	bool			result			= false;
	IGraphBuilder	*pGraph			= 0;
	IMediaControl	*pMediaControl	= 0;
	IMediaSeeking	*pMediaSeeking	= 0;
	IBaseFilter		*pAudio			= 0;
	IBaseFilter		*pVideo			= 0;
	IBaseFilter		*pMuxer			= 0;
	IBaseFilter		*pOutput		= 0;
	IPin			*pVideoOut		= 0;
	IPin			*pAudioOut		= 0;
	IPin			*pMuxerOut		= 0;
	IPin			*pMuxerInA		= 0;
	IPin			*pMuxerInV		= 0;
	IPin			*pOutputIn		= 0;
	IFileSinkFilter	*pSinkFilter	= 0;
	IStreamTimeSettings	*pAudioTime	= 0;

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);
	if(FAILED(hr))
		goto cleanup;
#ifdef GRF_FILE
	hr = LoadGraphFile(pGraph, L"D:\\Documents\\Local Source Code\\NativeMuxTest\\Debug\\mux.GRF");
	if(FAILED(hr))
		goto cleanup;
#else
	printf("Building FilterGraph...\n");
	// add input files
	hr = pGraph->AddSourceFilter(video, video, &pVideo);
	if(FAILED(hr))
		goto cleanup;
	hr = pGraph->AddSourceFilter(audio, audio, &pAudio);
	if(FAILED(hr))
		goto cleanup;

	// get filters for muxing
	hr = CoCreateInstance(CLSID_AMMT_M2PS_MUXER, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pMuxer);
	if(FAILED(hr))
		goto cleanup;
	hr = CoCreateInstance(CLSID_AMMT_SMART_FILE_DUMP, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void **)&pOutput);
	if(FAILED(hr))
		goto cleanup;

	// setup output name
	hr = pOutput->QueryInterface(&pSinkFilter);
	if(FAILED(hr))
		goto cleanup;
	hr = pSinkFilter->SetFileName(output, NULL);
	if(FAILED(hr))
		goto cleanup;

	// add to graph
	hr = pGraph->AddFilter(pMuxer, L"AMMT MPEG2 PS Muxer");
	hr = pGraph->AddFilter(pOutput, output);

	// setup connections
	// video/audio input
	hr = GetPin(pVideo, PINDIR_OUTPUT, &pVideoOut);
	if(FAILED(hr))
		goto cleanup;
	hr = GetPin(pAudio, PINDIR_OUTPUT, &pAudioOut);
	if(FAILED(hr))
		goto cleanup;
	// muxer audio
	hr = GetPin(pMuxer, PINDIR_INPUT, &pMuxerInA);
	if(FAILED(hr))
		goto cleanup;
	hr = pGraph->Connect(pAudioOut, pMuxerInA);
	if(FAILED(hr))
		goto cleanup;
	//muxer video
	hr = GetPin(pMuxer, PINDIR_INPUT, &pMuxerInV, 1);
	if(FAILED(hr))
		goto cleanup;
	hr = pGraph->Connect(pVideoOut, pMuxerInV);
	if(FAILED(hr))
		goto cleanup;
	// muxer output
	hr = GetPin(pOutput, PINDIR_INPUT, &pOutputIn);
	if(FAILED(hr))
		goto cleanup;
	hr = GetPin(pMuxer, PINDIR_OUTPUT, &pMuxerOut);
	if(FAILED(hr))
		goto cleanup;
	hr = pGraph->Connect(pMuxerOut, pOutputIn);
	if(FAILED(hr))
		goto cleanup;

	// set time offset
	hr = pMuxerInA->QueryInterface(&pAudioTime);
	if(FAILED(hr))
		goto cleanup;
	hr = pAudioTime->SetTimeCorrection(offset, 0);
	if(FAILED(hr))
		goto cleanup;
#endif

	printf("\rMultiplexing...");
	// preview filters in graph
	//hr = EnumFilters(pGraph);
	if(FAILED(hr))
		goto cleanup;

	// get control ptr so we can start it
	hr = pGraph->QueryInterface(IID_IMediaControl, (void**)&pMediaControl);
	if(FAILED(hr))
		goto cleanup;

	// get seeking ptr so we can monitor status
	hr = pGraph->QueryInterface(IID_IMediaSeeking, (void**)&pMediaSeeking);
	if(FAILED(hr))
		goto cleanup;

	// start graph
	hr = pMediaControl->Run();
	if(FAILED(hr))
		goto cleanup;

	// get duration to determine progress
	hr = pMediaSeeking->SetTimeFormat(&TIME_FORMAT_NONE);
	if(FAILED(hr))
		goto cleanup;
	LONGLONG length = 0;
	LONGLONG position = 0;
	hr = pMediaSeeking->GetDuration(&length);
	if(FAILED(hr))
		goto cleanup;

	// wait for state to change, or position to reach end
	OAFilterState state;
	while(length != position)
	{
		Sleep(250);
		hr = pMediaControl->GetState(0, &state);
		if(FAILED(hr))
			continue;
		if(state != State_Running)
			break;

		hr = pMediaSeeking->GetPositions(&position, 0);
 		if(FAILED(hr))
			continue;

		// display progress
		printf("\rMultiplexing...%0.0lf%%", ((double)position / (double)length) * 100.0);
	}
	printf("\rMultiplexing...Done.\n");
	result = true;

cleanup:
	if(FAILED(hr))
		printf("Multiplex failed: 0x%x\n", hr);

	SAFE_RELEASE(pAudioTime);
	SAFE_RELEASE(pSinkFilter);
	SAFE_RELEASE(pMuxer);
	SAFE_RELEASE(pMuxerOut);
	SAFE_RELEASE(pMuxerInA);
	SAFE_RELEASE(pMuxerInV);
	SAFE_RELEASE(pOutput);
	SAFE_RELEASE(pOutputIn);
	SAFE_RELEASE(pAudio);
	SAFE_RELEASE(pAudioOut);
	SAFE_RELEASE(pVideo);
	SAFE_RELEASE(pVideoOut);
	SAFE_RELEASE(pMediaControl);
	SAFE_RELEASE(pMediaSeeking);
	SAFE_RELEASE(pGraph);
	return(result);
}

