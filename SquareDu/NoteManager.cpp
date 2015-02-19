// NoteManager.cpp
// SquareDuino v1.0.0
//   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
// _| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_
//
// (c) 2014-2015, Alfred "Ben" Roney, Ph.D., All rights reserved.
//
// This class matches "note off" events to "note on" events, and gracefully
// accepts fat-fingered trills and runs.
//

#include "NoteManager.h"

#include "DebugSerial.h"

#if DEBUGSERIAL
#include <SoftwareSerial.h>
extern DEBUGSERIALTYPE DEBUGSERIALNAME;
#endif

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
// The timer periods for each note in the MIDI scale live here. The frequencies
// have been multiplied by 3 to obtain perfect fifths via frequency division.
// The notes are also offset to map the most precise pitches to the default range
// of the AKAI MPK49 performance controller. Beware of the high notes, the processor
// isn't fast enough to play all of them.
//
unsigned int NoteManager::getPeriodForNoteNumber(byte inNoteNumber)
{
  static const unsigned int noteCounts[] = {
    10192, 9620, 9080, 8570, 8089, 7635, 7206, 6802, 6420, 6060, 5719, 5398, 5095, 4809, 4539, 4284, 4044, 
    3817, 3603, 3400, 3209, 3029, 2859, 2699, 2547, 2404, 2269, 2142, 2021, 1908, 1801, 1700, 1604, 1514, 
    1429, 1349, 1273, 1202, 1134, 1070, 1010, 953, 900, 849, 802, 757, 714, 674, 636, 600, 567, 535, 505, 
    476, 449, 424, 400, 378, 357, 336, 318, 300, 283, 267, 252, 238, 224, 212, 200, 188, 178, 168, 158, 
    149, 141, 133, 125, 118, 112, 105, 99, 94, 88, 83, 79, 74, 70, 66, 62, 59, 55, 52, 49, 46, 44, 41, 39, 
    37, 34, 32, 31, 29, 27, 26, 24, 23, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 12, 11, 10, 10, 9, 8, 8, 7, 
    7, 6, 6, 6    
  };
  
  DEBUGSERIALWRITE("NoteManager::getPeriodForNoteNumber(");
  DEBUGSERIALPRINT(int(inNoteNumber), DEC);
  DEBUGSERIALWRITE("): ");
  
  inNoteNumber = (inNoteNumber < noteOffset) ? (inNoteNumber % octave) : (inNoteNumber - noteOffset);

  DEBUGSERIALPRINT(int(inNoteNumber), DEC);
  DEBUGSERIALWRITE(" ");

  const unsigned int periodForNoteNumber = noteCounts[inNoteNumber];

  DEBUGSERIALPRINTLN(int(periodForNoteNumber), DEC);
 
  return periodForNoteNumber; 
}

// -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=- -=+=-
//
NoteManager::NoteManager()
: noteCount(0)
{
}

unsigned int NoteManager::getPeriodForActiveNote() const
{
  return (noteCount > 0) ? getPeriodForNoteNumber(noteList[noteCount-1]) : 0;
}

void NoteManager::addNote(byte inNoteNumber)
{
  if (noteCount < maxNotesOn)
  {
    noteList[noteCount++] = inNoteNumber;
  }
}

void NoteManager::removeNote(byte inNoteNumber)
{
  byte noteIndex = getNoteIndex(inNoteNumber);
  
  if (noteIndex == noteCount)
  {
    noteCount = 0; // note off doesn't match a note on, kill 'em all
  }
  else
  {
    removeNoteAtIndex(noteIndex);
  }
}

byte NoteManager::getNoteIndex(byte inNoteNumber) const
{
  byte i = 0;
  
  for (; i < noteCount; ++i)
  {
    if (inNoteNumber == noteList[i])
    {
      break; // we found it, stop incrementing i
    }
  }
  
  return i; // returns noteCount if it wasn't found
}

void NoteManager::removeNoteAtIndex(byte i)
{
  if (i < noteCount)
  {
    for (; i < (noteCount-1); ++i)
    {
      noteList[i] = noteList[i+1];
    }

    noteCount--;
  }
}


