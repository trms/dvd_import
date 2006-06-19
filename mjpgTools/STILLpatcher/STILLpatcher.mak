# Microsoft Developer Studio Generated NMAKE File, Based on STILLpatcher.dsp
!IF "$(CFG)" == ""
CFG=STILLpatcher - Win32 Debug
!MESSAGE No configuration specified. Defaulting to STILLpatcher - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "STILLpatcher - Win32 Release" && "$(CFG)" != "STILLpatcher - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "STILLpatcher.mak" CFG="STILLpatcher - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "STILLpatcher - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "STILLpatcher - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "STILLpatcher - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\STILLpatcher.lib"


CLEAN :
	-@erase "$(INTDIR)\MPeG.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\STILLpatcher.pch"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\STILLpatcher.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\STILLpatcher.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STILLpatcher.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\STILLpatcher.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\MPeG.obj"

"$(OUTDIR)\STILLpatcher.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "STILLpatcher - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\STILLpatcher.lib" "$(OUTDIR)\STILLpatcher.bsc"


CLEAN :
	-@erase "$(INTDIR)\MPeG.obj"
	-@erase "$(INTDIR)\MPeG.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\STILLpatcher.pch"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\STILLpatcher.bsc"
	-@erase "$(OUTDIR)\STILLpatcher.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STILLpatcher.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\STILLpatcher.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\MPeG.sbr"

"$(OUTDIR)\STILLpatcher.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\STILLpatcher.lib" 
LIB32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\MPeG.obj"

"$(OUTDIR)\STILLpatcher.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("STILLpatcher.dep")
!INCLUDE "STILLpatcher.dep"
!ELSE 
!MESSAGE Warning: cannot find "STILLpatcher.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "STILLpatcher - Win32 Release" || "$(CFG)" == "STILLpatcher - Win32 Debug"
SOURCE=.\MPeG.cpp

!IF  "$(CFG)" == "STILLpatcher - Win32 Release"


"$(INTDIR)\MPeG.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STILLpatcher.pch"


!ELSEIF  "$(CFG)" == "STILLpatcher - Win32 Debug"


"$(INTDIR)\MPeG.obj"	"$(INTDIR)\MPeG.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\STILLpatcher.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "STILLpatcher - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\STILLpatcher.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\STILLpatcher.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "STILLpatcher - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\STILLpatcher.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\STILLpatcher.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

