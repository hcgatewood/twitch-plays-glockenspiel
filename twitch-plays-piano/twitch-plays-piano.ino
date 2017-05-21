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
const Note BOTTOM_NOTE = Note(4, NoteName::G);

// The delay between powering and unpowering a solenoid.
const unsigned long NOTE_DELAY = 10;

// The delay between playing notes in a chord.
const unsigned long CHORD_DELAY = 1;

// The number of milliseconds to wait between each check
// that we're still connected to wifi.
const int WIFI_CHECK_INTERVAL_MS = 20 * 1000;
elapsedMillis ms_since_last_wifi_check;

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

void connect_wifi() {
  bool connection = false;
  bool joined = false;

  helper = new IrcHelper("6s08", "iesc6s08");
  Serial.println("IRC helper constructed!");

  Serial.println("Connecting to Twitch chat...");

  // Connect to Twitch IRC server
  while (!connection) {
    connection = helper->connect_to("irc.chat.twitch.tv", 6667, NICKNAME, PASSWORD, IRC_READY_MSG);
    if (!connection) {
      Serial.println("Could not connect to Twitch chat! Retrying. FeelsBadMan");
    }
  }
  Serial.println("Connected to Twitch chat! PogChamp");

  // Join our channel
  while (!joined) {
    joined = helper->join_channel(NICKNAME);
    if (!joined) {
      Serial.println("Could not join channel! Retrying.");
    }
  }
  Serial.println("Channel joined, messages incoming!");

  digitalWrite(LED_PIN, HIGH);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LED_PIN, OUTPUT);

  pinMode(OUTPUT_BIT_0, OUTPUT);
  pinMode(OUTPUT_BIT_1, OUTPUT);
  pinMode(OUTPUT_BIT_2, OUTPUT);
  pinMode(OUTPUT_BIT_3, OUTPUT);
  pinMode(OUTPUT_BIT_4, OUTPUT);
  clear_address();

  Serial.println("Constructing IRC helper...");
  ms_since_last_wifi_check = 0;
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
      } else if (message.startsWith("play_demo") && (sender == "ununoctium118" || sender == "hcgatewood")) {
        demo();
      }
    }
    helper->trim_buffer();
  }

  if (ms_since_last_wifi_check > WIFI_CHECK_INTERVAL_MS) {
    ms_since_last_wifi_check = 0;
    Serial.println("Checking wifi connection");
    bool connection = helper->is_connected();
    if (!connection) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("    > connection failed");
      Serial.println("    > connection restarting");
      connect_wifi();
    } else {
      Serial.println("    > connection ok");
    }
  }
}

void note(Note n) {
  write_address(n.get_output_address(BOTTOM_NOTE));
  delay(NOTE_DELAY);
  clear_address();
}

void chord(Note n1, Note n2, Note n3) {
  write_address(n1.get_output_address(BOTTOM_NOTE));
  delay(NOTE_DELAY);
  clear_address();
  delay(CHORD_DELAY);

  write_address(n2.get_output_address(BOTTOM_NOTE));
  delay(NOTE_DELAY);
  clear_address();
  delay(CHORD_DELAY);

  write_address(n3.get_output_address(BOTTOM_NOTE));
  delay(NOTE_DELAY);
  clear_address();
  delay(CHORD_DELAY);
}

void chord(Note n1, Note n2) {
  write_address(n1.get_output_address(BOTTOM_NOTE));
  delay(NOTE_DELAY);
  clear_address();
  delay(CHORD_DELAY);

  write_address(n2.get_output_address(BOTTOM_NOTE));
  delay(NOTE_DELAY);
  clear_address();
  delay(CHORD_DELAY);
}

#define CHORD3(n1, o1, n2, o2, n3, o3) chord(Note(o1, NoteName::n1), Note(o2, NoteName::n2), Note(o3, NoteName::n3))
#define CHORD2(n1, o1, n2, o2) chord(Note(o1, NoteName::n1), Note(o2, NoteName::n2))
#define NOTE(n1, o1) note(Note(o1, NoteName::n1))

