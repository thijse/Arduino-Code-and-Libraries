@echo off

set version=3.0

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
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\CommandMessenger\bin /s /q
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\CommandMessenger\obj /s /q
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\Receive\bin /s /q 
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\Receive\obj /s /q 
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\SendAndReceive\bin /s /q 
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\SendAndReceive\obj /s /q 
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\SendAndReceiveArguments\bin /s /q 
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\SendAndReceiveArguments\obj /s /q 
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\SendAndReceiveBinaryArguments\bin /s /q 
rmdir  ..\..\Arduino-Libraries\CmdMessenger\CSharp\SendAndReceiveBinaryArguments\obj /s /q 

del  ..\..\Arduino-Libraries\CmdMessenger\Arduino wiki.txt

Echo create zipfiles



7za.exe a -tzip "..\..\Arduino-Libraries\zippedlibs\CmdMessenger %version%.zip" "..\..\Arduino-Libraries\CmdMessenger"
7za.exe a -ttar "..\..\Arduino-Libraries\zippedlibs\CmdMessenger" "..\..\Arduino-Libraries\CmdMessenger"
7za.exe a -tgzip "..\..\Arduino-Libraries\zippedlibs\CmdMessenger %version%.tar.gz" "..\..\Arduino-Libraries\zippedlibs\CmdMessenger.tar"
del "..\..\Arduino-Libraries\zippedlibs\CmdMessenger.tar"
