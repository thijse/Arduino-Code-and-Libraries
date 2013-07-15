
#include <Wire.h>
#include <Time.h>
#include <Shell.h>
#include <TimeUtils.h>
#include <MemIO.h>
#include <DS1307RTC.h>
#include <DCF77.h>
#include <RTCDCF77.h>

#define DCF_PIN 2	         // Connection pin to DCF 77 device
#define DCF_INTERRUPT 0		 // Interrupt number associated with pin

DCF77 DCF = DCF77(DCF_PIN,DCF_INTERRUPT,false);

// define STATUS command
const char s_cmdStatus[] PROGMEM = "STATUS";
const char s_cmdStatusDesc[] PROGMEM = "status";
const char s_cmdStatusArgs[] PROGMEM = "none";

// define TIME command
const char s_cmdTime[] PROGMEM = "TIME";
const char s_cmdTimeDesc[] PROGMEM = "Read time";
const char s_cmdTimeArgs[] PROGMEM = "[HH:MM:SS]";

// define RESET command
const char s_cmdReset[] PROGMEM = "RESET";
const char s_cmdResetDesc[] PROGMEM = "Reset status";
const char s_cmdResetArgs[] PROGMEM = "none";

// define TEST command
const char s_cmdSet[] PROGMEM = "SET";
const char s_cmdSetDesc[] PROGMEM = "Set";
const char s_cmdSetArgs[] PROGMEM = "none";


// List of all commands that are implemented.
const command_t commands[] PROGMEM = {
  {s_cmdTime, cmdTime, s_cmdTimeDesc, s_cmdTimeArgs},
  {s_cmdSet, cmdSet, s_cmdSetDesc, s_cmdSetArgs},
  {s_cmdStatus, cmdStatus, s_cmdStatusDesc, s_cmdStatusArgs},
  {s_cmdReset, cmdReset, s_cmdResetDesc, s_cmdResetArgs},
  {0, 0} // zero's indicate end of command list
};

time_t startTime;
TimeUtils* convertTime = new TimeUtils();  
RTCDCF77 *RTCDCF;

// Set up of interactive shell
void setup() {
    Serial.begin(9600);
    RTCDCF = new RTCDCF77(&RTC, &DCF); // create an instance for the user
    
    // Set shell commands        
    ShellConsole.setCommands(commands);    
    //Set shell prompt
    ShellConsole.setPrompt(prompt);
    // Show help
    ShellConsole.showHelp();
    
    delay(500);
    // set initial time
    DCF.Start();
}

// Process interactive shell
void loop() {
  // Process shell input
  delay(100);
  ShellConsole.processInput();
}

// Implementation of prompt
void prompt()
{
  tmElements_t timeElements; 
  convertTime->setTime(now());
  convertTime->printTime();
  Serial.print("> ");
}

// Define "TIME" command.
void cmdTime(const char *args)
{     
    DCF.Stop();
    delay(100);
    // Read the current time.
    Serial.print("Curr RTCDCF: ");  
    printDateTime(RTCDCF->get());
    Serial.print("Corr RTCDCF: ");  
    printDateTime(RTCDCF->get());
     DCF.Start();       
    delay(100);
}

// Define "Set" command.
void cmdSet(const char *args)
{    
    Serial.println("Setting DCF time");      
    DCF.Stop();
    delay(100);
    if (RTCDCF->update())
    {
      Serial.println("Succesful update of DCF time");
    } else {
      Serial.println("Unsuccesful update of DCF time");
    }
    DCF.Start();       
    delay(100);
}

// Define "RESET" command.
void cmdReset(const char *args)
{
  RTCDCF->reset(); 
  Serial.println("Reset drift values ");  
}

// Define "STATUS" command.
void cmdStatus(const char *args)
{
  DCF.Stop();
  delay(100);
  
//  tmElements_t timeElements; 
//  timeElements.Day = 180;
  
 // time_t maxTime = makeTime(timeElements)
 // Serial.println("Getting Status");
  
  
  Stat status = RTCDCF->getStatus();
  Serial.println("Getting Status");

  Serial.print("Previous ref time ");
  printDateTime(status.previousRefTime);
  
  Serial.print("Summed time drift (s)");
  Serial.println(status.drift);
	  
  Serial.print("Summed time interval (s)");
  Serial.println(status.interval);

  Serial.print("Number of time updates ");  
  Serial.println(status.updates);
  
  // Calculate corrections to be used
  double correctionFactor = (double)status.drift/(double)status.interval;
  double secondOrder      = 1.0 / (1.0-sq(correctionFactor));

  Serial.print("Drift correction factor ");
  Serial.println(correctionFactor,5);
  
  Serial.print("DCFRTC uncorrected time ");
  printDateTime(RTCDCF->getUncorrected());
  
  Serial.print("DCFRTC corrected time ");  
  printDateTime(RTCDCF->get());
  Serial.print("DCF  time ");  
  DCF.fetchTimeUpdate();
  printDateTime(DCF.getTime());
  DCF.Start();       
  delay(100);  
}


// Print time
void printDateTime(time_t time) {
    TimeElements _timeElements;
    breakTime(time, _timeElements); 
    Serial.print(_timeElements.Hour);
    Serial.print(':');
    Serial.print(_timeElements.Minute);
    Serial.print(':');
    Serial.print(_timeElements.Second);
    Serial.print(' ');
    Serial.print(_timeElements.Day, DEC);
    Serial.print('-');
    Serial.print(_timeElements.Month, DEC);
    Serial.print('-');
    Serial.println(_timeElements.Year+1970, DEC);
}
