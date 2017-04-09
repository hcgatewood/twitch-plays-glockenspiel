#include "types.h"
#include "command.h"
#include "irc.h"

// Declare global variables here

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LED_PIN, OUTPUT);
  while (!Serial);

  // It might be better to move this chunk to the IRC file and abstract away the fact
  // that we're using wifi at all, so that the only thing this file has to be aware of
  // is incoming messages.
  WIFI_SERIAL.begin(WIFI_BAUD);
  while (!WIFI_SERIAL);
  WIFI_SERIAL.println("AT");
  bool found_response = false;
  String response = "";
  unsigned long start_time = millis();
  while (!found_response && millis() - start_time < WIFI_TIMEOUT) {
    if (WIFI_SERIAL.available()) {
      char c = WIFI_SERIAL.read();
      response += c;
      if (response.endsWith("OK")) {
        found_response = true;
      }
    }
  }
  if (!found_response) {
    Serial.println("WiFi module not connected!");
    while (true);
  } else {
    Serial.println("WiFi module connected and active!");
  }
  // TODO connect to a WiFi network
  // TODO open raw TCP connection to irc.chat.twitch.tv
  // TODO authenticate
  // TODO join a channel

  // Initialize globals here
}

void loop() {
  // TODO state machine update
}
