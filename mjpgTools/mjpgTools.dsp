# Microsoft Developer Studio Project File - Name="mjpgTools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mjpgTools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mjpgTools.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mjpgTools - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\jpeglib\release\jpeglib.lib .\mjpeglib\release\mjpeglib.lib .\tools\release\tools.lib .\mpg2enc\release\mpg2enc.lib .\mplex\release\mplex.lib .\STILLpatcher\release\STILLpatcher.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "mjpgTools - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MJPGTOOLS_EXPORTS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib .\jpeglib\debug\jpeglib.lib .\mjpeglib\debug\mjpeglib.lib .\tools\debug\tools.lib .\mpg2enc\debug\mpg2enc.lib .\mplex\Debug\mplex.lib .\STILLpatcher\Debug\STILLpatcher.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"LIBCD.lib" /pdbtype:sept /verbose:lib
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "mjpgTools - Win32 Release"
# Name "mjpgTools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_yuvscaler.cpp
# End Source File
# Begin Source File

SOURCE=.\cjpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\djpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg2yuv.cpp
# End Source File
# Begin Source File

SOURCE=.\jpegInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\mjpgTools.cpp
# End Source File
# Begin Source File

SOURCE=.\mjpgTools.def
# End Source File
# Begin Source File

SOURCE=.\mjpgTools.rc
# End Source File
# Begin Source File

SOURCE=.\Mpg2enc.cpp
# End Source File
# Begin Source File

SOURCE=.\Mplex.cpp
# End Source File
# Begin Source File

SOURCE=.\ppmtoyuv.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StillPatcher.cpp
# End Source File
# Begin Source File

SOURCE=.\yuvtoppm.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_yuvscaler.h
# End Source File
# Begin Source File

SOURCE=.\cjpeg.h
# End Source File
# Begin Source File

SOURCE=.\djpeg.h
# End Source File
# Begin Source File

SOURCE=.\jpeg2yuv.h
# End Source File
# Begin Source File

SOURCE=.\jpegInfo.h
# End Source File
# Begin Source File

SOURCE=.\mjpgTools.h
# End Source File
# Begin Source File

SOURCE=.\Mpg2enc.h
# End Source File
# Begin Source File

SOURCE=.\Mplex.h
# End Source File
# Begin Source File

SOURCE=.\ppmtoyuv.h
# End Source File
# Begin Source File

SOURCE=.\privateMessages.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\StillPatcher.h
# End Source File
# Begin Source File

SOURCE=.\yuvtoppm.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
