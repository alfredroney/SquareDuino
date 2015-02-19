// Oscillator.cpp
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// We will manipulate the timers directly in this sketch,
// so we use the AVR headers.
// For a timer primer, turn to these tutorials:
// http://www.engblaze.com/we-interrupt-this-program-to-bring-you-a-tutorial-on-arduino-interrupts/
// http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/
//

#include "Oscillator.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "MIDIShield.h"

#include "DebugSerial.h"

#if DEBUGSERIAL
#include <SoftwareSerial.h>
extern DEBUGSERIALTYPE DEBUGSERIALNAME;
#endif

const byte Oscillator::oscPin[numOscs] = {
  MIDIShield::Output::sub,
  MIDIShield::Output::subFifth,
  MIDIShield::Output::root,
  MIDIShield::Output::fifth
};

const byte Oscillator::oscPrd[numOscs] = { 
  Oscillator::subPrd, 
  Oscillator::subFifthPrd, 
  Oscillator::rootPrd, 
  Oscillator::fifthPrd 
};

Oscillator::Oscillator()
: basePeriod(maxPeriod) // initialize to OFF state
{
  resetOscs();
  
  for (byte i=0; i<numOscs; ++i)
  {
    pinMode(oscPin[i], OUTPUT);
  }
  
  pinMode(gateOutPin,    OUTPUT);
  pinMode(invGateOutPin, OUTPUT);
  
  digitalWrite(gateOutPin,    LOW );
  digitalWrite(invGateOutPin, HIGH);
}

void Oscillator::goOnline()
{
  DEBUGSERIALWRITE("Oscillator::goOnline()\r\n");
  
#if NOTDEBUGSERIAL
  cli();  // disable global interrupts

  // initialize Timer1 in CTC-mode (variable frequency)
  TCCR1A = 0;
  
  TIMSK1 = (1 << OCIE1A); // enable Timer1 compare interrupt

  TCCR1B = (1 << CS11) | (0 << CS10) // prescale by 8
         | (1 << WGM12); // turn on CTC mode

  OCR1A = basePeriod; // stuff the comparison register
  TCNT1 = 0; // clear the counter

  sei(); // re-enable global interrupts
#endif
}

void Oscillator::play(unsigned int inPeriod)
{
  if (basePeriod != inPeriod) 
  {
    basePeriod = inPeriod;

    digitalWrite(gateOutPin,    HIGH);
    digitalWrite(invGateOutPin, LOW );
    
    DEBUGSERIALWRITE("Oscillator::play(");
    DEBUGSERIALPRINT(int(inPeriod), DEC);
    DEBUGSERIALWRITE(")\r\n");
    
#if NOTDEBUGSERIAL
    cli();             // temporarily stop the timer interrupts
    resetOscs();       // reset all the voices
    OCR1A = basePeriod; // stuff the comparison register
    TCNT1 = 0;         // reset the hardware counter
    sei();             // resume timer interrupts
#endif
  }
}

void Oscillator::stop()
{ 
  play(maxPeriod); 
  digitalWrite(gateOutPin,    LOW );
  digitalWrite(invGateOutPin, HIGH);
}

void Oscillator::resetOscs()
{
  for (byte i=0; i<numOscs; ++i)
  {
    oscCnt[i] = 0;
  }
}


