#include <EEPROMex.h>

int address = 0;

struct keyValuePair {
   int key;
   float value;
};

keyValuePair keyValueInput[2];
keyValuePair keyValueOutput[2];

void setup()
{
  Serial.begin(9600);  
  
  keyValueInput.key = 4;
  keyValueInput.value = 8.8;
  
  EEPROM.writeBlock<keyValuePair>(address, keyValueInput);
  EEPROM.readBlock<keyValuePair>(address, keyValueOutput);  
  
  Serial.print(keyValueOutput.key);
  Serial.print("\t");
  Serial.print(keyValueOutput.value);
  Serial.println();  
}

void loop()
{
  // Nothing to do during loop
}    

