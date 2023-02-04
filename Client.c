#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <math.h>

#include "utils.h"

extern int bufferSize;

int getIsLowerThanMaxRetry(); // check if the sending times is still lower than max re-try times.
float getNewWaitInterval(int); // get the new wait intervals in seconds.
int getSecondsOfFloatSeconds(float); //get the number of seconds.
int getMicroSecondsOfFloatSeconds(float); // get the number of microseconds.
void setTimeout(struct timeval*, int , int ); // set the time interval
void setSocketOption(int*, struct timeval*); // set the option of socket.
void handleFailed(const int , int * ,struct timeval *, int ); //handle when sending is failed.
void configurationHandler(const int, char**);

static char* fileName;
static double multiplier = 2.f;
static double baseWaitInterval = 0.5;
static double maxWaitInterval = 8;
static int maxTry = 10;

int main(int argc, char **argv)
{
    configurationHandler(argc, argv);

    char *SocketIPAddress;
    int SocketPort;
    SocketPort = getPort(argc, argv);
    SocketIPAddress = getIpAddress(argc, argv);

    printf("--------------------------------------------------\n");
    printf("UDP Client is running\n");
    printf("The configurations : \n");
    printf("\tThe IP address is %s\n", SocketIPAddress);
    printf("\tThe Port is %d\n", SocketPort);
    printf("\tThe multiplier is %f\n", multiplier);
    printf("\tThe baseWaitInterval is %f\n", baseWaitInterval);
    printf("\tThe maxWaitInterval is %f\n", maxWaitInterval);
    printf("\tThe maxTry is %d\n", maxTry);
    printf("--------------------------------------------------\n");

    struct sockaddr_in IPaddress;

    char SendingBuffer[bufferSize];
    char ReceivingBuffer[bufferSize];

    socklen_t addressSize;
    addressSize = sizeof(IPaddress);

    int SocketFD;
    //Declare the socket
    SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    IPaddress.sin_port = htons(SocketPort);
    IPaddress.sin_addr.s_addr = inet_addr(SocketIPAddress);
    IPaddress.sin_family = AF_INET;

    cleanBuffer(SendingBuffer);
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
        isSent = sendto(SocketFD, SendingBuffer, bufferSize, 0, (struct sockaddr*) &IPaddress, addressSize);
        SentTimes++;
        if( isSent < 0 )  //failed
        {
            handleFailed(SentTimes, &lowerThanMaxRetry, &timeout, SocketFD);
            continue;
        }
        printf("[+]Message sent : %s\n", SendingBuffer);

        cleanBuffer(ReceivingBuffer);

        isReceived = recvfrom(SocketFD, ReceivingBuffer, bufferSize, 0, (struct sockaddr*)&IPaddress, &addressSize);


        if( isReceived < 0 )  //failed
        {
            handleFailed(SentTimes, &lowerThanMaxRetry, &timeout, SocketFD);
            continue;
        }
        else{
            printf("[+]Message received: %s\n", ReceivingBuffer);
        }

    }
//    close(SocketFD);
    if(isReceived < 0)
    {
        printf("[-]Message sending exceeded the maximum re-try times!\n");
        exit(1);
    }
    printf("[+]Message sent successfully!\n");
    exit(0);

    return 0;
}

void configurationHandler(const int argc, char** argv)
{
    fileName = argv[0];
    if(argc > 7 || argc == 4 )
    {
        e_ErrorType error = Input;
        errorHandler(fileName, error);
    }
    if(argc == 6){
        multiplier = atof(argv[2]);
        baseWaitInterval = atof(argv[3]);
        maxWaitInterval = atof(argv[4]);
        maxTry = atof(argv[5]);
    }
    if(argc == 7){
        multiplier = atof(argv[3]);
        baseWaitInterval = atof(argv[4]);
        maxWaitInterval = atof(argv[5]);
        maxTry = atof(argv[6]);
    }

}


int getIsLowerThanMaxRetry()
{
    static int times = 0;
    if(times >= maxTry) return 0;
    times++;
    return 1;
}

float getNewWaitInterval(int n)
{
    float waitInterval = baseWaitInterval * pow(multiplier,n-1);
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
    float newInterval = getNewWaitInterval(SentTimes);
    int Seconds = getSecondsOfFloatSeconds(newInterval);
    int microSeconds = getMicroSecondsOfFloatSeconds(newInterval);

    setTimeout(timeout, Seconds, microSeconds);
    setSocketOption(&SocketFD, timeout);
    printf("[-]Message sending Failed!, Resending it after %f Seconds...\n", newInterval);

    *lowerThanMaxRetry = getIsLowerThanMaxRetry(); // Check if it is still lower than Max re-try times
}

