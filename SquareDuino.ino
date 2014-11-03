// SquareDuino v0.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// A real-time MIDI-controlled AF oscillator and CV generator sketch for
// 16 MHz ATMEGA 328P-based Arduino development boards in conjunction with
// the SparkFun MIDI Shield. Tested on the UNO R3 and Duemilanove.
// 
// Always remember to protect your hearing and respect your neighbors.
// And groove . . . don't forget to groove!
//
#include "MIDIShield.h" // some useful mappings

// We will manipulate the timers directly in this sketch,
// so we use the AVR headers.
// For a timer primer, turn to these tutorials:
// http://www.engblaze.com/we-interrupt-this-program-to-bring-you-a-tutorial-on-arduino-interrupts/
// http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/
//
#include <avr/io.h>
#include <avr/interrupt.h>

// Since we're using the main serial IO for MIDI, we write
// debugging messages to a software serial port on some
// spare pins. Use a FTDI breakout to monitor it.
// Keep it off unless you need it, since it uses timers
// under the hood. This macro disables the AV and CV outputs.
#define DEBUGMIDIPARSER 0

#if DEBUGMIDIPARSER
#include "SoftwareSerial.h"
SoftwareSerial debugSerial(MIDIShield::Serial::kDebugRX,
                           MIDIShield::Serial::kDebugTX);
#endif

// Defines some useful constants.
enum {
  kA440 = 69, // index into noteCounts[]
  kMaxNotesOn = 32, // ghosts appear . . .
  kNumOscs = 3,
  kRootPrd  = 3, // divide by three to get the root
  kFifthPrd = 2, // divide by two to get the perfect fifth
  kSubPrd   = 6  // divide by six to get the sub-octave
};

// The following array contains the length (in timer steps) of the period of
// a note in the MIDI scale. They were computed with Mathematica.
const unsigned int noteCounts[] = {
  10192, 9620, 9080, 8570, 8089, 7635, 7206, 6802, 6420, 6060, 5719, 5398, 5095, 4809, 4539, 4284, 4044, 
 3817, 3603, 3400, 3209, 3029, 2859, 2699, 2547, 2404, 2269, 2142, 2021, 1908, 1801, 1700, 1604, 1514, 
 1429, 1349, 1273, 1202, 1134, 1070, 1010, 953, 900, 849, 802, 757, 714, 674, 636, 600, 567, 535, 505, 
 476, 449, 424, 400, 378, 357, 336, 318, 300, 283, 267, 252, 238, 224, 212, 200, 188, 178, 168, 158, 
 149, 141, 133, 125, 118, 112, 105, 99, 94, 88, 83, 79, 74, 70, 66, 62, 59, 55, 52, 49, 46, 44, 41, 39, 
 37, 34, 32, 31, 29, 27, 26, 24, 23, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 12, 11, 10, 10, 9, 8, 8, 7, 
 7, 6, 6, 6
};

const byte oscPin[kNumOscs] = {
  MIDIShield::Output::kSub,
  MIDIShield::Output::kRoot,
  MIDIShield::Output::kFifth
};
const byte oscPrd[kNumOscs] = { kSubPrd, kRootPrd, kFifthPrd };
      byte oscCnt[kNumOscs];

void resetOscs() {
  for (byte i=0; i<kNumOscs; ++i) {
    oscCnt[i] = 0;
  }
}

// We need to handle multiple notes on at once for fat-fingerd
// trills and runs. I am not a keyboard player, so
// this was necessary to make it playable for me. It also
// handles the note on/off issues that can arise from
// drawing patterns in a DAW.
volatile byte noteCount = 0; // Number of "note on" messages we have received
volatile byte noteList[kMaxNotesOn]; // list of "note on" messages in arrival order

// Convenience routines that classify the status header of a MIDI message.
byte isNoteOn(byte byte1, byte byte3) {
  return byte3 and (0x90 == (byte1 & 0xF0));
}

byte isNoteOff(byte byte1, byte byte3) {
  return ((0x00 == byte3) and (0x90 == (byte1 & 0xF0)))
      or (0x80 == (byte1 & 0xF0));
}

byte isCommandByte(byte x) {
  return x & (1<<7);
}

