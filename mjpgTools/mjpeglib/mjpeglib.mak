# Microsoft Developer Studio Generated NMAKE File, Based on mjpeglib.dsp
!IF "$(CFG)" == ""
CFG=mjpeglib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mjpeglib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mjpeglib - Win32 Release" && "$(CFG)" != "mjpeglib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mjpeglib.mak" CFG="mjpeglib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mjpeglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mjpeglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mjpeglib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mjpeglib.lib"


CLEAN :
	-@erase "$(INTDIR)\avilib.obj"
	-@erase "$(INTDIR)\colorspace.obj"
	-@erase "$(INTDIR)\editlist.obj"
	-@erase "$(INTDIR)\jpegutils.obj"
	-@erase "$(INTDIR)\lav_common.obj"
	-@erase "$(INTDIR)\lav_io.obj"
	-@erase "$(INTDIR)\mjpeg_logging.obj"
	-@erase "$(INTDIR)\mpegconsts.obj"
	-@erase "$(INTDIR)\subsample.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\yuv4mpeg.obj"
	-@erase "$(INTDIR)\yuv4mpeg_ratio.obj"
	-@erase "$(INTDIR)\yuvscaler.obj"
	-@erase "$(OUTDIR)\mjpeglib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\mjpeglib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mjpeglib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mjpeglib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\avilib.obj" \
	"$(INTDIR)\colorspace.obj" \
	"$(INTDIR)\editlist.obj" \
	"$(INTDIR)\jpegutils.obj" \
	"$(INTDIR)\lav_common.obj" \
	"$(INTDIR)\lav_io.obj" \
	"$(INTDIR)\mjpeg_logging.obj" \
	"$(INTDIR)\mpegconsts.obj" \
	"$(INTDIR)\subsample.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\yuv4mpeg.obj" \
	"$(INTDIR)\yuv4mpeg_ratio.obj" \
	"$(INTDIR)\yuvscaler.obj"

"$(OUTDIR)\mjpeglib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mjpeglib.lib" "$(OUTDIR)\mjpeglib.bsc"


CLEAN :
	-@erase "$(INTDIR)\avilib.obj"
	-@erase "$(INTDIR)\avilib.sbr"
	-@erase "$(INTDIR)\colorspace.obj"
	-@erase "$(INTDIR)\colorspace.sbr"
	-@erase "$(INTDIR)\editlist.obj"
	-@erase "$(INTDIR)\editlist.sbr"
	-@erase "$(INTDIR)\jpegutils.obj"
	-@erase "$(INTDIR)\jpegutils.sbr"
	-@erase "$(INTDIR)\lav_common.obj"
	-@erase "$(INTDIR)\lav_common.sbr"
	-@erase "$(INTDIR)\lav_io.obj"
	-@erase "$(INTDIR)\lav_io.sbr"
	-@erase "$(INTDIR)\mjpeg_logging.obj"
	-@erase "$(INTDIR)\mjpeg_logging.sbr"
	-@erase "$(INTDIR)\mpegconsts.obj"
	-@erase "$(INTDIR)\mpegconsts.sbr"
	-@erase "$(INTDIR)\subsample.obj"
	-@erase "$(INTDIR)\subsample.sbr"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\tools.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\yuv4mpeg.obj"
	-@erase "$(INTDIR)\yuv4mpeg.sbr"
	-@erase "$(INTDIR)\yuv4mpeg_ratio.obj"
	-@erase "$(INTDIR)\yuv4mpeg_ratio.sbr"
	-@erase "$(INTDIR)\yuvscaler.obj"
	-@erase "$(INTDIR)\yuvscaler.sbr"
	-@erase "$(OUTDIR)\mjpeglib.bsc"
	-@erase "$(OUTDIR)\mjpeglib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mjpeglib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mjpeglib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\avilib.sbr" \
	"$(INTDIR)\colorspace.sbr" \
	"$(INTDIR)\editlist.sbr" \
	"$(INTDIR)\jpegutils.sbr" \
	"$(INTDIR)\lav_common.sbr" \
	"$(INTDIR)\lav_io.sbr" \
	"$(INTDIR)\mjpeg_logging.sbr" \
	"$(INTDIR)\mpegconsts.sbr" \
	"$(INTDIR)\subsample.sbr" \
	"$(INTDIR)\tools.sbr" \
	"$(INTDIR)\yuv4mpeg.sbr" \
	"$(INTDIR)\yuv4mpeg_ratio.sbr" \
	"$(INTDIR)\yuvscaler.sbr"

"$(OUTDIR)\mjpeglib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mjpeglib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\avilib.obj" \
	"$(INTDIR)\colorspace.obj" \
	"$(INTDIR)\editlist.obj" \
	"$(INTDIR)\jpegutils.obj" \
	"$(INTDIR)\lav_common.obj" \
	"$(INTDIR)\lav_io.obj" \
	"$(INTDIR)\mjpeg_logging.obj" \
	"$(INTDIR)\mpegconsts.obj" \
	"$(INTDIR)\subsample.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\yuv4mpeg.obj" \
	"$(INTDIR)\yuv4mpeg_ratio.obj" \
	"$(INTDIR)\yuvscaler.obj"

"$(OUTDIR)\mjpeglib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mjpeglib.dep")
!INCLUDE "mjpeglib.dep"
!ELSE 
!MESSAGE Warning: cannot find "mjpeglib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mjpeglib - Win32 Release" || "$(CFG)" == "mjpeglib - Win32 Debug"
SOURCE=.\avilib.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\avilib.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\avilib.obj"	"$(INTDIR)\avilib.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\colorspace.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\colorspace.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\colorspace.obj"	"$(INTDIR)\colorspace.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\editlist.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\editlist.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\editlist.obj"	"$(INTDIR)\editlist.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jpegutils.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\jpegutils.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\jpegutils.obj"	"$(INTDIR)\jpegutils.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\lav_common.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\lav_common.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\lav_common.obj"	"$(INTDIR)\lav_common.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\lav_io.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\lav_io.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\lav_io.obj"	"$(INTDIR)\lav_io.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mjpeg_logging.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\mjpeg_logging.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\mjpeg_logging.obj"	"$(INTDIR)\mjpeg_logging.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mpegconsts.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\mpegconsts.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\mpegconsts.obj"	"$(INTDIR)\mpegconsts.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\subsample.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\subsample.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\subsample.obj"	"$(INTDIR)\subsample.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\tools.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\tools.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\tools.obj"	"$(INTDIR)\tools.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\yuv4mpeg.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\yuv4mpeg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\yuv4mpeg.obj"	"$(INTDIR)\yuv4mpeg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\yuv4mpeg_ratio.c

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\yuv4mpeg_ratio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\yuv4mpeg_ratio.obj"	"$(INTDIR)\yuv4mpeg_ratio.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\yuvscaler.cpp

!IF  "$(CFG)" == "mjpeglib - Win32 Release"


"$(INTDIR)\yuvscaler.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"


"$(INTDIR)\yuvscaler.obj"	"$(INTDIR)\yuvscaler.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

