using System;
using System.Text;
using System.IO;

namespace Utilities.DVDImport
{
	/// <summary>
	/// Summary description for IFOParse.
	/// </summary>
	public class IFOParse
	{
		private int				m_numAudio		= 0;
		private string			m_audioFormat	= "";
		private string			m_aspectRatio	= "";
		private string			m_resolution	= "";
		private string			m_videoMode		= "";
		private int				m_frameRate		= 0;
		private int				m_duration		= 0;
		private int				m_pgcCount		= 0;

		public IFOParse(string filename)
		{
			FileStream fs = File.OpenRead(filename);
			try
			{
				#region Header
				// verify valid video IFO header
				byte[] id = new byte[12];
				if(fs.Read(id, 0, 12) != 12)
					throw new IOException("Error reading IFO file");
				string idStr = Encoding.ASCII.GetString(id, 0, 12);
				if(idStr != "DVDVIDEO-VTS")
					throw new Exception("Invalid IFO Header");
				#endregion

				#region Version
				// verify IFO version (1.0 or 1.1)
				if(fs.Seek(0x21, SeekOrigin.Begin) != 0x21)
					throw new IOException("Error reading IFO file");
				byte[] version = new byte[1];
				if(fs.Read(version, 0, 1) != 1)
					throw new IOException("Error reading IFO file");
				if(version[0] != 0x10 && version[0] != 0x11)
					throw new Exception("Invalid IFO Header Version");
				#endregion
				
				#region PGC Info
				// find offset to pgc tables
				if(fs.Seek(0xcc, SeekOrigin.Begin) != 0xcc)
					throw new IOException("Error reading IFO file");
				byte[] pgcOffsetData = new byte[4];
				if(fs.Read(pgcOffsetData, 0, 4) != 4)
					throw new IOException("Error reading IFO file");
				int pgcOffset = (pgcOffsetData[0] << 24) + (pgcOffsetData[1] << 16) + (pgcOffsetData[2] << 8) + pgcOffsetData[3];

				// get count of pgcs
				if(fs.Seek(pgcOffset * 2048, SeekOrigin.Begin) != pgcOffset * 2048)
					throw new IOException("Error reading IFO file");
				byte[] pgciData = new byte[2];
				if(fs.Read(pgciData, 0, 2) != 2)
					throw new IOException("Error reading IFO file");
				m_pgcCount = (pgciData[0] << 8) + pgciData[1];

				for(int i = 1; i <= m_pgcCount; i++)
				{
					// get pgc's info offset within pgc (length is 4 bytes into pgc info)
					if(fs.Seek((pgcOffset * 2048) + (i * 8) + 4, SeekOrigin.Begin) != (pgcOffset * 2048) + (i * 8) + 4)
						throw new IOException("Error reading IFO file");
					byte[] pgcStartData = new byte[4];
					if(fs.Read(pgcStartData, 0, 4) != 4)
						throw new IOException("Error reading IFO file");
					int pgcStart = (pgcStartData[0] << 24) + (pgcStartData[1] << 16) + (pgcStartData[2] << 8) + pgcStartData[3];

					// get pgc's playback time
					if(fs.Seek((pgcOffset * 2048) + pgcStart + 4, SeekOrigin.Begin) != (pgcOffset * 2048) + pgcStart + 4)
						throw new IOException("Error reading IFO file");
					byte[] playbackTimeData = new byte[4];
					if(fs.Read(playbackTimeData, 0, 4) != 4)
						throw new IOException("Error reading IFO file");

					// frame rate
					int frameRate = (playbackTimeData[3] & 0xC0) >> 6;
					if(frameRate == 3)
						m_frameRate = 30;
					else if(frameRate == 1)
						m_frameRate = 25;

					// duration
					int playbackTime = (playbackTimeData[0] << 24) + (playbackTimeData[1] << 16) + (playbackTimeData[2] << 8) + playbackTimeData[3];

					int hour = ((playbackTime >> 28) & 0x0f) * 10 + ((playbackTime >> 24) & 0x0f);
					int min = ((playbackTime >> 20) & 0x0f) * 10 + ((playbackTime >> 16) & 0x0f);
					int sec = ((playbackTime >> 12) & 0x0f) * 10 + ((playbackTime >> 8) & 0x0f);
					int frame = ((playbackTime >> 4) & 0x0f) * 10 + (playbackTime & 0x0f);

					m_duration += hour * 3600 + min * 60 + sec;
				}
				#endregion

				#region Video
				if(fs.Seek(0x200, SeekOrigin.Begin) != 0x200)
					throw new IOException("Error reading IFO file");
				byte[] videoAttributes = new byte[2];
				if(fs.Read(videoAttributes, 0, 2) != 2)
					throw new IOException("Error reading IFO file");
				byte va1 = videoAttributes[0];
				byte va2 = videoAttributes[1];

				// video mode
				if(((va1 & 0x30) >> 4) == 0)
					m_videoMode = "NTSC";
				else if(((va1 & 0x30) >> 4) == 1)
					m_videoMode = "PAL";

				// aspect
				if(((va1 & 0xC) >> 2) == 0)
					m_aspectRatio = "4:3";
				else if(((va1 & 0xC) >> 2) == 3)
					m_aspectRatio = "16:9";

				// resolution
				int resolution = ((va2 & 0x38) >> 3);
				switch(resolution)
				{
					case 0:
						m_resolution = "720x480";
						break;
					case 1:
						m_resolution = "704x480";
						break;
					case 2:
						m_resolution = "352x480";
						break;
					case 3:
						m_resolution = "352x240";
						break;
				}
				#endregion

				#region Audio
				// find count of audio streams
				if(fs.Seek(0x203, SeekOrigin.Begin) != 0x203)
					throw new IOException("Error reading IFO file");
				byte[] numAudio = new byte[1];
				if(fs.Read(numAudio, 0, 1) != 1)
					throw new IOException("Error reading IFO file");
				m_numAudio = numAudio[0];
				byte[] audioFormatData = new byte[1];
				if(fs.Read(audioFormatData, 0, 1) != 1)
					throw new IOException("Error reading IFO file");
				int audioFormat = (audioFormatData[0] & 0xe0) >> 5;
				switch(audioFormat)
				{
					case 0:
						m_audioFormat = "AC3";
						break;
					case 2:
						m_audioFormat = "MPEG1";
						break;
					case 3:
						m_audioFormat = "MPEG2";
						break;
					case 4:
						m_audioFormat = "LPCM";
						break;
					case 6:
						m_audioFormat = "DTS";
						break;
				}
				#endregion
			}
			finally
			{
				fs.Close();
			}
		}

		public int AudioStreamCount
		{
			get
			{
				return(m_numAudio);
			}
		}

		public int ProgramChainCount
		{
			get
			{
				return(m_pgcCount);
			}
		}

		public string AspectRatio
		{
			get
			{
				return(m_aspectRatio);
			}
		}
	
		public string Resolution
		{
			get
			{
				return(m_resolution);
			}
		}
	
		public string VideoMode
		{
			get
			{
				return(m_videoMode);
			}
		}

		public string AudioFormat
		{
			get
			{
				return(m_audioFormat);
			}
		}

		/// <summary>
		/// return number of frames per second
		/// </summary>
		public int FrameRate
		{
			get
			{
				return(m_frameRate);
			}
		}
		
		public int Length
		{
			get
			{
				return(m_duration);
			}
		}
	}
}