// We cue up the MIDI messages, for parsing during the
// main program loop.
byte rcvdCnt = 0;
byte midiMsg[3];
byte midiMsgRcvd = 0;

byte messageComplete() {
  return 3==rcvdCnt;
}

byte resetParser() {
  rcvdCnt = midiMsgRcvd = 0;
}

// We echo MIDI input to allow daisy-chaining of instruments.
void Midi_Send(byte cmd, byte data1, byte data2) {
#if DEBUGMIDIPARSER
  debugSerial.write("MIDI ECHO: 0x");
  debugSerial.print(int(cmd),HEX);
  
  debugSerial.write(" 0x");
  debugSerial.print(int(data1),HEX);

  debugSerial.write(" 0x");
  debugSerial.println(int(data2),HEX);
#endif

  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}

// This is a callback for when serial data arrives.
void serialEvent() {
  if(midiMsgRcvd) {
#if DEBUGMIDIPARSER
    debugSerial.write("waiting...\r\n"); 
#endif
    return; // don't read the next byte until we've processed the last message
  }
  while(Serial.available()) {
    byte rcvdByte = Serial.read();
    if(isCommandByte(rcvdByte)) {
      // reset parser on arrival of a command byte
      rcvdCnt = 1;
      midiMsg[0] = rcvdByte;
    } else {
      midiMsg[rcvdCnt++] = rcvdByte;
    }
    midiMsgRcvd = messageComplete();
  }
}

// When the note value changes, update the value.
void setNoteCount(byte thisNote) {
#if DEBUGMIDIPARSER
  debugSerial.write("setNoteCount(");
  debugSerial.print(int(thisNote),DEC);
  debugSerial.write(")\r\n");
#else
  cli(); // temporarily stop the timer interrupts
  OCR1A = noteCounts[thisNote]; // stuff the appropriate register
  TCNT1 = 0; // reset the counter
  sei(); // resume timer interrupts
#endif
}

// push a note onto the end of the list
void pushNoteList(byte thisNote) {
  noteList[noteCount++] = thisNote;
  setNoteCount(thisNote);
}

// pop a note off of the list
void popNoteList() {
  if(noteCount > 0) {
    noteCount--;
  }
  // if the user is still holding the previous note,
  // make it the current pitch
  if(noteCount > 0) {
      setNoteCount(noteList[noteCount-1]);
  }
}

// lookup a note in the list and return its index
byte getNoteIndex(const byte note) {
  for(byte i=0; i<kMaxNotesOn; ++i) {
    if(note == noteList[i]) {
      return i;
    }
  }  
  return kMaxNotesOn;
}

// does just what it says
void removeNoteAtIndex(byte i) {
  if(i>=noteCount) return;
  for(;i<(noteCount-1); ++i) {
    noteList[i] = noteList[i+1];
  }
  noteCount--;
#if DEBUGMIDIPARSER
  debugSerial.write("NOTES:");
  for(i=0; i<noteCount; ++i) {
    debugSerial.write(" 0x");
    debugSerial.print(noteList[i],HEX);
  }
  debugSerial.println();
#endif
}

// We only reset the PWM outputs when they change. To
// do this, we need to keep track of the current setpoint.
int lastPot0 = 1;
int lastPot1 = 1;

// Throttle the processor by only reading the ADCs and
// updating the CVs every so many times through the loop.
const int updateInterval = 2048;
int updateCounter = updateInterval;

