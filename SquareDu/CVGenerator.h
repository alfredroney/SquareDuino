#ifndef CVGENERATOR_H_ABR20150102
#define CVGENERATOR_H_ABR20150102
//
// CVGenerator.h
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// This class uses the two low-precision timers to generate control voltages via
// PWM at a frequency well outside of the audio band, relaxing the lowpass filter
// requirements.
//

#include <Arduino.h>

class CVGenerator
{
public:

  enum {
    modWheelCCNumber =  1, // modulation wheel
    expPedalCCNumber = 11  // expression pedal
  };

  CVGenerator();

  void setModWheelValue(byte inValue);
  void setExpPedalValue(byte inValue);

  void goOnline(); // call during setup()

private:

  byte modWheelValue;
  byte expPedalValue;

  void updateCVOutputs();

  // Map the inputs to outputs. This could be a complex
  // constant-Q mapping for a VCF, or just a simple
  // input-to-output scaling.
  void computeNewCVOutputs(byte& cv0, byte& cv1) const;
};

#define CVGENERATOR_CREATE_INSTANCE() CVGenerator cvGenerator;

#endif // #ifndef CVGENERATOR_H_ABR20150102

