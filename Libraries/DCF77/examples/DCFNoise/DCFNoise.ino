/*
 * DCFNoise.ino - DCF77 debug Example
 * Thijs Elenbaas, 2013
 * This example code is in the public domain.
 
  This simple example shows the raw signal coming from the DCF decoder.
  
  Pulse-to-Pulse is approximately 1000 ms and pulse with is approx 100ms or 200ms
  The axis underestimates the elapsed time slightly, because a single loop takes a bit
  longer than 10ms.
*/

#define BLINKPIN 13
#define DCF77PIN 2
#define PULSESTART LOW
#

int prevSensorValue=!PULSESTART;
int averageWindow = 200;
double ValueSummed;
double ValueSquaredSummed;
  
void setup() {
  Serial.begin(9600);
  pinMode(DCF77PIN, INPUT);
  pinMode(13, OUTPUT);
  Serial.println("0ms       100ms     200ms     300ms     400ms     500ms     600ms     700ms     800ms     900ms     1000ms    1100ms    1200ms");
}

void loop() {
  ValueSummed = 0.0;
  ValueSquaredSummed=0.0;
  for (int i=0;i<averageWindow; i++) {
    double sensorValue = (double)analogRead(DCF77PIN)/1024.0;

    ValueSummed += sensorValue;
    ValueSquaredSummed += (sensorValue*sensorValue);
  }
   

  double ValueSquaredSummedAvg = ValueSquaredSummed / averageWindow;
  double ValueSummedSqAvg = sq(ValueSummed/ averageWindow);

  double std = sqrt(ValueSquaredSummedAvg-ValueSummedSqAvg);
 
  Serial.print("value");
  Serial.print(ValueSummed/averageWindow);
   Serial.print(" +/- ");
  Serial.println(std);
  delay(0);
}
