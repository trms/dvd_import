# Microsoft Developer Studio Generated NMAKE File, Based on mpg2enc.dsp
!IF "$(CFG)" == ""
CFG=mpg2enc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mpg2enc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mpg2enc - Win32 Release" && "$(CFG)" != "mpg2enc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpg2enc.mak" CFG="mpg2enc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpg2enc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mpg2enc - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "mpg2enc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mpg2enc.lib"


CLEAN :
	-@erase "$(INTDIR)\conform.obj"
	-@erase "$(INTDIR)\fdctref.obj"
	-@erase "$(INTDIR)\idct.obj"
	-@erase "$(INTDIR)\motionest.obj"
	-@erase "$(INTDIR)\motionsearch.obj"
	-@erase "$(INTDIR)\mpeg2enc.obj"
	-@erase "$(INTDIR)\MpegEnc.obj"
	-@erase "$(INTDIR)\predict.obj"
	-@erase "$(INTDIR)\putbits.obj"
	-@erase "$(INTDIR)\puthdr.obj"
	-@erase "$(INTDIR)\putmpg.obj"
	-@erase "$(INTDIR)\putpic.obj"
	-@erase "$(INTDIR)\putseq.obj"
	-@erase "$(INTDIR)\putvlc.obj"
	-@erase "$(INTDIR)\quantize.obj"
	-@erase "$(INTDIR)\ratectl.obj"
	-@erase "$(INTDIR)\readpic.obj"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\transfrm.obj"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writepic.obj"
	-@erase "$(OUTDIR)\mpg2enc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\mpg2enc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mpg2enc.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mpg2enc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fdctref.obj" \
	"$(INTDIR)\idct.obj" \
	"$(INTDIR)\conform.obj" \
	"$(INTDIR)\motionest.obj" \
	"$(INTDIR)\motionsearch.obj" \
	"$(INTDIR)\putvlc.obj" \
	"$(INTDIR)\putbits.obj" \
	"$(INTDIR)\puthdr.obj" \
	"$(INTDIR)\putmpg.obj" \
	"$(INTDIR)\putpic.obj" \
	"$(INTDIR)\putseq.obj" \
	"$(INTDIR)\predict.obj" \
	"$(INTDIR)\readpic.obj" \
	"$(INTDIR)\ratectl.obj" \
	"$(INTDIR)\quantize.obj" \
	"$(INTDIR)\writepic.obj" \
	"$(INTDIR)\transfrm.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\mpeg2enc.obj" \
	"$(INTDIR)\MpegEnc.obj" \
	"$(INTDIR)\types.obj"

"$(OUTDIR)\mpg2enc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mpg2enc.lib" "$(OUTDIR)\mpg2enc.bsc"


CLEAN :
	-@erase "$(INTDIR)\conform.obj"
	-@erase "$(INTDIR)\conform.sbr"
	-@erase "$(INTDIR)\fdctref.obj"
	-@erase "$(INTDIR)\fdctref.sbr"
	-@erase "$(INTDIR)\idct.obj"
	-@erase "$(INTDIR)\idct.sbr"
	-@erase "$(INTDIR)\motionest.obj"
	-@erase "$(INTDIR)\motionest.sbr"
	-@erase "$(INTDIR)\motionsearch.obj"
	-@erase "$(INTDIR)\motionsearch.sbr"
	-@erase "$(INTDIR)\mpeg2enc.obj"
	-@erase "$(INTDIR)\mpeg2enc.sbr"
	-@erase "$(INTDIR)\MpegEnc.obj"
	-@erase "$(INTDIR)\MpegEnc.sbr"
	-@erase "$(INTDIR)\predict.obj"
	-@erase "$(INTDIR)\predict.sbr"
	-@erase "$(INTDIR)\putbits.obj"
	-@erase "$(INTDIR)\putbits.sbr"
	-@erase "$(INTDIR)\puthdr.obj"
	-@erase "$(INTDIR)\puthdr.sbr"
	-@erase "$(INTDIR)\putmpg.obj"
	-@erase "$(INTDIR)\putmpg.sbr"
	-@erase "$(INTDIR)\putpic.obj"
	-@erase "$(INTDIR)\putpic.sbr"
	-@erase "$(INTDIR)\putseq.obj"
	-@erase "$(INTDIR)\putseq.sbr"
	-@erase "$(INTDIR)\putvlc.obj"
	-@erase "$(INTDIR)\putvlc.sbr"
	-@erase "$(INTDIR)\quantize.obj"
	-@erase "$(INTDIR)\quantize.sbr"
	-@erase "$(INTDIR)\ratectl.obj"
	-@erase "$(INTDIR)\ratectl.sbr"
	-@erase "$(INTDIR)\readpic.obj"
	-@erase "$(INTDIR)\readpic.sbr"
	-@erase "$(INTDIR)\stats.obj"
	-@erase "$(INTDIR)\stats.sbr"
	-@erase "$(INTDIR)\transfrm.obj"
	-@erase "$(INTDIR)\transfrm.sbr"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\types.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\writepic.obj"
	-@erase "$(INTDIR)\writepic.sbr"
	-@erase "$(OUTDIR)\mpg2enc.bsc"
	-@erase "$(OUTDIR)\mpg2enc.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mpg2enc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mpg2enc.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\fdctref.sbr" \
	"$(INTDIR)\idct.sbr" \
	"$(INTDIR)\conform.sbr" \
	"$(INTDIR)\motionest.sbr" \
	"$(INTDIR)\motionsearch.sbr" \
	"$(INTDIR)\putvlc.sbr" \
	"$(INTDIR)\putbits.sbr" \
	"$(INTDIR)\puthdr.sbr" \
	"$(INTDIR)\putmpg.sbr" \
	"$(INTDIR)\putpic.sbr" \
	"$(INTDIR)\putseq.sbr" \
	"$(INTDIR)\predict.sbr" \
	"$(INTDIR)\readpic.sbr" \
	"$(INTDIR)\ratectl.sbr" \
	"$(INTDIR)\quantize.sbr" \
	"$(INTDIR)\writepic.sbr" \
	"$(INTDIR)\transfrm.sbr" \
	"$(INTDIR)\stats.sbr" \
	"$(INTDIR)\mpeg2enc.sbr" \
	"$(INTDIR)\MpegEnc.sbr" \
	"$(INTDIR)\types.sbr"

