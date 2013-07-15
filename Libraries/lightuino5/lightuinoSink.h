/*? <section name="lightuino">
 * Lightuino, CCShield, and M5451 LED driver
 * Version 2.0 Author: G. Andrew Stone
 * Created March, 2009
 * Released under the MIT license (http://www.opensource.org/licenses/mit-license.php)
 *
 * This is an Arduino Library, to install see: http://www.arduino.cc/en/Hacking/LibraryTutorial
 * Quick Installation instructions:
 * Copy this file and the CCShield.h file to a new directory called
 * "CCShield" in the hardware\libraries directory in your arduino installation.  
 * Then restart the Arduino IDE.
 *
 * For example: 
 * mkdir c:\arduino-0017\hardware\libraries\CCShield
 * copy *.* c:\arduino-0017\hardware\libraries\CCShield
 */

#ifndef CCSHIELD_H
#define CCSHIELD_H


//#include "WConstants.h"
#include "avr/pgmspace.h"
#include <inttypes.h>

//?<section name="constants">

//?<const name="M5451_NUMOUTS">The number of output pins on the M5451 chip</const>
#define M5451_NUMOUTS 35

//?<const name="CCShield_NUMOUTS">The number of output pins on the shield board</const>
#define CCShield_NUMOUTS 70
//?<const name="Lightuino_NUMOUTS">The number of output pins on the Lightuino board</const>
#define Lightuino_NUMOUTS 70
//?<const name="Lightuino_NUM_SINKS">The number of output pins on the Lightuino board</const>
#define Lightuino_NUM_SINKS 70

//?<const name="CCShield_FASTSET">Flag selecting the algorithm used to send data to the M5451 chips</const>
#define CCShield_FASTSET 1
//?<const name="CCShield_BY32">Flag selecting the mapping between memory bits and M5451 output lines</const>
#define CCShield_BY32 2

//?<const name="Lightuino_FASTSET">Flag selecting the algorithm used to send data to the M5451 chips</const>
#define Lightuino_FASTSET 1
//?<const name="Lightuino_BY32">Flag selecting the mapping between memory bits and M5451 output lines</const>
#define Lightuino_BY32 2

//?</section>

//?<section name="classes">

/*? <class name="LightuinoSink">
   This class provides the basic functions to control the M5451 chip; in particular, it drives 2 chips simultaneously
   which is what the CCShield and Lightuino boards are populated with.
*/
class LightuinoSink
 {
  public:
     
  //? <var name="flags" type="uint8_t">
  // What algorithm to use to clock the data out to the M5451 chips.  
  // You may choose a Lightuino_FASTSET which works only with 88,168 or 328 chipsets.  
  // If fastset is not selected, the code should work with any arduino clone (it uses digitalWrite).
  // However the speed may not be fast enough to PWM the Leds without a lot of flickering.
  // Lightuino_BY32 selects how the parameters in the set() call map to output lines.  In the BY32 case, the first
  // parameter to set handles lines 0-31 on the left connector, the second sets lines 0-31 on the right connector,
  // and the third parameter handles the leftover 6 lines.
  // If BY32 is NOT set, the parameters to the set() call are handled as if they were a single continuous bitmap.</var>
  uint8_t flags;
  //? <var name="clockPin"> What pin to use to as the M5451 clock (any digital pin can be selected on the board by solder blob short)</var>
  static const uint8_t clockPin = Lightuino_CLOCK_PIN;
  //? <var>What pin to use to to control brightness (any digital pin can be selected on the board by solder blob short)
  // note that to make brightness work, use a jumper to select software controller brightness instead of resistor controlled. </var>
  static const uint8_t brightPin = Lightuino_BRIGHTNESS_PIN;

  //? <var>What pins to use to send serial data to each M5451 (need 1 pin for each chip)
  // (any digital pin can be selected on the board by solder blob short)</var>
  static const uint8_t serDataPin1 = Lightuino_SER_DATA_LEFT_PIN;
  static const uint8_t serDataPin2 = Lightuino_SER_DATA_RIGHT_PIN;
  
  //? <var>Whether a set should "finish" it off or leave one bit left to clock in (call finish()) to clock that last bit)</var>
  uint8_t finishReq;
  
  //? <ctor>Constructor takes all of the pins needed.</ctor>
  LightuinoSink();
  
  //? <method>Turn on/off certain lines, using direct register access -- may not work on Arduino variants!
  // Parameters a,b,c are a bitmap; each bit corresponds to a particular M5451 output
  // The first 70 bits are used (i.e. all of parameter a and b, and just a few bits in parameter c.</method>
  void set(unsigned long int a, unsigned long int b, unsigned long int c);
      
  //? <method>The same set function conveniently taking an array</method>
  void set(unsigned long int a[3]);

  //? <method>The same set function conveniently taking a byte array (should be 9 bytes long)</method>
  void set(unsigned char* a);

  //? <method>Turn on/off certain lines, using direct register access -- may not work on Arduino variants!
  // Parameters a,b,c are a bitmap; each bit corresponds to a particular M5451 output
  // The first 70 bits are used (i.e. all of parameter a and b, and just a few bits in parameter c</method>
  void fastSet(unsigned long int a, unsigned long int b, unsigned long int c);
      
  //? <method>The same set function conveniently taking an array</method>
  void fastSet(unsigned long int a[3]);

  //? <method>This clocks in the last bit if finishReq is False -- the purpose of this is to leave the LEDs on for the maximum time in things like matrices</method>
  void finish(void);

  // <method>
  // Reorder the set bits as:
  // input[0] -> Chip 0 lines 0->31
  // input[1] -> Chip 1 lines 0->31
  // input[2] -> bits 0-2 chip 0 lines 32-34.  bits 3-5 chip 1 lines 32 - 34.</method>
  void fastSetBy32(unsigned long int input[3]);
  void fastSetBy32(unsigned long int left, unsigned long int right, unsigned long int overflow);
  void fastSetBy32(unsigned char* chars);
  
    // <method>Turn on/off certain lines, using digitalWrite.
  // Parameters a,b,c are a bitmap; each bit corresponds to a particular M5451 output
  // The first 70 bits are used (i.e. all of parameter a and b, and just a few bits in parameter c</method>
  void safeSet(unsigned long int a, unsigned long int b, unsigned long int c);
  void safeSet(unsigned long int a[3]);
  
  // <method>Set the overall brightness using the M5451 brightness selection (assuming its under software control)</method>
  void setBrightness(uint8_t b);
  void setupLookup();
  
  private:
  int mydelay(unsigned long int clk);
  uint8_t dpOn[4];
  uint8_t bpOn[4];
};
//?</class>

