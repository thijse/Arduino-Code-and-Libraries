/*? <section name="lightuino">
 */ 
#include "lightuino.h"

#ifndef lightuinoSourceDriverH
#define lightuinoSourceDriverH

//?? The number of source driver lines available to you.
#define Lightuino_NUMSRCDRVR  16

//?<class name="LightuinoSourceDriver">
// This class controls the source drivers on the Lightuino board.  There are 16 source drivers which are capable of
// providing a maximum of 500mA of current each at the voltage that you select.  Note however these chips do not have
// the heat dissipation to turn them all on full power at the same time -- that is not their purpose.
// The onboard LM317 voltage regulator supports 1.5A. So in practice you are limited to a total of 1.5A.
class LightuinoSourceDriver
  {
    private:
	
	static const unsigned char clkPin=Lightuino_SRC_CLOCK_PIN;
	static const unsigned char dataPin=Lightuino_SRC_DATA_PIN;
	static const unsigned char strobePin=Lightuino_SRC_STROBE_PIN;
	static const unsigned char enaPin=Lightuino_SRC_ENABLE_PIN;
    public:
    //? <ctor>The constructor takes which digital pins are connected to the source driver chip(s).</ctor>
    LightuinoSourceDriver(unsigned char clkPin=Lightuino_SRC_CLOCK_PIN, unsigned char dataPin=Lightuino_SRC_DATA_PIN, unsigned char strobePin=Lightuino_SRC_STROBE_PIN, unsigned char enaPin=Lightuino_SRC_ENABLE_PIN);

    //? <method> Pass in a 16 bit number.  Bit 0 controls whether driver line 0 is on, bit one for driver line 
    // one and so on. </method>
    void set(unsigned int bitMap, unsigned int depth=16);
    void shifts(unsigned int bit, unsigned int depth);
	
    //? <method> Line 0 gets the passed "bit" state.  line 1 gets line 0's state, 2 gets 1 and so forth.</method>
    void shift(unsigned char bit);
    //? <method> Turn them all off.  The state is retained... "output enable" is simply turned off</method>
    void off(void);
    //? <method> Turn them all on.  "output enable" is simply turned on.  Note that shift and set automatically
    //  do this. </method>
    void on(void);
  };
//?</class>

//? <class name="Mic5891">This is the same class as LightuinoSourceDriver, just named by the chip used.</class>
#define Mic5891 LightuinoSourceDriver

#endif
//?</section>


