// MIDIShield.h v0.0.0
// (c) 2014, Alfred "Ben" Roney, Ph.D., All rights reserved
//
// Convenience header for SparkFun MIDI Shield on Arduino Uno R3 and Duemilanove.
// 
// Always remember to protect your hearing and respect your neighbors.
// And groove . . . don't forget to groove!
//
#ifndef _MIDISHIELD_H_ABR20140104_
#define _MIDISHIELD_H_ABR20140104_

#include <Arduino.h>

namespace MIDIShield {
    namespace Serial {
      enum {
        kMIDIBAUD  = 31250,
        kDebugBAUD = 57600,
        kDebugRX = 12,
        kDebugTX = 13
      };
    }
    namespace Output {
      enum {
        kSub   =  9,
        kRoot  = 10,
        kFifth =  5,

        kSig =  8,

        kCV0 =  3,
        kCV1 = 11
      };
    }
    namespace Pot {
      enum ID {
        kA0 = 0,
        kA1 = 1
      };
      static inline int readPot(ID pot_num) {
        return analogRead(pot_num);
      }
    }  
  
    namespace Switch {
      enum ID {
        kS8 = 2,
        kS7 = 3,
        kS6 = 4
      };
      static inline char isDepressed(ID switch_num) {
        return not digitalRead(switch_num);
      }
    }
    
    namespace StatusLED {
      enum ID {
        k1 = 7,
        k2 = 6
      };
      char isLit(ID LED_num) {
        return !digitalRead(LED_num);
      }
    };
}

#endif // #ifndef _MIDISHIELD_H_ABR20140104_
