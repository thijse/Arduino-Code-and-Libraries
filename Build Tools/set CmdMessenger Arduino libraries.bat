echo off

set SOURCE=%~dp0\..\Arduino-Libraries\zippedlibs\CmdMessenger
set TARGET=C:\Users\nly96630\Documents\Arduino


echo Create symbolic link from 
echo    %SOURCE%
echo  to 
echo    %TARGET%

if exist "%TARGET%" rmdir "%TARGET%"
"%~dp0\ln.exe"  "%SOURCE%" "%TARGET%"