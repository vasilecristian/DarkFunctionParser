@echo off 
SET MAKETOOL=..\..\premake\release\premake5.exe

rem %MAKETOOL% --to=vs2012 vs2012
%MAKETOOL% --to=vs2013 vs2013

rem %MAKETOOL% --arch=win8app --to=vs2013_w8 vs2013
rem %MAKETOOL% --arch=macosx --to=xcode4\macosx xcode4
%MAKETOOL% --arch=ios --to=xcode4\ios xcode4

%MAKETOOL% --to=android_s2g android_s2g

%MAKETOOL% --arch=win8app --to=vs2013_w8 vs2013
