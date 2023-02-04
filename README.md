# udp_clientserver_with_retransmission_mechanism

## Environment
- Debian 10.13
- gcc version 8.3.0 (Debian 8.3.0-6)

## Default configurations
```c
multiplier = 2.0;
base wait interval = 0.5;
max wait interval = 8.0;
ip address = 127.0.0.1;
port = 5000;
```
- These can be change by ```char** argv```.

## Assumption
- re-try when:
  - Sending timeout or failed.
  - The server didn't echo the message.

## Build
```c
gcc Clinet.c utils.c -o Client -lm
gcc Server.c utils.c -o Server
```

## Run
#### Client
```c
./Client <port> <ip address> <multiplier> <baseWaitInterval> <maxWaitInterval> <maxTry>
or
./Client <port> <multiplier> <baseWaitInterval> <maxWaitInterval> <maxTry>
or
./Client <port> <ip address>
or
./Client <port> 
or
./Client 
```

#### Server
```c
./Server <port> <ip adadress>
or
./Server <port>
or
./Server
```

