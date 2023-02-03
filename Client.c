#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <math.h>

#include "utils.h"

extern int bufferSize; // get the buffer size of utils.c
int getIsLowerThanMaxRetry(); // check if the sending times is still lower than max re-try times.
float getNewWaitInterval(int,float ,float, float); // get the new wait intervals in seconds.
int getSecondsOfFloatSeconds(float); //get the number of seconds.
int getMicroSecondsOfFloatSeconds(float); // get the number of microseconds.
void setTimeout(struct timeval*, int , int ); // set the time interval
void setSocketOption(int*, struct timeval*); // set the option of socket.
void handleFailed(const int , int * ,struct timeval *, int ); //handle when sending is failed.

static char* fileName;

int main(int argc, char **argv)
{
    fileName = argv[0];
    if(argc < 2 || argc > 3)
    {
        e_ErrorType error = Input;
        errorHandler(fileName, error);
    }

    char *SocketIPAddress;
    int SocketPort;
    SocketPort = getPort(argc, argv);
    SocketIPAddress = getIpAddress(argc, argv);

    struct sockaddr_in IPaddress;

    char SendingBuffer[bufferSize];
    char ReceivingBuffer[bufferSize];

    socklen_t addressSize;
    int SocketFD;
    //Declare the socket
    SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    IPaddress.sin_port = htons(SocketPort);
    IPaddress.sin_addr.s_addr = inet_addr(SocketIPAddress);
    IPaddress.sin_family = AF_INET;

    bzero(SendingBuffer, bufferSize);
    char input[bufferSize];
    getInput(input);
    strcpy(SendingBuffer, input);

    //Set Timeout
    struct timeval timeout;
    setTimeout(&timeout, 0, 500000);
    setSocketOption(&SocketFD, &timeout);


    int isSent = -1;
    int isReceived = -1;
    int lowerThanMaxRetry;
    lowerThanMaxRetry = getIsLowerThanMaxRetry();

    int SentTimes = 0;
    while(isReceived < 0 && lowerThanMaxRetry){
        isSent = sendto(SocketFD, SendingBuffer, bufferSize, 0, (struct sockaddr*) &IPaddress, sizeof(IPaddress));
        SentTimes++;
        if(isSent < 0){
            handleFailed(SentTimes, &lowerThanMaxRetry, &timeout, SocketFD);
            continue;
        }
        printf("[+]Data sent : %s\n", SendingBuffer);

        bzero(ReceivingBuffer, bufferSize);
        addressSize = sizeof(IPaddress);
        isReceived = recvfrom(SocketFD, ReceivingBuffer, bufferSize, 0, (struct sockaddr*)&IPaddress, &addressSize);

        // < 0 means it's failed.
        if( isReceived < 0 )
        {
            handleFailed(SentTimes, &lowerThanMaxRetry, &timeout, SocketFD);
            continue;
        }
        else{
            printf("[+]Data received: %s\n", ReceivingBuffer);
        }

    }

    if(isReceived < 0)
    {
        printf("[-]Message sending exceeded the maximum re-try times!\n");
        exit(1);
    }
    exit(0);

//    close(SocketFD);
    return 0;
}

int getIsLowerThanMaxRetry()
{
    static int times = 0;
    if(times >= 10) return 0;
    times++;
    return 1;
}

float getNewWaitInterval(int n, float baseInterval, float multiplier, float maxWaitInterval )
{
    float waitInterval = baseInterval * pow(multiplier,n-1);
    if(waitInterval > maxWaitInterval) return maxWaitInterval;
    return waitInterval;
}

int getSecondsOfFloatSeconds(float waitInterval)
{
    return (int)waitInterval;
}

int getMicroSecondsOfFloatSeconds(float waitInterval)
{
    int Seconds = getSecondsOfFloatSeconds(waitInterval);
    int microSeconds = 1000000*(waitInterval - Seconds);

    return microSeconds;
}

void setTimeout(struct timeval* timeout, int seconds, int microsecond)
{
    timeout->tv_sec = seconds;
    timeout->tv_usec = microsecond;
}

void setSocketOption(int *SocketFD, struct timeval* timeout)
{
    if(setsockopt(*SocketFD, SOL_SOCKET, SO_RCVTIMEO,timeout,sizeof(*timeout))<0){
        e_ErrorType error = SetSocketOpt;
        errorHandler(fileName, error);
    }

    if(setsockopt(*SocketFD, SOL_SOCKET, SO_SNDTIMEO,timeout,sizeof(*timeout))<0){
        e_ErrorType error = SetSocketOpt;
        errorHandler(fileName, error);
    }
}

void handleFailed(const int SentTimes, int *lowerThanMaxRetry ,struct timeval *timeout, int SocketFD)
{
    float multiplier = 2.f;
    float baseWaitInterval = 0.5;
    int maxWaitInterval = 8;
    float newInterval = getNewWaitInterval(SentTimes, baseWaitInterval, multiplier, maxWaitInterval);
    int Seconds = getSecondsOfFloatSeconds(newInterval);
    int microSeconds = getMicroSecondsOfFloatSeconds(newInterval);

    setTimeout(timeout, Seconds, microSeconds);
    setSocketOption(&SocketFD, timeout);
    printf("[-]Message sending Failed!, Resending it after %f Seconds...\n", newInterval);

    *lowerThanMaxRetry = getIsLowerThanMaxRetry(); // Check if it is still lower than Max re-try times
}
