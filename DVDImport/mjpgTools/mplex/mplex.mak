# Microsoft Developer Studio Generated NMAKE File, Based on mplex.dsp
!IF "$(CFG)" == ""
CFG=mplex - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mplex - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mplex - Win32 Release" && "$(CFG)" != "mplex - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mplex.mak" CFG="mplex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mplex - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mplex - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "mplex - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mplex.lib"


CLEAN :
	-@erase "$(INTDIR)\ac3strm_in.obj"
	-@erase "$(INTDIR)\audiostrm_out.obj"
	-@erase "$(INTDIR)\bits.obj"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\inputstrm.obj"
	-@erase "$(INTDIR)\interact.obj"
	-@erase "$(INTDIR)\mpastrm_in.obj"
	-@erase "$(INTDIR)\multplex.obj"
	-@erase "$(INTDIR)\padstrm.obj"
	-@erase "$(INTDIR)\stillsstream.obj"
	-@erase "$(INTDIR)\systems.obj"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\videostrm_in.obj"
	-@erase "$(INTDIR)\videostrm_out.obj"
	-@erase "$(OUTDIR)\mplex.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\mplex.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mplex.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mplex.lib" 
LIB32_OBJS= \
	"$(INTDIR)\types.obj" \
	"$(INTDIR)\bits.obj" \
	"$(INTDIR)\vector.obj" \
	"$(INTDIR)\inputstrm.obj" \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\multplex.obj" \
	"$(INTDIR)\padstrm.obj" \
	"$(INTDIR)\systems.obj" \
	"$(INTDIR)\interact.obj" \
	"$(INTDIR)\videostrm_out.obj" \
	"$(INTDIR)\videostrm_in.obj" \
	"$(INTDIR)\audiostrm_out.obj" \
	"$(INTDIR)\ac3strm_in.obj" \
	"$(INTDIR)\stillsstream.obj" \
	"$(INTDIR)\mpastrm_in.obj"

"$(OUTDIR)\mplex.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mplex.lib" "$(OUTDIR)\mplex.bsc"


CLEAN :
	-@erase "$(INTDIR)\ac3strm_in.obj"
	-@erase "$(INTDIR)\ac3strm_in.sbr"
	-@erase "$(INTDIR)\audiostrm_out.obj"
	-@erase "$(INTDIR)\audiostrm_out.sbr"
	-@erase "$(INTDIR)\bits.obj"
	-@erase "$(INTDIR)\bits.sbr"
	-@erase "$(INTDIR)\buffer.obj"
	-@erase "$(INTDIR)\buffer.sbr"
	-@erase "$(INTDIR)\inputstrm.obj"
	-@erase "$(INTDIR)\inputstrm.sbr"
	-@erase "$(INTDIR)\interact.obj"
	-@erase "$(INTDIR)\interact.sbr"
	-@erase "$(INTDIR)\mpastrm_in.obj"
	-@erase "$(INTDIR)\mpastrm_in.sbr"
	-@erase "$(INTDIR)\multplex.obj"
	-@erase "$(INTDIR)\multplex.sbr"
	-@erase "$(INTDIR)\padstrm.obj"
	-@erase "$(INTDIR)\padstrm.sbr"
	-@erase "$(INTDIR)\stillsstream.obj"
	-@erase "$(INTDIR)\stillsstream.sbr"
	-@erase "$(INTDIR)\systems.obj"
	-@erase "$(INTDIR)\systems.sbr"
	-@erase "$(INTDIR)\types.obj"
	-@erase "$(INTDIR)\types.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vector.obj"
	-@erase "$(INTDIR)\vector.sbr"
	-@erase "$(INTDIR)\videostrm_in.obj"
	-@erase "$(INTDIR)\videostrm_in.sbr"
	-@erase "$(INTDIR)\videostrm_out.obj"
	-@erase "$(INTDIR)\videostrm_out.sbr"
	-@erase "$(OUTDIR)\mplex.bsc"
	-@erase "$(OUTDIR)\mplex.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gd /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\mplex.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mplex.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\types.sbr" \
	"$(INTDIR)\bits.sbr" \
	"$(INTDIR)\vector.sbr" \
	"$(INTDIR)\inputstrm.sbr" \
	"$(INTDIR)\buffer.sbr" \
	"$(INTDIR)\multplex.sbr" \
	"$(INTDIR)\padstrm.sbr" \
	"$(INTDIR)\systems.sbr" \
	"$(INTDIR)\interact.sbr" \
	"$(INTDIR)\videostrm_out.sbr" \
	"$(INTDIR)\videostrm_in.sbr" \
	"$(INTDIR)\audiostrm_out.sbr" \
	"$(INTDIR)\ac3strm_in.sbr" \
	"$(INTDIR)\stillsstream.sbr" \
	"$(INTDIR)\mpastrm_in.sbr"

"$(OUTDIR)\mplex.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mplex.lib" 
LIB32_OBJS= \
	"$(INTDIR)\types.obj" \
	"$(INTDIR)\bits.obj" \
	"$(INTDIR)\vector.obj" \
	"$(INTDIR)\inputstrm.obj" \
	"$(INTDIR)\buffer.obj" \
	"$(INTDIR)\multplex.obj" \
	"$(INTDIR)\padstrm.obj" \
	"$(INTDIR)\systems.obj" \
	"$(INTDIR)\interact.obj" \
	"$(INTDIR)\videostrm_out.obj" \
	"$(INTDIR)\videostrm_in.obj" \
	"$(INTDIR)\audiostrm_out.obj" \
	"$(INTDIR)\ac3strm_in.obj" \
	"$(INTDIR)\stillsstream.obj" \
	"$(INTDIR)\mpastrm_in.obj"

"$(OUTDIR)\mplex.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("mplex.dep")
!INCLUDE "mplex.dep"
!ELSE 
!MESSAGE Warning: cannot find "mplex.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mplex - Win32 Release" || "$(CFG)" == "mplex - Win32 Debug"
SOURCE=.\ac3strm_in.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\ac3strm_in.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\ac3strm_in.obj"	"$(INTDIR)\ac3strm_in.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\audiostrm_out.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\audiostrm_out.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\audiostrm_out.obj"	"$(INTDIR)\audiostrm_out.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\bits.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\bits.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\bits.obj"	"$(INTDIR)\bits.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\buffer.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\buffer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\buffer.obj"	"$(INTDIR)\buffer.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\inputstrm.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\inputstrm.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\inputstrm.obj"	"$(INTDIR)\inputstrm.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\interact.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\interact.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\interact.obj"	"$(INTDIR)\interact.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mpastrm_in.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\mpastrm_in.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\mpastrm_in.obj"	"$(INTDIR)\mpastrm_in.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\multplex.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\multplex.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\multplex.obj"	"$(INTDIR)\multplex.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\padstrm.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\padstrm.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\padstrm.obj"	"$(INTDIR)\padstrm.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\stillsstream.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\stillsstream.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\stillsstream.obj"	"$(INTDIR)\stillsstream.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\systems.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\systems.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\systems.obj"	"$(INTDIR)\systems.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\types.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\types.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\types.obj"	"$(INTDIR)\types.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\vector.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\vector.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\vector.obj"	"$(INTDIR)\vector.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\videostrm_in.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\videostrm_in.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\videostrm_in.obj"	"$(INTDIR)\videostrm_in.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\videostrm_out.cpp

!IF  "$(CFG)" == "mplex - Win32 Release"


"$(INTDIR)\videostrm_out.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "mplex - Win32 Debug"


"$(INTDIR)\videostrm_out.obj"	"$(INTDIR)\videostrm_out.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

