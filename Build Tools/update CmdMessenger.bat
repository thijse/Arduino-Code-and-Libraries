@echo off

set version=3.3

echo copy library

echo Update documentation

rmdir  ..\..\Arduino-Libraries\CmdMessenger\Documentation /s /q
mkdir ..\..\Arduino-Libraries\CmdMessenger\Documentation\PC
mkdir ..\..\Arduino-Libraries\CmdMessenger\Documentation\Arduino
doxygen "CmdMessenger Arduino.doxy"
doxygen "CmdMessenger PC.doxy"

echo Copy files

echo Command Messenger
rmdir ..\..\Arduino-Libraries\CmdMessenger /s /q
mkdir ..\..\Arduino-Libraries\CmdMessenger
xcopy ..\..\Arduino-Code-and-Libraries\Libraries\CmdMessenger ..\..\Arduino-Libraries\CmdMessenger /E /Y /C
xcopy ..\..\Arduino-Code-and-Libraries\Libraries\CmdMessenger\CSharp\* ..\..\Arduino-Libraries\CmdMessenger\CSharp\ /E /Y

rem echo Copy PID library  PID_v1
rem rmdir ..\..\Arduino-Libraries\PID_v1 /s /q
rem mkdir ..\..\Arduino-Libraries\PID_v1
rem xcopy ..\..\Arduino-Code-and-Libraries\Libraries\PID_v1 ..\..\Arduino-Libraries\PID_v1 /E /Y /C

rem echo Copy Thermocouple library Adafruit_MAX31855
rem rmdir ..\..\Arduino-Libraries\Adafruit_MAX31855 /s /q
rem mkdir ..\..\Arduino-Libraries\Adafruit_MAX31855
rem xcopy ..\..\Arduino-Code-and-Libraries\Libraries\Adafruit_MAX31855 ..\..\Arduino-Libraries\Adafruit_MAX31855 /E /Y /C

echo Copy Temperature controller library
rmdir ..\..\Arduino-Libraries\TemperatureController /s /q
mkdir ..\..\Arduino-Libraries\TemperatureController
xcopy ..\..\Arduino-Code-and-Libraries\Libraries\TemperatureController ..\..\Arduino-Libraries\TemperatureController /E /Y /C

Echo Clean up copy Command Messenger
del /s ..\..\Arduino-Libraries\CmdMessenger\*.bak
for /D %%f in (..\..\Arduino-Libraries\CmdMessenger\CSharp\*) do (
    rmdir  %%f\bin /s /q
    rmdir  %%f\obj /s /q
)
rmdir ..\..\Arduino-Libraries\CmdMessenger\local /s /q 

Echo create zipfiles

del "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.zip"
del "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.tar.gz"

7za.exe a -tzip "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.zip" "..\..\Arduino-Libraries\CmdMessenger"
7za.exe a -tzip "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.zip" "..\..\Arduino-Libraries\TemperatureController"
7za.exe a -tzip "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.zip" "..\..\Arduino-Code-and-Libraries\Adafruit_MAX31855"
7za.exe a -tzip "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.zip" "..\..\Arduino-Code-and-Libraries\PID_v1"

7za.exe a -ttar "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger" "..\..\Arduino-Libraries\CmdMessenger"
7za.exe a -ttar "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger" "..\..\Arduino-Libraries\TemperatureController"
7za.exe a -ttar "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger" "..\..\Arduino-Code-and-Libraries\Adafruit_MAX31855"
7za.exe a -ttar "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger" "..\..\Arduino-Code-and-Libraries\PID_v1"

7za.exe a -tgzip "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.tar.gz" "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger.tar"
del "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger.tar"

echo Create Arduino working library only for CmdMessenger

rmdir "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger" /s /q 
mkdir "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger" 
mkdir "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger\Libraries" 

7za.exe x -y -o"..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger\Libraries" "..\..\Arduino-Libraries\Packaged Libraries\CmdMessenger %version%.zip" 

pause