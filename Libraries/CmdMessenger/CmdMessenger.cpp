
// check this out: http://www.appelsiini.net/2011/simple-usart-with-avr-libc

// ADDED FOR COMPATIBILITY WITH WIRING ??
extern "C" {
  #include <stdlib.h>
  #include <stdarg.h>
}
#include <stdio.h>

#include "CmdMessenger.h"
#include <Streaming.h>

/*
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
*/

//////////////////// Cmd Messenger imp ////////////////
CmdMessenger::CmdMessenger(Stream &ccomms)
{
  init(ccomms,' ',';');
}

CmdMessenger::CmdMessenger(Stream &ccomms, char fld_separator)
{
  init(ccomms,fld_separator,';');
}

CmdMessenger::CmdMessenger(Stream &ccomms, char fld_separator, char cmd_separator)
{
  init(ccomms,fld_separator,cmd_separator);
}

void CmdMessenger::attach(messengerCallbackFunction newFunction) {
	default_callback = newFunction;
}

void CmdMessenger::attach(byte msgId, messengerCallbackFunction newFunction) {
    if (msgId > 0 && msgId <= MAXCALLBACKS) // <= ? I think its ok ?
	callbackList[msgId-1] = newFunction;
}

void CmdMessenger::discard_LF_CR()
{
  discard_newlines = true;
}

void CmdMessenger::print_LF_CR()
{
  print_newlines   = true;
}

void CmdMessenger::init(Stream &ccomms, char fld_separator, char cmd_separator)
{
  comms = &ccomms;
  
  discard_newlines = false;
  print_newlines   = false;

  field_separator   = fld_separator;
  command_separator = cmd_separator;

  bufferLength = MESSENGERBUFFERSIZE;
  bufferLastIndex = MESSENGERBUFFERSIZE -1;
  reset();

  default_callback = NULL;
  for (int i = 0; i < MAXCALLBACKS; i++)
    callbackList[i] = NULL;

  pauseProcessing = false;
}

void CmdMessenger::reset() {
        bufferIndex = 0;
        current = NULL;
        last = NULL;
        dumped = 1;
}


// Gets next argument. Returns true if an argument is available
uint8_t CmdMessenger::next()
{
  char * temppointer= NULL;
  // Currently, cmd messenger only supports 1 char for the field seperator
  const char seperator_tokens[] = { field_separator,'\0' };
  switch (messageState)
  {
    case 0:
    return 0;
    case 1:
    temppointer = buffer;
    messageState = 2;
    default:
    if (dumped)
      current = strtok_r(temppointer,seperator_tokens,&last);
    if (current != NULL)
    {
      dumped = 0;
      return 1; 
    }
  }
  return 0;
}

// Gets next argument. Returns true. 
// Added as alternative name for next()
uint8_t CmdMessenger::available()
{
  return next();
}

// Processes a byte and determines if the line has been terminated
uint8_t CmdMessenger::processLine(int serialByte) {
    messageState = 0;
    char serialChar = (char)serialByte;

    if (serialByte > 0) {

      // Currently, cmd messenger only supports 1 char for the command seperator
      if(serialChar == command_separator)
      {
        buffer[bufferIndex]=0;
        if(bufferIndex > 0)
        {
          messageState = 1;
          current = buffer;
        }
        reset();
      }
      else
      {
        buffer[bufferIndex]=serialByte;
        bufferIndex++;
        if (bufferIndex >= bufferLastIndex) reset();

        if(discard_newlines && (serialChar != field_separator))
          if((serialChar == '\n') || (serialChar == '\r'))
            reset();
      }
    }

    return messageState;
}

// Processes a byte and determines if the line has been terminated
uint8_t CmdMessenger::processAndCallBack(int serialByte) {
	int messageState = processLine(serialByte);	

	// If waiting for acknowledge command	
    if ( messageState == 1 ) {
    	handleMessage();
    }
    return messageState;
}

// Processes a byte and determines if the line has been terminated
uint8_t CmdMessenger::processAndWaitForAck(int serialByte, int AckCommand) {
	//*comms << "1, processAndWaitForAck,";
	int messageState = processLine(serialByte);	
    if ( messageState == 1 ) {
		int id = readInt();
		if (AckCommand==id) {
			//*comms << " matched! ;" << endl;
			return true;
		} else {
			//*comms << " unmatched;" << endl;
			return false;
		}
	}
	//*comms << "No command;" << endl;
	return false;
}

int CmdMessenger::CommandID() {
   return _lastCommandId;
}

