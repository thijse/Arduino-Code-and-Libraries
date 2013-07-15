#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Shell.h>
#include <TimeUtils.h>
#include <MemIO.h>

int addressRamTime;
int addressStoreCounter;

// define TIME command
const char s_cmdTime[] PROGMEM = "TIME";
const char s_cmdTimeDesc[] PROGMEM = "Read or write the current time";
const char s_cmdTimeArgs[] PROGMEM = "[HH:MM:SS]";

// define DATE command
const char s_cmdDate[] PROGMEM = "DATE";
const char s_cmdDateDesc[] PROGMEM = "Read or write the current date";
const char s_cmdDateArgs[] PROGMEM = "[YYYYMMDD]";

// define START command
const char s_cmdStart[] PROGMEM = "START";
const char s_cmdStartDesc[] PROGMEM = "Start RTC clock";
const char s_cmdStartArgs[] PROGMEM = "";

// define STOP command
const char s_cmdStop[] PROGMEM = "STOP";
const char s_cmdStopDesc[] PROGMEM = "STOP RTC clock";
const char s_cmdStopArgs[] PROGMEM = "";


// define MEM command
const char s_cmdMem[] PROGMEM = "MEM";
const char s_cmdMemDesc[] PROGMEM = "Store current time in RTC Ram";
const char s_cmdMemArgs[] PROGMEM = "";

// define SETCLOCK command
const char s_cmdSetClock[] PROGMEM = "SETCLOCK";
const char s_cmdSetClockDesc[] PROGMEM = "Update internal clock with RTC time";
const char s_cmdSetClockArgs[] PROGMEM = "";

// define STATUS command
const char s_cmdStatus[] PROGMEM = "STATUS";
const char s_cmdStatusDesc[] PROGMEM = "Show device status";
const char s_cmdStatusArgs[] PROGMEM = "";


// List of all commands that are implemented.
const command_t commands[] PROGMEM = {
  {s_cmdTime, cmdTime, s_cmdTimeDesc, s_cmdTimeArgs},
  {s_cmdDate, cmdDate, s_cmdDateDesc, s_cmdDateArgs},
  {s_cmdStart, cmdStart, s_cmdStartDesc, s_cmdStartArgs},
  {s_cmdStop, cmdStop, s_cmdStopDesc, s_cmdStopArgs},
  {s_cmdMem, cmdMem, s_cmdMemDesc, s_cmdMemArgs},
  {s_cmdSetClock, cmdSetClock, s_cmdSetClockDesc, s_cmdSetClockArgs},  
  {s_cmdStatus, cmdStatus, s_cmdStatusDesc, s_cmdStatusArgs},
  {0, 0} // zero's indicate end of command list
};

// Set up of interactive shell
void setup() {
    Serial.begin(9600);
    // Set shell commands
    
    // Always get the adresses first and in the same order
    addressRamTime      = RTC.RAM->getAddress(sizeof(time_t));
    addressStoreCounter = RTC.RAM->getAddress(sizeof(uint16_t));
    
    ShellConsole.setCommands(commands);    
    //Set shell prompt
    ShellConsole.setPrompt(prompt);
    // Show help
    ShellConsole.showHelp();
}

// Process interactive shell
void loop() {
  // Process shell input
  ShellConsole.processInput();
}

// Implementation of prompt
void prompt()
{
  tmElements_t timeElements; 
  TimeUtils convert = TimeUtils(now());
  convert.printTime();
  Serial.print("> ");
}

// Define "TIME" command.
void cmdTime(const char *args)
{     
   // Read the current time & date from RTC
   TimeUtils convert = TimeUtils(RTC.get());
   
   // Update RTC time from console
    if (*args != '\0') {
        convert.parseTime(args);
        Serial.print("Time will be set to: ");
        convert.printTime();
        Serial.println();
        // set time & date to RTC
        RTC.set(convert.getTime());
    }

    // Read the current time.
    Serial.print("Current RTC date and time is: ");  
    printDateAndTime(RTC.get());
}

// Define "DATE" command.
void cmdDate(const char *args)
{       
   // Read the current time & date from RTC
   TimeUtils convert = TimeUtils(RTC.get());
   
    // Update date from console
    if (*args != '\0') {
        convert.parseDate(args);
        Serial.print("Date will be set to: ");
        convert.printDate();
        Serial.println();
        // set time & date to RTC
        RTC.set(convert.getTime());        
    }

    // Read the current date.
    Serial.print("Current RTC date and time is: ");
    printDateAndTime(RTC.get());
}

// Define "START" command.
void cmdStart(const char *args)
{
  Serial.print("RTC is started");
  RTC.startClock();
}

// Define "STOP" command.
void cmdStop(const char *args)
{
  Serial.print("RTC is stopped");
  RTC.stopClock();
}

// Define "MEM" command.
void cmdMem(const char *args)
{ 
    time_t  RamTime; 
     
   // Read the time & date from RTC RAM
   RTC.RAM->readBlock<time_t>(addressRamTime, RamTime);
   
   //Display time
   //TimeUtils convert = TimeUtils(RamTime);
   Serial.print("Last stored time: ");
   printDateAndTime(RamTime);    
   
   // Write the current time & date to RTC RAM
   Serial.print("Current time: ");   
   printDateAndTime(RTC.get());   
   RTC.RAM->writeBlock<time_t>(addressRamTime, RTC.get());
      
   // Read and show new time & date from RTC RAM
   Serial.print("New stored time: ");
   RTC.RAM->readBlock<time_t>(addressRamTime, RamTime);
   printDateAndTime(RamTime);
   
   //Update write counter
   uint16_t updateCounter;    
   RTC.RAM->readBlock<uint16_t>(addressStoreCounter, updateCounter);
   updateCounter++;
   Serial.print("Ram time was updated ");   
   Serial.print(updateCounter);
   Serial.println(" times");
   RTC.RAM->writeBlock<uint16_t>(addressStoreCounter, updateCounter);
}

// Define "SETCLOCK" command.
void cmdSetClock(const char *args)
{
   setTime(RTC.get());
   
   Serial.print("internal clock was updated "); 
   printDateAndTime(now()); 
}

// Define "STATUS" command.
void cmdStatus(const char *args)
{
  RTC.get(); // try and fetch time. This will search for chip
  if (RTC.chipPresent()) { 
       Serial.println("RTC clock was found"); 
  } else { 
       Serial.println("RTC clock was not found"); 
  }            
}

// convenience function to display date and time
void printDateAndTime(time_t time) 
{   
   TimeUtils convert = TimeUtils(time);  
   convert.printDate();
    Serial.print(" ");
    convert.printTime();
    Serial.println();
}
