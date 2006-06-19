# Microsoft Developer Studio Generated NMAKE File, Based on jpeglib.dsp
!IF "$(CFG)" == ""
CFG=jpeglib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to jpeglib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "jpeglib - Win32 Release" && "$(CFG)" != "jpeglib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeglib.mak" CFG="jpeglib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeglib - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "jpeglib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\jpeglib.lib"


CLEAN :
	-@erase "$(INTDIR)\jcapimin.obj"
	-@erase "$(INTDIR)\jcapistd.obj"
	-@erase "$(INTDIR)\jccoefct.obj"
	-@erase "$(INTDIR)\jccolor.obj"
	-@erase "$(INTDIR)\jcdctmgr.obj"
	-@erase "$(INTDIR)\jchuff.obj"
	-@erase "$(INTDIR)\jcinit.obj"
	-@erase "$(INTDIR)\jcmainct.obj"
	-@erase "$(INTDIR)\jcmarker.obj"
	-@erase "$(INTDIR)\jcmaster.obj"
	-@erase "$(INTDIR)\jcomapi.obj"
	-@erase "$(INTDIR)\jcparam.obj"
	-@erase "$(INTDIR)\jcphuff.obj"
	-@erase "$(INTDIR)\jcprepct.obj"
	-@erase "$(INTDIR)\jcsample.obj"
	-@erase "$(INTDIR)\jctrans.obj"
	-@erase "$(INTDIR)\jdapimin.obj"
	-@erase "$(INTDIR)\jdapistd.obj"
	-@erase "$(INTDIR)\jdatadst.obj"
	-@erase "$(INTDIR)\jdatasrc.obj"
	-@erase "$(INTDIR)\jdcoefct.obj"
	-@erase "$(INTDIR)\jdcolor.obj"
	-@erase "$(INTDIR)\jddctmgr.obj"
	-@erase "$(INTDIR)\jdhuff.obj"
	-@erase "$(INTDIR)\jdinput.obj"
	-@erase "$(INTDIR)\jdmainct.obj"
	-@erase "$(INTDIR)\jdmarker.obj"
	-@erase "$(INTDIR)\jdmaster.obj"
	-@erase "$(INTDIR)\jdmerge.obj"
	-@erase "$(INTDIR)\jdphuff.obj"
	-@erase "$(INTDIR)\jdpostct.obj"
	-@erase "$(INTDIR)\jdsample.obj"
	-@erase "$(INTDIR)\jdtrans.obj"
	-@erase "$(INTDIR)\jerror.obj"
	-@erase "$(INTDIR)\jfdctflt.obj"
	-@erase "$(INTDIR)\jfdctfst.obj"
	-@erase "$(INTDIR)\jfdctint.obj"
	-@erase "$(INTDIR)\jidctflt.obj"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jidctint.obj"
	-@erase "$(INTDIR)\jidctred.obj"
	-@erase "$(INTDIR)\jmemmgr.obj"
	-@erase "$(INTDIR)\jmemnobs.obj"
	-@erase "$(INTDIR)\jquant1.obj"
	-@erase "$(INTDIR)\jquant2.obj"
	-@erase "$(INTDIR)\jutils.obj"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\jpeglib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\jpeglib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\jpeglib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\jpeglib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\jcapimin.obj" \
	"$(INTDIR)\jcapistd.obj" \
	"$(INTDIR)\jccoefct.obj" \
	"$(INTDIR)\jccolor.obj" \
	"$(INTDIR)\jcdctmgr.obj" \
	"$(INTDIR)\jchuff.obj" \
	"$(INTDIR)\jcinit.obj" \
	"$(INTDIR)\jcmainct.obj" \
	"$(INTDIR)\jcmarker.obj" \
	"$(INTDIR)\jcmaster.obj" \
	"$(INTDIR)\jcomapi.obj" \
	"$(INTDIR)\jcparam.obj" \
	"$(INTDIR)\jcphuff.obj" \
	"$(INTDIR)\jcprepct.obj" \
	"$(INTDIR)\jcsample.obj" \
	"$(INTDIR)\jctrans.obj" \
	"$(INTDIR)\jdapimin.obj" \
	"$(INTDIR)\jdapistd.obj" \
	"$(INTDIR)\jdatadst.obj" \
	"$(INTDIR)\jdatasrc.obj" \
	"$(INTDIR)\jdcoefct.obj" \
	"$(INTDIR)\jdcolor.obj" \
	"$(INTDIR)\jddctmgr.obj" \
	"$(INTDIR)\jdhuff.obj" \
	"$(INTDIR)\jdinput.obj" \
	"$(INTDIR)\jdmainct.obj" \
	"$(INTDIR)\jdmarker.obj" \
	"$(INTDIR)\jdmaster.obj" \
	"$(INTDIR)\jdmerge.obj" \
	"$(INTDIR)\jdphuff.obj" \
	"$(INTDIR)\jdpostct.obj" \
	"$(INTDIR)\jdsample.obj" \
	"$(INTDIR)\jdtrans.obj" \
	"$(INTDIR)\jerror.obj" \
	"$(INTDIR)\jfdctflt.obj" \
	"$(INTDIR)\jfdctfst.obj" \
	"$(INTDIR)\jfdctint.obj" \
	"$(INTDIR)\jidctflt.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jidctint.obj" \
	"$(INTDIR)\jidctred.obj" \
	"$(INTDIR)\jmemmgr.obj" \
	"$(INTDIR)\jmemnobs.obj" \
	"$(INTDIR)\jquant1.obj" \
	"$(INTDIR)\jquant2.obj" \
	"$(INTDIR)\jutils.obj" \
	"$(INTDIR)\tools.obj"

