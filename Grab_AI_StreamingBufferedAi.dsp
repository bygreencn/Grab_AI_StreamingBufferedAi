# Microsoft Developer Studio Project File - Name="Grab_AI_StreamingBufferedAi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Grab_AI_StreamingBufferedAi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Grab_AI_StreamingBufferedAi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Grab_AI_StreamingBufferedAi.mak" CFG="Grab_AI_StreamingBufferedAi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Grab_AI_StreamingBufferedAi - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Grab_AI_StreamingBufferedAi - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Grab_AI_StreamingBufferedAi - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"../bin/release/Grab_AI_StreamingBufferedAi.exe"

!ELSEIF  "$(CFG)" == "Grab_AI_StreamingBufferedAi - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"../bin/debug/Grab_AI_StreamingBufferedAi.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Grab_AI_StreamingBufferedAi - Win32 Release"
# Name "Grab_AI_StreamingBufferedAi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConfigurationDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SimpleGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\StreamingBufferedAi.cpp
# End Source File
# Begin Source File

SOURCE=.\StreamingBufferedAi.rc
# End Source File
# Begin Source File

SOURCE=.\StreamingBufferedAiDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XFileOpenListView.cpp
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XFolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XHistoryCombo.cpp
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XHyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XWinVer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConfigurationDlg.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SimpleGraph.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\StreamingBufferedAi.h
# End Source File
# Begin Source File

SOURCE=.\StreamingBufferedAiDlg.h
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XFileOpenListView.h
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XFolderDialog.h
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XFolderDialogRes.h
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XHistoryCombo.h
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XHyperLink.h
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XTrace.h
# End Source File
# Begin Source File

SOURCE=.\XFolderDialog\XWinVer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\background.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmapList.bmp
# End Source File
# Begin Source File

SOURCE=.\res\StreamingBufferedAi.ico
# End Source File
# End Group
# End Target
# End Project
# Section Grab_AI_StreamingBufferedAi : {6AC557F8-E2F6-4075-A0BC-8789ACF7D054}
# 	1:14:IDD_COM_SELECT:104
# 	2:16:Resource Include:resource.h
# 	2:14:ComSelectDlg.h:ComSelectDlg.h
# 	2:14:IDD_COM_SELECT:IDD_COM_SELECT
# 	2:10:ENUM: enum:enum
# 	2:20:CLASS: CComSelectDlg:CComSelectDlg
# 	2:19:Application Include:Grab_AI_StreamingBufferedAi.h
# 	2:16:ComSelectDlg.cpp:ComSelectDlg.cpp
# End Section
# Section Grab_AI_StreamingBufferedAi : {969DA674-B9DE-4437-9FE2-752D4BD37B2E}
# 	2:17:SheetSettings.cpp:SheetSettings.cpp
# 	2:15:SheetSettings.h:SheetSettings.h
# 	2:21:CLASS: CSheetSettings:CSheetSettings
# 	2:19:Application Include:Grab_AI_StreamingBufferedAi.h
# End Section
# Section Grab_AI_StreamingBufferedAi : {95A7794D-166C-44C4-A831-244B318B2C91}
# 	1:14:IDD_COM_EVENTS:105
# 	2:16:Resource Include:resource.h
# 	2:10:ENUM: enum:enum
# 	2:21:CLASS: CPageComEvents:CPageComEvents
# 	2:15:PageComEvents.h:PageComEvents.h
# 	2:19:Application Include:Grab_AI_StreamingBufferedAi.h
# 	2:17:PageComEvents.cpp:PageComEvents.cpp
# 	2:14:IDD_COM_EVENTS:IDD_COM_EVENTS
# End Section
# Section Grab_AI_StreamingBufferedAi : {4A5F0A87-03BA-479C-BF29-0B189BEA9F57}
# 	1:16:IDD_COM_SETTINGS:106
# 	2:16:Resource Include:resource.h
# 	2:23:CLASS: CPageComSettings:CPageComSettings
# 	2:16:IDD_COM_SETTINGS:IDD_COM_SETTINGS
# 	2:19:PageComSettings.cpp:PageComSettings.cpp
# 	2:17:PageComSettings.h:PageComSettings.h
# 	2:10:ENUM: enum:enum
# 	2:19:Application Include:Grab_AI_StreamingBufferedAi.h
# End Section
