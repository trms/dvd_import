# Microsoft Developer Studio Generated NMAKE File, Based on tools.dsp
!IF "$(CFG)" == ""
CFG=tools - Win32 Debug
!MESSAGE No configuration specified. Defaulting to tools - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "tools - Win32 Release" && "$(CFG)" != "tools - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tools.mak" CFG="tools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tools - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "tools - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "tools - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\tools.lib"


CLEAN :
	-@erase "$(INTDIR)\cdjpeg.obj"
	-@erase "$(INTDIR)\rdbmp.obj"
	-@erase "$(INTDIR)\rdcolmap.obj"
	-@erase "$(INTDIR)\rdgif.obj"
	-@erase "$(INTDIR)\rdppm.obj"
	-@erase "$(INTDIR)\rdrle.obj"
	-@erase "$(INTDIR)\rdswitch.obj"
	-@erase "$(INTDIR)\rdtarga.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wrbmp.obj"
	-@erase "$(INTDIR)\wrgif.obj"
	-@erase "$(INTDIR)\wrppm.obj"
	-@erase "$(INTDIR)\wrrle.obj"
	-@erase "$(INTDIR)\wrtarga.obj"
	-@erase "$(OUTDIR)\tools.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\tools.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tools.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cdjpeg.obj" \
	"$(INTDIR)\rdtarga.obj" \
	"$(INTDIR)\rdgif.obj" \
	"$(INTDIR)\rdppm.obj" \
	"$(INTDIR)\rdrle.obj" \
	"$(INTDIR)\rdswitch.obj" \
	"$(INTDIR)\rdbmp.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\rdcolmap.obj" \
	"$(INTDIR)\wrtarga.obj" \
	"$(INTDIR)\wrgif.obj" \
	"$(INTDIR)\wrppm.obj" \
	"$(INTDIR)\wrrle.obj" \
	"$(INTDIR)\wrbmp.obj"

"$(OUTDIR)\tools.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "tools - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\tools.lib"


CLEAN :
	-@erase "$(INTDIR)\cdjpeg.obj"
	-@erase "$(INTDIR)\rdbmp.obj"
	-@erase "$(INTDIR)\rdcolmap.obj"
	-@erase "$(INTDIR)\rdgif.obj"
	-@erase "$(INTDIR)\rdppm.obj"
	-@erase "$(INTDIR)\rdrle.obj"
	-@erase "$(INTDIR)\rdswitch.obj"
	-@erase "$(INTDIR)\rdtarga.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wrbmp.obj"
	-@erase "$(INTDIR)\wrgif.obj"
	-@erase "$(INTDIR)\wrppm.obj"
	-@erase "$(INTDIR)\wrrle.obj"
	-@erase "$(INTDIR)\wrtarga.obj"
	-@erase "$(OUTDIR)\tools.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\tools.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tools.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\tools.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cdjpeg.obj" \
	"$(INTDIR)\rdtarga.obj" \
	"$(INTDIR)\rdgif.obj" \
	"$(INTDIR)\rdppm.obj" \
	"$(INTDIR)\rdrle.obj" \
	"$(INTDIR)\rdswitch.obj" \
	"$(INTDIR)\rdbmp.obj" \
	"$(INTDIR)\tools.obj" \
	"$(INTDIR)\rdcolmap.obj" \
	"$(INTDIR)\wrtarga.obj" \
	"$(INTDIR)\wrgif.obj" \
	"$(INTDIR)\wrppm.obj" \
	"$(INTDIR)\wrrle.obj" \
	"$(INTDIR)\wrbmp.obj"

"$(OUTDIR)\tools.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("tools.dep")
!INCLUDE "tools.dep"
!ELSE 
!MESSAGE Warning: cannot find "tools.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "tools - Win32 Release" || "$(CFG)" == "tools - Win32 Debug"
SOURCE=.\cdjpeg.c

"$(INTDIR)\cdjpeg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rdbmp.c

"$(INTDIR)\rdbmp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rdcolmap.c

"$(INTDIR)\rdcolmap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rdgif.c

"$(INTDIR)\rdgif.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rdppm.c

"$(INTDIR)\rdppm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rdrle.c

"$(INTDIR)\rdrle.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rdswitch.c

"$(INTDIR)\rdswitch.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\rdtarga.c

"$(INTDIR)\rdtarga.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\tools.cpp

"$(INTDIR)\tools.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wrbmp.c

"$(INTDIR)\wrbmp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wrgif.c

"$(INTDIR)\wrgif.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wrppm.c

"$(INTDIR)\wrppm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wrrle.c

"$(INTDIR)\wrrle.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wrtarga.c

"$(INTDIR)\wrtarga.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

