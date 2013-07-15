#ifndef lightuinoPwmH
#define lightuinoPwmH

#include "lightuinoSink.h"

// #define SAFEMODE

//? <const name="CCShield_MAX_BRIGHTNESS">Maximum number of gradations of brightness</const>
#define CCShield_MAX_BRIGHTNESS (4096*2)
//? <const name="Lightuino_MAX_BRIGHTNESS">Maximum number of gradations of brightness</const>
#define Lightuino_MAX_BRIGHTNESS (4096*2)

//? <const name="Lightuino_MIN_INTENSITY">
// This is the mininum intensity to set for flickerless viewing.  It is subjective, and also depends upon
// how much time you are spending doing other things.  The LED will be OFF for MIN_INTENSITY/MAX_BRIGHTNESS amount of time
// When this is (say) 1/256, you can see the LED flicker since it only blinks once per 256 loop iterations!
// Strangely, the amount of visible flicker also depends upon how much current you put across the LEDs (ie. the "brightness" selector)</const>
#define CCShield_MIN_INTENSITY 17  
#define Lightuino_MIN_INTENSITY 17  

//?<class name="FlickerBrightness">
// The FlickerBrightness class changes the apparent brightness of individual LEDs by turning them off and on rapidly.
// This technique is called PWM (pulse-width modulation) and if done fast enough, the fact of persistence of vision means
// that the flickering is not seen.
// The "loop" function must be called periodically and rapidly.  If the LEDs appear to flicker, "loop" should be called more
// often (or do not make the LEDs that dim).
class FlickerBrightness
{
  private:
  static void doLoop();			 // Static call to loop()
  
  public:
  //? <ctor>The constructor takes a Lightuino object</ctor>
  FlickerBrightness(LightuinoSink& brd);
  ~FlickerBrightness();
  
  //? <method>This function performs a marquee function by shifting which intensity corresponds with which actual output in Lightuino</method>
  void shift(char amt=1); 
   
  //? <method>Call this function periodically and rapidly to blink the LEDs and therefore create the illusion of brightness</method>
  void loop(void);
  
  //? <method> Call the loop() function automatically in the background...</method>
  void StartAutoLoop(int rate=4096);
  
  //? <method> Stop automatic looping</method>
  void StopAutoLoop(void);
  
  //? <var>Set this variable to the desired brightness, indexed by the LED you want to control</var>
  int brightness[Lightuino_NUMOUTS];
  
  //? <var>Set this variable to force a cutoff to zero below this brightness level.  This stops a perception of flickering</var>
  int minBrightness;
  
  // Private
  //int bresenham[Lightuino_NUMOUTS];
  char offset;
  unsigned int frame;
  LightuinoSink& brd;
  FlickerBrightness* next;
};
//?</class>

#define LightuinoPwm FlickerBrightness

//? <class>The ChangeBrightness class modifies the brightness of the LEDs smoothly and linearly through a desired
// brightness range.
class ChangeBrightness
{
  public:
  //?<method> Pass the constructor a FlickerBrightness object and also a callback (optional).  When this class finished moving a LED
  // to the desired brightness, it will call the callback so you can program a new intensity target in.</method>
  ChangeBrightness(FlickerBrightness& thebrd, void (*doneCallback)(ChangeBrightness& me, int led)=0);
  
  /*?<method> Transition the light at index 'led' to intensity over count iterations</method> */
  void set(uint8_t led, uint16_t intensity, int count);
  
  //?<method> Call loop periodically and rapidly to change the brightness.  This function calls FlickerBrightness.loop(), so
  // it is unnecessary for you to do so.</method>
  void loop(void);
 
  // semi-private variables 
  FlickerBrightness& brd;
  int  change[Lightuino_NUMOUTS];
  int  count[Lightuino_NUMOUTS];
  uint16_t destination[Lightuino_NUMOUTS];  
  int  bresenham[Lightuino_NUMOUTS];
  void (*doneCall)(ChangeBrightness& me, int led);
};
//?</class>

#endif
