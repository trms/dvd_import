// DSUtils.h

#pragma once

using namespace System;
#define RELEASE(i) {if (i) i->Release(); i = NULL;}

namespace Utilities {
	namespace DVDImport {

		public ref class DSUtils
		{
		public:
			void Test(String ^stream);
			long Preview(System::Collections::ArrayList ^ranges, System::Collections::ArrayList ^vobs);
			long DemuxAudio(System::Collections::ArrayList ^ranges, System::Collections::ArrayList ^vobs);
		};
	}
}


class CVOBStream : public CAsyncStream
{
public:
	CVOBStream(int vobCount, LPWSTR *vobNames, int *vobSectors, int cellCount, int *cellStarts, int *cellEnds) :
	  m_llLength(0)
    {
		LONGLONG totalSectors = 0;
		for(int i = 0; i < cellCount; i++)
			totalSectors += (cellEnds[i] - cellStarts[i]);
		m_llLength = totalSectors * 2048;
		m_llPosition = 0;
		m_dwKBPerSec = INFINITE;
        m_dwTimeStart = timeGetTime();

		m_vobCount = vobCount;
		m_vobNames = vobNames;
		m_vobSectors = vobSectors;
		m_vobFiles = new HANDLE[vobCount];
		for(int i = 0; i < vobCount; i++)
		{
			m_vobFiles[i] = CreateFile(m_vobNames[i],
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              NULL,
                              OPEN_EXISTING,
                              0,
                              NULL);
			if(m_vobFiles[i] == INVALID_HANDLE_VALUE)
				throw("Can't open VOB");
		}
		m_cellCount = cellCount;
		m_cellStarts = cellStarts;
		m_cellEnds = cellEnds;

    }

	  ~CVOBStream()
	  {
		  for(int i = 0; i < m_vobCount; i++)
			  CloseHandle(m_vobFiles[i]);
		  delete m_vobFiles;
	  }

    HRESULT SetPointer(LONGLONG llPos)
    {
        if (llPos < 0 || llPos > m_llLength) {
            return S_FALSE;
        } else {
			m_llPosition = llPos; // we'll do the file seek on the read
            return S_OK;
        }
    }

    HRESULT Read(PBYTE pbBuffer,
                 DWORD dwBytesToRead,
                 BOOL bAlign,
                 LPDWORD pdwBytesRead)
    {
        CAutoLock lck(&m_csLock);
        DWORD dwReadLength;

        /*  Wait until the bytes are here! */
        DWORD dwTime = timeGetTime();
        if (m_llPosition + dwBytesToRead > m_llLength) {
            dwReadLength = (DWORD)(m_llLength - m_llPosition);
        } else {
            dwReadLength = dwBytesToRead;
        }

        DWORD dwTimeToArrive =
            ((DWORD)m_llPosition + dwReadLength) / m_dwKBPerSec;
        if (dwTime - m_dwTimeStart < dwTimeToArrive) {
            Sleep(dwTimeToArrive - dwTime + m_dwTimeStart);
        }

		try
		{
			// read each sector that contains data for the request, copy portion requested into output buffer
			LONGLONG startSector = 0;
			LONGLONG startSectorOffset = 0;
			LONGLONG endSector = 0;
			LONGLONG endSectorOffset = 0;
			LONGLONG bufOffset = 0;

			startSector = FindSector(m_llPosition, &startSectorOffset);
			endSector = FindSector(m_llPosition + dwReadLength, &endSectorOffset);

			for(int sector = startSector; sector < endSector; sector++)
			{
				PBYTE buf[2048];
				ReadSector(buf, FindVOBSector(sector));
				int bufIndex = 0;
				if(sector == startSector)
					bufIndex = startSectorOffset;
				int bufLen = 2048;
				if(sector == endSector - 1)
					bufLen = 2048 - endSectorOffset;

				memcpy(pbBuffer + bufOffset, buf + bufIndex, bufLen);
				bufOffset += bufLen;
			}
			*pdwBytesRead = bufOffset;
			m_llPosition += *pdwBytesRead;
		}
		catch( ... )
		{
			*pdwBytesRead = 0;
			return(ERROR_CANTREAD);
		}
        return S_OK;
    }

