#ifndef MIDISHIELD_H_ABR20140104
#define MIDISHIELD_H_ABR20140104
//
// MIDIShield.h
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// Convenience header for SparkFun MIDI Shield on Arduino Uno R3 and Duemilanove.
// 
// Always remember to protect your hearing and respect your neighbors.
// And groove . . . don't forget to groove!
//

#include <Arduino.h>

namespace MIDIShield {
    namespace Serial {
      enum {
        midiBAUD  = 31250,
        debugBAUD = 57600,
        debugRX = 12,
        debugTX = 13
      };
    }
    namespace Output {
      enum {
        sub      =  8,
        subFifth =  9,
        root     = 10,
        fifth    =  5,

        cv0 =  3,
        cv1 = 11
      };
    }
    namespace Pot {
      enum ID {
        a0 = 0,
        a1 = 1
      };
      static inline int readPot(ID pot_num) {
        return analogRead(pot_num);
      }
    }  
  
    namespace Switch {
      enum ID {
        // these are the IDs defined in the schematic
        s8 = 2,
        s7 = 3,
        s6 = 4
      };
      static inline char isDepressed(ID switch_num) {
        return not digitalRead(switch_num);
      }
    }
    
    namespace StatusLED {
      enum ID {
        led1 = 7,
        led2 = 6
      };
      static inline char isLit(ID LED_num) {
        return !digitalRead(LED_num);
      }
    };
}

#endif // #ifndef _MIDISHIELD_H_ABR20140104_
