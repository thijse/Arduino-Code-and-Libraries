// *** DataLogging ***

// This example expands the previous SendandReceiveArguments example. 
// The Arduino will now wait for the StartLogging command, before sending analog data to the PC
// and sent multiple float values. 

#include <CmdMessenger.h>  // CmdMessenger

// Blinking led variables 
unsigned long previousToggleLed = 0;   // Last time the led was toggled
bool ledState                   = 0;   // Current state of Led
const int kBlinkLed             = 13;  // Pin of internal Led

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial);

// Thermocouple pins
int AnalogPin1           = 0;
int AnalogPin2           = 1;
bool acquireData         = false;
unsigned long sampleInterval       = 100; // 0.1 second, 10 Hz interval
unsigned long previousSampleMillis = 0;
long startAcqMillis       = 0;


// This is the list of recognized commands. These can be commands that can either be sent or received. 
// In order to receive, attach a callback function to these events
enum
{
  // Commands
  kAcknowledge         , // Command to acknowledge that cmd was received
  kError               , // Command to report errors
  kStartLogging        , // Command to request that the Arduino starts logging
  kPlotDataPoint       , // Command to request that the PC plots datapoints
};

// Commands we send from the PC and want to receive on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below.

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kStartLogging, OnStartLogging);
}

// ------------------  C A L L B A C K S -----------------------

// Called when a received command has no attached function
void OnUnknownCommand()
{
  cmdMessenger.sendCmd(kError,"Command without attached callback");
}

// Callback function that responds that Arduino is ready (has booted up)
void OnArduinoReady()
{
  cmdMessenger.sendCmd(kAcknowledge,"Arduino ready");
}

// Callback function calculates the sum of the two received float values
void OnStartLogging()
{
  // Start data acquisition
  startAcqMillis = millis();
  acquireData    = true;
  cmdMessenger.sendCmd(kAcknowledge,"Start Logging");
}

// ------------------ M A I N  ----------------------

// Setup function
void setup() 
{
  // Listen on serial connection for messages from the pc
  Serial.begin(115200); 

  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Send the status to the PC that says the Arduino has booted
  cmdMessenger.sendCmd(kAcknowledge,"Arduino has started!");

  // set pin for blink LED
  pinMode(kBlinkLed, OUTPUT);
}

// Returns if it has been more than interval (in ms) ago. Used for periodic actions
bool hasExpired(unsigned long &prevTime, unsigned long interval) {
  if (  millis() - prevTime > interval ) {
    prevTime = millis();
    return true;
  } else     
    return false;
}

// Loop function
void loop() 
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();
 
  // Do measurement after certain sample interval
  if (hasExpired(previousSampleMillis,sampleInterval)) 
  { 
    if (acquireData) {
      measure();
    }
  }
}

// simple readout of two Analog pins. 
void measure() {
   
   float seconds = (float) (millis()-startAcqMillis) /1000.0 ;
   float Analog1 = analogRead(AnalogPin1);
   float Analog2 = analogRead(AnalogPin2);   
   
   cmdMessenger.sendCmdStart(kPlotDataPoint); 
   cmdMessenger.sendCmdArg(seconds,4);   
   cmdMessenger.sendCmdArg(Analog1);   
   cmdMessenger.sendCmdArg(Analog2);   
   cmdMessenger.sendCmdEnd();
} 
