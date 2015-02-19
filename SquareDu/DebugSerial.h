#ifndef DEBUGSERIAL_H_ABR20150102
#define DEBUGSERIAL_H_ABR20150102
//
// DebugSerial.h
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// Since we're using the main serial IO for MIDI, we write
// debugging messages to a software serial port on some
// spare pins. Use a FTDI breakout to monitor it.
// Keep it off unless you need it, since it uses timers
// under the hood. This macro disables the AF and CV outputs.
//
// For some reason I could not define a SoftwareSerial object
// outside of the project.ino file, so we use this hacked-up
// macro set to centralize what we can.
//
#define DEBUGSERIAL 0

#define NOTDEBUGSERIAL !(DEBUGSERIAL)

#define DEBUGSERIALTYPE SoftwareSerial
#define DEBUGSERIALNAME debugSerial

#if DEBUGSERIAL

#define DEBUGSERIALWRITE(A)      DEBUGSERIALNAME.write((A))
#define DEBUGSERIALPRINT(A, B)   DEBUGSERIALNAME.print((A), (B))
#define DEBUGSERIALPRINTLN(A, B) DEBUGSERIALNAME.println((A), (B))

#else

#define DEBUGSERIALWRITE(A)
#define DEBUGSERIALPRINT(A, B) 
#define DEBUGSERIALPRINTLN(A, B)

#endif

#endif // #ifndef DEBUGSERIAL_H_ABR20150102

