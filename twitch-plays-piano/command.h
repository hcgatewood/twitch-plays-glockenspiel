#ifndef COMMAND_H
#define COMMAND_H

#include <Arduino.h>

enum class NoteName {
  A,
  Ais,
  B,
  C,
  Cis,
  D,
  Dis,
  E,
  F,
  Fis,
  G,
  Gis,
};

class Note {
  private:
    uint8_t _octave;
    NoteName _name;

  public:
    Note(int octave, NoteName name) :
      _octave(octave), _name(name) {}

    // Builds a Note assuming octave 4
    Note(NoteName name) :
      Note(4, name) {}

    // Builds the default Note, C4
    Note() :
      Note(NoteName::C) {}

    // Disallow copying
    void operator=(const Note& _) = delete;

    // Parses a note string to a Note.
    bool operator>>(const String& input);

    bool operator>(const Note& other) const;
    bool operator==(const Note& other) const;
    bool operator!=(const Note& other) const;
    bool operator<(const Note& other) const;

    // Gets the address of the solenoid to activate to play this note given the lowest
    // actuated note and assuming that all notes chromatically after it are actuated. If this
    // note is lower than the given note, returns -1.
    int get_output_address(const Note& bottom) const;


    uint8_t octave() const {
      return _octave;
    }

    uint8_t& octave() {
      return _octave;
    }

    NoteName letter() const {
      return _name;
    }

    NoteName& letter() {
      return _name;
    }
};

#endif
