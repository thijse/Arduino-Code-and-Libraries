/*
  CmdMessenger - library that provides command based messaging

  The library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Initial Messenger Library - Thomas Ouellet Fredericks.
    CmdMessenger Version 1    - Neil Dudman.
    CmdMessenger Version 2    - Dreamcat4.
	CmdMessenger Version 3    - Thijs Elenbaas.
	  3.0  - Bugfixes on 2.2
	       - Wait for acknowlegde
	       - Multi-argument commands
           - Escaping of strings
           - Sending of binary data of any type
			 (uses escaping for efficiency)
 */

extern "C" {
#include <stdlib.h>
#include <stdarg.h>
}
#include <stdio.h>
#include <CmdMessenger.h>
#include <Streaming.h>

#define _CMDMESSENGER_VERSION 3_0 // software version of this library

/****  Private functions   ****/

/**
 * CmdMessenger constructor
 */
CmdMessenger::CmdMessenger(Stream &ccomms, const char fld_separator, const char cmd_separator, const char esc_character)
{
    init(ccomms,fld_separator,cmd_separator, esc_character);
}

/**
 * Enables printing newline after a sent command
 */
void CmdMessenger::init(Stream &ccomms, const char fld_separator, const char cmd_separator, const char esc_character)
{
    default_callback = NULL;
    comms            = &ccomms;
    print_newlines    = false;
    field_separator   = fld_separator;
    command_separator = cmd_separator;
    escape_character  = esc_character;
    bufferLength      = MESSENGERBUFFERSIZE;
    bufferLastIndex   = MESSENGERBUFFERSIZE -1;
    reset();

    default_callback  = NULL;
    for (int i = 0; i < MAXCALLBACKS; i++)
        callbackList[i] = NULL;

    pauseProcessing   = false;
}

/**
 * Attaches an default function for commands that are not explicitly attached
 */
void CmdMessenger::attach(messengerCallbackFunction newFunction)
{
    default_callback = newFunction;
}

/**
 * Attaches a function to a command ID
 */
void CmdMessenger::attach(byte msgId, messengerCallbackFunction newFunction)
{
    if (msgId > 0 && msgId <= MAXCALLBACKS)
        callbackList[msgId-1] = newFunction;
}

/**
 * Enables printing newline after a sent command
 */
void CmdMessenger::print_LF_CR()
{
    print_newlines = true;
}

/**
 * Resets the command buffer and message state
 */
void CmdMessenger::reset()
{
    bufferIndex = 0;
    current     = NULL;
    last        = NULL;
    dumped      = true;
}

/**
 * Gets next argument. Returns true if an argument is available
 */
bool CmdMessenger::next()
{
    char * temppointer= NULL;
    // Currently, cmd messenger only supports 1 char for the field seperator
    switch (messageState) {
    case kProccesingMessage:
        return false;
    case kEndOfMessage:
        temppointer = buffer;
        messageState = kProcessingArguments;
    default:
        if (dumped)
            current = split_r(temppointer,field_separator,&last);
        if (current != NULL) {
            dumped = true;
            return true;
        }
    }
    return false;
}

/**
 * Returns if an argument is available. Alias for next()
 */
bool CmdMessenger::available()
{
    return next();
}

/**
 * Processes bytes and determines message state
 */
uint8_t CmdMessenger::processLine(int serialByte)
{
    messageState = kProccesingMessage;
    char serialChar = (char)serialByte;
    bool escaped = isEscaped(&serialChar,escape_character,&CmdlastChar);
//	*comms << "1, " << (int) serialChar << " ;";
    if (serialByte > 0 || escaped) {
        if((serialChar == command_separator) && !escaped) {
            buffer[bufferIndex]=0;
            if(bufferIndex > 0) {
                messageState = kEndOfMessage;
                current = buffer;
                CmdlastChar='\0';
            }
            reset();
        } else {
            buffer[bufferIndex]=serialByte;
            bufferIndex++;
            if (bufferIndex >= bufferLastIndex) reset();
        }
    }
    return messageState;
}

/**
 * Processes a byte stream, and handles dispatches callbacks, if commands are received
 */
uint8_t CmdMessenger::processAndCallBack(int serialByte)
{
    int messageState = processLine(serialByte);

    // If waiting for acknowledge command
    if ( messageState == kEndOfMessage ) {
        handleMessage();
    }
    return messageState;
}

/**
 * Processes a byte and determines if the line has been terminated
 */
