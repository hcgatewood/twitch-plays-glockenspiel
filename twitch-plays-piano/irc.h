#ifndef IRC_H
#define IRC_H

#include <Arduino.h>

class IrcHelper {
  private:
    void (*callback)(String, String);
    bool has_tcp;
    bool has_channel;

    bool check_for_hardware();
    bool try_to_connect(String wifi, String password);
    bool tcp_send(String data);
    void empty_rx();

  public:
    IrcHelper(String wifi, String password);

    //
    bool connect_to(String url, int port, String username, String password, String expected);

    // The callback takes the username of the sender and the message itself, and is called whenever a message is received.
    bool join_channel(String channelName, void (*callback)(String, String));
};

bool wait_for_response(String response, unsigned long timeout);

#endif
