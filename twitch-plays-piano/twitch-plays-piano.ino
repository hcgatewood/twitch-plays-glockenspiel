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

  Chord c;
  String buf;
  
  if (!(c << "!C5")) {
    Serial.println("Test chord 1 parsing failed!");
  }
  c.to_string(buf);
  Serial.print("Test chord 1 (C major): ");
  Serial.println(buf);
  
  if (!(c << "!Cm")) {
    Serial.println("Test chord 2 parsing failed!");
  }
  c.to_string(buf);
  Serial.print("Test chord 2 (C minor): ");
  Serial.println(buf);

  if (!(c << "!C7")) {
    Serial.println("Test chord 3 parsing failed!");
  }
  c.to_string(buf);
  Serial.print("Test chord 3 (C7): ");
  Serial.println(buf);

  if (!(c << "!G7")) {
    Serial.println("Test chord 4 parsing failed!");
  }
  c.to_string(buf);
  Serial.print("Test chord 4 (G7): ");
  Serial.println(buf);

  if (!(c << "!Ebo")) {
    Serial.println("Test chord 5 parsing failed!");
  }
  c.to_string(buf);
  Serial.print("Test chord 5 (Eb diminshed): ");
  Serial.println(buf);

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
  // TODO state machine update
}
