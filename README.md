# udp_clientserver_with_retransmission_mechanism

## Environment
- Debian 10.13
- gcc version 8.3.0 (Debian 8.3.0-6)

## Default configurations
```c
multiplier = 2.0;
base wait interval = 0.5;
max wait interval = 8.0;
max re-try : 10;
ip address = 127.0.0.1;
port = 5000;
```
- These can be change by ```char** argv```.

## Assumption
- I assume the re-try mechanism:
  - If a message is sent, but the client didn't get a response within a timeout time, The client increase the timeout interval by exponential backoff algorithm.
  - Instead of "wait for a time then send" , I send then wait for response.
  - Modify a little code can reach the "wait for a time then send" result.
  - Since the requirement only says "The interval between each retry".
- re-try when:
  - Sending timeout or failed.
  - The server didn't send(echo) any message back.

## 假設
- 我假設re-try的機制是當一個"可容忍時間的時間"內，我沒有收到response或者傳送沒有出去(失敗)，我就增加下次的"可容忍時間的時間"。
  - 傳送後，若超過"可容忍時間"沒有回應，則再傳送。而可容忍時間會透過exponential backoff algorithm調整。
  - 若要修改成先"等待某個時間再傳送"，只需調整幾行code。（不做timeout，直接等待某段時間再傳送）
  - 因為requirement寫"The interval between each retry"，因此做這樣的假設
- 當以下狀況發現視為失敗，就會重新傳送:
  - 傳送等待過久或失敗。
  - 伺服器沒有回傳任何訊息。

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





