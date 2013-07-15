#ifndef Shell_h
#define Shell_h

#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif


typedef void (*commandFunc)(const char *args);
typedef void (*promptFunc)();

// define HELP command
const char s_cmdHelp[] PROGMEM = "HELP";
const char s_cmdHelpDesc[] PROGMEM = "Prints this help message";

typedef struct
{
    const prog_char *name;
    commandFunc func;
    const prog_char *desc;
    const prog_char *args;
} command_t;


class Shell {
	private:
		const command_t *_commands;
		promptFunc _prompt;
		char buffer[64];
		size_t buflen;		
		void processCommand(const char *buf);
		bool matchString(const prog_char *name, const char *str, int len);				
		char upperCase(char character);
    public:
		Shell();
		void setCommands(const command_t *commands);
		void setPrompt(promptFunc prompt);		
		void processInput();
		void printProgString(const prog_char *str);
		void showHelp();		
};

extern Shell ShellConsole;

#endif // Shell_h 