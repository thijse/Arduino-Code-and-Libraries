#include <EEPROMex.h>

// ID of the settings block
#define CONFIG_VERSION "ls1"

// Tell it where to store your config data in EEPROM
#define memoryBase 32

bool ok  = true;
int configAdress=0;

// Example settings structure
struct StoreStruct {
    char version[4];   // This is for mere detection if they are your settings
    int a, b;          // The variables of your settings
    char c;
    long d;
    float e[6];
} storage = { 
    CONFIG_VERSION,
    220, 1884,
    'c',
    10000,
    {4.5, 5.5, 7, 8.5, 10, 12}
};

void setup() {
  EEPROM.setMemPool(memoryBase, EEPROMSizeUno); //Set memorypool base to 32, assume Arduino Uno board
  configAdress  = EEPROM.getAddress(sizeof(StoreStruct)); // Size of config object 
  ok = loadConfig();
}

void loop() {
  // [...]
  int i = storage.c - 'a';
  // [...]
  storage.c = 'a';
  if (ok)
    saveConfig();
  // [...]
}

bool loadConfig() {
  EEPROM.readBlock(configAdress, storage);
  return (storage.version == CONFIG_VERSION);
}

void saveConfig() {
   EEPROM.writeBlock(configAdress, storage);
}

