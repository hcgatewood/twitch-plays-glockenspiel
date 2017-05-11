"""
Read in data from notes.json and send to our Twitch channel.

Usage: `python3 play_notes.py`
Notes:
    - Careful about getting rate limited! The limits per 30 seconds
      are 30 messages for non-mods and 100 messages for mods. Pass that
      limit and your IP gets banned for 8 hours. See
      https://help.twitch.tv/customer/portal/articles/1302780-twitch-irc.
    - Expects fields in notes.json to be filled out (pass, nick, notes).
    - Notes field is array of strings of form e.g. 'C# 400', with two
      space-separated fields denoting the note/chord and then the delay
      in milliseconds until the next note should play.
"""

import json
import socket
import time

FILE = 'notes.json'
# FILE = 'my-notes.json'

TARGET = 'irc.chat.twitch.tv'
PORT = 6667
CHANNEL = 'piano_bot_s08'

# create an ipv4 (AF_INET) socket object using the tcp protocol (SOCK_STREAM)
CLIENT = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


def pass_nick_notes():
    with open(FILE, 'r') as f:
        raw_content = f.read()
        j = json.loads(raw_content)
        passw = j['pass']
        nick = j['nick']
        notes = j['notes']
        return (passw, nick, notes)


def send_msg(msg):
    send('PRIVMSG #{} :{}'.format(CHANNEL, msg))


def send(msg):
    CLIENT.send(bytes(msg + '\r\n', 'utf-8'))


def main():
    print('parsing json for pass, nick, notes')
    passw, nick, notes = pass_nick_notes()
    if passw is None or len(notes) == 0:
        print('> FIX YOUR NOTES.JSON')
        return
    print('\t> found pass:', passw)
    print('\t> found nick:', nick)
    print('\t> found notes:', notes)

    # connect the client
    print('connecting to twitch')
    print('\t> target:', TARGET)
    print('\t> port:', PORT)
    CLIENT.connect((TARGET, PORT))
    print('\t> connected')

    # send some data (in this case a HTTP GET request)
    print('sending pass and nick')
    send('PASS ' + passw)
    send('NICK ' + nick)

    # receive the response data (4096 is recommended buffer size)
    print('\nreceived response:')
    response1 = str(CLIENT.recv(4096), 'utf-8')
    print(response1)

    print('joining channel: ' + CHANNEL)
    send('JOIN #' + CHANNEL)

    print('\nreceived response:')
    response2 = ''
    while ':End of /NAMES list' not in response2:
        response2 += str(CLIENT.recv(4096), 'utf-8')
    print(response2)

    print('sending notes')
    for pair in notes:
        pair = pair.split()
        note = pair[0]
        pause_ms = int(pair[1])
        print('\t> note, pause:', note, pause_ms)
        send_msg(note)
        time.sleep(pause_ms / 1000)


if __name__ == '__main__':
    main()
