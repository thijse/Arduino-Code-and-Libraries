#include <EEPROMex.h>

int address = 0;

void setup()
{
  Serial.begin(9600);  

    Serial.println("-----------------------------------------------------");     
    Serial.println("Check how much time until EEPROM ready to be accessed");     
    Serial.println("-----------------------------------------------------");      
    int startMillis;
    int endMillis; 
    int waitMillis;

    // Write byte..       
    startMillis = millis();
    EEPROM.writeByte(address,16);
    endMillis = millis();            
    // .. and wait for ready    
    waitMillis = 0;   
    while (!EEPROM.isReady()) { delay(1); waitMillis++; }

    Serial.print("Time to write 1 byte  (ms)                        : "); 
    Serial.println(endMillis-startMillis); 
    Serial.print("Recovery time after writing byte (ms)             : "); 
    Serial.println(waitMillis);    
            
    // Write long ..       
    startMillis = millis();
    EEPROM.writeLong(address,106);
    endMillis = millis();               
    // .. and wait for ready    
    waitMillis = 0;   
    while (!EEPROM.isReady()) { delay(1); waitMillis++; }
    Serial.print("Time to write Long (4 bytes) (ms)                 : "); 
    Serial.println(endMillis-startMillis); 
    Serial.print("Recovery time after writing long (ms)             : "); 
    Serial.println(waitMillis);    
    
    // Read long ..
    startMillis = millis();
    EEPROM.readLong(address);
    endMillis = millis();
    // .. and wait for ready      
    waitMillis = 0;   
    while (!EEPROM.isReady()) { delay(1); waitMillis++; }
    Serial.print("Time to read Long (4 bytes) (ms)                  : ");    
    Serial.println(endMillis-startMillis);     
    Serial.print("Recovery time after reading long (ms)             : "); 
    Serial.println(waitMillis);      
 
    // Write times arrays 
    int itemsInArray = 7;
    byte array7[]    = {64, 32, 16, 8 , 4 , 2 , 1 };
    byte arraydif7[] = {1 , 2 , 4 , 8 , 16, 32, 64};    
    byte arrayDif3[] = {1 , 0 , 4 , 0 , 16, 0 , 64};
    byte output[sizeof(array7)];

    // Time to write 7 byte array 
    startMillis = millis();
    EEPROM.writeBlock<byte>(address, array7, itemsInArray);
    endMillis = millis(); 
    Serial.print("Time to write 7 byte array  (ms)                  : ");    
    Serial.println(endMillis-startMillis); 

    // Time to update 7 byte array with 7 new values
    startMillis = millis();    
    EEPROM.updateBlock<byte>(address, arraydif7, itemsInArray);
    endMillis = millis(); 
    Serial.print("Time to update 7 byte array with 7 new values (ms): ");    
    Serial.println(endMillis-startMillis); 

    // Time to update 7 byte array with 3 new values
    startMillis = millis();    
    EEPROM.updateBlock<byte>(address, arrayDif3, itemsInArray);
    endMillis = millis(); 
    Serial.print("Time to update 7 byte array with 3 new values (ms): ");    
    Serial.println(endMillis-startMillis);

    // Time to read 7 byte array
    startMillis = millis(); 
    EEPROM.readBlock<byte>(address, output, itemsInArray);   
    endMillis = millis(); 
    Serial.print("Time to read 7 byte array (ms)                    : ");    
    Serial.println(endMillis-startMillis);
}

void loop()
{ }    