//////////////////////////////////////
// loop - the main program loop
//
void loop () {

  //*************** MIDI MESSAGE PARSER ******************//
  if(midiMsgRcvd)
  {
    byte byte1 = midiMsg[0];
    byte byte2 = midiMsg[1];
    byte byte3 = midiMsg[2];

    resetParser();
    
    byte thisNote = byte2 & 0x7F;
    byte noteIndex = getNoteIndex(thisNote);

    if(isNoteOn(byte1,byte3) and (noteCount < kMaxNotesOn)) {
      if(noteIndex < kMaxNotesOn) {
        // Note is already in the list, so another finger must have lifted.
//      do {
          removeNoteAtIndex(noteIndex);
//      } while(kMaxNotesOn != getNoteIndex(thisNote)); // could be more than one
      }
      pushNoteList(thisNote);
    }
    if(isNoteOff(byte1,byte3) and (noteCount > 0)) {
      if((kMaxNotesOn == noteIndex) or (1 == noteCount)) {
        // note wasn't in the list, or this was the only active note,
        // kill 'em all
        noteCount = 0;
      } else if((noteCount - 1) == noteIndex) {
        popNoteList();
      } else {
        removeNoteAtIndex(noteIndex); // make sure we don't resurrect this one
      }
    }

    Midi_Send(byte1, byte2, byte3);
  }
  
  //******* CONTROL VOLTAGES ********//
  if(updateInterval == updateCounter++) {
    updateCounter = 0;
    
    const int pot0 = MIDIShield::Pot::readPot(MIDIShield::Pot::kA0);
    const int pot1 = MIDIShield::Pot::readPot(MIDIShield::Pot::kA1);
    
    if( (pot0 != lastPot0) or (pot1 != lastPot1)) {
      // These may need to be tweaked depending 
      // for the specific device being controlled.
      // The map is reversed so the knobs on the
      // SparkFun MIDI shield will increase the
      // voltages as you turn them clockwise.
      // Forget 11, these go all the way to 254!
      int cv0 = map(pot0, 0, 1023, 254, 1);
      int cv1 = map(pot1, 0, 1023, 254, 1);
      
#if DEBUGMIDIPARSER
      debugSerial.write("cv0 = ");
      debugSerial.print(int(cv0),DEC);
      debugSerial.write(", cv1 = ");
      debugSerial.print(int(cv1),DEC);
      debugSerial.write("\r\n");
#else
      OCR2A = cv0;
      OCR2B = cv1;
#endif

      lastPot0 = pot0;
      lastPot1 = pot1;
    } 
  }
}


//////////////////////////////////////
// setup - initialization routine
//
void setup() {
  Serial.begin(MIDIShield::Serial::kMIDIBAUD);
#if DEBUGMIDIPARSER
  debugSerial.begin(MIDIShield::Serial::kDebugBAUD);  
  debugSerial.write("INIT\r\n");
  debugSerial.write("WARNING: OUTPUTS ARE DISABLED\r\n");
#endif

  // SoftwareSerial uses the timers, so we cannot use them while
  // debugging the MIDI parser.
  //
#if !DEBUGMIDIPARSER
  pinMode(MIDIShield::Output::kSub  , OUTPUT);  
  pinMode(MIDIShield::Output::kRoot , OUTPUT);  
  pinMode(MIDIShield::Output::kFifth, OUTPUT);  
  pinMode(MIDIShield::Output::kSig  , OUTPUT);  
  pinMode(MIDIShield::Output::kCV0  , OUTPUT);
  pinMode(MIDIShield::Output::kCV1  , OUTPUT);

  cli();  // disable global interrupts
  
  // set up the two 8-bit timers for PWM
  TCCR2A = (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
  TCCR2B = (1 << CS20);
  
  OCR2A = lastPot0;
  OCR2B = lastPot1;
  
  
  // initialize Timer1 in CTC-mode (variable frequency)
  TCCR1A = 0; // 
  
  TIMSK1 = (1 << OCIE1A); // enable Timer1 compare interrupt

  TCCR1B = (1 << CS11) | (1 << CS10) // prescale by 64
         | (1 << WGM12); // turn on CTC mode

  OCR1A = noteCounts[kA440];

  sei(); // re-enable global interrupts
#endif
}

// Interrupt service routine for Timer1
// When the count resets, toggle the output state
// if a note is on, otherwise stay low.
ISR(TIMER1_COMPA_vect) {
  for (byte i=0; i<kNumOscs; ++i) {
    if (not oscCnt[i]) {
      digitalWrite(oscPin[i], (noteCount > 0) and not digitalRead(oscPin[i]));
    }      
    oscCnt[i] += 1;
    if (oscCnt[i] == oscPrd[i]) {
      oscCnt[i] = 0;
    }
  }
  digitalWrite(MIDIShield::Output::kSig,
               (noteCount > 0) and !digitalRead(MIDIShield::Output::kSig));
}

