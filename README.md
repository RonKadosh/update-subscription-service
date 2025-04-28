# Overview
The World Cup Update Subscription Service project simulates a live community-driven system where users can stay updated with real-time game events during the World Cup. (Written in 2023 and was relevant back then)
### Users can:
- Subscribe to different game channels
- Report updates (events) about matches
- Receive updates from other users subscribed to the same game

## This system includes both a server and a client:

### Server
- Written in Java.
- Provides full STOMP (Simple Text Oriented Messaging Protocol) server services.
- Supports two server modes:
  - Thread-Per-Client (TPC) mode
  - Reactor (Non-blocking I/O) mode
- Server mode is chosen via command-line arguments at server startup.
- Handles multiple users concurrently, with real-time event dissemination across topics (game channels).

### Client
- Written in C++.
- Implements the client-side logic for:
  - Connecting to the server
  - Subscribing to matches
  - Sending match reports
  - Receiving updates
  - Managing user session and game summaries
- Communicates with the server over TCP, using STOMP protocol for message exchange.

# File Structure
    server/
    ├── src/main/java/bgu/spl/net/
    │   ├── api/                # STOMP Protocol interfaces
    │   ├── impl/stomp/          # STOMP Protocol server implementation
    │   └── srv/                 # Server-side connection handling (TPC + Reactor)
    ├── pom.xml                  # Maven build file
    client/
    ├── src/
    │   ├── ConnectionHandler.cpp/h
    │   ├── StompClient.cpp
    │   ├── StompProtocol.cpp/h
    │   ├── User.cpp/h
    │   ├── event.cpp/h
    │   └── json.hpp             # JSON header (for event parsing)
    ├── include/                 # Header files
    ├── bin/                     # Compiled binaries
    ├── Makefile                 # Makefile for building the client

# Build Instructions

## Building the Server (Java)
Make sure you have Java 8 and Maven installed.
1. Navigate to the server/ directory:
```bash
cd server/
```
2. Build the project using Maven:
```bash
mvn clean install
```
3. After a successful build, the runnable .jar file will be generated inside:
```bash
server/target/server-1.0.jar
```

## Building the Client (C++)
- Make sure you have:
  - g++ compiler (supporting C++11)
  - Boost library installed (specifically boost::asio)
  - pthread library

1. Navigate to the client/ directory:
```bash
cd client/
```
2. Build the client by running:
```bash
make
```
3. After compilation, the executable will be located at:
```bash
client/bin/StompWCIClient
```
4. (Optional) To clean all compiled files:
```bash
make clean
```

### Important Notes
- Server Build Output:
  The server is a JAR file (server-1.0.jar) and needs to be executed with java -jar.
- Client Build Output:
  The client is a native executable (compiled binary).
- Both components must follow the STOMP 1.2 protocol and expect UTF-8 encoding, ending each frame with a null character \u0000.

# How To Run
## Start the Server
After building the server (see Build Instructions), you can start it with:
```bash
java -jar target/server-1.0.jar <port> <server_type>
```
  - <port> : The port number you want the server to listen on (e.g., 7777).
  - <server_type>: Choose between:
    - tpc → Thread-Per-Client model
    - reactor → Reactor model

### Example (Thread-Per-Client):
```bash
java -jar target/server-1.0.jar 7777 tpc
```
### Example (Reactor):
```bash
java -jar target/server-1.0.jar 7777 reactor
```

## Run the Client
After building the client (see Build Instructions), you can start the client by running:
```bash
bin/StompWCIClient
```
The client will immediately wait for input commands from the user.

## Client Commands
| Command | Syntax | Description |
|:--------|:-------|:------------|
| login | login <host>:<port> <username> <password> | Connects to the server at given host and port with provided credentials. |
| join | join <game_name> | Subscribes to updates from a specific game (e.g., `join Brazil_Argentina`). |
| exit | exit <game_name> | Unsubscribes from a specific game. |
| report | report <path_to_json_file> | Sends a report containing events loaded from a JSON file. |
| summary | summary <game_name> <user> <path_to_output_file> | Generates a game summary report for a specific user and saves it to a file. |
| logout | logout | Disconnects gracefully from the server. |

### Notes:
- Commands are **case-sensitive** and should be typed exactly as shown.
- After `login`, a socket listener thread is automatically started.
- `report` requires a properly structured JSON file (see example in [Example Event JSON](#example-event-json)).
- `summary` allows you to generate event summaries based on received updates.

## Example Session

Connect to server:
```bash
login 127.0.0.1:7777 username password
```

Subscribe to a game:
```bash
join Brazil_Argentina
```

Report events from a JSON file:
```bash
report events/brazil_vs_argentina.json
```

Generate a game summary:
```bash
summary Brazil_Argentina username summary.txt
```

Exit from the game channel:
```bash
exit Brazil_Argentina
```

Logout:
```bash
logout
```
## Example Event JSON
```bash
{ 
  "team a": "Brazil", 
  "team b": "Argentina", 
  "events": [ 
    { 
      "event name": "Kickoff", 
      "time": 0, 
      "general game updates": { "stadium": "Lusail Stadium" }, 
      "team a updates": { "coach": "Tite" }, 
      "team b updates": { "coach": "Scaloni" }, 
      "description": "The match has started." 
    }
  ] 
}
```
## Authors / Credits

- **Author:** Ron Kadosh
- **Course:** System Programming Laboratory (SPL), Ben-Gurion University (Semester A, 2023)
- **Notes:**  
  The basic project structure and the Server TCP/REACTOR implementation were provided by the course staff and TAs.  
  All core client-side logic and server-side STOMP protocol handling were implemented by the author.
