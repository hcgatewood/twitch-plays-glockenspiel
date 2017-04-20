#include "irc.h"

#include "types.h"

IrcHelper::IrcHelper(String wifi, String password) {
  WIFI_SERIAL.begin(WIFI_BAUD);
  while (!WIFI_SERIAL);
  empty_rx();

  while (!check_for_hardware());
  Serial.println("WiFi module detected!");
  empty_rx();

  while (!try_to_connect(wifi, password));
  Serial.println("WiFi connected!");
  empty_rx();
}

bool IrcHelper::check_for_hardware() {
  Serial.println("Looking for WiFi module...");
  WIFI_SERIAL.println("AT");
  return wait_for_response("OK", 1000);
}

bool IrcHelper::try_to_connect(String wifi, String password) {
  Serial.println("Trying to connect to a network...");
  WIFI_SERIAL.println("AT+CWJAP=\"" + wifi + "\",\"" + password + "\"");
  return wait_for_response("OK", 10000); // Wait up to 10 seconds for the connection
}

bool IrcHelper::tcp_send(String data) {
  empty_rx();
  WIFI_SERIAL.print("AT+CIPSEND=");
  WIFI_SERIAL.println(data.length());
  WIFI_SERIAL.println(data);
  return wait_for_response("SEND OK", 1000);
}

void IrcHelper::empty_rx() {
  while (WIFI_SERIAL.available()) {
    WIFI_SERIAL.read();
  }
}

bool IrcHelper::connect_to(String url, int port, String username, String password, String expected) {
  WIFI_SERIAL.println("AT+CIPSTART=\"TCP\",\"" + url + "\"," + port);
  if (!wait_for_response("OK", 1000)) {
    Serial.println("Could not connect to IRC server!");
    return false;
  } else {
    Serial.println("Connected to IRC server!");
  }

  if (!tcp_send("PASS " + password)) {
    Serial.println("Could not send password!");
    return false;
  } else {
    Serial.println("Sent password!");
  }

  if (!tcp_send("NICK " + username)) {
    Serial.println("Could not send nickname!");
    return false;
  } else {
    Serial.println("Sent nickname!");
  }

  return wait_for_response(expected, 3000);
}

bool IrcHelper::join_channel(String channel) {
  if (!tcp_send("JOIN #" + channel)) {
    Serial.println("Could not join channel!");
    return false;
  }

  return wait_for_response(":End of /NAMES list", 1000);
}

bool wait_for_response(String expected, unsigned long timeout) {
  String response = "";
  const unsigned long start = millis();
  while (millis() - start < timeout) {
    if (WIFI_SERIAL.available() > 0) {
      char c = WIFI_SERIAL.read();
      response = String(response + c);
      if (response.endsWith(expected)) {
        return true;
      }
    }
  }
  return false;
}
