/* Arduino DigitalPin Library
 * Copyright (C) 2012 by William Greiman
 *
 * This file is part of the Arduino DigitalPin Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino DigitalPin Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <DigitalIO.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
//------------------------------------------------------------------------------
/**
 * Set USE_ARDUINO_PIN_DB nonzero to use pin definitions from the Arduino
 * hardware/variants folders.
 */
#define USE_ARDUINO_PIN_DB 0
#if USE_ARDUINO_PIN_DB
#include <Arduino.h>
#else  // USE_ARDUINO_PIN_DB
//------------------------------------------------------------------------------
/**
 * \class PinBit_t
 * \brief struct to store port index and bit number in 8-bits.
 */
struct PinBit_t {
  unsigned pin : 4;  /**< index for pinAddress array */
  unsigned bit : 4;  /**< pin bit number */
};
//------------------------------------------------------------------------------
#if defined(__AVR_ATmega168__)\
||defined(__AVR_ATmega168P__)\
||defined(__AVR_ATmega328P__)
// 168 and 328 Arduinos

// port register addresses
static volatile uint8_t* pinAddress[] PROGMEM = {&PINB, &PINC, &PIND};

// port address indices
static const uint8_t PB = 0;
static const uint8_t PC = PB + 1;
static const uint8_t PD = PC + 1;

// port index and bit number for each pin
static PinBit_t pinBit[] PROGMEM = {
 {PD, 0},  // D0  0
 {PD, 1},  // D1  1
 {PD, 2},  // D2  2
 {PD, 3},  // D3  3
 {PD, 4},  // D4  4
 {PD, 5},  // D5  5
 {PD, 6},  // D6  6
 {PD, 7},  // D7  7
 {PB, 0},  // B0  8
 {PB, 1},  // B1  9
 {PB, 2},  // B2 10
 {PB, 3},  // B3 11
 {PB, 4},  // B4 12
 {PB, 5},  // B5 13
 {PC, 0},  // C0 14
 {PC, 1},  // C1 15
 {PC, 2},  // C2 16
 {PC, 3},  // C3 17
 {PC, 4},  // C4 18
 {PC, 5}   // C5 19
};
//------------------------------------------------------------------------------
#elif defined(__AVR_ATmega1280__)\
|| defined(__AVR_ATmega2560__)
// Mega
// port register addresses
static volatile uint8_t* pinAddress[] PROGMEM = {
  &PINA, &PINB, &PINC, &PIND, &PINE, &PINF,
  &PING, &PINH, &PINJ, &PINK, &PINL};
  
// port address indices
static const uint8_t PA = 0;
static const uint8_t PB = PA + 1;
static const uint8_t PC = PB + 1;
static const uint8_t PD = PC + 1;
static const uint8_t PE = PD + 1;
static const uint8_t PF = PE + 1;
static const uint8_t PG = PF + 1;
static const uint8_t PH = PG + 1;
static const uint8_t PJ = PH + 1;
static const uint8_t PK = PJ + 1;
static const uint8_t PL = PK + 1;

