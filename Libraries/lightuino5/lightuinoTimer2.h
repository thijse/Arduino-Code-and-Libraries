#ifndef LightuinoTimer2_h
#define LightuinoTimer2_h

namespace LightuinoTimer2 {
	extern unsigned char timerLoadValue;
	int prescalerToMultiplier(int myReg);
	int calcPrescaler(float rate);
	extern void (*func)();	
	void set(void (*f)());
	void start(float rate);
	void stop();
}

#endif
