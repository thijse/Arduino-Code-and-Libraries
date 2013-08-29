#ifndef CmdMessenger_h
#define CmdMessenger_h

#include <inttypes.h>
#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif

//#include "Stream.h"

extern "C"
{
  // Our callbacks are always method signature: void cmd(void);
  typedef void (*messengerCallbackFunction) (void);
}

#define MAXCALLBACKS 50         // The maximum number of unique commands
#define MESSENGERBUFFERSIZE 64  // The maximum length of the buffer (defaults to 64)
#define DEFAULT_TIMEOUT 5000    // Abandon incomplete messages if nothing heard after 5 seconds

// Message States
enum
{
  // Commands
  kProccesingMessage,
  kEndOfMessage,
  kProcessingArguments,
};

class CmdMessenger
{

protected:
  bool _startCommand;
  uint8_t _lastCommandId;
  uint8_t bufferIndex;          // Index where to write the data
  uint8_t bufferLength;         // Is set to MESSENGERBUFFERSIZE
  uint8_t bufferLastIndex;      // The last index of the buffer

  char ArglastChar;             //bookkeeping of argument escape char 
  char CmdlastChar;             //bookkeeping of command escape char 

  messengerCallbackFunction default_callback;
  messengerCallbackFunction callbackList[MAXCALLBACKS];
  bool pauseProcessing;

  void handleMessage ();
  void init (Stream & comms, const char fld_separator,
             const char cmd_separator, const char esc_character);
  bool CheckForAck (int AckCommand);
  uint8_t processLine (int serialByte);
  uint8_t processAndCallBack (int serialByte);
  bool processAndWaitForAck (int serialByte, int AckCommand);
  void reset ();

  int findNext (char *str, char delim);
  char *split_r (char *str, const char delim, char **nextp);
    template < class T > void writeBinary (const T & value)
  {
    const byte *bytePointer = (const byte *) (const void *) &value;
    for (unsigned int i = 0; i < sizeof (value); i++)
      {
        printEsc (*bytePointer);
        *bytePointer++;
      }
  }

  template < class T > T readBin (char *str)
  {
    T value;
    unescape (str);
    byte *bytePointer = (byte *) (const void *) &value;
    for (unsigned int i = 0; i < sizeof (value); i++)
      {
        *bytePointer = str[i];
        *bytePointer++;
      }
    return value;
  }
  void printEsc (char *str);
  void printEsc (char str);
  bool isEscaped (char *currChar, const char escapeChar, char *lastChar);

  char buffer[MESSENGERBUFFERSIZE];     // Buffer that holds the data
  uint8_t messageState;
  bool dumped;
  char *current;                // Pointer to current data
  char *last;
  char prevChar;                // Previous char needed for unescaping
  Stream *comms;
  char command_separator;
  char field_separator;
  char escape_character;
public:
  CmdMessenger (Stream & comms, const char fld_separator =
                ',', const char cmd_separator =
                ';', const char esc_character = '/');

  void attach (messengerCallbackFunction newFunction);
  //void discard_LF_CR();
  void print_LF_CR ();

  bool next ();
  bool available ();
  uint8_t CommandID ();

  int readInt ();
  char readChar ();
  float readFloat ();
  char *readString ();
  void copyString (char *string, uint8_t size);
  uint8_t compareString (char *string);

  template < class T > void sendCmdBinArg (T arg)
  {
    if (_startCommand)
      {
        comms->print (field_separator);
        writeBinary (arg);
      }
  }

  template < class T > T readBinary ()
  {
    if (next ())
      {
        dumped = true;
        return readBin < T > (current);
      }
  }
  void attach (byte msgId, messengerCallbackFunction newFunction);

  void unescape (char *fromChar);
// Send command, including single argument
  template < class T >
    bool sendCmd (int cmdId, T arg, bool reqAc = false, int ackCmdId =
                  1, int timeout = DEFAULT_TIMEOUT)
  {
    sendCmdStart (cmdId);
    sendCmdArg (arg);
    sendCmdEnd (reqAc, ackCmdId, timeout);
  }
  template < class T >
    bool sendBinCmd (int cmdId, T arg, bool reqAc = false, int ackCmdId =
                     1, int timeout = DEFAULT_TIMEOUT)
  {
    sendCmdStart (cmdId);
    sendCmdBinArg (arg);
    sendCmdEnd (reqAc, ackCmdId, timeout);
  }

  void sendCmdStart (int cmdId);
  void sendCmdEscArg (char *arg);
  void sendCmdfArg (char *fmt, ...);
  // Send argument. 
  // Note that this will only succeed if a sendCmdStart has been issued first
  template < class T > void sendCmdArg (T arg, int n)
  {
    if (_startCommand)
      {
        comms->print (field_separator);
        comms->print (arg, n);
      }
  }
  template < class T > void sendCmdArg (T arg)
  {
    if (_startCommand)
      {
        comms->print (field_separator);
        comms->print (arg);
      }
  }
  bool sendCmdEnd (bool reqAc = false, int ackCmdId = 1, int timeout =
                   DEFAULT_TIMEOUT);

  void feedinSerialData ();
  //bool discard_newlines;
  bool print_newlines;
  bool blockedTillReply (int timeout = DEFAULT_TIMEOUT, int ackCmdId = 1);
};
#endif
