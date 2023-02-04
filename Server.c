#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "utils.h"

extern int bufferSize;

void main(int argc, char **argv)
{
    char *ServerIpAddress;
    int ServerPort;
    ServerPort = getPort(argc, argv);
    ServerIpAddress = getIpAddress(argc, argv);

    printf("--------------------------------\n");
    printf("UDP Server is running\n");
    printf("The configurations : \n");
    printf("\tThe ip address is %s\n", ServerIpAddress);
    printf("\tThe port is %d\n", ServerPort);
    printf("--------------------------------\n");

    struct sockaddr_in serverAddress, clientAddress;
    char buffer[bufferSize];
    socklen_t addressSize;
    int SocketFD;


    //Create a UDP socket
    SocketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if(SocketFD < 0){
        e_ErrorType error = Socket;
        errorHandler(argv[0], error);
    }

    serverAddress.sin_port = htons(ServerPort);
    serverAddress.sin_addr.s_addr = inet_addr(ServerIpAddress);
    serverAddress.sin_family = AF_INET;
    int connection = bind(SocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if(connection < 0){
        e_ErrorType error = Bind;
        errorHandler(argv[0], error);
    }

    cleanBuffer(buffer);

    addressSize = sizeof(clientAddress);
    //receive the datagram
    int index;
    index = recvfrom(SocketFD, buffer, sizeof(buffer),
                 0, (struct sockaddr*)&clientAddress, &addressSize); //receive message from server
    buffer[index] = '\n';
    printf("[+]Message received : %s\n", buffer);

    sendto(SocketFD, buffer, 1024, 0,(struct sockaddr*)&clientAddress, sizeof(clientAddress));
    printf("[+]Message sent back : %s\n", buffer);

//    close(SocketFD);
    exit(0);
}