// port index and bit number for each pin
static PinBit_t pinBit[] PROGMEM = {
  {PE, 0},  // E0  0
  {PE, 1},  // E1  1
  {PE, 4},  // E4  2
  {PE, 5},  // E5  3
  {PG, 5},  // G5  4
  {PE, 3},  // E3  5
  {PH, 3},  // H3  6
  {PH, 4},  // H4  7
  {PH, 5},  // H5  8
  {PH, 6},  // H6  9
  {PB, 4},  // B4 10
  {PB, 5},  // B5 11
  {PB, 6},  // B6 12
  {PB, 7},  // B7 13
  {PJ, 1},  // J1 14
  {PJ, 0},  // J0 15
  {PH, 1},  // H1 16
  {PH, 0},  // H0 17
  {PD, 3},  // D3 18
  {PD, 2},  // D2 19
  {PD, 1},  // D1 20
  {PD, 0},  // D0 21
  {PA, 0},  // A0 22
  {PA, 1},  // A1 23
  {PA, 2},  // A2 24
  {PA, 3},  // A3 25
  {PA, 4},  // A4 26
  {PA, 5},  // A5 27
  {PA, 6},  // A6 28
  {PA, 7},  // A7 29
  {PC, 7},  // C7 30
  {PC, 6},  // C6 31
  {PC, 5},  // C5 32
  {PC, 4},  // C4 33
  {PC, 3},  // C3 34
  {PC, 2},  // C2 35
  {PC, 1},  // C1 36
  {PC, 0},  // C0 37
  {PD, 7},  // D7 38
  {PG, 2},  // G2 39
  {PG, 1},  // G1 40
  {PG, 0},  // G0 41
  {PL, 7},  // L7 42
  {PL, 6},  // L6 43
  {PL, 5},  // L5 44
  {PL, 4},  // L4 45
  {PL, 3},  // L3 46
  {PL, 2},  // L2 47
  {PL, 1},  // L1 48
  {PL, 0},  // L0 49
  {PB, 3},  // B3 50
  {PB, 2},  // B2 51
  {PB, 1},  // B1 52
  {PB, 0},  // B0 53
  {PF, 0},  // F0 54
  {PF, 1},  // F1 55
  {PF, 2},  // F2 56
  {PF, 3},  // F3 57
  {PF, 4},  // F4 58
  {PF, 5},  // F5 59
  {PF, 6},  // F6 60
  {PF, 7},  // F7 61
  {PK, 0},  // K0 62
  {PK, 1},  // K1 63
  {PK, 2},  // K2 64
  {PK, 3},  // K3 65
  {PK, 4},  // K4 66
  {PK, 5},  // K5 67
  {PK, 6},  // K6 68
  {PK, 7}   // K7 69
};
//------------------------------------------------------------------------------
#elif defined(__AVR_ATmega644P__)\
|| defined(__AVR_ATmega644__)\
|| defined(__AVR_ATmega1284P__)
// Sanguino

// port register addresses
static volatile uint8_t* pinAddress[] PROGMEM =
  {&PINA, &PINB, &PINC, &PIND};

// port address indices
static const uint8_t PA = 0
static const uint8_t PB = PA + 1;
static const uint8_t PC = PB + 1;
static const uint8_t PD = PC + 1;

// port index and bit number for each pin
static PinBit_t pinBit[] PROGMEM = {
  {PB, 0},  // B0  0
  {PB, 1},  // B1  1
  {PB, 2},  // B2  2
  {PB, 3},  // B3  3
  {PB, 4},  // B4  4
  {PB, 5},  // B5  5
  {PB, 6},  // B6  6
  {PB, 7},  // B7  7
  {PD, 0},  // D0  8
  {PD, 1},  // D1  9
  {PD, 2},  // D2 10
  {PD, 3},  // D3 11
  {PD, 4},  // D4 12
  {PD, 5},  // D5 13
  {PD, 6},  // D6 14
  {PD, 7},  // D7 15
  {PC, 0},  // C0 16
  {PC, 1},  // C1 17
  {PC, 2},  // C2 18
  {PC, 3},  // C3 19
  {PC, 4},  // C4 20
  {PC, 5},  // C5 21
  {PC, 6},  // C6 22
  {PC, 7},  // C7 23
  {PA, 7},  // A7 24
  {PA, 6},  // A6 25
  {PA, 5},  // A5 26
  {PA, 4},  // A4 27
  {PA, 3},  // A3 28
  {PA, 2},  // A2 29
  {PA, 1},  // A1 30
  {PA, 0}   // A0 31
};
//------------------------------------------------------------------------------
#elif defined(__AVR_ATmega32U4__)

// port register addresses
static volatile uint8_t* pinAddress[] PROGMEM =
  {&PINB, &PINC, &PIND, &PINF};

// port address indices
static const uint8_t PB = 0;
static const uint8_t PC = PB + 1;
static const uint8_t PD = PC + 1;
static const uint8_t PF = PD + 1;
//------------------------------------------------------------------------------
#ifdef CORE_TEENSY
// Teensy 2.0