    LONGLONG Size(LONGLONG *pSizeAvailable)
    {
        LONGLONG llCurrentAvailable =
            Int32x32To64((timeGetTime() - m_dwTimeStart),m_dwKBPerSec);

        *pSizeAvailable = min(m_llLength, llCurrentAvailable);
        return m_llLength;
    }

    DWORD Alignment()
    {
        return 1;
    }
    void Lock()
    {
        m_csLock.Lock();
    }
    void Unlock()
    {
        m_csLock.Unlock();
    }

private:
    CCritSec       m_csLock;
    LONGLONG		m_llLength;
    LONGLONG       m_llPosition;
    DWORD          m_dwKBPerSec;
    DWORD          m_dwTimeStart;
	int m_vobCount;
	LPWSTR *m_vobNames;
	int *m_vobSectors;
	HANDLE *m_vobFiles;
	int m_cellCount;
	int *m_cellStarts;
	int *m_cellEnds;

	LONGLONG FindSector(LONGLONG offset, LONGLONG *bufferOffset)
	{
		LONGLONG sector = offset / 2048;
		*bufferOffset = offset % 2048;
		return(sector);
	}

	LONGLONG FindVOBSector(LONGLONG sector)
	{
		// use sector ranges for cells to remove any gaps to find the sector within the vob from the sector in the offset stream
		LONGLONG offset = 0;
		LONGLONG position = 0;
		LONGLONG lastEnd = 0;
		for(int i = 0; i < m_cellCount; i++)
		{
			position += m_cellEnds[i] - m_cellStarts[i];
			offset += m_cellStarts[i] - lastEnd;

			if(sector < position)
				return(offset + sector);

			lastEnd = m_cellEnds[i];
		}
		return(sector);
	}

	void ReadSector(PVOID pbBuffer, LONGLONG sector)
	{
		LONGLONG lastEnd = 0;
		for(int i = 0; i < m_vobCount; i++)
		{
			if (sector >= lastEnd && sector < lastEnd + m_vobSectors[i])
			{
				// got it
				HANDLE hFile = m_vobFiles[i];
				LONGLONG s = (sector - lastEnd) * 2048;
				LARGE_INTEGER ls;
				ls.QuadPart = s;
				SetFilePointerEx(hFile, ls, 0, FILE_BEGIN);
				DWORD bytesRead;
				ReadFile(hFile, pbBuffer, 2048, &bytesRead, 0);
				return;
			}
			lastEnd += m_vobSectors[i];
		}
	}

	HANDLE FindVOB(LONGLONG offset)
	{
		LONGLONG lastEnd = 0;
		for(int i = 0; i < m_vobCount; i++)
		{
			if (offset >= (lastEnd * 2048) && offset < ((lastEnd + m_vobSectors[i]) * 2048))
			{
				HANDLE hFile = m_vobFiles[i];
				LONGLONG s = offset - (lastEnd * 2048);
				LARGE_INTEGER ls;
				ls.QuadPart = s;
				SetFilePointerEx(hFile, ls, 0, FILE_BEGIN);
				return(hFile);
			}
			lastEnd += m_vobSectors[i];
		}
		return(INVALID_HANDLE_VALUE);
	}
};


class CMemReader : public CAsyncReader
{
public:

    //  We're not going to be CoCreate'd so we don't need registration
    //  stuff etc
    STDMETHODIMP Register()
    {
        return S_OK;
    }

    STDMETHODIMP Unregister()
    {
        return S_OK;
    }

    CMemReader(CVOBStream *pStream, CMediaType *pmt, HRESULT *phr) :
        CAsyncReader(NAME("VOB Stream\0"), NULL, pStream, phr)
    {
        m_mt = *pmt;
    }
};
