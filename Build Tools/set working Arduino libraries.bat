echo off
pushd.
cd /d  %~dp0

set SOURCE=..\..\Arduino-Code-and-Libraries
set TARGET=C:\Users\nly96630\Documents\Arduino


echo Create symbolic link from 
echo    %SOURCE%
echo  to 
echo    %TARGET%

if exist "%TARGET%" rmdir "%TARGET%"
ln.exe  "%SOURCE%" "%TARGET%"
popd