// This example demonstrates CmdMessenger's callback  & attach methods
// For Arduino Uno and Arduino Duemilanove board (may work with other)

// Download these into your Sketches/libraries/ folder...

// CmdMessenger library available from https://github.com/dreamcat4/cmdmessenger
#include <CmdMessenger.h>

// Base64 library available from https://github.com/adamvr/arduino-base64
#include <Base64.h>

// Streaming library available from http://arduiniana.org/libraries/streaming/
#include <Streaming.h>

// Mustnt conflict / collide with our message payload data. Fine if we use base64 library ^^ above
char field_separator = ',';
char command_separator = ';';

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);


// ------------------ S E R I A L  M O N I T O R -----------------------------



// ------------------ C M D  L I S T I N G ( T X / R X ) ---------------------

// We can define up to a default of 50 cmds total, including both directions (send + recieve)
// and including also the first 4 default command codes for the generic error handling.
// If you run out of message slots, then just increase the value of MAXCALLBACKS in CmdMessenger.h

    // This is the list of recognized commands. These can be commands that can either be sent or received. 
    // In order to receive, attach a callback function to these events
    // 
    // Default commands
    // Note that commands work both directions:
    // - All commands can be sent
    // - Commands that have callbacks attached can be received
    // 
    // This means that both sides should have an identical command list:
    // one side can either send it or receive it (sometimes both)
    // If an command exists on only side 1 or 2, it means can either be send 
    // by 1 but not received by 2, or vice versa.  As such it can be removed
    
enum
{
  // Default commands
  kCOMM_ERROR      , // reports serial port comm error (only works for some comm errors)
  kComment         ,  // Comment, see arguments
  kACK             , // acknowledgment that cmd was received
  kARE_YOU_READY   , // Command asking if other side is ready
  kERR             , // Reports incorrectly formatted cmd, or cmd not recognised
  // Custom commands
  AskUsIfReady     ,
  YouAreReady      ,
  // End command
  kSEND_CMDS_END,         // Mustn't delete this line
};

// Commands we send from the PC and want to recieve on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below vv.


void attachCommandCallbacks()
{
  // Attach default / generic callback methods
  cmdMessenger.attach(kARE_YOU_READY, arduino_ready);
  cmdMessenger.attach(unknownCmd);
  cmdMessenger.attach(AskUsIfReady, on_AskUsIfReady);
}

// ------------------ C A L L B A C K  M E T H O D S -------------------------

void startAcq()
{
  // Message data is any ASCII bytes (0-255 value). But can't contain the field
  // separator, command separator chars you decide (eg ',' and ';')
  cmdMessenger.sendCmd(kACK,"startAcq cmd recieved");
  while ( cmdMessenger.available() )
  {
    char buf[350] = { '\0' };
    cmdMessenger.copyString(buf, 350);
    if(buf[0])
      cmdMessenger.sendCmd(kACK, buf);
  }
}



// ------------------ D E F A U L T  C A L L B A C K S -----------------------

void arduino_ready()
{
  // In response to ping. We just send a throw-away Acknowledgement to say "im alive"
  cmdMessenger.sendCmd(kACK,"Arduino ready");
}

void unknownCmd()
{
  // Default response for unknown commands and corrupt messages
  cmdMessenger.sendCmd(kERR,"Unknown command");
}

void on_AskUsIfReady()
{
  // The other side asks us to send kARE_YOU_READY command, wait for
  //acknowledge
   int isAck = cmdMessenger.sendCmd(kARE_YOU_READY, "Asking PC if ready", true, kACK,5000 );
  // Now we send back whether or not we got an acknowlegdement  
  cmdMessenger.sendCmd(YouAreReady,isAck);
  //cmdMessenger.sendCmdStart(YouAreReady);  
  //cmdMessenger.sendCmdArg(isAck);    
  //cmdMessenger.sendCmdEnd();

}


// ------------------ E N D  C A L L B A C K  M E T H O D S ------------------



// ------------------ S E T U P ----------------------------------------------





void setup() 
{
  // Listen on serial connection for messages from the pc
  //Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial
  Serial.begin(115200); // Arduino Uno, Mega, with AT8u2 USB

  // cmdMessenger.discard_LF_CR(); // Useful if your terminal appends CR/LF, and you wish to remove them
  cmdMessenger.print_LF_CR();   // Make output more readable whilst debugging in Arduino Serial Monitor
  


  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  arduino_ready();

  // blink
  pinMode(13, OUTPUT);
}


// ------------------ M A I N ( ) --------------------------------------------

// Timeout handling
long timeoutInterval = 2000; // 2 seconds
long previousMillis = 0;
int counter = 0;

void timeout()
{
  // blink
  if (counter % 2)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);
  counter ++;
}  

void loop() 
{
  // Process incoming serial data, if any
  cmdMessenger.feedinSerialData();

  // handle timeout function, if any
  if (  millis() - previousMillis > timeoutInterval )
  {
    timeout();
    previousMillis = millis();
  }

  // Loop.
}
