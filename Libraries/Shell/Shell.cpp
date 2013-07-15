/*
 * Shell.h - library for interactive console shell 
  
  Copyright (c) Thijs Elenbaas 2013
  
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
  
  16 Feb 2013 - Initial release

 */

#include "Shell.h"

// Initialize shell
Shell::Shell()
{	
	buflen = 0;
}
// Set commands 
void Shell::setCommands(const command_t *commands) {
	_commands = commands;
} 

// Set prompt
void Shell::setPrompt(promptFunc prompt) {
	_prompt = prompt;
} 

// Process console input
void Shell::processInput() {
  
   
   //Serial.println("1");
   if (Serial.available()) {
        // Process serial input for commands from the host.
        int ch = Serial.read();
        if (ch == 0x0A || ch == 0x0D) {		
            // End of the current command.  Blank lines are ignored.
            if (buflen > 0) {
                buffer[buflen] = '\0';
                buflen = 0;
				// Try to process the command
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

// Match a data-space string where the name comes from PROGMEM.
bool Shell::matchString(const prog_char *name, const char *str, int len)
{
    for (;;) {
        char ch1 = (char)(pgm_read_byte(name));
		char ch2 = *str;
        if (ch1 == '\0')
            return len == 0;
        else if (len == 0)
            break;
        if (upperCase(ch1) != upperCase(ch2))
            break;
        ++name;
        ++str;
        --len;
    }
    return false;
}

// Convert char to upper case
char Shell::upperCase(char character) {
	if (character >= 'a' && character <= 'z') {
		character = character - 'a' + 'A';
	}
	return character;
}

// Process commands from console.
void Shell::processCommand(const char *buf)
{
   _prompt();
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
	
	//Loop through all commands
	for (;;) {	    
        const prog_char *name = (const prog_char *)
            (pgm_read_word(&(_commands[index].name)));
        if (!name)
            break;
        if (matchString(name, cmd, len)) {
		   Serial.print(cmd);;
            commandFunc func =
                (commandFunc)(pgm_read_word(&(_commands[index].func)));
            (*func)(buf);
            return;
        }
        ++index;
    }
	//Check for help
	{
        const prog_char *name = (const prog_char *)
            (pgm_read_word(&(s_cmdHelp)));
        if (matchString(name, cmd, len)) {
			showHelp();	
            return;			
        }
    }
	
    // Unknown command.
    Serial.println("Unknown command, valid commands are:");
    showHelp();
}

// Print String from flash
void Shell::printProgString(const prog_char *str)
{
    for (;;) {
        char ch = (char)(pgm_read_byte(str));
        if (ch == '\0')
            break;
        Serial.print(ch);
        ++str;
    }
}

// Show command descriptions
void Shell::showHelp()
{
    // Show all commands
	int index = 0;
    for (;;) {
        const prog_char *name = (const prog_char *)
            (pgm_read_word(&(_commands[index].name)));
        if (!name)
            break;
        const prog_char *desc = (const prog_char *)
            (pgm_read_word(&(_commands[index].desc)));
        const prog_char *args = (const prog_char *)
            (pgm_read_word(&(_commands[index].args)));
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
	// Show help
	printProgString(s_cmdHelp);
	Serial.println();
	Serial.print("    ");
	printProgString(s_cmdHelpDesc);
	Serial.println();
}

Shell ShellConsole = Shell(); // create an instance for the user