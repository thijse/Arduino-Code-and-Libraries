
/*
* Code for fading a single LED in a visually exponential scale, using PWM
*/

// Output
int ledPin   = 3;  // LED,   connected to digital pin 3

// Program variables
int i     = 0; // Loop counter    
int count = 1;
int wait  = 5; // 50ms (.05 second) delay; shorten for faster fades
int DEBUG = 0; // DEBUG counter; if set to 1, wi ll write values back via serial

// Setup
void setup()
{
  pinMode(ledPin,   OUTPUT);   // sets the pins as output
  if (DEBUG) {
    Serial.begin(9600);  
  }
}

// Main program
void loop()
{  
  i += count;      // Increment counter
  if (i > 255) { 
    count = -1;
    i = 255;  
  } 
  if (i < 0) {
      count = 1;
      i = 0;  
  }
  float scaledOut = logScaled(i);
  analogWrite(ledPin,   scaledOut);   // Write current value to LED pin
  if (DEBUG) {
    Serial.print(i);       
    Serial.print(", ");       
    Serial.println(scaledOut);       
  }
  delay(wait);                       // Pause for 'wait' milliseconds before resuming the loop

}

//scales input of 0..255 to 0..255 on a exponential scale
float logScaled(int input) 
{
  return (255.0/(exp(1.0)-1.0))*(exp(float(input)/255.0)-1.0);
}
  
