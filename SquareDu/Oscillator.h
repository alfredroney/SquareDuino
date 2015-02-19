#ifndef OSCILLATOR_H_ABR20150101
#define OSCILLATOR_H_ABR20150101
//
// Oscillator.h
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// This class encapsulates using the high-precision (16-bit) hardware timer in
// CTC ("Clear Timer on Compare match") mode. When the timer counter matches
// the stored value, it is reset to zero on the next cycle and an Interrupt
// Service Routine ("ISR") is called. In the ISR, we use running cycle counts
// to generate several harmonically-related tones by toggling output pins.
//
// A gate signal that indicates when a note is playing is also output, along
// with its logical negation. These can be used to trigger ADSR processors,
// gates, or whatever else you might like to syncronize with the notes that
// are played.
//
#include <Arduino.h>

#include "MIDIShield.h"

class Oscillator
{
public:
  
  Oscillator();
  
  void goOnline();
  
  void play(unsigned int inPeriod);
  
  void stop(); // causes all oscillator outputs to go LOW
  
  void doUpdate();
  
  
private:

  // Defines some useful constants.
  enum {
    numOscs = 4,
  
    rootPrd     = 3, // divide by three to get the root
    fifthPrd    = 2, // divide by two to get the perfect fifth
    subPrd      = 6, // divide by six to get the sub-octave
    subFifthPrd = 4, // divide by four to get the sub's perfect fifth
    
    // Write CV gate signal on the LED pull-down pins.
    gateOutPin    = MIDIShield::StatusLED::led1, // red, pull-down
    invGateOutPin = MIDIShield::StatusLED::led2, // green, pull-down
    
    maxPeriod = 0xffff // use to indicate OFF state of oscillator
  };
  
  static const byte oscPin[numOscs]; // the physical pin toggled
  static const byte oscPrd[numOscs]; // multiple of the base period
  
  byte oscCnt[numOscs]; // current divided oscillator counts
  
  unsigned int basePeriod; // base rate runs at 3x the fundamental

  inline bool noteIsPlaying() const { return basePeriod != maxPeriod; }
  
  void resetOscs();
};

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// Defined in the header to facilitate inlining. At each interrupt, loop over the
// voices and increment their counters. If the count equals the period of the
// given oscillator, toggle the output state of the corresponding pin.
//
// If a note is not playing, just reset the oscillators. Don't waste cycles checking
// if they are already cleared.
//
inline void Oscillator::doUpdate()
{
  if (noteIsPlaying())
  {
    for (byte i=0; i<numOscs; ++i)
    {
      if (not oscCnt[i])
      {
        digitalWrite(oscPin[i], not digitalRead(oscPin[i]));
      }
      
      oscCnt[i] += 1;
      
      if (oscCnt[i] == oscPrd[i])
      {
        oscCnt[i] = 0;
      }
    }
  }
  else
  {
    resetOscs();
  }
}

#define OSCILLATOR_CREATE_INSTANCE() Oscillator oscillator; \
ISR(TIMER1_COMPA_vect) { oscillator.doUpdate(); }

#endif // #ifndef OSCILLATOR_H_ABR20150101

