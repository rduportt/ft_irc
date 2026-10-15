*This project has been created as part of the 42 curriculum by rduport, tirimesc.*

# ft_irc — IRC Server in C++98

## Description

ft_irc is an Internet Relay Chat server implemented in C++98. The goal of this project is to build a fully functional IRC server that real IRC clients (such as irssi) can connect to and interact with.

The server handles multiple clients simultaneously using a single `poll()` call with non-blocking I/O. It supports authentication, channels, private messaging, and channel operator commands.

## Features

- Multi-client support with non-blocking I/O (`poll()`)
- TCP/IP communication (IPv4)
- User authentication with password
- Nickname and username registration
- Channel creation and management
- Private messaging (user to user and user to channel)
- Channel operator commands: `KICK`, `INVITE`, `TOPIC`, `MODE`
- Channel modes: `+i` (invite-only), `+t` (topic restricted), `+k` (password), `+o` (operator), `+l` (user limit)

## Instructions

### Compilation

```bash
make
```

### Running the server

```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 mypassword
```

### Connecting with irssi

```bash
irssi
/connect 127.0.0.1 6667 mypassword yournick
/join #general
```

### Connecting with netcat (for debugging)

```bash
nc -C 127.0.0.1 6667
PASS mypassword
NICK alice
USER alice 0 * :Alice
JOIN #general
PRIVMSG #general :hello world
```

### Makefile rules

| Rule | Description |
|------|-------------|
| `make` | Compile the project |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binary |
| `make re` | Recompile from scratch |

## Supported Commands

| Command | Description |
|---------|-------------|
| `PASS` | Authenticate with server password |
| `NICK` | Set or change nickname |
| `USER` | Register username and realname |
| `JOIN` | Join or create a channel |
| `PART` | Leave a channel |
| `PRIVMSG` | Send a message to a user or channel |
| `KICK` | Eject a user from a channel (operator only) |
| `INVITE` | Invite a user to a channel (operator only) |
| `TOPIC` | View or change the channel topic |
| `MODE` | Change channel modes (operator only) |
| `PING` | Keep-alive with the server |
| `QUIT` | Disconnect from the server |

## Resources

### IRC Protocol

- [RFC 1459 — Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
- [Modern IRC documentation](https://modern.ircdocs.horse/)

### Reference Client

- [irssi](https://irssi.org/) — used as the reference IRC client for testing

### AI Usage

AI was used during this project for the following tasks:

- **Architecture guidance** — understanding how to structure the server, client, channel, and command classes
- **Protocol clarification** — explaining IRC message formats, numeric replies, and the RFC specifications
- **Debugging assistance** — identifying bugs in parsing, mode handling, and buffer management