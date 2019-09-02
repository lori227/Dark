@echo off
setlocal enabledelayedexpansion

set svn=%cd%\..\subversion\bin\svn.exe
%svn% up

call _build_server.bat

cd ../
call updateserverconfig.bat
pause