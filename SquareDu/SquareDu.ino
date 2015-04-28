// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// A real-time MIDI-controlled AF oscillator and CV generator sketch for
// 16 MHz ATMEGA 328P-based Arduino development boards in conjunction with
// the SparkFun MIDI Shield. Tested on the UNO R3 and Duemilanove.
// 
// Always remember to protect your hearing and respect your neighbors.
// And groove . . . don't forget to groove!
//
#include "MIDIShield.h"  // some useful hardware mappings
#include <MIDI.h>        // https://github.com/FortySevenEffects/arduino_midi_library
#include "Oscillator.h"  // Code for the oscillator voices
#include "NoteManager.h" // track note on/off messages
#include "CVGenerator.h" // generate dual control voltages
#include "DebugSerial.h" // normally disabled, see header

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// This bit of ugliness is a workaround for a build issue.
//
#if DEBUGSERIAL

#include <SoftwareSerial.h>

DEBUGSERIALTYPE DEBUGSERIALNAME(MIDIShield::Serial::debugRX,
                                MIDIShield::Serial::debugTX);

void debugSerialBegin()
{
  DEBUGSERIALNAME.begin(MIDIShield::Serial::debugBAUD);
  DEBUGSERIALWRITE("INIT\r\n");
  DEBUGSERIALWRITE("WARNING: OUTPUTS ARE DISABLED\r\n");
}

#define DEBUGSERIALBEGIN debugSerialBegin()

#else

#define DEBUGSERIALBEGIN /* empty */

#endif

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// These are macros that define the interfaces to the
// hardware facilities used by the synth.
//
MIDI_CREATE_DEFAULT_INSTANCE();
OSCILLATOR_CREATE_INSTANCE();
CVGENERATOR_CREATE_INSTANCE();
NOTEMANAGER_CREATE_INSTANCE();

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// Define the callbacks used to parameterize the MIDI class.
//
void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
  noteManager.addNote(inNote);
  
  oscillator.play(noteManager.getPeriodForActiveNote());
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity)
{
  noteManager.removeNote(inNote);
  
  unsigned int newPeriod = noteManager.getPeriodForActiveNote();
  
  if (newPeriod > 0)
  {
    oscillator.play(newPeriod); 
  }
  else
  {
    oscillator.stop();
  }
}

void handleControlChange(byte inChannel, byte inCCNumber, byte inValue)
{  
  switch (inCCNumber)
  {
    case CVGenerator::modWheelCCNumber: // modulation wheel
      cvGenerator.setModWheelValue(inValue);
      break;
    case CVGenerator::expPedalCCNumber: // expression pedal
      cvGenerator.setExpPedalValue(inValue);
      break;
    default:
      // do nothing
      break;
  }
}

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// Only throttle back the MIDI updates if the signal generators are running.
//
enum { 
#if DEBUGSERIAL
  updateInterval = 1
#else
  updateInterval = 128
#endif
};
unsigned int updateCounter;

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// Let's get ready to rumble . . .
//
void setup()
{
  DEBUGSERIALBEGIN;
  
  oscillator.goOnline();
  cvGenerator.goOnline();

  // register callbacks
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  
  // start parser
  MIDI.begin();

  // initialize throttling counter  
  updateCounter = 0;
}

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// The MIDI class dispatches the callbacks as the appropriate MIDI messages
// are received, so there's not much to the loop function.
//
void loop()
{
  if (0 == updateCounter)
  {
    MIDI.read();
  }
  
  if (updateInterval == updateCounter++)
  {
    updateCounter = 0;
  }
}

