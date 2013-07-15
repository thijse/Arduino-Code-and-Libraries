/*
 * IRremote: IRrecvDump - dump details of IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 */

#include <IRremote.h>
#include <lightuino5.h>

// These "print" wrappers just output to BOTH USB and UART serial ports (code at the bottom)
void println(char*s);
void print(char*s);
void print(int i,char format=DEC);
void println(int i,char format=DEC);


int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  Serial.begin(9600);
  Usb.begin();
  irrecv.enableIRIn(); // Start the receiver
  println("Connection initialized");
}

// Dumps out the decode_results structure.
// Call this after IRrecv::decode()
// void * to work around compiler issue
//void dump(void *v) {
//  decode_results *results = (decode_results *)v
void dump(decode_results *results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    print("Unknown encoding: ");
  } 
  else if (results->decode_type == NEC) {
    print("Decoded NEC: ");
  } 
  else if (results->decode_type == SONY) {
    print("Decoded SONY: ");
  } 
  else if (results->decode_type == RC5) {
    print("Decoded RC5: ");
  } 
  else if (results->decode_type == RC6) {
    print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {	
    print("Decoded PANASONIC - Address: ");
    print(results->panasonicAddress,HEX);
    print(" Value: ");
  }
  else if (results->decode_type == JVC) {
     print("Decoded JVC: ");
  }
  print(results->value, HEX);
  print(" (");
  print(results->bits, DEC);
  println(" bits)");
  print("Raw (");
  print(count, DEC);
  print("): ");

  for (int i = 0; i < count; i++) {
    if ((i % 2) == 1) {
      print(results->rawbuf[i]*USECPERTICK, DEC);
    } 
    else {
      print(-(int)results->rawbuf[i]*USECPERTICK, DEC);
    }
    print(" ");
  }
  println("");
}


void loop() {
  if (irrecv.decode(&results)) {
    println("Code: ");
    println(results.value, HEX);
    dump(&results);
    irrecv.resume(); // Receive the next value
  }
}

// These "print" wrappers just output to BOTH serial ports
// but you probably just want to use the Usb serial...
void println(char*s)
{
  Usb.println(s);
}

void print(char*s)
{
  Usb.print(s);
}
void print(int i,char format)
{
  Usb.print(i,format);
}

void println(int i,char format)
{
  Usb.println(i,format);
}
