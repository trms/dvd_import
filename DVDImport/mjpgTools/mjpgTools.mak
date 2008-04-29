# Microsoft Developer Studio Generated NMAKE File, Based on mjpgTools.dsp
!IF "$(CFG)" == ""
CFG=mjpgTools - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mjpgTools - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mjpgTools - Win32 Release" && "$(CFG)" != "mjpgTools - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mjpgTools.mak" CFG="mjpgTools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mjpgTools - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mjpgTools - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mjpgTools.dll"

!ELSE 

ALL : "STILLpatcher - Win32 Release" "mplex - Win32 Release" "mpg2enc - Win32 Release" "mjpeglib - Win32 Release" "tools - Win32 Release" "jpeglib - Win32 Release" "$(OUTDIR)\mjpgTools.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"jpeglib - Win32 ReleaseCLEAN" "tools - Win32 ReleaseCLEAN" "mjpeglib - Win32 ReleaseCLEAN" "mpg2enc - Win32 ReleaseCLEAN" "mplex - Win32 ReleaseCLEAN" "STILLpatcher - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\_yuvscaler.obj"
	-@erase "$(INTDIR)\cjpeg.obj"
	-@erase "$(INTDIR)\djpeg.obj"
	-@erase "$(INTDIR)\jpeg2yuv.obj"
	-@erase "$(INTDIR)\jpegInfo.obj"
	-@erase "$(INTDIR)\mjpgTools.obj"
	-@erase "$(INTDIR)\mjpgTools.pch"
	-@erase "$(INTDIR)\mjpgTools.res"
	-@erase "$(INTDIR)\Mpg2enc.obj"
	-@erase "$(INTDIR)\Mplex.obj"
	-@erase "$(INTDIR)\ppmtoyuv.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StillPatcher.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\yuvtoppm.obj"
	-@erase "$(OUTDIR)\mjpgTools.dll"
	-@erase "$(OUTDIR)\mjpgTools.exp"
	-@erase "$(OUTDIR)\mjpgTools.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /Fp"$(INTDIR)\mjpgTools.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mjpgTools.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mjpgTools.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\jpeglib\release\jpeglib.lib .\mjpeglib\release\mjpeglib.lib .\tools\release\tools.lib .\mpg2enc\release\mpg2enc.lib .\mplex\release\mplex.lib .\STILLpatcher\release\STILLpatcher.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\mjpgTools.pdb" /machine:I386 /def:".\mjpgTools.def" /out:"$(OUTDIR)\mjpgTools.dll" /implib:"$(OUTDIR)\mjpgTools.lib" 
DEF_FILE= \
	".\mjpgTools.def"
LINK32_OBJS= \
	"$(INTDIR)\_yuvscaler.obj" \
	"$(INTDIR)\cjpeg.obj" \
	"$(INTDIR)\djpeg.obj" \
	"$(INTDIR)\jpeg2yuv.obj" \
	"$(INTDIR)\jpegInfo.obj" \
	"$(INTDIR)\mjpgTools.obj" \
	"$(INTDIR)\Mpg2enc.obj" \
	"$(INTDIR)\Mplex.obj" \
	"$(INTDIR)\ppmtoyuv.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StillPatcher.obj" \
	"$(INTDIR)\yuvtoppm.obj" \
	"$(INTDIR)\mjpgTools.res" \
	".\jpeglib\Release\jpeglib.lib" \
	".\tools\Release\tools.lib" \
	".\mjpeglib\Release\mjpeglib.lib" \
	".\mpg2enc\Release\mpg2enc.lib" \
	".\mplex\Release\mplex.lib" \
	".\STILLpatcher\Release\STILLpatcher.lib"

"$(OUTDIR)\mjpgTools.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mjpgTools.dll" "$(OUTDIR)\mjpgTools.bsc"

!ELSE 