// port index and bit number for each pin
static PinBit_t pinBit[] PROGMEM = {
  {PB, 0},  // B0  0
  {PB, 1},  // B1  1
  {PB, 2},  // B2  2
  {PB, 3},  // B3  3
  {PB, 7},  // B7  4
  {PD, 0},  // D0  5
  {PD, 1},  // D1  6
  {PD, 2},  // D2  7
  {PD, 3},  // D3  8
  {PC, 6},  // C6  9
  {PC, 7},  // C7 10
  {PD, 6},  // D6 11
  {PD, 7},  // D7 12
  {PB, 4},  // B4 13
  {PB, 5},  // B5 14
  {PB, 6},  // B6 15
  {PF, 7},  // F7 16
  {PF, 6},  // F6 17
  {PF, 5},  // F5 18
  {PF, 4},  // F4 19
  {PF, 1},  // F1 20
  {PF, 0},  // F0 21
  {PD, 4},  // D4 22
  {PD, 5},  // D5 23
  {PE, 6}   // E6 24
};
//------------------------------------------------------------------------------
#else  // CORE_TEENSY
// Leonardo

// port index and bit number for each pin
static PinBit_t pinBit[] PROGMEM = {
  {PD, 2},  // D2  0
  {PD, 3},  // D3  1
  {PD, 1},  // D1  2
  {PD, 0},  // D0  3
  {PD, 4},  // D4  4
  {PC, 6},  // C6  5
  {PD, 7},  // D7  6
  {PF, 6},  // F6  7
  {PB, 4},  // B4  8
  {PB, 5},  // B5  9
  {PB, 6},  // B6 10
  {PB, 7},  // B7 11
  {PD, 6},  // D6 12
  {PC, 7},  // C7 13
  {PB, 3},  // B3 14
  {PB, 1},  // B1 15
  {PB, 2},  // B2 16
  {PB, 0},  // B0 17
  {PF, 7},  // F7 18
  {PF, 6},  // F6 19
  {PF, 5},  // F5 20
  {PF, 4},  // F4 21
  {PF, 1},  // F1 22
  {PF, 0},  // F0 23
  {PD, 4},  // D4 24
  {PD, 7},  // D7 25
  {PB, 4},  // B4 26
  {PB, 5},  // B5 27
  {PB, 6},  // B6 28
  {PD, 6}   // D6 29
};
#endif  // CORE_TEENSY
//------------------------------------------------------------------------------
#elif defined(__AVR_AT90USB646__)\
|| defined(__AVR_AT90USB1286__)
// Teensy++ 1.0 & 2.0

// port register addresses
static volatile uint8_t* pinAddress[] PROGMEM =
  {&PINA, &PINB, &PINC, &PIND, &PINE, &PINF};

// port address indices
static const uint8_t PA = 0;
static const uint8_t PB = PA + 1;
static const uint8_t PC = PB + 1;
static const uint8_t PD = PC + 1;
static const uint8_t PE = PD + 1;
static const uint8_t PF = PE + 1;