"$(OUTDIR)\jpeglib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\jpeglib.lib" "$(OUTDIR)\jpeglib.bsc"


CLEAN :
	-@erase "$(INTDIR)\jcapimin.obj"
	-@erase "$(INTDIR)\jcapimin.sbr"
	-@erase "$(INTDIR)\jcapistd.obj"
	-@erase "$(INTDIR)\jcapistd.sbr"
	-@erase "$(INTDIR)\jccoefct.obj"
	-@erase "$(INTDIR)\jccoefct.sbr"
	-@erase "$(INTDIR)\jccolor.obj"
	-@erase "$(INTDIR)\jccolor.sbr"
	-@erase "$(INTDIR)\jcdctmgr.obj"
	-@erase "$(INTDIR)\jcdctmgr.sbr"
	-@erase "$(INTDIR)\jchuff.obj"
	-@erase "$(INTDIR)\jchuff.sbr"
	-@erase "$(INTDIR)\jcinit.obj"
	-@erase "$(INTDIR)\jcinit.sbr"
	-@erase "$(INTDIR)\jcmainct.obj"
	-@erase "$(INTDIR)\jcmainct.sbr"
	-@erase "$(INTDIR)\jcmarker.obj"
	-@erase "$(INTDIR)\jcmarker.sbr"
	-@erase "$(INTDIR)\jcmaster.obj"
	-@erase "$(INTDIR)\jcmaster.sbr"
	-@erase "$(INTDIR)\jcomapi.obj"
	-@erase "$(INTDIR)\jcomapi.sbr"
	-@erase "$(INTDIR)\jcparam.obj"
	-@erase "$(INTDIR)\jcparam.sbr"
	-@erase "$(INTDIR)\jcphuff.obj"
	-@erase "$(INTDIR)\jcphuff.sbr"
	-@erase "$(INTDIR)\jcprepct.obj"
	-@erase "$(INTDIR)\jcprepct.sbr"
	-@erase "$(INTDIR)\jcsample.obj"
	-@erase "$(INTDIR)\jcsample.sbr"
	-@erase "$(INTDIR)\jctrans.obj"
	-@erase "$(INTDIR)\jctrans.sbr"
	-@erase "$(INTDIR)\jdapimin.obj"
	-@erase "$(INTDIR)\jdapimin.sbr"
	-@erase "$(INTDIR)\jdapistd.obj"
	-@erase "$(INTDIR)\jdapistd.sbr"
	-@erase "$(INTDIR)\jdatadst.obj"
	-@erase "$(INTDIR)\jdatadst.sbr"
	-@erase "$(INTDIR)\jdatasrc.obj"
	-@erase "$(INTDIR)\jdatasrc.sbr"
	-@erase "$(INTDIR)\jdcoefct.obj"
	-@erase "$(INTDIR)\jdcoefct.sbr"
	-@erase "$(INTDIR)\jdcolor.obj"
	-@erase "$(INTDIR)\jdcolor.sbr"
	-@erase "$(INTDIR)\jddctmgr.obj"
	-@erase "$(INTDIR)\jddctmgr.sbr"
	-@erase "$(INTDIR)\jdhuff.obj"
	-@erase "$(INTDIR)\jdhuff.sbr"
	-@erase "$(INTDIR)\jdinput.obj"
	-@erase "$(INTDIR)\jdinput.sbr"
	-@erase "$(INTDIR)\jdmainct.obj"
	-@erase "$(INTDIR)\jdmainct.sbr"
	-@erase "$(INTDIR)\jdmarker.obj"
	-@erase "$(INTDIR)\jdmarker.sbr"
	-@erase "$(INTDIR)\jdmaster.obj"
	-@erase "$(INTDIR)\jdmaster.sbr"
	-@erase "$(INTDIR)\jdmerge.obj"
	-@erase "$(INTDIR)\jdmerge.sbr"
	-@erase "$(INTDIR)\jdphuff.obj"
	-@erase "$(INTDIR)\jdphuff.sbr"
	-@erase "$(INTDIR)\jdpostct.obj"
	-@erase "$(INTDIR)\jdpostct.sbr"
	-@erase "$(INTDIR)\jdsample.obj"
	-@erase "$(INTDIR)\jdsample.sbr"
	-@erase "$(INTDIR)\jdtrans.obj"
	-@erase "$(INTDIR)\jdtrans.sbr"
	-@erase "$(INTDIR)\jerror.obj"
	-@erase "$(INTDIR)\jerror.sbr"
	-@erase "$(INTDIR)\jfdctflt.obj"
	-@erase "$(INTDIR)\jfdctflt.sbr"
	-@erase "$(INTDIR)\jfdctfst.obj"
	-@erase "$(INTDIR)\jfdctfst.sbr"
	-@erase "$(INTDIR)\jfdctint.obj"
	-@erase "$(INTDIR)\jfdctint.sbr"
	-@erase "$(INTDIR)\jidctflt.obj"
	-@erase "$(INTDIR)\jidctflt.sbr"
	-@erase "$(INTDIR)\jidctfst.obj"
	-@erase "$(INTDIR)\jidctfst.sbr"
	-@erase "$(INTDIR)\jidctint.obj"
	-@erase "$(INTDIR)\jidctint.sbr"
	-@erase "$(INTDIR)\jidctred.obj"
	-@erase "$(INTDIR)\jidctred.sbr"
	-@erase "$(INTDIR)\jmemmgr.obj"
	-@erase "$(INTDIR)\jmemmgr.sbr"
	-@erase "$(INTDIR)\jmemnobs.obj"
	-@erase "$(INTDIR)\jmemnobs.sbr"
	-@erase "$(INTDIR)\jquant1.obj"
	-@erase "$(INTDIR)\jquant1.sbr"
	-@erase "$(INTDIR)\jquant2.obj"
	-@erase "$(INTDIR)\jquant2.sbr"
	-@erase "$(INTDIR)\jutils.obj"
	-@erase "$(INTDIR)\jutils.sbr"
	-@erase "$(INTDIR)\tools.obj"
	-@erase "$(INTDIR)\tools.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\jpeglib.bsc"
	-@erase "$(OUTDIR)\jpeglib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\jpeglib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\jpeglib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\jcapimin.sbr" \
	"$(INTDIR)\jcapistd.sbr" \
	"$(INTDIR)\jccoefct.sbr" \
	"$(INTDIR)\jccolor.sbr" \
	"$(INTDIR)\jcdctmgr.sbr" \
	"$(INTDIR)\jchuff.sbr" \
	"$(INTDIR)\jcinit.sbr" \
	"$(INTDIR)\jcmainct.sbr" \
	"$(INTDIR)\jcmarker.sbr" \
	"$(INTDIR)\jcmaster.sbr" \
	"$(INTDIR)\jcomapi.sbr" \
	"$(INTDIR)\jcparam.sbr" \
	"$(INTDIR)\jcphuff.sbr" \
	"$(INTDIR)\jcprepct.sbr" \
	"$(INTDIR)\jcsample.sbr" \
	"$(INTDIR)\jctrans.sbr" \
	"$(INTDIR)\jdapimin.sbr" \
	"$(INTDIR)\jdapistd.sbr" \
	"$(INTDIR)\jdatadst.sbr" \
	"$(INTDIR)\jdatasrc.sbr" \
	"$(INTDIR)\jdcoefct.sbr" \
	"$(INTDIR)\jdcolor.sbr" \
	"$(INTDIR)\jddctmgr.sbr" \
	"$(INTDIR)\jdhuff.sbr" \
	"$(INTDIR)\jdinput.sbr" \
	"$(INTDIR)\jdmainct.sbr" \
	"$(INTDIR)\jdmarker.sbr" \
	"$(INTDIR)\jdmaster.sbr" \
	"$(INTDIR)\jdmerge.sbr" \
	"$(INTDIR)\jdphuff.sbr" \
	"$(INTDIR)\jdpostct.sbr" \
	"$(INTDIR)\jdsample.sbr" \
	"$(INTDIR)\jdtrans.sbr" \
	"$(INTDIR)\jerror.sbr" \
	"$(INTDIR)\jfdctflt.sbr" \
	"$(INTDIR)\jfdctfst.sbr" \
	"$(INTDIR)\jfdctint.sbr" \
	"$(INTDIR)\jidctflt.sbr" \
	"$(INTDIR)\jidctfst.sbr" \
	"$(INTDIR)\jidctint.sbr" \
	"$(INTDIR)\jidctred.sbr" \
	"$(INTDIR)\jmemmgr.sbr" \
	"$(INTDIR)\jmemnobs.sbr" \
	"$(INTDIR)\jquant1.sbr" \
	"$(INTDIR)\jquant2.sbr" \
	"$(INTDIR)\jutils.sbr" \
	"$(INTDIR)\tools.sbr"

