@echo off
setlocal enabledelayedexpansion

set curpath=%cd% 
set svn=%cd%\_subversion\bin\svn.exe

%svn% up
%svn% commit -m "update resource by excel config"

for /f "delims=" %%i in ('%svn% info ^| findstr "Revision"') do set svnversion=%%i
set svnversion=%svnversion:~10%

::计算时间 2019-04-01 开始
set /a starttime=1554048000

Echo WScript.Echo((new Date()).getTime())>sjc.vbs
for /f %%i in ('cscript -nologo -e:jscript sjc.vbs') do set sjc=%%i
del sjc.vbs
set nowtime=%sjc:~0,10%

set /a passtime=%nowtime%-%starttime%
set /a day=%passtime%/86400

set version=1.0.%day%.%svnversion%
echo %version%
set filename=chess_resource_%version%.tar.gz

::resource
::config
set outputpath=%cd%\_gcm\conf_output
rd /s /q %outputpath%
if not exist %outputpath%\config ( mkdir %outputpath%\config )
xcopy /y config\*.xml %outputpath%\config\
xcopy /y config\server\*.xml %outputpath%\config\
::script
if not exist %outputpath%\script ( mkdir %outputpath%\script )
xcopy /S /y script\* %outputpath%\script\

::pack
cd %cd%\_gcm\builder\
tar -zcvf %filename% ..\conf_output\*

certutil -hashfile %filename% MD5 >APP_MD5_TXT
set index=0
for /f "tokens=* delims= " %%i in (APP_MD5_TXT) do (
	if !index! == 1 (set md5=%%i)
 	set /a index=index+1
)
del APP_MD5_TXT

gcm_build.exe -p "chess" -s %svnversion% -b "develop" -c 1.1 -m "debug" -v %version% -n 1.2 -t 3 -fn %filename% -md5 %md5%
del %filename%

::reload
curl -d "{\"command\":\"resource\", \"value\":\"%filename%\", \"appname\":\"*\", \"apptype\":\"*\", \"appid\":\"*\", \"zoneid\":\"0\"}" http://192.168.2.31:9999/deploy