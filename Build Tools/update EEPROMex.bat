rem @echo off

set version=9.1
set library=EEPROMEx

echo copy library

echo Update documentation

rmdir  ..\..\Arduino-Libraries\%library%\Documentation /s /q
mkdir ..\..\Arduino-Libraries\%library%\Documentation
doxygen "%library%.doxy"

echo Copy files

echo Copy %library%
rmdir ..\..\Arduino-Libraries\%library% /s /q
mkdir ..\..\Arduino-Libraries\%library%
xcopy ..\..\Arduino-Code-and-Libraries\Libraries\%library% ..\..\Arduino-Libraries\%library% /E /Y /C

Echo Clean up copy %library%
del /s ..\..\Arduino-Libraries\%library%\*.bak

Echo create zipfiles

del "..\..\Arduino-Libraries\Packaged Libraries\%library% %version%.zip"
del "..\..\Arduino-Libraries\Packaged Libraries\%library% %version%.tar.gz"

Echo create zip file
7za.exe a -tzip "..\..\Arduino-Libraries\Packaged Libraries\%library% %version%.zip" "..\..\Arduino-Libraries\%library%"

Echo create tar file
7za.exe a -ttar "..\..\Arduino-Libraries\Packaged Libraries\%library%" "..\..\Arduino-Libraries\%library%"
7za.exe a -tgzip "..\..\Arduino-Libraries\Packaged Libraries\%library% %version%.tar.gz" "..\..\Arduino-Libraries\Packaged Libraries\%library%.tar"
del "..\..\Arduino-Libraries\Packaged Libraries\%library%.tar"

echo Create Arduino working library only for %library%

rmdir "..\..\Arduino-Libraries\Packaged Libraries\%library%" /s /q 
mkdir "..\..\Arduino-Libraries\Packaged Libraries\%library%" 
mkdir "..\..\Arduino-Libraries\Packaged Libraries\%library%\Libraries" 

7za.exe x -y -o"..\..\Arduino-Libraries\Packaged Libraries\%library%\Libraries" "..\..\Arduino-Libraries\Packaged Libraries\%library% %version%.zip" 

pause