// port index and bit number for each pin
static PinBit_t pinBit[] PROGMEM = {
  {PD, 0},  // D0  0
  {PD, 1},  // D1  1
  {PD, 2},  // D2  2
  {PD, 3},  // D3  3
  {PD, 4},  // D4  4
  {PD, 5},  // D5  5
  {PD, 6},  // D6  6
  {PD, 7},  // D7  7
  {PE, 0},  // E0  8
  {PE, 1},  // E1  9
  {PC, 0},  // C0 10
  {PC, 1},  // C1 11
  {PC, 2},  // C2 12
  {PC, 3},  // C3 13
  {PC, 4},  // C4 14
  {PC, 5},  // C5 15
  {PC, 6},  // C6 16
  {PC, 7},  // C7 17
  {PE, 6},  // E6 18
  {PE, 7},  // E7 19
  {PB, 0},  // B0 20
  {PB, 1},  // B1 21
  {PB, 2},  // B2 22
  {PB, 3},  // B3 23
  {PB, 4},  // B4 24
  {PB, 5},  // B5 25
  {PB, 6},  // B6 26
  {PB, 7},  // B7 27
  {PA, 0},  // A0 28
  {PA, 1},  // A1 29
  {PA, 2},  // A2 30
  {PA, 3},  // A3 31
  {PA, 4},  // A4 32
  {PA, 5},  // A5 33
  {PA, 6},  // A6 34
  {PA, 7},  // A7 35
  {PE, 4},  // E4 36
  {PE, 5},  // E5 37
  {PF, 0},  // F0 38
  {PF, 1},  // F1 39
  {PF, 2},  // F2 40
  {PF, 3},  // F3 41
  {PF, 4},  // F4 42
  {PF, 5},  // F5 43
  {PF, 6},  // F6 44
  {PF, 7}   // F7 45
};
//------------------------------------------------------------------------------
#else  // CPU type
#error unknown CPU type
#endif  // CPU type
#endif  // USE_ARDUINO_PIN_DB
//==============================================================================
/** constructor
 * \param[in] pin pin assigned to this object.
 */
DigitalIO::DigitalIO(uint8_t pin) {
  begin(pin);
}
//------------------------------------------------------------------------------
/** Initialize pin bit mask and port address.
 * \param[in] pin Arduino board pin number.
 * \return true for success or false if invalid pin number.
 */
bool DigitalIO::begin(uint8_t pin) {
#if USE_ARDUINO_PIN_DB
  if (pin >= NUM_DIGITAL_PINS) return false;
  uint8_t port = digitalPinToPort(pin);
  pinReg_ = portInputRegister(port);
  bit_ = digitalPinToBitMask(pin);
#else  // USE_ARDUINO_PIN_DB
  union {
    uint8_t u8;
    PinBit_t pb;
  } u;
  if (pin >= sizeof(pinBit)) return false;
  u.u8 = pgm_read_byte(&pinBit[pin]);
  bit_ = 1 << u.pb.bit;
  pinReg_ = (volatile uint8_t*)pgm_read_word(pinAddress + u.pb.pin);
#endif  // USE_ARDUINO_PIN_DB
  mask_ = ~bit_;
  portReg_ = pinReg_ + 2;
  return true;
}
//------------------------------------------------------------------------------
/** Configure the pin
 *
 * \param[in] mode Configure as output mode if true else input mode.
 * \param[in] data For output mode set pin high if true else low.
 *                 For input mode enable 20K pullup if true else Hi-Z.
 *
 * This function may be used with interrupts enabled or disabled.
 * The previous interrupt state will be restored.
 */
void DigitalIO::config(bool mode, bool data) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    modeISR(mode);
    writeISR(data);
  }
}
//------------------------------------------------------------------------------
/** Set pin mode
 *
 * \param[in] pinMode If true, output mode else input mode.
 *
 * This function may be used with interrupts enabled or disabled.
 * The previous interrupt state will be restored.
 */
void DigitalIO::modeRestore(bool pinMode) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    modeISR(pinMode);
  }
}
//==============================================================================
/**
 * Set pin level high if output mode or enable 20K pullup if input mode.
 *
 * This function may be used with interrupts enabled or disabled.
 * The previous interrupt state will be restored.
 */
void DigitalIO::highRestore(){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    highISR();
  }
}
//------------------------------------------------------------------------------
/**
 * Set pin level low if output mode or disable 20K pullup if input mode.
 *
 * This function may be used with interrupts enabled or disabled.
 * The previous interrupt state will be restored.
 */
void DigitalIO::lowRestore(){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    lowISR();
  }
}
//------------------------------------------------------------------------------
/**  Write pin.
 * \param[in] level If output mode set pin high if true else low.
 * If input mode enable 20K pullup if true else disable pullup.
 *
 * This function may be used with interrupts enabled or disabled.
 * The previous interrupt state will be restored.
 */
void DigitalIO::writeRestore(bool level) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    writeISR(level);
  }
}