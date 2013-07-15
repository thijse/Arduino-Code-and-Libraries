#ifndef DigitalIO_h
#define DigitalIO_h
#include <util/atomic.h>
#include <avr/io.h>
//------------------------------------------------------------------------------
/** DigitalIO version YYYYMMDD */
#define DIGITAL_IO_VERSION 20120804
//------------------------------------------------------------------------------
/**
 * \class DigitalIO
 * \brief Digital AVR port I/O with runtime pin number.
 */
class DigitalIO {
 public:
  /** Create a DigitalIO object with no assigned pin. */
  DigitalIO() : bit_(0), mask_(0XFF) {}
  explicit DigitalIO(uint8_t pin);
  bool begin(uint8_t pin);
  void config(bool mode, bool data);
//  void mode(bool pinMode);
  //----------------------------------------------------------------------------
  /** \return Pin's level */
  inline __attribute__((always_inline))
  bool read() {return *pinReg_ & bit_;}
  //----------------------------------------------------------------------------
  /** toggle a pin
   *
   * If the pin is in output mode toggle the pin's level.
   * If the pin is in input mode toggle the state of the 20K pullup.
   */
  inline __attribute__((always_inline))
  void toggle() {*pinReg_ = bit_;}
  //============================================================================
  /**
   * Set pin high if output mode or enable 20K pullup if input mode.
   *
   * This function must be called with interrupts disabled.
   * This function will not change the interrupt state.
   */
  inline __attribute__((always_inline))
  void highISR() {writeISR(1);}
  /**
   * Set pin low if output mode or disable 20K pullup if input mode.
   *
   * This function must be called with interrupts disabled.
   * This function will not change the interrupt state.
   */
  inline __attribute__((always_inline))
  void lowISR() {writeISR(0);}
  /**
   * Set pin mode
   * \param[in] mode if true set output mode else input mode.
   *
   * mode() does not enable or disable the 20K pullup for input mode.
   *
   * This function must be called with interrupts disabled.
   * This function will not change the interrupt state.
   */
  inline __attribute__((always_inline))
  void modeISR(bool mode) {
    volatile uint8_t* ddrReg = pinReg_ + 1;
    *ddrReg = mode ? *ddrReg | bit_ : *ddrReg & mask_;
  }
  /**  Write pin.
   *
   * \param[in] level If output mode set pin high if true else low.
   * If input mode enable 20K pullup if true else disable pullup.
   *
   * This function must be called with interrupts disabled.
   * This function will not change the interrupt state.
   */
  inline __attribute__((always_inline))
  void writeISR(bool level) {
    *portReg_ = level ? *portReg_ | bit_ : *portReg_ & mask_;
  }
  //============================================================================
  /**
   * Set pin level high if output mode or enable 20K pullup if input mode.
   *
   * This function will enable interrupts.  This function should not be
   * called in an ISR or where interrupts are disabled.
   */
  inline __attribute__((always_inline))
  void high() {ATOMIC_BLOCK(ATOMIC_FORCEON) {highISR();}}
  /**
   * Set pin level low if output mode or disable 20K pullup if input mode.
   *
   * This function will enable interrupts.  This function should not be
   * called in an ISR or where interrupts are disabled.
   */
  inline __attribute__((always_inline))
  void low(){ATOMIC_BLOCK(ATOMIC_FORCEON) {lowISR();}}
  /**
   * Set pin mode
   * \param[in] pinMode if true set output mode else input mode.
   *
   * mode() does not enable or disable the 20K pullup for input mode.
   *
   * This function will enable interrupts.  This function should not be
   * called in an ISR or where interrupts are disabled.
   */
  inline __attribute__((always_inline))
  void mode(bool pinMode) {ATOMIC_BLOCK(ATOMIC_FORCEON) {modeISR(pinMode);}}
  /**  Write pin.
   *
   * \param[in] level If output mode set pin high if true else low.
   * If input mode enable 20K pullup if true else disable pullup.
   *
   * This function will enable interrupts.  This function should not be
   * called in an ISR or where interrupts are disabled.
   */
  inline __attribute__((always_inline))
  void write(bool level) {ATOMIC_BLOCK(ATOMIC_FORCEON) {writeISR(level);}}
  //============================================================================
  void highRestore();
  void lowRestore();
  void modeRestore(bool mode);
  void writeRestore(bool level);
  
 private:
  uint8_t bit_;
  uint8_t mask_;
  volatile uint8_t* pinReg_;
  volatile uint8_t* portReg_;
};
#endif  // DigitalIO_h