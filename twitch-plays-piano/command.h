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
    int8_t _octave;
    int8_t _default_octave = '5' - '0';
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

    // Parses a note string to a Note.
    bool operator<<(const String& input);

    bool operator>(const Note& rhs) const;
    bool operator==(const Note& rhs) const;
    bool operator!=(const Note& rhs) const;
    bool operator<(const Note& rhs) const;
    Note operator+(const uint8_t half_steps) const;

    // Gets the address of the solenoid to activate to play this note given the lowest
    // actuated note and assuming that all notes chromatically after it are actuated. If this
    // note is lower than the given note, returns -1.
    uint8_t get_output_address(const Note& bottom) const;

    // Writes a textual representation of this note to the given string.
    void to_string(String& buf) const;


    int8_t octave() const {
      return _octave;
    }

    int8_t& octave() {
      return _octave;
    }

    NoteName letter() const {
      return _name;
    }

    NoteName& letter() {
      return _name;
    }
};

class Chord {
  private:
    // Chords are up to 4 notes
    Note _notes[4];
    size_t _num_notes;

  public:
    Chord(Note n1, Note n2, Note n3, Note n4) :
      _notes { n1, n2, n3, n4 }, _num_notes(4) {}

    Chord(Note n1, Note n2, Note n3) :
      _notes { n1, n2, n3, Note() }, _num_notes(3) {}

    Chord(Note n1, Note n2) :
      _notes { n1, n2, Note(), Note() }, _num_notes(2) {}

    Chord() :
      Chord(Note(), Note(), Note()) {}

    bool operator<<(const String& input);

    uint8_t get_output_address(const Note& bottom, size_t index) const;

    size_t num_notes() const {
      return _num_notes;
    }

    void to_string(String& buf) const;
};
#endif
