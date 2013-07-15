#ifndef lightuinoAniH
#define lightuinoAniH

#include "lightuinoPwm.h"

/*?<class name="Ani">
  This is an abstract base class describing the state of an animation.  
  This subsystem attempts to solve the problem of animation reuse.  There is a problem with reusing animations -- you
  can only run a single one at a time because the Arduino is not threaded. 
  But with Ani objects you call "loop" to tick forward one hop in the animation.  Therefore your code can call "loop"
  on multiple Ani objects within your main loop to run multiple animations simultaneously.
  <html>
  A typical use is shown below.  In the first line the object is created.
  Then it is drawn for the first time.  Next we enter the animation loop.  Finally it is erased.
  <pre>
     AniWiper w(leds,0,32,100,CCShield_MAX_BRIGHTNESS/2-1);
     
     w.draw();
     for (int i=0;i!=100;i++)
     {
       w.loop();
       mydelay(100-i);
     }
     w.erase();
  </pre>
  </html>
*/
class Ani
  {
    public:
    Ani* lst;
    //?<method> Move forward one frame</method>
    virtual void next(void);
    //?<method>Undo a light setting</method>
    virtual void erase(void);
    //?<method>Do a light setting</method>
    virtual void draw(void);   

    //?<method> Execute a single "step".  This consists of: erase(), next(), draw(), and is the function you should call within your loop.</method>
    virtual void loop(void);
  };
//?</class>

//?<class name="AniSweep">
//This class just lights LEDs sequentially from some start position to some end position.
//When it gets to the end it starts over.  Either direction is possible.
class AniSweep: public Ani
{
  public:
    FlickerBrightness& leds;
    char start, end;
    char delayTime;
    int intensityChange;
    
    char cpos;
    char dir;
    
    AniSweep(FlickerBrightness& pleds, char pstart, char pend, char pdelayTime,int pintensityChange);
    
    virtual void erase(void);
    virtual void next(void);
    virtual void draw(void);
};
//?</class>

//?<class name="AniWiper">
//This class just lights LEDs sequentially from some start position to some end position.
//But unlike <ref>AniSweep</ref> when it gets to the end turns around and goes back (like a windshield wiper).
class AniWiper: public Ani
  {
    public:
    FlickerBrightness& leds;
    char start, end;
    char delayTime;
    int intensityChange;
    
    char cpos;
    char dir;
    AniWiper(FlickerBrightness& pleds, char pstart, char nleds, char pdelayTime,int pintensityChange);
      
    virtual void erase(void);
    virtual void next(void);
    virtual void draw(void);
  };
//?</class>


//?<section name="functions">
//? <fn>This function draws the current frame of all the animations in the passed Ani tree</fn>
void AniDraw(Ani& pani);
//? <fn>This function moves to the next frame of all the animations in the passed Ani tree</fn>
void AniNext(Ani& pani);
//? <fn>This function erases the current frame of all the animations in the passed Ani tree</fn>
void AniErase(Ani& pani);

//? <fn>This function does an Erase/Next/Draw triplet all the animations in the passed Ani tree</fn>
void AniLoop(Ani& pani);

//?</section>
#endif