//?<class>
class CCShield:public LightuinoSink
{
 public:
  //?<ctor>Constructor takes all of the pins needed.</ctor>
 CCShield():LightuinoSink() {};

};
//?</class>

//?<class>
class Lightuino:public LightuinoSink
{
 public:
  //?<ctor>Constructor takes all of the pins needed.</ctor>
 Lightuino(uint8_t serDataPin1,uint8_t serDataPin2):LightuinoSink() {};

};
//?</class>



//?<class>
//This class allows you to define a bit pattern in FLASH memory and then it will run through that pattern.
//A "pattern-compiler" Google-gadget exists <a href="http://code.google.com/p/arduino-m5451-current-driver/wiki/LedAnimationGenerator">here</a>
class AniPattern
{
  public:
  AniPattern(Lightuino& shld, prog_uchar* anim,prog_uint16_t* delayLst, int total_frames): shield(shld) {flags=0; delays=delayLst; ani=anim; numFrames=total_frames; curFrame=0; curDelay=0; anidir=1;}

  Lightuino& shield;
  int curFrame;
  int curDelay;
  int numFrames;
  int anidir;
  uint8_t flags;
  prog_uchar* ani;
  prog_uint16_t* delays;
  
  void setBackForth(uint8_t yes=1) { if (yes) flags |= 1; else flags &= ~1; }
  void setReverse(uint8_t yes =1) { if (yes) anidir = -1; else anidir = 1; }
  
  void setPos(int i) { if (i<numFrames) curFrame = i;}
  void next(void);
};
//?</class>

//?</section>

//?<section name="functions">
// <fn>This function sets a particular bit by index in a bitmap contained in parameters a,b,c
// you can then pass these into the Lightuino "set" function. </fn>
void setbit(unsigned char offset,unsigned long int* a, unsigned long int* b, unsigned char* c);

//? <fn>This function clears a particular bit by index in a bitmap contained in parameters a,b,c
// you can then pass these into the Lightuino "set" function. </fn>
void clearbit(unsigned char offset,unsigned long int* a, unsigned long int* b, unsigned char* c);

//? <fn>This function reverses the bits in a 16 bit integer. </fn>
unsigned int reverse16bits(unsigned int x);

//?</section>

//?</section>
#endif
