# Microsoft Developer Studio Project File - Name="mjpeglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mjpeglib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mjpeglib.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mjpeglib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mjpeglib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mjpeglib - Win32 Release"
# Name "mjpeglib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\avilib.c
# End Source File
# Begin Source File

SOURCE=.\colorspace.c
# End Source File
# Begin Source File

SOURCE=.\editlist.c
# End Source File
# Begin Source File

SOURCE=.\jpegutils.c
# End Source File
# Begin Source File

SOURCE=.\lav_common.c
# End Source File
# Begin Source File

SOURCE=.\lav_io.c
# End Source File
# Begin Source File

SOURCE=.\mjpeg_logging.c
# End Source File
# Begin Source File

SOURCE=.\mpegconsts.c
# End Source File
# Begin Source File

SOURCE=.\subsample.c
# End Source File
# Begin Source File

SOURCE=.\tools.c
# End Source File
# Begin Source File

SOURCE=.\yuv4mpeg.c
# End Source File
# Begin Source File

SOURCE=.\yuv4mpeg_ratio.c
# End Source File
# Begin Source File

SOURCE=.\yuvscaler.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\avilib.h
# End Source File
# Begin Source File

SOURCE=.\colorspace.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\editlist.h
# End Source File
# Begin Source File

SOURCE=.\jpegutils.h
# End Source File
# Begin Source File

SOURCE=.\lav_common.h
# End Source File
# Begin Source File

SOURCE=.\lav_io.h
# End Source File
# Begin Source File

SOURCE=.\mjpeg_logging.h
# End Source File
# Begin Source File

SOURCE=.\mjpeg_types.h
# End Source File
# Begin Source File

SOURCE=.\mpegconsts.h
# End Source File
# Begin Source File

SOURCE=.\subsample.h
# End Source File
# Begin Source File

SOURCE=.\tools.h
# End Source File
# Begin Source File

SOURCE=.\yuv4mpeg.h
# End Source File
# Begin Source File

SOURCE=.\yuv4mpeg_intern.h
# End Source File
# Begin Source File

SOURCE=.\yuvscaler.hpp
# End Source File
# End Group
# End Target
# End Project
