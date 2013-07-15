#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

char buffer[64];
size_t buflen;

const char *days[] = {
    "Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "
};

const char *months[] = {
    " Jan ", " Feb ", " Mar ", " Apr ", " May ", " Jun ",
    " Jul ", " Aug ", " Sep ", " Oct ", " Nov ", " Dec "
};

static uint8_t monthLengths[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

inline bool isLeapYear(unsigned int year)
{
    if ((year % 100) == 0)
        return (year % 400) == 0;
    else
        return (year % 4) == 0;
}

inline uint8_t monthLength(tmElements_t *time)
{
    if (time->Month != 2 || !isLeapYear((time->Year+1970)))
        return monthLengths[time->Month - 1];
    else
        return 29;
}

void setup() {
    Serial.begin(9600);
    buflen = 0;
    cmdHelp(0);
}

void loop() {
  processInput();
}

void processInput() {
    if (Serial.available()) {
        // Process serial input for commands from the host.
        int ch = Serial.read();
        if (ch == 0x0A || ch == 0x0D) {
            // End of the current command.  Blank lines are ignored.
            if (buflen > 0) {
                buffer[buflen] = '\0';
                buflen = 0;
              processCommand(buffer);
            }
        } else if (ch == 0x08) {
            // Backspace over the last character.
            if (buflen > 0)
                --buflen;
        } else if (buflen < (sizeof(buffer) - 1)) {
            // Add the character to the buffer after forcing to upper case.
            if (ch >= 'a' && ch <= 'z')
                buffer[buflen++] = ch - 'a' + 'A';
            else
                buffer[buflen++] = ch;
        }
    }
}


// Print 2-digit value
void printDec2(int value)
{
    Serial.print((char)('0' + (value / 10)));
    Serial.print((char)('0' + (value % 10)));
}

// Print String from flash
void printProgString(const prog_char *str)
{
    for (;;) {
        char ch = (char)(pgm_read_byte(str));
        if (ch == '\0')
            break;
        Serial.print(ch);
        ++str;
    }
}

// Read value from sub-string
byte readField(const char *args, int &posn, int maxValue)
{
    int value = -1;
    if (args[posn] == ':' && posn != 0)
        ++posn;
    while (args[posn] >= '0' && args[posn] <= '9') {
        if (value == -1)
            value = 0;
        value = (value * 10) + (args[posn++] - '0');
        if (value > 99)
            return 99;
    }
    if (value == -1 || value > maxValue)
        return 99;
    else
        return value;
}


// List of all commands that are implemented.
typedef void (*commandFunc)(const char *args);
typedef struct
{
    const prog_char *name;
    commandFunc func;
    const prog_char *desc;
    const prog_char *args;
} command_t;

// define TIME command
const char s_cmdTime[] PROGMEM = "TIME";
const char s_cmdTimeDesc[] PROGMEM = "Read or write the current time";
const char s_cmdTimeArgs[] PROGMEM = "[HH:MM:SS]";

// define DATE command
const char s_cmdDate[] PROGMEM = "DATE";
const char s_cmdDateDesc[] PROGMEM = "Read or write the current date";
const char s_cmdDateArgs[] PROGMEM = "[YYYYMMDD]";

// define HELP command
const char s_cmdHelp[] PROGMEM = "HELP";
const char s_cmdHelpDesc[] PROGMEM = "Prints this help message";


// make list of commands

const command_t commands[] PROGMEM = {
  {s_cmdTime, cmdTime, s_cmdTimeDesc, s_cmdTimeArgs},
  {s_cmdDate, cmdDate, s_cmdDateDesc, s_cmdDateArgs},
  {s_cmdHelp, cmdHelp, s_cmdHelpDesc},
    {0, 0}
};
/*
const command_t commandTime  = {s_cmdTime, cmdTime, s_cmdTimeDesc, s_cmdTimeArgs};
const command_t commandDate  = {s_cmdDate, cmdDate, s_cmdDateDesc, s_cmdDateArgs};
const command_t commandHelp  =  {s_cmdHelp, cmdHelp, s_cmdHelpDesc};
const command_t commands[] = {
  commandTime,
  commandDate,
  commandHelp

};
*/

void prompt()
{
   tmElements_t time; 
   RTC.read(time);
   printTime(&time); 
   Serial.print("> ");
}


// Implementation of  "TIME" command.
void cmdTime(const char *args)
{
   tmElements_t time;   
  
   // Read the current time & date from RTC
   RTC.read(time);
   
   // Update time from console
    if (*args != '\0') {
        // Set the current time.
        int posn = 0;
        time.Hour = readField(args, posn, 23);
        time.Minute = readField(args, posn, 59);
        if (args[posn] != '\0')
            time.Second = readField(args, posn, 59);
        else
            time.Second = 0;
        if (time.Hour == 99 || time.Minute == 99 || time.Second == 99) {
            Serial.println("Invalid time format; use HH:MM:SS");
            return;
        }
        RTC.set(makeTime(time));
        Serial.print("Time will be set to: ");
        printTime(&time);
        Serial.println();
    }

    // Read the current time.
    Serial.print("Current RTC time is: ");
    RTC.read(time);
    printTime(&time);
    Serial.println();

}

void printTime(tmElements_t *time) {
      printDec2(time->Hour);
    Serial.print(':');
    printDec2(time->Minute);
    Serial.print(':');
    printDec2(time->Second);

}

void printDate(tmElements_t *time) {
    Serial.print(weekday(makeTime(*time)));
    Serial.print(days[weekday(makeTime(*time)) - 1]);
    Serial.print(time->Day, DEC);
    Serial.print(months[time->Month - 1]);
    Serial.println(time->Year+1970, DEC);
}

// Implementation of  "DATE" command.
void cmdDate(const char *args)
{
    tmElements_t time;  
    
    // Read the current time & date from RTC
    RTC.read(time);
   
    // Update time from console
    if (*args != '\0') {
        // Set the current date.
        
        String date = args;

        int year = strintToInt(date.substring(0,4));
        if (year < 2000 || year >= 2100) {
            Serial.println("Year must be between 2000 and 2099");
            return;
        }
        time.Year = year-1970;        
        Serial.println(year);
        
        time.Month = strintToInt(date.substring(4,6));
        Serial.println(time.Month);

        time.Day = strintToInt(date.substring(6,8));
        Serial.println(time.Day);

        if (time.Month < 1 || time.Month > 12) {
            Serial.println("Month must be between 1 and 12");
            return;
        }
        uint8_t len = monthLength(&time);
        if (time.Day < 1 || time.Day > len) {
            Serial.print("Day must be between 1 and ");
            Serial.println(len, DEC);
            return;
        }
        RTC.set(makeTime(time));
        Serial.print("Date has been set to: ");
    }

    // Read the current date.
    RTC.read(time);
    
    printDate(&time);
/*    
    Serial.print(days[weekday(makeTime(time)) - 1]);
    Serial.print(time.Day, DEC);
    Serial.print(months[time.Month - 1]);
    Serial.println(time.Year, DEC);
    */
}

int strintToInt(String value) {
 char chars[value.length()+1]; 
 value.toCharArray(chars, value.length()+1);
 return atoi(chars);
}



// Implementation of "HELP" command.
void cmdHelp(const char *)
{
    int index = 0;
    for (;;) {
        const prog_char *name = (const prog_char *)
            (pgm_read_word(&(commands[index].name)));
        if (!name)
            break;
        const prog_char *desc = (const prog_char *)
            (pgm_read_word(&(commands[index].desc)));
        const prog_char *args = (const prog_char *)
            (pgm_read_word(&(commands[index].args)));
        printProgString(name);
        if (args) {
            Serial.print(' ');
            printProgString(args);
        }
        Serial.println();
        Serial.print("    ");
        printProgString(desc);
        Serial.println();
        ++index;
    }
}

// Match a data-space string where the name comes from PROGMEM.
bool matchString(const prog_char *name, const char *str, int len)
{
    for (;;) {
        char ch1 = (char)(pgm_read_byte(name));
        if (ch1 == '\0')
            return len == 0;
        else if (len == 0)
            break;
        if (ch1 >= 'a' && ch1 <= 'z')
            ch1 = ch1 - 'a' + 'A';
        char ch2 = *str;
        if (ch2 >= 'a' && ch2 <= 'z')
            ch2 = ch2 - 'a' + 'A';
        if (ch1 != ch2)
            break;
        ++name;
        ++str;
        --len;
    }
    return false;
}

// Process commands from console.
void processCommand(const char *buf)
{

   prompt();
   Serial.println(buf);
  // Skip white space at the start of the command.
    while (*buf == ' ' || *buf == '\t')
        ++buf;
    if (*buf == '\0')
        return;     // Ignore blank lines.

    // Extract the command portion of the line.
    const char *cmd = buf;
    int len = 0;
    for (;;) {
        char ch = *buf;
        if (ch == '\0' || ch == ' ' || ch == '\t')
            break;
        ++buf;
        ++len;
    }

    // Skip white space after the command name and before the arguments.
    while (*buf == ' ' || *buf == '\t')
        ++buf;

    // Find the command and execute it.
    int index = 0;
    for (;;) {
        const prog_char *name = (const prog_char *)
            (pgm_read_word(&(commands[index].name)));
        if (!name)
            break;
        if (matchString(name, cmd, len)) {
            commandFunc func =
                (commandFunc)(pgm_read_word(&(commands[index].func)));
            (*func)(buf);
            return;
        }
        ++index;
    }

    // Unknown command.
    Serial.println("Unknown command, valid commands are:");
    cmdHelp(0);
}