"$(OUTDIR)\jpeglib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\jpeglib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\jcapimin.obj" \
	"$(INTDIR)\jcapistd.obj" \
	"$(INTDIR)\jccoefct.obj" \
	"$(INTDIR)\jccolor.obj" \
	"$(INTDIR)\jcdctmgr.obj" \
	"$(INTDIR)\jchuff.obj" \
	"$(INTDIR)\jcinit.obj" \
	"$(INTDIR)\jcmainct.obj" \
	"$(INTDIR)\jcmarker.obj" \
	"$(INTDIR)\jcmaster.obj" \
	"$(INTDIR)\jcomapi.obj" \
	"$(INTDIR)\jcparam.obj" \
	"$(INTDIR)\jcphuff.obj" \
	"$(INTDIR)\jcprepct.obj" \
	"$(INTDIR)\jcsample.obj" \
	"$(INTDIR)\jctrans.obj" \
	"$(INTDIR)\jdapimin.obj" \
	"$(INTDIR)\jdapistd.obj" \
	"$(INTDIR)\jdatadst.obj" \
	"$(INTDIR)\jdatasrc.obj" \
	"$(INTDIR)\jdcoefct.obj" \
	"$(INTDIR)\jdcolor.obj" \
	"$(INTDIR)\jddctmgr.obj" \
	"$(INTDIR)\jdhuff.obj" \
	"$(INTDIR)\jdinput.obj" \
	"$(INTDIR)\jdmainct.obj" \
	"$(INTDIR)\jdmarker.obj" \
	"$(INTDIR)\jdmaster.obj" \
	"$(INTDIR)\jdmerge.obj" \
	"$(INTDIR)\jdphuff.obj" \
	"$(INTDIR)\jdpostct.obj" \
	"$(INTDIR)\jdsample.obj" \
	"$(INTDIR)\jdtrans.obj" \
	"$(INTDIR)\jerror.obj" \
	"$(INTDIR)\jfdctflt.obj" \
	"$(INTDIR)\jfdctfst.obj" \
	"$(INTDIR)\jfdctint.obj" \
	"$(INTDIR)\jidctflt.obj" \
	"$(INTDIR)\jidctfst.obj" \
	"$(INTDIR)\jidctint.obj" \
	"$(INTDIR)\jidctred.obj" \
	"$(INTDIR)\jmemmgr.obj" \
	"$(INTDIR)\jmemnobs.obj" \
	"$(INTDIR)\jquant1.obj" \
	"$(INTDIR)\jquant2.obj" \
	"$(INTDIR)\jutils.obj" \
	"$(INTDIR)\tools.obj"

