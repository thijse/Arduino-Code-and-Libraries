// This example demonstrates CmdMessenger's callback & attach methods
// For Arduino Uno and Arduino Duemilanove board (may work with other)



#include <CmdMessenger.h>  // CmdMessenger library available from https://github.com/dreamcat4/cmdmessenger
//#include <Base64.h>      // Base64 library available from https://github.com/adamvr/arduino-base64
#include <Streaming.h>   // Streaming library available from http://arduiniana.org/libraries/streaming/

// Must not conflict / collide with our message payload data. Fine if we use base64 library ^^ above
char field_separator   = ',';
char command_separator = ';';
char escape_separator  = '/';

// Blinking led variables 
unsigned long previousToggleLed = 0;   // Last time the led was toggled
bool ledState                   = 0;   // Current state of Led
const int kBlinkLed             = 13;  // Pin of internal Led

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);


// ------------------ C M D  L I S T I N G ( T X / R X ) ---------------------

// We can define up to a default of 50 cmds total, including both directions (send + receive)
// and including also the first 4 default command codes for the generic error handling.
// If you run out of message slots, then just increase the value of MAXCALLBACKS in CmdMessenger.h

 // This is the list of recognized commands. These can be commands that can either be sent or received. 
 // In order to receive, attach a callback function to these events
 // 
 // Note that commands work both directions:
 // - All commands can be sent
 // - Commands that have callbacks attached can be received
 // 
 // This means that both sides should have an identical command list:
 // both sides can either send it or receive it (or even both)    
enum
{
  // Commands
  kCommError           , // Command reports serial port comm error (only works for some comm errors)
  kComment             , // Command to sent comment in argument
  kAcknowledge         , // Command to acknowledge that cmd was received
  kAreYouReady         , // Command to ask if other side is ready
  kError               , // Command to report errors
  kAskUsIfReady        , // Command to ask other side to ask if ready 
  kYouAreReady         , // Command to acknowledge that other is ready
  kFloatAddition       , // Command to request add two floats
  kFloatAdditionResult , // Command to report addition result
  kStringPing          ,
  kStringPong          ,
  kBinaryValuePing     , 
  kBinaryValuePong     , 
};

// Commands we send from the PC and want to receive on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below.


void attachCommandCallbacks()
{
  // Attach callback methods
  //cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kAreYouReady, OnArduinoReady);
  cmdMessenger.attach(kAskUsIfReady, OnAskUsIfReady);
  cmdMessenger.attach(kFloatAddition, OnFloatAddition);
  cmdMessenger.attach(kStringPing, OnStringPing);
  cmdMessenger.attach(kBinaryValuePing, OnBinaryValuePing);
}


// ------------------  C A L L B A C K S -----------------------

void OnArduinoReady()
{
  // In response to ping. We just send a throw-away Acknowledgment to say "i'm ready"
  cmdMessenger.sendCmd(kAcknowledge,"Arduino ready");
}

void OnUnknownCommand()
{
  // Default response for unknown commands and corrupt messages
  cmdMessenger.sendCmd(kError,"Unknown command");
  cmdMessenger.sendCmdStart(kYouAreReady);  
  cmdMessenger.sendCmdArg("Command without attached callback");    
  cmdMessenger.sendCmdArg(cmdMessenger.CommandID());    
  cmdMessenger.sendCmdEnd();
}

void OnAskUsIfReady()
{
  // The other side asks us to send kAreYouReady command, wait for
  //acknowledge
   int isAck = cmdMessenger.sendCmd(kAreYouReady, "Asking PC if ready", true, kAcknowledge,1000 );
  // Now we send back whether or not we got an acknowledgments  
  cmdMessenger.sendCmd(kYouAreReady,isAck?1:0);
}

void OnFloatAddition()
{
   float a = cmdMessenger.readFloatArg();
   float b = cmdMessenger.readFloatArg();
  // Now we send back the result of the addition
  cmdMessenger.sendCmd(kFloatAdditionResult,a + b);
}

void OnStringPing()
{
  // ReadString gives back string pointer to the argument
  // The string will only be valid as long as no new command has been read
  // To get a persisent string, use copyString()
   char * a = cmdMessenger.readStringArg();
   // Unescape can be done in place
   cmdMessenger.unescape(a);
   cmdMessenger.sendCmdStart(kStringPong);
   cmdMessenger.sendCmdEscArg(a);
   cmdMessenger.sendCmdEnd();
  // Now we send the same string
  //cmdMessenger.sendCmd(kStringPong, a);
}

void OnBinaryValuePing()
{
  // ReadString gives back string pointer to the argument

   float a = cmdMessenger.readBinArg<float>();
    cmdMessenger.sendBinCmd(kBinaryValuePong, a);
}

// ------------------ M A I N ( ) ----------------------



void setup() 
{
  // Listen on serial connection for messages from the pc
  //Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial

  // 115200 is the max speed on Arduino Uno, Mega, with AT8u2 USB
  // SERIAL_8N1 is the default config, but we want to make certain
  // that we have 8 bits to our disposal
  Serial.begin(115200,SERIAL_8N1); 

  // Makes output more readable whilst debugging in Arduino Serial Monitor, 
  // but uses more bytes 
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Set command to PC to say we're ready
  //OnArduinoReady();
  
  cmdMessenger.sendCmd(kAcknowledge,"Arduino has resetted!");

  // set pin for blink LED
  pinMode(kBlinkLed, OUTPUT);
}

bool hasExpired(unsigned long &prevTime, unsigned long interval) {
  if (  millis() - prevTime > interval ) {
    prevTime = millis();
    return true;
  } else     
    return false;
}

void loop() 
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();

  // toggle LED. If the LED does not toggle every timeoutInterval, 
  // this means the callbacks my the Messenger are taking a longer time than that  
  if (hasExpired(previousToggleLed,2000)) // Every 2 secs
  {
    toggleLed();  
  } 
}

// Toggle led state
void toggleLed()
{  
  ledState = !ledState;
  digitalWrite(kBlinkLed, ledState?HIGH:LOW);
}  
