// *** TemperatureControl ***

// This example expands the previous SendandReceiveArguments example. 
// The Arduino will now wait for the StartLogging command, before sending analog data to the PC
// and sent multiple float values. 

#include <Adafruit_MAX31855.h>
#include <CmdMessenger.h>  
#include <PID_v1.h>
#include <DoEvery.h>   

// Attach a new CmdMessenger object to the default Serial port
CmdMessenger cmdMessenger = CmdMessenger(Serial);

const int heaterPwmInterval              = 3000;
const int measureInterval                = 100;

DoEvery tempTimer(measureInterval);
DoEvery pidTimer(heaterPwmInterval);

// Initial PID settings
double pidP                        = 1500;
double pidI                        = 25;
double pidD                        = 0;

// Thermocouple pins
int thermoDO                       = 3;
int thermoCS                       = 4;
int thermoCLK                      = 5;

// Solid State switch pin
const int switchPin                = 4;

bool acquireData                   = false;
bool controlHeater 				   = false;

long startAcqMillis                = 0;

double CurrentTemperature          = 20; 
double goalTemperature             = 20;

bool switchState                   = false;

Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);

double output;
PID pid(&CurrentTemperature, &output, &goalTemperature,pidP,pidI,pidD,DIRECT);


// This is the list of recognized commands. These can be commands that can either be sent or received. 
// In order to receive, attach a callback function to these events
enum
{
  // Commands
  kAcknowledge         , // Command to acknowledge that cmd was received
  kError               , // Command to report errors
  kStartLogging        , // Command to request logging start      (typically PC -> Arduino)
  kPlotDataPoint       , // Command to request datapoint plotting (typically Arduino -> PC)
  kSetGoalTemperature  ,
};

// Commands we send from the PC and want to receive on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below.

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(OnUnknownCommand);
  cmdMessenger.attach(kStartLogging, OnStartLogging);
  cmdMessenger.attach(kSetGoalTemperature, OnSetGoalTemperature);
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

// Callback function that sets leds blinking frequency
void OnSetGoalTemperature()
{
  // Read led state argument, interpret string as boolean
  goalTemperature = cmdMessenger.readFloatArg();  
  controlHeater = true;
  //cmdMessenger.sendCmd(kAcknowledge,goalTemperature);
  cmdMessenger.sendCmdStart(kAcknowledge); 
  cmdMessenger.sendCmdArg("*****"); 
  cmdMessenger.sendCmdArg(goalTemperature,5); 
  cmdMessenger.sendCmdArg("******"); 
  cmdMessenger.sendCmdEnd();
}

// ------------------ M A I N  ----------------------

// Setup function
void setup() 
{
  // Listen on serial connection for messages from the pc
  Serial.begin(115200); 
  
  //initialize  timers
  tempTimer.reset();
  pidTimer.reset();

  //initialize the PID variables
  pid.SetOutputLimits(0,heaterPwmInterval);

  CurrentTemperature= thermocouple.readCelsius();
  //setpoint=thermocouple.readCelsius();

  //prepare PID port for writing
  pinMode(switchPin, OUTPUT);  

  //turn the PID on
  pid.SetMode(AUTOMATIC);

  // Set pid sample time to the measure interval
  pid.SetSampleTime(measureInterval);

  // Adds newline to every command
  cmdMessenger.printLfCr();   

  // Attach my application's user-defined callback methods
  attachCommandCallbacks();

  // Send the status to the PC that says the Arduino has booted
  cmdMessenger.sendCmd(kAcknowledge,"Arduino has started!");
}

// Loop function
void loop() 
{
  // Process incoming serial data, and perform callbacks
  cmdMessenger.feedinSerialData();
 
  //every 100 ms, update the temperature
  if(tempTimer.check()) measure();
 
  // Update pidTimer
  pidTimer.check();
  
 if (controlHeater) {
	  //compute new PID parameters  
	  pid.Compute();
	 
	  //output on pin PID_OUT based on PID output and timer
	  heaterPWM();
 }
}

// simple readout of two Analog pins. 
void measure() {
   
   float seconds             = (float) (millis()-startAcqMillis) /1000.0 ;
   //float internalTemperature = thermocouple.readInternal();
   CurrentTemperature        = thermocouple.readCelsius(); 
     
  
   cmdMessenger.sendCmdStart(kPlotDataPoint);  
   cmdMessenger.sendCmdArg(seconds,4);   
   //cmdMessenger.sendCmdArg(internalTemperature,5);   
   cmdMessenger.sendCmdArg(CurrentTemperature,5);   
   cmdMessenger.sendCmdArg(goalTemperature,5);
   cmdMessenger.sendCmdArg((double)((double)output/(double)heaterPwmInterval));
   cmdMessenger.sendCmdArg((bool)switchState);   
   cmdMessenger.sendCmdEnd();
   
   
} 

void heaterPWM()
{
  //output on pin PID_OUT based on PID output and timer
  if(pidTimer.before(output)) {
    switchState=HIGH;
  } else { 
    switchState=LOW;
  }
  digitalWrite(switchPin,switchState);
}

