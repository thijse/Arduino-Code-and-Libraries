#ifndef LightuinoIR
#define LightuinoIR

// Most IRs differentiate a 1 vs a 0 by the length of a pulse.  In other words, a "1" may turn the IR LED "on" for twice as long as a "0".
// The IR LED is turned off to break up the bits.  This is not entirely intuitive where you might think that a 1 is the IR-LED being "on" and a 0 is it being "off".

// These constants define how long in microseconds a 1 is vs a 0.
// The variation is the fudge factor.  For example a variation of 2 means that the signal's length can vary by + or - 1/2 of the speced ideal time.
// For example given ZEROTIME of 600, and a VARIATION of 2, signals from 300-900 will be interpreted as a 0
#define IR_ZEROTIME 600UL   // Actually 600
#define IR_ONETIME  1200UL  // Actually 1200
#define IR_VARIATION 2UL

// Some IR protocols indicate the start of a new code by a very long pulse.  STARTTIME specifies this.
// Its not strictly necessary to define this because the quiettime can also split up pulses, but getting this correct
// will allow the system to correctly interpret rapid signal.
#define IR_STARTTIME 5000UL

// QUIETTIME speccs how long to wait with no pulses before deciding that the code is complete.
#define IR_QUIETTIME 60000UL

// To determine the bits, should I measure the length of a pulse (1) or the length of the silence between pulses (0).  Typically its the length of a pulse.
// You can determine this by plotting the waveform and seeing whether the pulse or the silence varies (that's the SIGNAL_STATE).
// Each non-signal portion will essentially the same width (except perhaps for preambles and suffixes). 

#define Lightuino_IR_CODEBUFLEN 8
#define Lightuino_IR_CODETYPE unsigned long int

class IrReceiver
  {
    public:
    static char pin;
    static unsigned long int zeroTimeMin;
    static unsigned long int zeroTimeMax;
    static unsigned long int oneTimeMin;
    static unsigned long int oneTimeMax;
    static unsigned long int startTime;
    static unsigned long int quietTime;
    static unsigned int      variation;
    static char signalState;
    static Lightuino_IR_CODETYPE codes[Lightuino_IR_CODEBUFLEN];
    static char lastCode;
    static char firstCode;
    
    IrReceiver(unsigned long int ZeroTime=IR_ZEROTIME,unsigned long int OneTime=IR_ONETIME,unsigned long int StartTime=IR_STARTTIME,unsigned long int QuietTime=IR_QUIETTIME,unsigned int Variation=IR_VARIATION,char SignalState=-1,char pin=2);
    Lightuino_IR_CODETYPE read();
    
    /* Pass 0 to wake up on any IR code -- note that this will wake up spuriously due to random IR.
       However, you could use 0 to get back to your code which could do more complex wakeup logic */
    void sleepUntil(Lightuino_IR_CODETYPE wakeCode);

    
    ~IrReceiver();
  };


/* Note codes vary by the IR remote manufacturer.  These codes work with the JY-520 IR remote.
   You can run the IrDemo() in the Lightuino begin_here sketch to discover the codes on your
   remote.
*/
enum
{
  Lightuino_IR_MUTE = 0x80AA2A,
  Lightuino_IR_AVTV = 0x880A22A,
  Lightuino_IR_POWER = 0x20808A2A,
  Lightuino_IR_UP = 0x8280282A,
  Lightuino_IR_LEFT = 0x280882A2,
  Lightuino_IR_DOWN = 0x8A80202A,
  Lightuino_IR_RIGHT = 0xA80802A2,
  Lightuino_IR_MENU = 0xA2880822,
  Lightuino_IR_CHPLUS = 0xA280082A,
  Lightuino_IR_CHMINUS = 0xAA80002A,
  Lightuino_IR_DASHSLASH = 0xA20008AA,
  Lightuino_IR_CIRCLE = 0x8AAA2,
  Lightuino_IR_VOLMINUS = 0x2A80802A,
  Lightuino_IR_VOLPLUS = 0x2280882A,
  Lightuino_IR_0 = 0xAAAA,
  Lightuino_IR_1 = 0x80002AAA,
  Lightuino_IR_2 = 0x20008AAA,
  Lightuino_IR_3 = 0xA0000AAA,
  Lightuino_IR_4 = 0x800A2AA,
  Lightuino_IR_5 = 0x880022AA,
  Lightuino_IR_6 = 0x280082AA,
  Lightuino_IR_7 = 0xA80002AA,
  Lightuino_IR_8 = 0x200A8AA,
  Lightuino_IR_9 = 0x820028AA
};


#endif
