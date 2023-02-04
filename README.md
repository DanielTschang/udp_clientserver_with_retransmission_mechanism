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
- Instead of waiting for a time interval, I reach the similar result by increasing the timeout interval.
  - If a message is sent, but the client didn't get a response within a timeout time, The client increase the response timeout interval.
  - instead of "wait then send" , I send then wait for response.
  - Modify a little of codes can reach the original requirement.
- re-try when:
  - Sending timeout or failed.
  - The server didn't send(echo) any message back.

## 假設
- 我把re-try的機制改為，當一個時間區間內，我沒有收到response或者傳送沒有出去，我就認定他為失敗。
  - 原本是 等待一個時間再傳送；我是傳送後，若超過可容忍時間沒有回應則在傳送，而可容忍時間會透過exponential backoff algorithm調整。
  - 若要修改成原先做法，只需調整幾行code。（不做timeout，直接等待某段時間再傳送）
- 當以下狀況發現視為失敗，就會重新傳送:
  - 傳送等待過久或失敗。
  - 伺服器沒有回傳訊息。

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