"$(OUTDIR)\jpeglib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("jpeglib.dep")
!INCLUDE "jpeglib.dep"
!ELSE 
!MESSAGE Warning: cannot find "jpeglib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "jpeglib - Win32 Release" || "$(CFG)" == "jpeglib - Win32 Debug"
SOURCE=.\jcapimin.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcapimin.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcapimin.obj"	"$(INTDIR)\jcapimin.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcapistd.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcapistd.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcapistd.obj"	"$(INTDIR)\jcapistd.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jccoefct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jccoefct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jccoefct.obj"	"$(INTDIR)\jccoefct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jccolor.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jccolor.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jccolor.obj"	"$(INTDIR)\jccolor.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcdctmgr.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcdctmgr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcdctmgr.obj"	"$(INTDIR)\jcdctmgr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jchuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jchuff.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jchuff.obj"	"$(INTDIR)\jchuff.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcinit.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcinit.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcinit.obj"	"$(INTDIR)\jcinit.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcmainct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcmainct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcmainct.obj"	"$(INTDIR)\jcmainct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcmarker.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcmarker.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcmarker.obj"	"$(INTDIR)\jcmarker.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcmaster.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcmaster.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcmaster.obj"	"$(INTDIR)\jcmaster.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcomapi.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcomapi.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcomapi.obj"	"$(INTDIR)\jcomapi.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcparam.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcparam.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcparam.obj"	"$(INTDIR)\jcparam.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcphuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcphuff.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcphuff.obj"	"$(INTDIR)\jcphuff.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcprepct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcprepct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcprepct.obj"	"$(INTDIR)\jcprepct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jcsample.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jcsample.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jcsample.obj"	"$(INTDIR)\jcsample.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jctrans.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jctrans.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jctrans.obj"	"$(INTDIR)\jctrans.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdapimin.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdapimin.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdapimin.obj"	"$(INTDIR)\jdapimin.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdapistd.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdapistd.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdapistd.obj"	"$(INTDIR)\jdapistd.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdatadst.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdatadst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdatadst.obj"	"$(INTDIR)\jdatadst.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdatasrc.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdatasrc.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdatasrc.obj"	"$(INTDIR)\jdatasrc.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdcoefct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdcoefct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdcoefct.obj"	"$(INTDIR)\jdcoefct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdcolor.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdcolor.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdcolor.obj"	"$(INTDIR)\jdcolor.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jddctmgr.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jddctmgr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jddctmgr.obj"	"$(INTDIR)\jddctmgr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdhuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdhuff.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdhuff.obj"	"$(INTDIR)\jdhuff.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdinput.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdinput.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdinput.obj"	"$(INTDIR)\jdinput.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmainct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdmainct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdmainct.obj"	"$(INTDIR)\jdmainct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmarker.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdmarker.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdmarker.obj"	"$(INTDIR)\jdmarker.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmaster.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdmaster.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdmaster.obj"	"$(INTDIR)\jdmaster.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdmerge.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdmerge.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdmerge.obj"	"$(INTDIR)\jdmerge.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdphuff.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdphuff.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdphuff.obj"	"$(INTDIR)\jdphuff.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdpostct.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdpostct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdpostct.obj"	"$(INTDIR)\jdpostct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdsample.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdsample.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdsample.obj"	"$(INTDIR)\jdsample.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jdtrans.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jdtrans.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jdtrans.obj"	"$(INTDIR)\jdtrans.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jerror.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jerror.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jerror.obj"	"$(INTDIR)\jerror.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jfdctflt.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jfdctflt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jfdctflt.obj"	"$(INTDIR)\jfdctflt.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jfdctfst.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jfdctfst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jfdctfst.obj"	"$(INTDIR)\jfdctfst.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jfdctint.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jfdctint.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jfdctint.obj"	"$(INTDIR)\jfdctint.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctflt.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jidctflt.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jidctflt.obj"	"$(INTDIR)\jidctflt.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctfst.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jidctfst.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jidctfst.obj"	"$(INTDIR)\jidctfst.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctint.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jidctint.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jidctint.obj"	"$(INTDIR)\jidctint.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jidctred.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jidctred.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jidctred.obj"	"$(INTDIR)\jidctred.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jmemmgr.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jmemmgr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jmemmgr.obj"	"$(INTDIR)\jmemmgr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jmemnobs.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jmemnobs.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jmemnobs.obj"	"$(INTDIR)\jmemnobs.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jquant1.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jquant1.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jquant1.obj"	"$(INTDIR)\jquant1.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jquant2.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jquant2.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jquant2.obj"	"$(INTDIR)\jquant2.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\jutils.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\jutils.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\jutils.obj"	"$(INTDIR)\jutils.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\tools.c

!IF  "$(CFG)" == "jpeglib - Win32 Release"


"$(INTDIR)\tools.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "jpeglib - Win32 Debug"


"$(INTDIR)\tools.obj"	"$(INTDIR)\tools.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

