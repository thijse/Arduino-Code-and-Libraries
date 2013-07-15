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

  // Write byte to EEPROM
  EEPROM.writeByte(address,18);

  // Now perform some stuff until the memory is ready again
  int loops;
  while (!EEPROM.isReady()) { 
    // Time sensitify functions
    delay(1); 
    loops++; 
  }
  Serial.print("EEPROM Recovery time  (ms) : "); 
  Serial.println(loops);  

  // Write nex byte to EEPROM
  EEPROM.writeByte(address,18);

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