ALL : "STILLpatcher - Win32 Debug" "mplex - Win32 Debug" "mpg2enc - Win32 Debug" "mjpeglib - Win32 Debug" "tools - Win32 Debug" "jpeglib - Win32 Debug" "$(OUTDIR)\mjpgTools.dll" "$(OUTDIR)\mjpgTools.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"jpeglib - Win32 DebugCLEAN" "tools - Win32 DebugCLEAN" "mjpeglib - Win32 DebugCLEAN" "mpg2enc - Win32 DebugCLEAN" "mplex - Win32 DebugCLEAN" "STILLpatcher - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\_yuvscaler.obj"
	-@erase "$(INTDIR)\_yuvscaler.sbr"
	-@erase "$(INTDIR)\cjpeg.obj"
	-@erase "$(INTDIR)\cjpeg.sbr"
	-@erase "$(INTDIR)\djpeg.obj"
	-@erase "$(INTDIR)\djpeg.sbr"
	-@erase "$(INTDIR)\jpeg2yuv.obj"
	-@erase "$(INTDIR)\jpeg2yuv.sbr"
	-@erase "$(INTDIR)\jpegInfo.obj"
	-@erase "$(INTDIR)\jpegInfo.sbr"
	-@erase "$(INTDIR)\mjpgTools.obj"
	-@erase "$(INTDIR)\mjpgTools.pch"
	-@erase "$(INTDIR)\mjpgTools.res"
	-@erase "$(INTDIR)\mjpgTools.sbr"
	-@erase "$(INTDIR)\Mpg2enc.obj"
	-@erase "$(INTDIR)\Mpg2enc.sbr"
	-@erase "$(INTDIR)\Mplex.obj"
	-@erase "$(INTDIR)\Mplex.sbr"
	-@erase "$(INTDIR)\ppmtoyuv.obj"
	-@erase "$(INTDIR)\ppmtoyuv.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\StillPatcher.obj"
	-@erase "$(INTDIR)\StillPatcher.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\yuvtoppm.obj"
	-@erase "$(INTDIR)\yuvtoppm.sbr"
	-@erase "$(OUTDIR)\mjpgTools.bsc"
	-@erase "$(OUTDIR)\mjpgTools.dll"
	-@erase "$(OUTDIR)\mjpgTools.exp"
	-@erase "$(OUTDIR)\mjpgTools.ilk"
	-@erase "$(OUTDIR)\mjpgTools.lib"
	-@erase "$(OUTDIR)\mjpgTools.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mjpgTools.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mjpgTools.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mjpgTools.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\_yuvscaler.sbr" \
	"$(INTDIR)\cjpeg.sbr" \
	"$(INTDIR)\djpeg.sbr" \
	"$(INTDIR)\jpeg2yuv.sbr" \
	"$(INTDIR)\jpegInfo.sbr" \
	"$(INTDIR)\mjpgTools.sbr" \
	"$(INTDIR)\Mpg2enc.sbr" \
	"$(INTDIR)\Mplex.sbr" \
	"$(INTDIR)\ppmtoyuv.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\StillPatcher.sbr" \
	"$(INTDIR)\yuvtoppm.sbr"

"$(OUTDIR)\mjpgTools.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\jpeglib\debug\jpeglib.lib .\mjpeglib\debug\mjpeglib.lib .\tools\debug\tools.lib .\mpg2enc\debug\mpg2enc.lib .\mplex\Debug\mplex.lib .\STILLpatcher\Debug\STILLpatcher.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\mjpgTools.pdb" /debug /machine:I386 /nodefaultlib:"LIBCD.lib" /def:".\mjpgTools.def" /out:"$(OUTDIR)\mjpgTools.dll" /implib:"$(OUTDIR)\mjpgTools.lib" /pdbtype:sept /verbose:lib 
DEF_FILE= \
	".\mjpgTools.def"