bool CmdMessenger::processAndWaitForAck(int serialByte, int AckCommand)
{
    //*comms << "1, processAndWaitForAck,";
    int messageState = processLine(serialByte);
    if ( messageState == kEndOfMessage ) {
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

/**
 * Returns the CommandID of the current command
 */
uint8_t CmdMessenger::CommandID()
{
    return _lastCommandId;
}

/**
 * Dispatches attached callbacks based on command
 */
void CmdMessenger::handleMessage()
{
    _lastCommandId = readInt();
    // if command attached, we will call it
    if (_lastCommandId > 0 && _lastCommandId <= MAXCALLBACKS && callbackList[_lastCommandId-1] != NULL)
        (*callbackList[_lastCommandId-1])();
    else // If command not attached, call default callback (if attached)
        if (default_callback!=NULL) (*default_callback)();
}

/**
 * Feeds serial data in CmdMessenger
 */
void CmdMessenger::feedinSerialData()
{
    while ( !pauseProcessing && comms->available( ) )
        processAndCallBack(comms->read( ) );
}

/**
 * 
 */
// Todo: Combine with processAndWaitForAck
bool CmdMessenger::CheckForAck(int AckCommand)
{
    while (  comms->available( ) ) {
        return processAndWaitForAck(comms->read( ), AckCommand );
    }
    return false;
}

/**
 * Waits for reply from sender or timeout before continuing
 */
bool CmdMessenger::blockedTillReply(int timeout, int ackCmdId)
{
    unsigned long time  = millis();
    unsigned long start = time;
    bool receivedAck    = false;
    while( (time - start ) < timeout && !receivedAck) {
        time = millis();
        receivedAck = CheckForAck(ackCmdId);
    }
    return receivedAck;
}


/****  Public functions   ****/


/****  Sending commands   ****/

/**
 * Send start of command. This makes it easy to send multiple arguments per command
 */
void CmdMessenger::sendCmdStart(int cmdId)
{
    _startCommand   = true;
    pauseProcessing = true;
    comms->print(cmdId);
}

/**
 * Send formatted argument.
 *  Note that floating points are not supported and resulting string is limited to 128 chars
 */
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
bool CmdMessenger::sendCmdEnd(bool reqAc, int ackCmdId, int timeout)
{
    bool ackReply = false;
    if (_startCommand) {
        comms->print(command_separator);
        if(print_newlines)
            comms->println(); // should append BOTH \r\n
        int tryCount = 0;

        if (reqAc) {
            ackReply = blockedTillReply(timeout, ackCmdId);
        }
    }
    pauseProcessing = false;
    _startCommand   = false;
    return ackReply;
}


/****  Receiving commands   ****/


// Read the next argument as int
int CmdMessenger::readInt()
{
    if (next()) {
        dumped = true;
        return atoi(current);
    }
    return 0;
}

// Read the next argument as char
char CmdMessenger::readChar()
{
    if (next()) {
        dumped = true;
        return current[0];
    }
    return 0;
}

// Read the next argument as float
float CmdMessenger::readFloat()
{
    if (next()) {
        dumped = true;
        return atof(current);
    }
    return 0;
}

// Read next argument as string.
// String is valid as long as current command exists
char* CmdMessenger::readString()
{
    if (next()) {
        dumped = true;
        return current;
    }
    return "\0";
}

// Return next argument as new string
// Use it when you want to persist the string
void CmdMessenger::copyString(char *string, uint8_t size)
{
    if (next()) {
        dumped = true;
        strlcpy(string,current,size);
    } else {
        if ( size ) string[0] = '\0';
    }
}

// Compare input string with the next argument
uint8_t CmdMessenger::compareString(char *string)
{
    if (next()) {
        if ( strcmp(string,current) == 0 ) {
            dumped = true;
            return 1;
        } else {
            return 0;
        }
    }
}

// Returns per character if it is escaped
bool CmdMessenger::isEscaped(char *currChar, const char escapeChar, char *lastChar)
{
    bool escaped;
    escaped   = (*lastChar==escapeChar);
    *lastChar = *currChar;

    // special case: the escape char has been escaped:
    if (*lastChar == escape_character && escaped) {
        *lastChar = '\0';
    }
    return escaped;
}

// Unescapes a string in place
void CmdMessenger::unescape(char *fromChar)
{
    // Move unescaped characters right
    char *toChar = fromChar;
    while (*fromChar != '\0') {
        if (*fromChar==escape_character) {
            fromChar++;
        }
        *toChar++=*fromChar++;
    }
    // Pad string with \0 if string was shortened
    for (; toChar<fromChar; toChar++) {
        *toChar='\0';
    }
}

// Print an escaped string
void CmdMessenger::printEsc(char *str)
{
    while (*str != '\0') {
        printEsc(*str++);
    }
}

// Print an escaped character
void CmdMessenger::printEsc(char str)
{
    if (str==field_separator || str==command_separator || str==escape_character || str=='\0') {
        comms->print(escape_character);
    }
    comms->print(str);
}

// Send an escaped command argument
void CmdMessenger::sendCmdEscArg(char* arg)
{
    if (_startCommand) {
        comms->print(field_separator);
        printEsc(arg);
    }
}

// Find next field
int CmdMessenger::findNext(char *str, char delim)
{
    int pos = 0;
    bool escaped;
    ArglastChar = NULL;
    while (*str != '\0') {
        escaped = isEscaped(str,escape_character,&ArglastChar);
        if (*str==field_separator && !escaped) {
            return pos;
        } else {
            str++;
            pos++;
        }
    }
    return pos;
}

// Split string in different tokens, based on delimiter
// This is basically strtok_r, but with support for an escape character
char* CmdMessenger::split_r(char *str, const char delim, char **nextp)
{
    char *ret;
    // if input null, this is not the first call, use the nextp pointer instead
    if (str == NULL) {
        str = *nextp;
    }
    // Strip leading delimiters
    while (findNext(str, delim)==0 && *str) {
        str++;
    }
    // If this is a \0 char, return null
    if (*str == '\0') {
        return NULL;
    }
    // Set start of return pointer to this position
    ret = str;
    // Find next delimiter
    str += findNext(str, delim);
    // and exchange this for a a \0 char. This will terminate the char
    if (*str) {
        *str++ = '\0';
    }
    // Set the next pointer to this char
    *nextp = str;
    // return current pointer
    return ret;
}

