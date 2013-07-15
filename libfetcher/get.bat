
rem Messenger
wget -O ./temp/CmdMessenger.zip https://github.com/dreamcat4/CmdMessenger/zipball/master
7za.exe x ./temp/CmdMessenger.zip -o./temp
move ./temp/dreamcat4-CmdMessenger-* .\libs\CmdMessenger


rem DCF77
wget -P ./temp https://github.com/downloads/thijse/Arduino-Libraries/DCF77.0.9.6.tar.gz 
tartool ./temp/DCF77.0.9.6.tar.gz ./libs