AC3DEC_WAVE v0.05 09/16/99
--------------------------

  AC3DEC_WAVE decodes audiostreams of Dolby-AC3 format, and sends a
  downmixed stereo-signal to a Windows waveOut device.  The device must
  support the stereo playback at the coded sample-rate (32, 44.1, or 48KHz.)

  AC3DEC_WAVE can optionally create an RIFF WAV file, by using the
  Windows ACM library.  ("Audio Compression" must be installed!)
  Currently, the only fully working "codec" is standard PCM.

  Some of the other codecs work, but a few don't.  The ADPCM codecs
  and Windows Media Audio Codec(s) do not work.

  This is a modified and recompiled version of Aaron Holtzmann's
  freeware AC3DEC-0.5.4 software decoder, available from
  ( NEW ) http://ess.engr.uvic.ca/~aholtzma/ac3
  ( OLD ) http://www.engr.uvic.ca/~aholtzma/ac3

  The VOB-demuxing routines are supplied by Brent Beyeler's BBDMUX.
  Source code for BBDMUX is available from
  http://members.home.net/beyeler/

What's new in this release
--------------------------
  Aaron's updated bit_allocate.c (0.5.5) is included in this release.
  The updated version correctly decodes the THX/Dolby VOB trailers found
  on the DVD-ROM of "DVD Demystified."

  The downmixing module contained two errors, rear-channels were phase
  inverted with respect to the ATSC AC-3 document.  Modes with 1 rear-
  channel (3/1, 2/1) were downmixed with the wrong coefficient.  These
  errors are fixed.

  Two bug-fixes apply only to AC-3 stereo (2.0) bitstreams :
    1) rematrixing
    2) phase-inversion (phsflginu)

  Note, the downmix module does not account for the possibility of
  overload (amplitude saturation.)  Some AC3 bitstreams, like the
  "Clinton Testimonial DVD", will exceed full-scale conversion.
  Overload detection is very slow without CPU-support for
  saturation arithmetic (i.e. MMX).

Usage
-----
  ac3dec.exe is a Win32 command-line application.  It must be run from
  a DOS-box.

          C:\> ac3dec.exe <AC3 file>  [optional output filename]

  If the output-filename is supplied, ac3dec will create a WAVE file.
  Otherwise, the decoded audio samples are sent to a waveOut device.
  AC3DEC can only play AC3 bitstreams.  AC3 Bitstreams may be in raw
  form (*.AC3) or part of an unencrypted VOB file.  AC3DEC does not
  play MPEG-audio streams.

  Playback control is very limited : <PAUSE> and <QUIT>

  (A Directshow version of AC3DEC is in the works.  When finished,
  the Directshow-filter will decode AC3-content from within any
  Directshow filtergraph, or Windows Media Player control.)

Requirements
--------------------
  * 1) Windows95/98/NT
  * 1) Pentium class CPU for real-time playback to waveOUT device
       (floating point processor required)
  * 2) Windows soundcard with 16-bit stereo playback @ 32/44.1/48KHz
       *** AC3DEC cannot convert between samplerates ***
  * 3) Audio Compression codecs (optional, required for RIFF WAVfile output)

  The last requirement will cause problems for legacy ISA soundcards.

  Nearly all AC3 bitstreams are coded @ 48KHz.  And the most popular ISA
  soundcard in history, Creative Lab's Soundblaster, does *NOT* support
  48KHz sampling.  If you have a PCI Soundblaster, you are O.K.

  If you have an ISA Soundblaster board (SB-16, AWE32, AWE64, etc.),
  you probably can't play AC3 files (but you can still convert AC3 -> WAV.)
  The SB-16WAVEFFECTS is the only ISA Soundblaster with 48KHz support!

