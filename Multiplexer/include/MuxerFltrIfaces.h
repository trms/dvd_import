//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//	Copyright (C) 2003 AMMTools. All rights reserved.
//	http://www.ammtools.com mailto:support@ammtools.com

#ifndef __MuxerFilterInterfaces_h__
#define __MuxerFilterInterfaces_h__

#ifdef __cplusplus
extern "C"{
#endif

// {A9A645B5-AD30-4b99-A5F7-65B9DF2C2508}
DEFINE_GUID(IID_IPacketizingStreamSettings, 
0xA9A645B5, 0xAD30, 0x4b99, 0xA5, 0xF7, 0x65, 0xB9, 0xDF, 0x2C, 0x25, 0x08);

// {A9A645B6-AD30-4b99-A5F7-65B9DF2C2508}
DEFINE_GUID(IID_IStreamTimeSettings, 
0xA9A645B6, 0xAD30, 0x4b99, 0xA5, 0xF7, 0x65, 0xB9, 0xDF, 0x2C, 0x25, 0x08);

// {A9A645B7-AD30-4b99-A5F7-65B9DF2C2508}
DEFINE_GUID(IID_IVideoStreamSettings, 
0xA9A645B7, 0xAD30, 0x4b99, 0xA5, 0xF7, 0x65, 0xB9, 0xDF, 0x2C, 0x25, 0x08);

// {A9A645B8-AD30-4b99-A5F7-65B9DF2C2508}
DEFINE_GUID(IID_IStreamBufferStatus, 
0xA9A645B8, 0xAD30, 0x4b99, 0xA5, 0xF7, 0x65, 0xB9, 0xDF, 0x2C, 0x25, 0x08);

// {0F2AC67C-6CCE-463a-94FC-F89080C68009}
DEFINE_GUID(IID_IIntervals, 
0xf2ac67c, 0x6cce, 0x463a, 0x94, 0xfc, 0xf8, 0x90, 0x80, 0xc6, 0x80, 0x9);

// {0F2AC67D-6CCE-463a-94FC-F89080C68009}
DEFINE_GUID(IID_ISplit, 
0xf2ac67d, 0x6cce, 0x463a, 0x94, 0xfc, 0xf8, 0x90, 0x80, 0xc6, 0x80, 0x9);

// {69F87C4C-52D4-4b5b-8D43-D9604AA39692}
DEFINE_GUID(IID_IMuxSettings, 
0x69f87c4c, 0x52d4, 0x4b5b, 0x8d, 0x43, 0xd9, 0x60, 0x4a, 0xa3, 0x96, 0x92);

// {69F87C4D-52D4-4b5b-8D43-D9604AA39692}
DEFINE_GUID(IID_IMuxProgress, 
0x69f87c4d, 0x52d4, 0x4b5b, 0x8d, 0x43, 0xd9, 0x60, 0x4a, 0xa3, 0x96, 0x92);

#if defined(__cplusplus) && !defined(CINTERFACE)

	//////////////////////////////////////////////////////////
	//	IPacketizingStreamSettings
	//	Exposed by input pins. Controls packetizing proccess
	//	of corresponding ES. 
	
	#define	PACKT_FLAG_PADDING	0x1

    MIDL_INTERFACE("A9A645B5-AD30-4b99-A5F7-65B9DF2C2508")
    IPacketizingStreamSettings : public IUnknown
    {
    public:
        virtual  HRESULT STDMETHODCALLTYPE SetMaxPacketSize( 
            /* [in] */ DWORD dwPacketSize) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetMaxPacketSize( 
            /* [in][out] */ DWORD* pdwPacketSize) = 0;

        virtual  HRESULT STDMETHODCALLTYPE SetPacketizingFlags( //	NOT IMPLEMENTED
            /* [in] */ DWORD dwFlags) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetPacketizingFlags( //	NOT IMPLEMENTED
            /* [in][out] */ DWORD* pdwFlags) = 0;
	};


	//////////////////////////////////////////////////////////
	//	IStreamTimeSettings
	//	Exposed by input pins. Controls timestamp generation for 
	//	corresponfing ES and generation of SCR.
	//
	//	If stream is selected as MasterClock, its timestamps are
	//	used for calculation of SCR.
	//
	//	Synchonization methods are used for PTS generation:
	//	Adaptive			- combined framerate and input timestamps;
	//	Input timestamps	- by IMediaSamples' timestamps only;
	//	FrameRate			- calculated by stream's frames duration
	//							only (input timestamps are ignored)
	//	External clock		- Not yet implemented
	//	
	//	Time correction - shift/stretch PTS values for "manual"
	//	streams synchronization.

	#define SYNC_METHOD_INVALID			0
	#define SYNC_ADAPTIVE				1
	#define SYNC_BY_INPUT_TIMESTAMPS	2
	#define SYNC_BY_FRAME_RATE			3
	#define SYNC_BY_REFERENCE_CLOCK		4

    MIDL_INTERFACE("A9A645B6-AD30-4b99-A5F7-65B9DF2C2508")
    IStreamTimeSettings : public IUnknown
    {
    public:
        virtual  HRESULT STDMETHODCALLTYPE SetAsMasterClock() = 0;
        
        virtual  HRESULT STDMETHODCALLTYPE IsMasterClock( 
            /* [in][out] */ BOOL *pbVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSyncMethod( 
            /* [in] */ DWORD dwMethodCode) = 0;
        
        virtual  HRESULT STDMETHODCALLTYPE GetSyncMethod( 
            /* [in][out] */ DWORD* dwMethodCode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSyncClock( 
            /* [in] */ IReferenceClock* pReferenceClock) = 0;// NOT IMPLEMENTED
        
        virtual HRESULT STDMETHODCALLTYPE GetSyncClock( 
            /* [in][out] */ IReferenceClock** ppReferenceClock) = 0;// NOT IMPLEMENTED
        
        virtual HRESULT STDMETHODCALLTYPE SetTimeCorrection( 
            /* [in] */ REFERENCE_TIME rtShift, /* [in] */ LONG llStretchRate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTimeCorrection( 
            /* [in][out] */ REFERENCE_TIME* prtShift, /* [in] */ LONG* pllStretchRate) = 0;        
    };


	//////////////////////////////////////////////////////////
	// IVideoStreamSettings
	// Exposed by input pin if it provides Video ES.
	// Controls Video stream multiplexing settings.


	#define	FLAG_MUX_IPB			0x1
	#define	FLAG_MUX_IP				0x2
	#define	FLAG_MUX_I				0x4

    MIDL_INTERFACE("A9A645B7-AD30-4b99-A5F7-65B9DF2C2508")
    IVideoStreamSettings : public IUnknown
    {
    public:
        virtual  HRESULT STDMETHODCALLTYPE RepeatSeqHeader(
			/* [in] */ BOOL bVal) = 0;
        
        virtual  HRESULT STDMETHODCALLTYPE IsRepeatingSeqHeader( 
            /* [in][out] */ BOOL *pbVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMuxFramesFlag( 
            /* [in] */ DWORD dwMethodCode) = 0;
        
        virtual  HRESULT STDMETHODCALLTYPE GetMuxFramesFlag( 
            /* [in][out] */ DWORD* dwMethodCode) = 0;
	};


	//////////////////////////////////////////////////////////
	// IStreamBufferStatus
	// Exposed by input pins. Provides information about 
	// current usage of input buffers.
	// NOT IMPLEMENTED

	struct BUFFER_STATUS
	{
		WCHAR		wcsStreamType[16];
		DWORD		dwCurBufferFilness;
		LONGLONG	rtBufferBeginTime;
		LONGLONG	rtBufferEndTiem;
		BOOL		bOverflowBlocked;
		BOOL		bTimeBlocked;
	};

    MIDL_INTERFACE("A9A645B8-AD30-4b99-A5F7-65B9DF2C2508")
    IStreamBufferStatus : public IUnknown
	{
        virtual HRESULT STDMETHODCALLTYPE GetStatus( 
            /* [in][out] */ BUFFER_STATUS* pStatus) = 0;        
	};


	//////////////////////////////////////////////////////////
	// IIntervals
	// Exposed by output pin and filter. Controls output stream
	// trimming  (extracting one playable segment from 
	// output stream).

	#define	INTERVAL_TIME			0x1
	#define	INTERVAL_FRAMES			0x2
	#define	INTERVAL_BYTEPOSITION	0x3

	struct INTERVAL
	{
		LONGLONG	llBegin;
		LONGLONG	llEnd;
		DWORD		dwFlags;

		LONGLONG	llWritten;
		BOOL		bCompleted;
	};
	
    MIDL_INTERFACE("0F2AC67C-6CCE-463a-94FC-F89080C68009")
    IIntervals : public IUnknown
    {
    public:
        virtual  HRESULT STDMETHODCALLTYPE GetIntervalsCount( 
            /* [in][out] */ DWORD *dwIntervalsCount) = 0;

        virtual  HRESULT STDMETHODCALLTYPE AddInterval(
			/* [in] */ INTERVAL* pInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInterval( 
            /* [in] */ DWORD dwIntervalNumber, /* [in] [out]*/ INTERVAL* ppInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveInterval( 
            /* [in] */ DWORD dwIntervalNumber) = 0;
	};


	/////////////////////////////////////////////////////////////////////
	// ISplit
	// Exposed by utput pin and filter. Controls output stream segmentation
	// (spliting output stream into the sequence of playable chunks).

	struct SPLIT
	{
		LONGLONG	llParamValue;
		DWORD		dwFlags;

		UINT		unCurIntervalNum;
		LONGLONG	llWrittenTotal;
		LONGLONG	llWrittenCurInterval;
		BOOL		bCompleted;
	};

    MIDL_INTERFACE("0F2AC67d-6CCE-463a-94FC-F89080C68009")
    ISplit : public IUnknown
    {
    public:
        virtual  HRESULT STDMETHODCALLTYPE SetSplitParams(
			/* [in] */ SPLIT* pSplit) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetSplitParams( 
            /* [in][out] */ SPLIT* pSplit) = 0;
   	};



	/////////////////////////////////////////////////////////////////////
	// IMuxSettings
	// Exposed by output pin and filter. Controls multiplexing mode and
	// settings.
	// Multiplexing modes:
	// MUX_NORMAL - full multiplex, output is a MPEG System stream
	//				(one PES or several multiplexed PESs with System Packs)
	// MUX_PES - Output is a PES. This mode is valid only if only 
	//			one input pin is connected.
	// MUX_ES - Output is a ES (pass-through mode). This mode is valid 
	//			only if only one input pin is connected.

	#define MUX_NORMAL	0x1
	#define MUX_PES		0x2
	#define MUX_ES		0x4


    MIDL_INTERFACE("69F87C4C-52D4-4b5b-8D43-D9604AA39692")
    IMuxSettings : public IUnknown
    {
    public:
        virtual  HRESULT STDMETHODCALLTYPE SetTimeBase( // NOT IMPLEMENTED
            /* [in] */ REFERENCE_TIME *prtBase) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetTimeBase( // NOT IMPLEMENTED
            /* [in][out] */ REFERENCE_TIME *prtBase) = 0;

        virtual  HRESULT STDMETHODCALLTYPE SetMuxModeFlags( 
            /* [in] */ DWORD dwFlags) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetMuxModeFlags( 
            /* [in][out] */ DWORD* pdwFlags) = 0;

        virtual  HRESULT STDMETHODCALLTYPE SetPackSize( 
            /* [in] */ DWORD dwSize, /* [in] */ BOOL bPadding) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetPackSize( 
            /* [in][out] */ DWORD* pdwSize, /* [in][out] */  BOOL* bpPadding) = 0;
	};


	/////////////////////////////////////////////////////////////////////
	// IMuxProgress
	// Exposed by output pin and filter. Provides current statistics of
	// multiplexed output stream.

	#define MUX_STATUS_NOT_RUNNING			0
	#define MUX_STATUS_INSIDE_INTERVAL		1
	#define MUX_STATUS_BETWEEN_INTERVALS	2

    MIDL_INTERFACE("69F87C4D-52D4-4b5b-8D43-D9604AA39692")
    IMuxProgress : public IUnknown
    {
    public:
        virtual  HRESULT STDMETHODCALLTYPE GetCurrentMuxTime( 
            /* [in][out] */ REFERENCE_TIME *prtCurTime) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetCurrentMuxBytePos(
			/* [in][out] */ ULONGLONG *pullCurrBytePos) = 0;

        virtual  HRESULT STDMETHODCALLTYPE GetCurrentMuxStatus(
			/* [in][out] */ DWORD *pdwMuxStatus) = 0;
	};


#endif // defined(__cplusplus) && !defined(CINTERFACE)

#ifdef __cplusplus
}
#endif

#endif // __MuxerFilterInterfaces_h__


