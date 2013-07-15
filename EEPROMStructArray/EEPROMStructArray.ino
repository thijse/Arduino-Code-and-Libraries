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

  keyValueInput[0].key = 2;
  keyValueInput[0].value = 4.4;
  
  keyValueInput[1].key = 4;
  keyValueInput[1].value = 8.8;
  
  EEPROM.writeBlock<keyValuePair>(address, keyValueInput,2);
  EEPROM.readBlock<keyValuePair>(address, keyValueOutput,2);  
  
  Serial.print(keyValueOutput[0].key);
  Serial.print("\t");
  Serial.print(keyValueOutput[0].value);
  Serial.println();  
  
  Serial.print(keyValueOutput[1].key);
  Serial.print("\t");
  Serial.print(keyValueOutput[1].value);
  Serial.println();  
}

void loop()
{ }    