Notes (...known limitations)
----------------------------
  The Wave-writer currently doesn't work with MS Audio Codec, IMA/Microsft
  ADPCM.  The wave-writer will *NEVER* work with MS Audio Codec (or
  Windows Media Audio.)  Microsoft does not permit these codecs to be
  accessed through ACM, only through the Windows Media API.

  AC3DEC is not "Dolby certified."  There may be some AC3 bitstreams
  that cause AC3DEC to crash.  Furthermore, 2.0 (stereo) bitstreams do not
  sound "quite right."  With 2.0 bistreams, treble sounds attenuated and
  base sounds over-emphasized.

  Bad AC3 bitstreams can cause ac3dec to crash.

  Performance-wise, AC3DEC is slow compared to commercial implementations
  of the Dolby-AC3 decoder.  On the development system (Celeron-450),
  XingDVD 2.03's AC3-filter consumes *HALF* as much CPU time.

  AC3DEC executes single-precision FPU instructions, favoring Intel CPUs
  over non-Intel CPUs.  On the Intel CPU, FPU-multiply is pipelined,
  whereas 32-bit integer multiply is not, so it's actually faster this way.

  ** multi-channel AC3 files ( >2 channels) are downmixed to a
  Dolby Pro-Logic compatible signal, but the mixing-levels may be incorrect.
  (The downmixing routines haven't yet been formally verified.)

  In play mode, AC3DEC_WAVE cannot deal with samplerate changes 
  in the ac3 bitstream (i.e. a change from 32KHz to 48KHz.)  This is a 
  limitation of the Windows waveOut API, and my output_winwave.c
  implementation.

  The VOB-demuxer is restricted to demuxing stream ID 0x80BD.  The next
  release will allow the user to specify the desired bitstream ID.

Missing features
----------------

  Not all features of the AC-3 specification are implemented.
  Here's a list of features that are *NOT* implemented :

    1) dynrng (dynamic range control)
    2) compre (wide dynamic compression control)
    3) dialog normalization

  Rest assured these operations would be performed after decoding.
  Thus, their omission will not cause bitstream parsing/decoding errors.
  But ac3dec's output may sound different from a commercial AC-3
  implementation (like those found in softDVD programs.)

Compilation
-----------
  Visual C++ 6.0 Professional
  To recompile this application, simply create a new Win32-console
  project, and add files with extension (*.c, *.cpp, *.h)
  Remember to remove output_linux.c, output.c, and output_solaris.c.
  (These source-files target other O/S.)

Sample bitstreams
-----------------
  I do not know of any sites with *.AC3 files for download.  However,
  several sites host *.VOB files for free download.  AC3DEC_WAVE v0.02
  and later can play AC3-audiostreams (stream 0x80BD only!) from VOB files.
  Please be warned that VOB files tend to be *VERY LARGE* 

  http://www.hollywood.com (Lost In Space, Dark City, Big Hit,
          Wedding Singer)

  ftp://ftp.xingtech.com (clip04.vob - clip from Star Trek: First Contact)

  ftp://ftp.csst.net login:csst pwd:csst
     Dolby Digital trailers
    
Revision history
----------------
 v0.05 - updated bit_allocate.c (0.5.5), should fix compatibility problems
         with some AC-3 bitstreams.
         corrections to downmix.c, rematrix.c,
         added phase-inversion to uncouple.c
         minor cosmetic changes (statistics report) to user interface

 v0.04 - lfe-channel now mixed at +10dB (instead of +9dB)
         improved resistance to "bad AC3 frames"
         updated downmix module (still not thoroughly tested)

 v0.03 - ac3dec_mci renamed to ac3dec_wave, to avoid confusion
         added ACM (audio compression manager) support, file output
         added downmix module

 v0.02 - integrated BBDMUX with ac3dec_mci, allows ac3dec
         to directly demux AC3 audiostream (0x80BD) from *.VOB files
         (unencrypted VOB files only!)

 v0.01 - first release of AC3DEC for Win32 (ac3dec_mci),
         output to Windows wave routines (waveOut library)