void CmdMessenger::handleMessage()
{
    // If we didnt want to use ASCII integer...
    // we would change the line below vv
	_lastCommandId = readInt();

	//Serial << "ID+" << id << endl;
	// Because readInt() can fail and return a 0 we can't
	// start our array index at that number
	if (_lastCommandId > 0 && _lastCommandId <= MAXCALLBACKS && callbackList[_lastCommandId-1] != NULL)
	  (*callbackList[_lastCommandId-1])();
	else // Cmd not registered default callback
	  (*default_callback)();
}

void CmdMessenger::feedinSerialData()
{
  while ( !pauseProcessing && comms->available( ) ) 
    processAndCallBack(comms->read( ) );
}

bool CmdMessenger::CheckForAck(int AckCommand)
{
  //*comms << "1, CheckForAck;";
  while (  comms->available( ) ) {
   //  *comms << "1, Com available;";
     return processAndWaitForAck(comms->read( ), AckCommand );
   }
   return false;
}

bool CmdMessenger::blockedTillReply(int timeout, int ackCmdId)
{
  unsigned long time = millis();
  unsigned long start = time;
  bool receivedAck = false;
  while( (time - start ) < timeout && !receivedAck) {
  //while( !receivedAck) {
	time = millis();
	receivedAck = CheckForAck(ackCmdId);
  }
  return receivedAck;  
}

// if the arguments in the future could be passed in as int/long/float etc
// then it might make sense to use the above writeReal????() methods
// I've removed them for now.

// Send command, including single argument
/*
bool CmdMessenger::sendCmd(int cmdId, char *arg, bool reqAc, int ackCmdId, int timeout)
{  
  pauseProcessing = true;
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
*/

char* CmdMessenger::split(
    char *str, 
    const char *delim, 
    char **nextp)
{
    char *ret;

    if (str == NULL)
    {
        str = *nextp;
    }

    str += strspn(str, delim);

    if (*str == '\0')
    {
        return NULL;
    }

    ret = str;

    str += strcspn(str, delim);

    if (*str)
    {
        *str++ = '\0';
    }

    *nextp = str;

    return ret;
}


// Send start of command. This makes it easy to send multiple arguments per command
void CmdMessenger::sendCmdStart(int cmdId, bool reqAc, 
			       int timeout, int retryCount)
{
  _reqAc = reqAc;
  _timeout = timeout;
  _retryCount = retryCount;
  _startCommand = true;
  
  pauseProcessing = true;
  comms->print(cmdId);
}

// Send formatted argument. 
// Note that floating points are not supported and resulting string is limited to 128 chars
void CmdMessenger::sendCmdfArg(char *fmt, ...)
{
	if (_startCommand) {
		char msg[128]; 
		va_list args;
		va_start (args, fmt );
		vsnprintf(msg, 188, fmt, args);
		va_end (args);
		
		comms->print(field_separator);
		comms->print(msg);
	}  
}

// Send end of command
char* CmdMessenger::sendCmdEnd()
{
  if (_startCommand) {
	comms->print(command_separator);
	if(print_newlines)
		comms->println(); // should append BOTH \r\n
	int tryCount = 0; 
	if (_reqAc) {    
		do {
		  blockedTillReply(_timeout);
		} while( tryCount < _retryCount);
	}	 
		  
  }
  pauseProcessing = false;
  _startCommand = false;
  return NULL;
}



// Not sure if it will work for signed.. check it out
/*unsigned char *CmdMessenger::writeRealInt(int val, unsigned char buff[2])
{
  buff[1] = (unsigned char)val;
  buff[0] = (unsigned char)(val >> 8);  
  buff[2] = 0;
  return buff;
}

char* CmdMessenger::writeRealLong(long val, char buff[4])
{
  //buff[1] = (unsigned char)val;
  //buff[0] = (unsigned char)(val >> 8);  
  return buff;
}

char* CmdMessenger::writeRealFloat(float val, char buff[4])
{
  //buff[1] = (unsigned char)val;
  //buff[0] = (unsigned char)(val >> 8);  
  return buff;
}
*/


int CmdMessenger::readInt()
{
  if (next())
  {
    dumped = 1;
    return atoi(current);
  }
  return 0;
}

char CmdMessenger::readChar()
{
  if (next())
  {
    dumped = 1;
    return current[0];
  }
  return 0;
}

float CmdMessenger::readFloat()
{
  if (next())
  {
    dumped = 1;
    return atof(current);
  }
  return 0;
}

void CmdMessenger::readString(char *string, uint8_t size)
{
  if (next())
  {
    dumped = 1;
    strlcpy(string,current,size);
  }
  else
  {
    if ( size ) string[0] = '\0';
  }
}

uint8_t CmdMessenger::compareString(char *string)
{
  if (next())
  {
    if ( strcmp(string,current) == 0 )
    {
      dumped = 1;
      return 1;
    }
    else
    {
      return 0;
    }
  } 
}
