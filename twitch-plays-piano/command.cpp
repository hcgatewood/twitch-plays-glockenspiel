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