"$(OUTDIR)\mpg2enc.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mpg2enc.lib" 
LIB32_OBJS= \
	"$(INTDIR)\fdctref.obj" \
	"$(INTDIR)\idct.obj" \
	"$(INTDIR)\conform.obj" \
	"$(INTDIR)\motionest.obj" \
	"$(INTDIR)\motionsearch.obj" \
	"$(INTDIR)\putvlc.obj" \
	"$(INTDIR)\putbits.obj" \
	"$(INTDIR)\puthdr.obj" \
	"$(INTDIR)\putmpg.obj" \
	"$(INTDIR)\putpic.obj" \
	"$(INTDIR)\putseq.obj" \
	"$(INTDIR)\predict.obj" \
	"$(INTDIR)\readpic.obj" \
	"$(INTDIR)\ratectl.obj" \
	"$(INTDIR)\quantize.obj" \
	"$(INTDIR)\writepic.obj" \
	"$(INTDIR)\transfrm.obj" \
	"$(INTDIR)\stats.obj" \
	"$(INTDIR)\mpeg2enc.obj" \
	"$(INTDIR)\MpegEnc.obj" \
	"$(INTDIR)\types.obj"

"$(OUTDIR)\mpg2enc.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("mpg2enc.dep")
!INCLUDE "mpg2enc.dep"
!ELSE 
!MESSAGE Warning: cannot find "mpg2enc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mpg2enc - Win32 Release" || "$(CFG)" == "mpg2enc - Win32 Debug"
SOURCE=.\conform.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\conform.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\conform.obj"	"$(INTDIR)\conform.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\fdctref.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\fdctref.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\fdctref.obj"	"$(INTDIR)\fdctref.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\idct.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\idct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\idct.obj"	"$(INTDIR)\idct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\motionest.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\motionest.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\motionest.obj"	"$(INTDIR)\motionest.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\motionsearch.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\motionsearch.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\motionsearch.obj"	"$(INTDIR)\motionsearch.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mpeg2enc.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\mpeg2enc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\mpeg2enc.obj"	"$(INTDIR)\mpeg2enc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MpegEnc.cpp

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\MpegEnc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\MpegEnc.obj"	"$(INTDIR)\MpegEnc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\predict.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\predict.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\predict.obj"	"$(INTDIR)\predict.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\putbits.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\putbits.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\putbits.obj"	"$(INTDIR)\putbits.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\puthdr.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\puthdr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\puthdr.obj"	"$(INTDIR)\puthdr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\putmpg.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\putmpg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\putmpg.obj"	"$(INTDIR)\putmpg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\putpic.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\putpic.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\putpic.obj"	"$(INTDIR)\putpic.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\putseq.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\putseq.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\putseq.obj"	"$(INTDIR)\putseq.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\putvlc.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\putvlc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\putvlc.obj"	"$(INTDIR)\putvlc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\quantize.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\quantize.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\quantize.obj"	"$(INTDIR)\quantize.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ratectl.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\ratectl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\ratectl.obj"	"$(INTDIR)\ratectl.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\readpic.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\readpic.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\readpic.obj"	"$(INTDIR)\readpic.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stats.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\stats.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\stats.obj"	"$(INTDIR)\stats.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\transfrm.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\transfrm.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\transfrm.obj"	"$(INTDIR)\transfrm.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\types.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\types.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\types.obj"	"$(INTDIR)\types.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\writepic.c

!IF  "$(CFG)" == "mpg2enc - Win32 Release"


"$(INTDIR)\writepic.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mpg2enc - Win32 Debug"


"$(INTDIR)\writepic.obj"	"$(INTDIR)\writepic.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

