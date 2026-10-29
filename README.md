# ft_irc

> An IRC server written in C++98.

## What's inside

- TCP sockets and non-blocking I/O with poll()
- Client authentication: PASS, NICK, USER
- Channels: JOIN, PRIVMSG, KICK, INVITE, TOPIC, MODE
- Channel operators and modes (i, t, k, o, l)

## Usage

```bash
make
./ircserv <port> <password>
```
Connect with an IRC client or with `nc -C localhost <port>`.

---

*Project completed at [42 Paris](https://42.fr/en/campus/paris/).*
