#ifndef NOTEMANAGER_H_ABR20150101
#define NOTEMANAGER_H_ABR20150101
//
// NoteManager.h
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// This class matches "note off" events to "note on" events, and gracefully
// accepts fat-fingered trills and runs.
//

#include <Arduino.h>

class NoteManager {
public:

  NoteManager();

  void addNote(byte inNoteNumber);
  
  void removeNote(byte inNoteNumber);

  unsigned int getPeriodForActiveNote() const;

private:

  enum {
    maxNotesOn = 128, // ghosts appear and fade away . . .
  
    octave = 12,
    noteOffset = 3*octave, // align useful range to Akai MPK49 keyboard
  };

  byte noteCount;
  byte noteList[maxNotesOn];

  static unsigned int getPeriodForNoteNumber(byte inNoteNumber);
  
  byte getNoteIndex(byte inNoteNumber) const;
  void removeNoteAtIndex(byte inNoteNumber);
};

#define NOTEMANAGER_CREATE_INSTANCE() NoteManager noteManager;

#endif #ifndef NOTEMANAGER_H_ABR20150101
