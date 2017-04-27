#include "command.h"

bool Note::operator<<(const String& input) {
  if (input.length() == 2) {
    const int octave_ascii = input.charAt(1);
    if (octave_ascii < '0' || octave_ascii > '9') {
      return false;
    }
    switch (input.charAt(0)) {
      case 'A':
        _name = NoteName::A;
        break;
      case 'B':
        _name = NoteName::B;
        break;
      case 'C':
        _name = NoteName::C;
        break;
      case 'D':
        _name = NoteName::D;
        break;
      case 'E':
        _name = NoteName::E;
        break;
      case 'F':
        _name = NoteName::F;
        break;
      case 'G':
        _name = NoteName::G;
        break;

      default:
        return false;
    }
    _octave = octave_ascii - '0';
    return true;
  }

  if (input.length() == 3) {
    const int octave_ascii = input.charAt(2);
    if (octave_ascii < '0' || octave_ascii > '9') {
      return false;
    }
    const String name = input.substring(0, 2);
    if (name.equals("A#") || name.equals("Bb")) {
      _name = NoteName::Ais;
    } else if (name.equals("B#")) {
      _name = NoteName::C;
    } else if (name.equals("Cb")) {
      _name = NoteName::B;
    } else if (name.equals("C#") || name.equals("Db")) {
      _name = NoteName::Cis;
    } else if (name.equals("D#") || name.equals("Eb")) {
      _name = NoteName::Dis;
    } else if (name.equals("E#")) {
      _name = NoteName::F;
    } else if (name.equals("Fb")) {
      _name = NoteName::E;
    } else if (name.equals("F#") || name.equals("Gb")) {
      _name = NoteName::Fis;
    } else if (name.equals("G#") || name.equals("Ab")) {
      _name = NoteName::Gis;
    } else {
      return false;
    }
    _octave = octave_ascii - '0';
    return true;
  }

  return false;
}

uint8_t Note::get_output_address(const Note& bottom) const {
  if (bottom > *this) {
    return -1;
  }

  uint8_t octave_dist = _octave - bottom._octave;
  uint8_t note_dist = static_cast<uint8_t>(_name) - static_cast<uint8_t>(bottom._name);
  return (12 * octave_dist) + note_dist;
}

Note Note::operator+(const uint8_t half_steps) const {
  Note n = *this;
  int val = static_cast<int>(_name) + half_steps;
  if (val > 11) {
    val -= 12;
    n.octave() += 1;
  }
  n.letter() = static_cast<NoteName>(val);
  return n;
}

void Note::to_string(String& buf) const {
  buf.reserve(3);
  switch (_name) {
    case NoteName::A:
      buf = "A";
      break;
    case NoteName::Ais:
      buf = "A#";
      break;
    case NoteName::B:
      buf = "B";
      break;
    case NoteName::C:
      buf = "C";
      break;
    case NoteName::Cis:
      buf = "C#";
      break;
    case NoteName::D:
      buf = "D";
      break;
    case NoteName::Dis:
      buf = "D#";
      break;
    case NoteName::E:
      buf = "E";
      break;
    case NoteName::F:
      buf = "F";
      break;
    case NoteName::Fis:
      buf = "F#";
      break;
    case NoteName::G:
      buf = "G";
      break;
    case NoteName::Gis:
      buf = "G#";
      break;
  }
  buf += _octave;
}

bool Note::operator>(const Note& rhs) const {
  if (_octave == rhs._octave) {
    return _name > rhs._name;
  }
  return _octave > rhs._octave;
}

bool Note::operator==(const Note& rhs) const {
  return _octave == rhs._octave && _name == rhs._name;
}

bool Note::operator!=(const Note& rhs) const {
  return !(rhs == *this);
}

bool Note::operator<(const Note& rhs) const {
  return rhs != *this && rhs > *this;
}

bool Chord::operator<<(const String& input) {
  if (!input.startsWith("!")) {
    return false;
  }

  // We assume octave 0, which will be corrected for by get_output_address.
  Note root;
  root.octave() = 0;
  char chord_ident;
  if (input.length() == 3) {
    switch (input.charAt(1)) {
      case 'A':
        root.letter() = NoteName::A;
        break;
      case 'B':
        root.letter() = NoteName::B;
        break;
      case 'C':
        root.letter() = NoteName::C;
        break;
      case 'D':
        root.letter() = NoteName::D;
        break;
      case 'E':
        root.letter() = NoteName::E;
        break;
      case 'F':
        root.letter() = NoteName::F;
        break;
      case 'G':
        root.letter() = NoteName::G;
        break;
      default:
        return false;
    }
    chord_ident = input.charAt(2);
  } else if (input.length() == 4) {
    const String name = input.substring(1, 3);
    if (name.equals("A#") || name.equals("Bb")) {
      root.letter() = NoteName::Ais;
    } else if (name.equals("B#")) {
      root.letter() = NoteName::C;
    } else if (name.equals("Cb")) {
      root.letter() = NoteName::B;
    } else if (name.equals("C#") || name.equals("Db")) {
      root.letter() = NoteName::Cis;
    } else if (name.equals("D#") || name.equals("Eb")) {
      root.letter() = NoteName::Dis;
    } else if (name.equals("E#")) {
      root.letter() = NoteName::F;
    } else if (name.equals("Fb")) {
      root.letter() = NoteName::E;
    } else if (name.equals("F#") || name.equals("Gb")) {
      root.letter() = NoteName::Fis;
    } else if (name.equals("G#") || name.equals("Ab")) {
      root.letter() = NoteName::Gis;
    } else {
      return false;
    }
    chord_ident = input.charAt(3);
  } else {
    return false;
  }

  _notes[0] = root;
  switch (chord_ident) {
    case '7':
      _notes[1] = root + 4;
      _notes[2] = root + 7;
      _notes[3] = root + 10;
      _num_notes = 4;
      return true;
    case '5':
      _notes[1] = root + 4;
      _notes[2] = root + 7;
      _num_notes = 3;
      return true;
    case 'm':
      _notes[1] = root + 3;
      _notes[2] = root + 7;
      _num_notes = 3;
      return true;
    case 'o':
      _notes[1] = root + 3;
      _notes[2] = root + 6;
      _num_notes = 3;
      return true;
    default:
      return false;
  }
}

void Chord::to_string(String& buf) const {
  String tempbuf = "";
  buf = "[";
  for (size_t i = 0; i < _num_notes; i++) {
    _notes[i].to_string(tempbuf);
    buf += tempbuf;
    buf += ",";
  }
  buf += "]";
}

