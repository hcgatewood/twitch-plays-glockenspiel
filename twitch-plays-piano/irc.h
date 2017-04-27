#ifndef IRC_H
#define IRC_H

#include <Arduino.h>

class IrcHelper {
  private:

    bool check_for_hardware();
    bool try_to_connect(String wifi, String password);
    bool tcp_send(String data);
    void empty_queues();

  public:
    IrcHelper(String wifi, String password);

    // Connects to the IRC server at the specified url and port, with the given username, password, and success string.
    bool connect_to(String url, int port, String username, String password, String expected);

    // The callback takes the username of the sender and the message itself, and is called whenever a message is received.
    bool join_channel(String channelName);
};

bool wait_for_response(String response, unsigned long timeout);

#endif
