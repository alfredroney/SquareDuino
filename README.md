
     ,---.                                       ,------.          ,--.   
    '   .-'  ,---. ,--.,--. ,--,--.,--.--. ,---. |  .-.  \ ,--.,--.`--',--,--,  ,---.
    `.  `-. | .-. ||  ||  |' ,-.  ||  .--'| .-. :|  |  \  :|  ||  |,--.|      \| .-. |
    .-'    |' '-' |'  ''  '\ '-'  ||  |   \   --.|  '--'  /'  ''  '|  ||  ||  |' '-' '
    `-----'  `-|  | `----'  `--`--'`--'    `----'`-------'  `----' `--'`--''--' `---'
               `--'
      _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
    _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_

SquareDuino v1.0.0
===========
&copy; 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.

A real-time MIDI-controlled AF oscillator and CV generator sketch for
16 MHz ATMEGA 328P-based Arduino development boards in conjunction with
the SparkFun MIDI Shield. Tested on the UNO R3 and Duemilanove.

Note range and CC assignments are based on the default settings for
an Akai MPK49 MIDI controller. Voltage outputs control my custom VCF
prototype. YMMV.

Always remember to protect your hearing and respect your neighbors.
And groove . . . don't forget to groove!

Current Status
==============
 * Public Beta (to be released in conjunction with supporting Instructable)
 * Note parsing with graceful trill feature
 * AF output: sub-root-fifth-fifth 8va
 * CV output: two 8-bit ultrasonic 5v PWM channels
 * No channel recognition
 * No velocity recognition
 * Mod wheel controls CV A (resonance)
 * Expresssion pedal controls CV B (frequency)

Maybe
=====
 * Maybe a Mathematica tuning calculator CDF?
 
Planned
=======
 * Convert to an open-source license
 * Channel selection filter
 * Make some YouTube demos that aren't lame
