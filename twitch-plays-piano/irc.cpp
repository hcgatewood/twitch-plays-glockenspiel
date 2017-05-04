#include "irc.h"

#include "types.h"

IrcHelper::IrcHelper(String wifi, String password) :
  _linebuf("") {
  WIFI_SERIAL.begin(WIFI_BAUD);
  while (!WIFI_SERIAL);
  empty_queues();

  while (!check_for_hardware());
  Serial.println("WiFi module detected!");
  empty_queues();

  while (!try_to_connect(wifi, password));
  Serial.println("WiFi connected!");
  empty_queues();
}

bool IrcHelper::check_for_hardware() {
  Serial.println("Looking for WiFi module...");
  WIFI_SERIAL.println("AT");
  return wait_for_response("OK", 1000);
}

bool IrcHelper::try_to_connect(String wifi, String password) {
  Serial.print("Trying to connect to network ");
  Serial.print(wifi);
  Serial.println("...");
  WIFI_SERIAL.println("AT+CWJAP=\"" + wifi + "\",\"" + password + "\"");
  return wait_for_response("OK", 10000); // Wait up to 10 seconds for the connection
}

bool IrcHelper::tcp_send(String data) {
  empty_queues();
  WIFI_SERIAL.print("AT+CIPSENDEX=");
  WIFI_SERIAL.println(data.length());
  if (!wait_for_response(">", 1000)) {
    return false;
  }
  WIFI_SERIAL.println(data);
  return wait_for_response("SEND OK", 1000);
}

void IrcHelper::empty_queues() {
  WIFI_SERIAL.flush();
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

  if (!tcp_send("PASS " + password + "\r\n")) {
    Serial.println("Could not send password!");
    return false;
  } else {
    Serial.println("Sent password!");
  }

  if (!tcp_send("NICK " + username + "\r\n")) {
    Serial.println("Could not send nickname!");
    return false;
  } else {
    Serial.println("Sent nickname!");
  }

  return wait_for_response(expected, 1000);
}

bool IrcHelper::join_channel(String channel) {
  if (!tcp_send("JOIN #" + channel + "\r\n")) {
    Serial.println("Could not join channel!");
    return false;
  }

  // Prepare the line buffer to receive data.
  _linebuf = "";

  // Wait for the end of the "names" list
  return wait_for_response("list", 3000);
}

int IrcHelper::try_read(unsigned long timeout) {
  if (!wait_for_response("IPD,", timeout, true)) {
    return 0;
  }

  // Determine how much data is available
  String length_of_data = "";
  while (length_of_data.length() < 100) {
    // Look for a ":", saving all data in the meantime to the buffer (excluding the colon).
    if (WIFI_SERIAL.available()) {
      const char c = WIFI_SERIAL.read();
      if (c == ':') {
        break;
      }
      length_of_data = length_of_data + c;
    }
  }
  const int amount_to_read = length_of_data.toInt();
  if (amount_to_read == 0) {
    Serial.println("    > could not determine length of data - some messages may be lost!");
    return 0;
  }
  Serial.print("    > IPD found, data length: ");
  Serial.println(amount_to_read);
  // The stupidest bug in existance is here: since the string literal is a pointer to part of the
  // statics section of memory, adding the amount_to_read integer to it made println print garbage
  // data. It could have crashed, but instead it tended to print part of the "connected to twitch chat"
  // message.
  // Serial.println("    > IPD found, data length: " + amount_to_read);

  int amount_read = 0;
  while (amount_read < amount_to_read) {
    if (WIFI_SERIAL.available()) {
      const char c = WIFI_SERIAL.read();
      _linebuf = _linebuf + c;
      amount_read += 1;
    }
  }

  return amount_read;
}

bool IrcHelper::is_message_received(String& sender, String& message) {
  // The _linebuf might contain many lines, so look for a "\n" and consider everything before that
  // a single line.
  const int line_end = _linebuf.indexOf("\n");
  if (line_end == -1) {
    return false;
  }

  // Split the buffer on the newline character.
  const String data = _linebuf.substring(0, line_end).trim();
  // Consume this line off the line buffer
  _linebuf = _linebuf.substring(line_end).trim();

  // Handle the case where the received message is a PING.
  if (is_ping_received(data)) {
    Serial.println("PING received");
    send_pong();
    return true;
  }

  // The name alwasys comes first. It is prefixed with a ":" and terminated with a "!".
  const int name_end = data.indexOf("!");
  if (name_end == -1) {
    return false;
  }
  sender = data.substring(1, name_end);

  // The only other ":"s prefix the message (or may be a part of the message).
  const int message_start = data.indexOf(":", 1);
  if (message_start == -1) {
    return false;
  }
  message = data.substring(message_start + 1);

  return true;
}

bool IrcHelper::is_ping_received(String data) {
  return data.startsWith("PING");
}

bool IrcHelper::send_pong() {
  String pong = "PONG :tmi.twitch.tv\r\n";
  Serial.println("    > responding with " + pong);
  const bool success = tcp_send(pong);
  Serial.println(String("    > pong send ") + (success ? "success" : "FAIL"));
  return success;
}

void IrcHelper::trim_buffer() {
  if (_linebuf.length() > 8192) {
    Serial.println("Warning: line buffer filled up. Clearing it and losing messages...");
    _linebuf = "";
  }
}

bool wait_for_response(String expected, unsigned long timeout, bool quiet) {
  if (!quiet) {
    Serial.println("    > waiting for \"" + expected + "\"");
  }
  String response = "";
  const unsigned long start = millis();
  while (millis() - start < timeout) {
    if (WIFI_SERIAL.available() > 0) {
      char c = WIFI_SERIAL.read();
      response = String(response + c);
      if (response.endsWith(expected)) {
        if (!quiet) {
          Serial.println("    > found it!");
        }
        return true;
      }
    }
  }
  if (!quiet) {
    Serial.println("    > didn't find it, found this instead:");
    Serial.println(response);
    Serial.println("--------");
  }
  return false;
}
