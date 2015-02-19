// CVGenerator.cpp
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.

// This class uses the two low-precision timers to generate control voltages via
// PWM at a frequency well outside of the audio band, relaxing the lowpass filter
// requirements.
//

#include "CVGenerator.h"

#include "MIDIShield.h"

#include "DebugSerial.h"

#if DEBUGSERIAL
#include <SoftwareSerial.h>
extern DEBUGSERIALTYPE DEBUGSERIALNAME;
#endif

CVGenerator::CVGenerator()
: modWheelValue(0)
, expPedalValue(0)
{
}

void CVGenerator::goOnline()
{
  DEBUGSERIALWRITE("CVGenerator::goOnline()\r\n");

#if NOTDEBUGSERIAL
  pinMode(MIDIShield::Output::cv0, OUTPUT);
  pinMode(MIDIShield::Output::cv1, OUTPUT);

  cli();  // disable global interrupts
#endif

  updateCVOutputs(); // compute new values and stuff the comparison registers

#if NOTDEBUGSERIAL

  // set up the two 8-bit timers for "Fast PWM Mode"
  TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
  TCCR2B = (1 << CS20);

  sei(); // re-enable global interrupts
#endif
}

// These only compute and update the timer comparison registers
// when the value changes. If a defective or poorly-programmed
// controller sends a steady stream of identical controller
// messages, we ignore all but the first.
void CVGenerator::setModWheelValue(byte inValue)
{
  if (modWheelValue != inValue)
  {
    DEBUGSERIALWRITE("CVGenerator::setModWheelValue(");
    DEBUGSERIALPRINT(int(inValue), DEC);
    DEBUGSERIALWRITE(")\r\n");
    
    modWheelValue = inValue;
 
    updateCVOutputs();
  }
}

void CVGenerator::setExpPedalValue(byte inValue)
{
  if (expPedalValue != inValue)
  {
    DEBUGSERIALWRITE("CVGenerator::setExpPedalValue(");
    DEBUGSERIALPRINT(int(inValue), DEC);
    DEBUGSERIALWRITE(")\r\n");
    
    expPedalValue = inValue;
    
    updateCVOutputs();
  }
}

void CVGenerator::updateCVOutputs()
{
  byte cv0 = 0, cv1 = 0;
  
  computeNewCVOutputs(cv0, cv1);
  
#if NOTDEBUGSERIAL
  // Stuff the "Outut Compare Register" used to generate the
  // PWM output for timers A and B. If the counter value in 
  // the matching hardware timer is < the stored value, the
  // output pin attached to this timer is set high. Since the
  // frequency is very high, we don't worry about the occasional
  // glitch due to setting OCR2x to a value < than the current
  // timer count.
  OCR2A = cv0;
  OCR2B = cv1;  
#endif
  
  DEBUGSERIALWRITE("CVGenerator::updateCVOutputs(): cv0=");
  DEBUGSERIALPRINT(int(cv0), DEC);
  DEBUGSERIALWRITE(" cv1=");
  DEBUGSERIALPRINTLN(int(cv1), DEC);
}

// My filter shield prototype needs to reverse the range
// for the frequency control input to obtain "wah-wah"
// behavior of the expression pedal. The resonance
// control maps directly. Adjust the computation to match your
// specific application.
//
// We don't allow the extremes so we don't have to
// deal with the edge cases at (0, 255).
//
void CVGenerator::computeNewCVOutputs(byte& outCV0, byte& outCV1) const
{
  outCV0 = map(expPedalValue, 0, 127, 254,   1);
  outCV1 = map(modWheelValue, 0, 127,   1, 254);
}
