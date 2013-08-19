

// This example demonstrates CmdMessenger's callback  & attach methods
// For Arduino Uno and Arduino Duemilanove board (may work with other)

#include <Adafruit_MAX31855.h>
#include <CmdMessenger.h>
#include <Base64.h>
#include <Streaming.h>

// Mustnt conflict / collide with our message payload data. Fine if we use base64 library ^^ above
char field_separator = ',';
char command_separator = ';';

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial, field_separator, command_separator);

// Thermocouple pins
int thermoDO = 3;
int thermoCS = 4;
int thermoCLK = 5;
bool acquireData = false;

long startAcqMillis;

Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

// ------------------ S E R I A L  M O N I T O R -----------------------------
// 


// ------------------ C M D  L I S T I N G ( T X / R X ) ---------------------

// Commands we send from the Arduino to be received on the PC
enum
{
  // Default commands
  kCOMM_ERROR      = 0, // Lets Arduino report serial port comm error back to the PC (only works for some comm errors)
  kACK             = 1, // Arduino acknowledges cmd was received
  kARE_YOU_READY   = 2, // After opening the comm port, send this cmd 02 from PC to check arduino is ready
  kERR             = 3, // Arduino reports badly formatted cmd, or cmd not recognised

  // Custom commands
  kACQ_STARTED     = 4, // Report that data acquisition has been started
  kACQ_STOPPED     = 5, // Report that data acquisition has been stopped
  kDATAPOINT       = 6, // Send new datapoint
  kACQ_ERR         = 7, // Report that data acquisition has been stopped

  // End command
  kSEND_CMDS_END,         // Mustn't delete this line
};

// Commands we send from the PC and want to recieve on the Arduino.
messengerCallbackFunction messengerCallbacks[] = 
{
  startAcq, // 8 Start Acquisition
  stopAcq,  // 9 Stop Acquisition
  NULL
};
// Its also possible (above ^^) to implement some symetric commands, when both the Arduino and
// PC / host are using each other's same command numbers. However we recommend only to do this if you
// really have the exact same messages going in both directions. Then specify the integers (with '=')


// ------------------ C A L L B A C K  M E T H O D S -------------------------

void startAcq()
{
  // Send back that data acquisition has started
  startAcqMillis= millis();
  acquireData = true;
  cmdMessenger.sendCmd(kACQ_STARTED, "Started acquisition");
}

void stopAcq()
{
  acquireData = false;
  cmdMessenger.sendCmd(kACQ_STOPPED, "Stopped acquisition");
  int isAck = cmdMessenger.sendCmd(kARE_YOU_READY, "Asking PC if ready", true, kACK,1000 );
  //if(isAck) { Serial << "Acknowledged!"; } else { Serial << "Not Acknowledged!"; } 
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

// ------------------ E N D  C A L L B A C K  M E T H O D S ------------------



// ------------------ S E T U P ----------------------------------------------

void attach_callbacks(messengerCallbackFunction* callbacks)
{
  int i = 0;
  int offset = kSEND_CMDS_END;
  while(callbacks[i])
  {
    cmdMessenger.attach(offset+i, callbacks[i]);
    i++;
  }
}

void setup() 
{
  // Listen on serial connection for messages from the pc
 Serial.begin(115200); // Arduino Uno, Mega, with AT8u2 USB
// Serial.begin(57600);  // Arduino Duemilanove, FTDI Serial
//Serial.begin(9600); 
  // cmdMessenger.discard_LF_CR(); // Useful if your terminal appends CR/LF, and you wish to remove them
  cmdMessenger.print_LF_CR();   // Make output more readable whilst debugging in Arduino Serial Monitor
  
  // Attach default / generic callback methods
  cmdMessenger.attach(kARE_YOU_READY, arduino_ready);
  cmdMessenger.attach(unknownCmd);

  // Attach my application's user-defined callback methods
  attach_callbacks(messengerCallbacks);

  // wait for MAX chip to stabilize
  delay(500);

  // Send command that Arduino is ready
  arduino_ready();

  // blink
  pinMode(13, OUTPUT);
}


// ------------------ M A I N ( ) --------------------------------------------


long sampleInterval = 10; // 1 second
long previousSampleMillis = 0;


 

void loop() 
{
  // Process incoming serial data, if any
  cmdMessenger.feedinSerialData();


  // Do measurement if needed
 //Serial.println("measure");
  if (  millis() - previousSampleMillis > sampleInterval )
  { 
    if (acquireData) {
      measure();
    }
    previousSampleMillis = millis();
  }

  // Loop.
}

void measure() {
  // basic readout test, just print the current temp

   float i = thermocouple.readInternal();
   float c = thermocouple.readCelsius();
  // unsigned long    t = millis();
   float seconds = (float) (millis()-startAcqMillis) /1000.0 ;
   
   
   if (isnan(c)) {
     cmdMessenger.sendCmd(kACQ_ERR,"Something wrong with thermocouple!");
   } else {
   cmdMessenger.sendCmdStart(kDATAPOINT);
   //cmdMessenger.sendCmdfArg("%lu,%lu",t,t);   
   cmdMessenger.sendCmdArg(seconds,4);   
   cmdMessenger.sendCmdArg(i);   
   cmdMessenger.sendCmdArg(c);   
   cmdMessenger.sendCmdEnd();
   }
 
//   delay(100);
}

