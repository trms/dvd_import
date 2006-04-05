#pragma once

#include <windows.h>
#include <dshow.h>

class Muxer
{
private:
	HRESULT Muxer::LoadGraphFile(IGraphBuilder *pGraph, const WCHAR* wszName);
	HRESULT Muxer::EnumFilters(IFilterGraph *pGraph);
	HRESULT Muxer::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
	HRESULT Muxer::GetPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin, int index);
public:
	Muxer(void);
	~Muxer(void);
	bool Multiplex(LPWSTR audio, LPWSTR video, LPWSTR output, long offset);
};
