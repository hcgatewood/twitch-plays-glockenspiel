#include "types.h"
#include "command.h"
#include "irc.h"

// Declare global variables here
const String IRC_PORTAL_URL = "irc.chat.twitch.tv";
const int IRC_PORTAL_PORT = 6667;
const String PASSWORD = "oauth:lfh7us91llv2z0z3zcf0hpup59mgo9";
const String NICKNAME = "piano_bot_s08";
const String IRC_READY_MSG = ":tmi.twitch.tv 376 piano_bot_s08 :>";

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
    Serial.println("Channel joined, messages incoming! Kreygasm");
  }
}

void loop() {
  // TODO state machine update
}
