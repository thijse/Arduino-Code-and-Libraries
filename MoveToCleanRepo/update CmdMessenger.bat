@echo off

set version=3.1

echo copy library

echo Update documentation

rmdir  ..\..\Arduino-Libraries\CmdMessenger\Documentation /s /q
mkdir ..\..\Arduino-Libraries\CmdMessenger\Documentation\PC
mkdir ..\..\Arduino-Libraries\CmdMessenger\Documentation\Arduino
doxygen "CmdMessenger Arduino.doxy"
doxygen "CmdMessenger PC.doxy"

echo Copy files

rmdir ..\..\Arduino-Libraries\CmdMessenger /s /q
mkdir ..\..\Arduino-Libraries\CmdMessenger
xcopy ..\..\Arduino-Code-and-Libraries\Libraries\CmdMessenger ..\..\Arduino-Libraries\CmdMessenger /E /Y /C
xcopy ..\..\Arduino-Code-and-Libraries\Libraries\CmdMessenger\CSharp\* ..\..\Arduino-Libraries\CmdMessenger\CSharp\ /E /Y

Echo Clean up copy
del /s ..\..\Arduino-Libraries\CmdMessenger\*.bak
for /D %%f in (..\..\Arduino-Libraries\CmdMessenger\CSharp\*) do (
    rmdir  %%f\bin /s /q
    rmdir  %%f\obj /s /q
)
rmdir ..\..\Arduino-Libraries\CmdMessenger\local /s /q 

Echo create zipfiles

del "..\..\Arduino-Libraries\zippedlibs\CmdMessenger %version%.zip"
del "..\..\Arduino-Libraries\zippedlibs\CmdMessenger %version%.tar.gz"

7za.exe a -tzip "..\..\Arduino-Libraries\zippedlibs\CmdMessenger %version%.zip" "..\..\Arduino-Libraries\CmdMessenger"
7za.exe a -ttar "..\..\Arduino-Libraries\zippedlibs\CmdMessenger" "..\..\Arduino-Libraries\CmdMessenger"
7za.exe a -tgzip "..\..\Arduino-Libraries\zippedlibs\CmdMessenger %version%.tar.gz" "..\..\Arduino-Libraries\zippedlibs\CmdMessenger.tar"
del "..\..\Arduino-Libraries\zippedlibs\CmdMessenger.tar"

pause