LINK32_OBJS= \
	"$(INTDIR)\_yuvscaler.obj" \
	"$(INTDIR)\cjpeg.obj" \
	"$(INTDIR)\djpeg.obj" \
	"$(INTDIR)\jpeg2yuv.obj" \
	"$(INTDIR)\jpegInfo.obj" \
	"$(INTDIR)\mjpgTools.obj" \
	"$(INTDIR)\Mpg2enc.obj" \
	"$(INTDIR)\Mplex.obj" \
	"$(INTDIR)\ppmtoyuv.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\StillPatcher.obj" \
	"$(INTDIR)\yuvtoppm.obj" \
	"$(INTDIR)\mjpgTools.res" \
	".\jpeglib\Debug\jpeglib.lib" \
	".\tools\Debug\tools.lib" \
	".\mjpeglib\Debug\mjpeglib.lib" \
	".\mpg2enc\Debug\mpg2enc.lib" \
	".\mplex\Debug\mplex.lib" \
	".\STILLpatcher\Debug\STILLpatcher.lib"

"$(OUTDIR)\mjpgTools.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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
!IF EXISTS("mjpgTools.dep")
!INCLUDE "mjpgTools.dep"
!ELSE 
!MESSAGE Warning: cannot find "mjpgTools.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mjpgTools - Win32 Release" || "$(CFG)" == "mjpgTools - Win32 Debug"
SOURCE=.\_yuvscaler.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\_yuvscaler.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\_yuvscaler.obj"	"$(INTDIR)\_yuvscaler.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\cjpeg.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\cjpeg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\cjpeg.obj"	"$(INTDIR)\cjpeg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\djpeg.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\djpeg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\djpeg.obj"	"$(INTDIR)\djpeg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\jpeg2yuv.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\jpeg2yuv.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\jpeg2yuv.obj"	"$(INTDIR)\jpeg2yuv.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\jpegInfo.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\jpegInfo.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\jpegInfo.obj"	"$(INTDIR)\jpegInfo.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\mjpgTools.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\mjpgTools.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\mjpgTools.obj"	"$(INTDIR)\mjpgTools.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\mjpgTools.rc

"$(INTDIR)\mjpgTools.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\Mpg2enc.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\Mpg2enc.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\Mpg2enc.obj"	"$(INTDIR)\Mpg2enc.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\Mplex.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\Mplex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\Mplex.obj"	"$(INTDIR)\Mplex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\ppmtoyuv.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\ppmtoyuv.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\ppmtoyuv.obj"	"$(INTDIR)\ppmtoyuv.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /Fp"$(INTDIR)\mjpgTools.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mjpgTools.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mjpgTools.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\mjpgTools.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\StillPatcher.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\StillPatcher.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\StillPatcher.obj"	"$(INTDIR)\StillPatcher.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

SOURCE=.\yuvtoppm.cpp

!IF  "$(CFG)" == "mjpgTools - Win32 Release"


"$(INTDIR)\yuvtoppm.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"


"$(INTDIR)\yuvtoppm.obj"	"$(INTDIR)\yuvtoppm.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mjpgTools.pch"


!ENDIF 

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

"jpeglib - Win32 Release" : 
   cd ".\jpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\jpeglib.mak CFG="jpeglib - Win32 Release" 
   cd ".."

"jpeglib - Win32 ReleaseCLEAN" : 
   cd ".\jpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\jpeglib.mak CFG="jpeglib - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

"jpeglib - Win32 Debug" : 
   cd ".\jpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\jpeglib.mak CFG="jpeglib - Win32 Debug" 
   cd ".."

"jpeglib - Win32 DebugCLEAN" : 
   cd ".\jpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\jpeglib.mak CFG="jpeglib - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

"tools - Win32 Release" : 
   cd ".\tools"
   $(MAKE) /$(MAKEFLAGS) /F .\tools.mak CFG="tools - Win32 Release" 
   cd ".."

