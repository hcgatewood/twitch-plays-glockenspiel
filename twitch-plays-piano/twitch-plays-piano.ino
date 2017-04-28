#include "types.h"
#include "command.h"
#include "irc.h"

const String IRC_PORTAL_URL = "irc.chat.twitch.tv";
const int IRC_PORTAL_PORT = 6667;
const String PASSWORD = "oauth:x7i7lmwvvcxwts6929gsfuumg9psdx";
const String NICKNAME = "piano_bot_s08";
const String IRC_READY_MSG = ":>";

IrcHelper* helper;

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LED_PIN, OUTPUT);
  while (!Serial);

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
      } else if (note << message) {
        note.to_string(out);
        Serial.println(sender + " sent this note: " + out);
      }
    }
    helper->trim_buffer();
  }
}
