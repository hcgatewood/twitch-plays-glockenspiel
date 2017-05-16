#ifndef IRC_H
#define IRC_H

#include <Arduino.h>

class IrcHelper {
  private:

    bool check_for_hardware();
    bool try_to_connect(String wifi, String password);
    // Include "\r\n" at the end of every data string.
    bool tcp_send(String data);
    void empty_queues();

    // Check the data for a received ping.
    bool is_ping_received(String data);
    // Send a pong back to the server.
    bool send_pong();

  public:
    String _linebuf;

    IrcHelper(String wifi, String password);

    // Connects to the IRC server at the specified url and port, with the given username, password, and success string.
    bool connect_to(String url, int port, String username, String password, String expected);

    // The callback takes the username of the sender and the message itself, and is called whenever a message is received.
    bool join_channel(String channel_name);

    // Checks the buffer for received messages. Consumes a line of the buffer.
    bool is_message_received(String& sender, String& message);

    // Tries to read data into the buffer, if it is present. Returns the amount of data read.
    int try_read(unsigned long timeout);

    // Trims the buffer, if it is too long. This is a safety mechanism to ensure that we don't run out of memory.
    void trim_buffer();

    // Check the AP connection. Return true if still connected, false otherwise.
    bool is_connected();
};

bool wait_for_response(String response, unsigned long timeout, bool quiet = false);

#endif