"tools - Win32 ReleaseCLEAN" : 
   cd ".\tools"
   $(MAKE) /$(MAKEFLAGS) /F .\tools.mak CFG="tools - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

"tools - Win32 Debug" : 
   cd ".\tools"
   $(MAKE) /$(MAKEFLAGS) /F .\tools.mak CFG="tools - Win32 Debug" 
   cd ".."

"tools - Win32 DebugCLEAN" : 
   cd ".\tools"
   $(MAKE) /$(MAKEFLAGS) /F .\tools.mak CFG="tools - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

"mjpeglib - Win32 Release" : 
   cd ".\mjpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\mjpeglib.mak CFG="mjpeglib - Win32 Release" 
   cd ".."

"mjpeglib - Win32 ReleaseCLEAN" : 
   cd ".\mjpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\mjpeglib.mak CFG="mjpeglib - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

"mjpeglib - Win32 Debug" : 
   cd ".\mjpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\mjpeglib.mak CFG="mjpeglib - Win32 Debug" 
   cd ".."

"mjpeglib - Win32 DebugCLEAN" : 
   cd ".\mjpeglib"
   $(MAKE) /$(MAKEFLAGS) /F .\mjpeglib.mak CFG="mjpeglib - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

"mpg2enc - Win32 Release" : 
   cd ".\mpg2enc"
   $(MAKE) /$(MAKEFLAGS) /F .\mpg2enc.mak CFG="mpg2enc - Win32 Release" 
   cd ".."

"mpg2enc - Win32 ReleaseCLEAN" : 
   cd ".\mpg2enc"
   $(MAKE) /$(MAKEFLAGS) /F .\mpg2enc.mak CFG="mpg2enc - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

"mpg2enc - Win32 Debug" : 
   cd ".\mpg2enc"
   $(MAKE) /$(MAKEFLAGS) /F .\mpg2enc.mak CFG="mpg2enc - Win32 Debug" 
   cd ".."

"mpg2enc - Win32 DebugCLEAN" : 
   cd ".\mpg2enc"
   $(MAKE) /$(MAKEFLAGS) /F .\mpg2enc.mak CFG="mpg2enc - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

"mplex - Win32 Release" : 
   cd ".\mplex"
   $(MAKE) /$(MAKEFLAGS) /F .\mplex.mak CFG="mplex - Win32 Release" 
   cd ".."

"mplex - Win32 ReleaseCLEAN" : 
   cd ".\mplex"
   $(MAKE) /$(MAKEFLAGS) /F .\mplex.mak CFG="mplex - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

"mplex - Win32 Debug" : 
   cd ".\mplex"
   $(MAKE) /$(MAKEFLAGS) /F .\mplex.mak CFG="mplex - Win32 Debug" 
   cd ".."

"mplex - Win32 DebugCLEAN" : 
   cd ".\mplex"
   $(MAKE) /$(MAKEFLAGS) /F .\mplex.mak CFG="mplex - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

"STILLpatcher - Win32 Release" : 
   cd ".\STILLpatcher"
   $(MAKE) /$(MAKEFLAGS) /F .\STILLpatcher.mak CFG="STILLpatcher - Win32 Release" 
   cd ".."

"STILLpatcher - Win32 ReleaseCLEAN" : 
   cd ".\STILLpatcher"
   $(MAKE) /$(MAKEFLAGS) /F .\STILLpatcher.mak CFG="STILLpatcher - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

"STILLpatcher - Win32 Debug" : 
   cd ".\STILLpatcher"
   $(MAKE) /$(MAKEFLAGS) /F .\STILLpatcher.mak CFG="STILLpatcher - Win32 Debug" 
   cd ".."

"STILLpatcher - Win32 DebugCLEAN" : 
   cd ".\STILLpatcher"
   $(MAKE) /$(MAKEFLAGS) /F .\STILLpatcher.mak CFG="STILLpatcher - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 


!ENDIF 

