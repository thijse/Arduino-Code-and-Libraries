#ifndef CmdMessenger_h
#define CmdMessenger_h

#include <inttypes.h>
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#include "Stream.h"

extern "C" {
  // Our callbacks are always method signature: void cmd(void);
  typedef void (*messengerCallbackFunction)(void);
}

#define MAXCALLBACKS 50        // The maximum number of unique commands
#define MESSENGERBUFFERSIZE 64 // The maximum length of the buffer (defaults to 64)
#define DEFAULT_TIMEOUT 5000   // Abandon incomplete messages if nothing heard after 5 seconds

#define 
class CmdMessenger
{  

protected:  
  bool    _reqAc;
  int     _timeout;
  int     _retryCount;
  bool    _startCommand;
  int      _lastCommandId;
  
  
  
  uint8_t bufferIndex;     // Index where to write the data
  uint8_t bufferLength;    // Is set to MESSENGERBUFFERSIZE
  uint8_t bufferLastIndex; // The last index of the buffer

  
  
  messengerCallbackFunction default_callback;
  messengerCallbackFunction callbackList[MAXCALLBACKS];

  // (not implemented, generally not needed)
  // when we are sending a message and requre answer or acknowledgement
  // suspend any processing (process()) when serial intterupt is received
  // Even though we usually only have single processing thread we still need
  // this i think because Serial interrupts.
  // Could also be usefull when we want data larger than MESSENGERBUFFERSIZE
  // we could send a startCmd, which could pauseProcessing and read directly
  // from serial all the data, send acknowledge etc and then resume processing  
  bool pauseProcessing;
    
  void handleMessage(); 
  void init(Stream &comms, char field_separator, char command_separator);
  bool CheckForAck(int AckCommand);
  uint8_t processLine(int serialByte);
  uint8_t processAndCallBack(int serialByte);
  uint8_t processAndWaitForAck(int serialByte, int AckCommand);
  void reset();
  char* split(char *str, const char *delim, char **nextp);
  
  char buffer[MESSENGERBUFFERSIZE]; // Buffer that holds the data
  uint8_t messageState;
  uint8_t dumped;
  char* current; // Pointer to current data
  char* last;
  char prevChar; // Previous char needed for unescaping
  
  
public:
  CmdMessenger(Stream &comms);
  CmdMessenger(Stream &comms, char fld_separator);
  CmdMessenger(Stream &comms, char fld_separator, char cmd_separator);

  void attach(messengerCallbackFunction newFunction);
  void discard_LF_CR();
  void print_LF_CR();

  uint8_t next();
  uint8_t available();
  int CommandID();
  
  int readInt();
  char readChar();
  float readFloat();
  void readString(char *string, uint8_t size);
  uint8_t compareString(char *string);

  // Polymorphism used to interact with serial class
  // Stream is an abstract base class which defines a base set
  // of functionality used by the serial classes.
  Stream *comms;
  
  
  void attach(byte msgId, messengerCallbackFunction newFunction);
  
 // bool sendCmd(int cmdId, char *arg, bool reqAc = false, int ackCmdId = 1, int timeout = DEFAULT_TIMEOUT );

// Send command, including single argument
  template <class T>
 bool sendCmd(int cmdId, T arg, bool reqAc = false, int ackCmdId = 1, int timeout = DEFAULT_TIMEOUT )
 {
  pauseProcessing = true;
  //ackReply = false;
  //*comms << cmdId << field_separator << msg << endl;
  comms->print(cmdId);
  comms->print(field_separator);
  comms->print(arg);
  comms->print(command_separator);
  if(print_newlines)
    comms->println(); // should append BOTH \r\n
  int ackReply;
  if (reqAc) {
      ackReply = blockedTillReply(timeout, ackCmdId);
  }
  
  pauseProcessing = false;
  return ackReply;
}	
	
  void sendCmdStart(int cmdId, bool reqAc = false, int timeout = DEFAULT_TIMEOUT, int retryCount = 10);
  void sendCmdfArg(char *fmt, ...);
  
  // Send argument. 
  // Note that this will only succeed if a sendCmdStart has been issued first
  template <class T>
  void sendCmdArg(T arg, int n)
  {
	if (_startCommand) {
		comms->print(field_separator);
		comms->print(arg,n);
	}
  }

  template <class T>
  void sendCmdArg(T arg)
  {
	if (_startCommand) {
		comms->print(field_separator);
		comms->print(arg);
	}
  }
  
  char* sendCmdEnd();
		
		
  void feedinSerialData();
  
  char command_separator;
  char field_separator;

  bool discard_newlines;
  bool print_newlines;

  bool blockedTillReply(int timeout = DEFAULT_TIMEOUT, int ackCmdId = 1);
};
#endif