void demo() {
  // Chromatic run, q=80, 750 ms/beat, 188 ms/sixteenth
  for (int i = 0; i < 25; i++) {
    write_address(i + 1);
    delay(NOTE_DELAY);
    clear_address();
    delay(178);
  }

  delay(1000);

  // We Are Number One, q=168, 357 ms/beat, 179 ms/eigth, 536 ms/dotted quarter, 89 ms/sixteenth
  NOTE(G, 5); delay(526);
  NOTE(D, 6); delay(169);

  NOTE(Cis, 6); delay(79);
  NOTE(D, 6); delay(79);
  NOTE(Cis, 6); delay(79);
  NOTE(D, 6); delay(79);
  NOTE(Cis, 6); delay(169);
  NOTE(D, 6); delay(169);

  NOTE(Ais, 6); delay(347);
  NOTE(G, 5); delay(347);

  delay(179);
  NOTE(G, 5); delay(169);
  NOTE(Ais, 6); delay(169);
  NOTE(D, 6); delay(169);

  NOTE(Dis, 6); delay(347);
  NOTE(Ais, 6); delay(347);

  NOTE(Dis, 6); delay(347);
  NOTE(F, 6); delay(347);

  NOTE(D, 6); delay(169);
  NOTE(Dis, 6); delay(169);
  NOTE(D, 6); delay(169);
  NOTE(Dis, 6); delay(169);

  NOTE(D, 6); delay(347);

  delay(1000);

  // Cruel Angel's Thesis, q=120, 500 ms/beat, 250 ms/eigth, 375 ms/dotted eigth, 125 ms/sixteenth
  CHORD2(D, 5, G, 5); delay(458);
  CHORD2(G, 5, Ais, 6); delay(458);
  CHORD3(Dis, 5, G, 5, C, 6); delay(342);
  CHORD3(Dis, 5, G, 5, Ais, 6); delay(342);
  NOTE(C, 6); delay(240);

  CHORD3(F, 5, A, 6, C, 6); delay(217);
  CHORD2(A, 6, C, 6); delay(238);
  CHORD2(A, 6, F, 6); delay(238);
  CHORD2(G, 5, Dis, 6); delay(238);
  CHORD2(F, 5, D, 6); delay(103);
  CHORD2(Dis, 5, C, 6); delay(228);
  CHORD2(F, 5, D, 6); delay(103);
  delay(500);

  CHORD3(G, 5, Ais, 6, D, 6); delay(467);
  CHORD3(G, 5, Ais, 6, F, 6); delay(467);
  CHORD2(G, 5, G, 6); delay(353);
  CHORD2(G, 5, C, 6); delay(353);
  NOTE(Ais, 6); delay(240);

  CHORD2(A, 6, F, 6); delay(228);
  CHORD2(A, 6, F, 6); delay(228);
  NOTE(D, 6); delay(240);
  NOTE(F, 6); delay(240);
  CHORD3(Ais, 6, D, 6, F, 6); delay(342);
  CHORD3(Ais, 6, D, 6, G, 6); delay(342);

  delay(1000);

  // Prelude #1, q=72, 833 ms/beat, 208 ms/sixteenth
  NOTE(A, 5); delay(198);
  NOTE(Cis, 5); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);
  NOTE(A, 5); delay(198);
  NOTE(Cis, 5); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);

  NOTE(A, 5); delay(198);
  NOTE(B, 5); delay(198);
  NOTE(Fis, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);
  NOTE(Fis, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);
  NOTE(A, 5); delay(198);
  NOTE(B, 5); delay(198);
  NOTE(Fis, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);
  NOTE(Fis, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);

  NOTE(Gis, 5); delay(198);
  NOTE(B, 5); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);
  NOTE(Gis, 5); delay(198);
  NOTE(B, 5); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(B, 6); delay(198);
  NOTE(D, 6); delay(198);

  NOTE(A, 5); delay(198);
  NOTE(Cis, 5); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);
  NOTE(A, 5); delay(198);
  NOTE(Cis, 5); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);
  NOTE(E, 5); delay(198);
  NOTE(A, 6); delay(198);
  NOTE(Cis, 6); delay(198);

  delay(1000);

  // Imperial March, q=108, 555 ms/beat, 278 ms/eigth, 417 ms/dotted eigth, 139 ms/sixteenth
  CHORD2(Ais, 5, G, 5); delay(533);
  CHORD2(Ais, 5, G, 5); delay(533);
  CHORD2(Ais, 5, G, 5); delay(533);
  CHORD2(Ais, 5, Dis, 5); delay(395);
  NOTE(Ais, 6); delay(129);

  CHORD2(Ais, 5, G, 5); delay(533);
  CHORD2(Ais, 5, Dis, 5); delay(395);
  NOTE(Ais, 6); delay(129);
  CHORD2(Ais, 5, G, 5); delay(533);
  delay(555);

  CHORD2(G, 5, D, 6); delay(533);
  CHORD2(G, 5, D, 6); delay(533);
  CHORD2(G, 5, D, 6); delay(533);
  CHORD2(Fis, 5, Dis, 6); delay(395);
  NOTE(Ais, 6); delay(129);

  CHORD2(Dis, 5, Fis, 5); delay(533);
  NOTE(Dis, 5); delay(407);
  NOTE(Ais, 6); delay(129);
  CHORD2(D, 5, G, 5); delay(1068);

  delay(1000);

  // Mario, q=200, 300 ms/beat, 150 ms/eigth, 200 ms/quarter note triplet
  CHORD3(D, 5, Fis, 5, E, 6); delay(117);
  CHORD3(D, 5, Fis, 5, E, 6); delay(117);
  delay(150);
  CHORD3(D, 5, Fis, 5, E, 6); delay(117);
  delay(150);
  CHORD3(D, 5, Fis, 5, C, 6); delay(117);
  CHORD3(D, 5, Fis, 5, E, 6); delay(117);
  delay(150);

  CHORD3(G, 5, B, 6, G, 6); delay(117);
  delay(3 * 150);
  CHORD2(G, 4, G, 5); delay(128);
  delay(3 * 150);

  for (int i = 0; i < 2; i++) {
    CHORD3(G, 4, E, 5, C, 6); delay(117);
    delay(2 * 150);
    CHORD2(C, 5, G, 5); delay(128);
    delay(2 * 150);
    CHORD2(G, 4, E, 5); delay(128);
    delay(150);

    delay(150);
    CHORD3(C, 5, F, 5, A, 6); delay(117);
    delay(150);
    CHORD3(D, 5, G, 5, B, 6); delay(117);
    delay(150);
    CHORD3(Cis, 5, Fis, 5, Ais, 6); delay(117);
    CHORD3(C, 5, F, 5, A, 6); delay(117);
    delay(150);

    CHORD3(C, 5, E, 5, G, 5); delay(167);
    CHORD3(C, 5, G, 5, E, 6); delay(167);
    CHORD3(E, 5, B, 6, G, 6); delay(167);
    CHORD3(C, 5, F, 5, A, 6); delay(117);
    delay(150);
    CHORD3(D, 5, A, 6, F, 6); delay(117);
    CHORD3(E, 5, B, 6, G, 6); delay(117);

    delay(150);
    CHORD3(C, 5, A, 6, E, 6); delay(117);
    delay(150);
    CHORD3(A, 5, E, 5, C, 6); delay(117);
    CHORD3(B, 5, F, 5, D, 6); delay(117);
    CHORD3(G, 4, D, 5, B, 6); delay(117);
    delay(2 * 150);
  }

  NOTE(C, 5); delay(140);
  delay(150);
  CHORD2(E, 6, G, 6); delay(128);
  CHORD3(G, 4, Dis, 6, Fis, 6); delay(117);
  CHORD2(D, 6, F, 6); delay(128);
  CHORD2(B, 6, Dis, 6); delay(128);
  NOTE(C, 5); delay(140);
  CHORD2(C, 6, E, 6); delay(128);

  NOTE(F, 5); delay(140);
  CHORD2(E, 5, Gis, 5); delay(128);
  CHORD2(F, 5, A, 6); delay(128);
  CHORD3(C, 5, G, 5, C, 6); delay(117);
  NOTE(C, 5); delay(140);
  CHORD2(C, 5, A, 6); delay(128);
  CHORD3(E, 5, F, 5, C, 6); delay(117);
  CHORD2(F, 5, D, 6); delay(128);

  NOTE(C, 5); delay(140);
  delay(150);
  CHORD2(E, 6, G, 6); delay(128);
  CHORD3(G, 4, Dis, 6, Fis, 6); delay(117);
  CHORD2(D, 6, F, 6); delay(128);
  CHORD2(B, 6, Dis, 6); delay(128);
  NOTE(C, 5); delay(140);
  CHORD2(C, 6, E, 6); delay(128);

  delay(150);
  CHORD3(F, 5, G, 5, C, 6); delay(117);
  delay(150);
  CHORD3(F, 5, G, 5, C, 6); delay(117);
  CHORD3(F, 5, G, 5, C, 6); delay(117);
  delay(150);
  NOTE(G, 4); delay(140);
  delay(150);

  NOTE(C, 5); delay(140);
  delay(150);
  CHORD2(E, 6, G, 6); delay(128);
  CHORD3(G, 4, Dis, 6, Fis, 6); delay(117);
  CHORD2(D, 6, F, 6); delay(128);
  CHORD2(B, 6, Dis, 6); delay(128);
  NOTE(C, 5); delay(140);
  CHORD2(C, 6, E, 6); delay(128);

  NOTE(F, 5); delay(140);
  CHORD2(E, 5, Gis, 5); delay(128);
  CHORD2(F, 5, A, 6); delay(128);
  CHORD3(C, 5, G, 5, C, 6); delay(117);
  NOTE(C, 5); delay(140);
  CHORD2(C, 5, A, 6); delay(128);
  CHORD3(E, 5, F, 5, C, 6); delay(117);
  CHORD2(F, 5, D, 6); delay(128);

  NOTE(C, 5); delay(140);
  delay(150);
  CHORD3(Gis, 4, Gis, 5, Dis, 6); delay(117);
  delay(2 * 150);
  CHORD3(Ais, 5, F, 5, D, 6); delay(117);
  delay(2 * 150);
  CHORD3(C, 5, E, 5, C, 6); delay(117);

  delay(1000);
}
