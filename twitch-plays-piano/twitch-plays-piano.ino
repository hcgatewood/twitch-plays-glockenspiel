#include "types.h"
#include "command.h"
#include "irc.h"

// The URL of the IRC server to connect to.
const String IRC_PORTAL_URL = "irc.chat.twitch.tv";

// The TCP port of the IRC server to connect to.
const int IRC_PORTAL_PORT = 6667;

// The password and username for the IRC service.
const String PASSWORD = "oauth:x7i7lmwvvcxwts6929gsfuumg9psdx";
const String NICKNAME = "piano_bot_s08";

// The message sent by the IRC server when the client is ready to join a channel.
const String IRC_READY_MSG = ":>";

// The lowest actuated note.
const Note BOTTOM_NOTE = Note();

// The delay between powering and unpowering a solenoid.
const unsigned long NOTE_DELAY = 10;

// The delay between playing notes in a chord.
const unsigned long CHORD_DELAY = 10;

IrcHelper* helper;

// Writes the 5 least significant bits to the output pins
void write_address(uint8_t value) {
  // 7 (MSB)
  // 6
  // 5        Goes to OUTPUT_BIT_0
  // 4        Goes to OUTPUT_BIT_1
  // 3        Goes to OUTPUT_BIT_2
  // 2        Goes to OUTPUT_BIT_3
  // 1        Goes to OUTPUT_BIT_4
  // 0 (LSB)  Goes to OUTPUT_BIT 5

  if (value == 0xff) {
    return;
  }

  Serial.print("writing value: ");
  Serial.println(value);

  digitalWrite(OUTPUT_BIT_0, (value & 0x10) != 0);
  digitalWrite(OUTPUT_BIT_1, (value & 0x08) != 0);
  digitalWrite(OUTPUT_BIT_2, (value & 0x04) != 0);
  digitalWrite(OUTPUT_BIT_3, (value & 0x02) != 0);
  digitalWrite(OUTPUT_BIT_4, (value & 0x01) != 0);
}
void clear_address() {
  digitalWrite(OUTPUT_BIT_0, 0);
  digitalWrite(OUTPUT_BIT_1, 0);
  digitalWrite(OUTPUT_BIT_2, 0);
  digitalWrite(OUTPUT_BIT_3, 0);
  digitalWrite(OUTPUT_BIT_4, 0);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LED_PIN, OUTPUT);
  //while (!Serial);

  pinMode(OUTPUT_BIT_0, OUTPUT);
  pinMode(OUTPUT_BIT_1, OUTPUT);
  pinMode(OUTPUT_BIT_2, OUTPUT);
  pinMode(OUTPUT_BIT_3, OUTPUT);
  pinMode(OUTPUT_BIT_4, OUTPUT);
  clear_address();

  Serial.println("Constructing IRC helper...");
  helper = new IrcHelper("6s08", "iesc6s08");
  Serial.println("IRC helper constructed!");

  if (!helper->connect_to("irc.chat.twitch.tv", 6667, NICKNAME, PASSWORD, IRC_READY_MSG)) {
    Serial.println("Could not connect to Twitch chat! FeelsBadMan");
    while (true);
  } else {
    Serial.println("Connected to Twitch chat! PogChamp");
  }

  if (!helper->join_channel(NICKNAME)) {
    Serial.println("Could not join channel!");
    while (true);
  } else {
    Serial.println("Channel joined, messages incoming!");
  }

  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  String sender = "";
  String message = "";
  Chord chord;
  Note note;
  String out;

  if (helper->try_read(100) > 0) {
    while (helper->is_message_received(sender, message)) {
      if (chord << message) {
        chord.to_string(out);
        Serial.println(sender + " sent this chord: " + out);

        for (size_t i = 0; i < chord.num_notes(); i++) {
          write_address(chord.get_output_address(BOTTOM_NOTE, i));
          delay(NOTE_DELAY);
          clear_address();
          delay(CHORD_DELAY);
        }
      } else if (note << message) {
        note.to_string(out);
        Serial.println(sender + " sent this note: " + out);

        write_address(note.get_output_address(BOTTOM_NOTE));
        delay(NOTE_DELAY);
        clear_address();
      }
    }
    helper->trim_buffer();
  }
}
