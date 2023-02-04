#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

int bufferSize = 1024;

/*
 * Checking if an IP-address is validated.
 */
int validateIP(char *ipAddress)
{
    if (ipAddress == NULL) return 0;
    char* ipAddressCopy = malloc(strlen(ipAddress)+1);
    strcpy(ipAddressCopy, ipAddress);
    int currentNumber, numberOfDots = 0;
    char *partIP;

    partIP = strtok(ipAddressCopy, "."); //cut the string by dot
    if (partIP == NULL) return 0;
    while (partIP) {
        if (!validateNumber(partIP)) return 0; //check whether the sub string is holding only number or not
        currentNumber = StringToInteger(partIP);
        if (currentNumber >= 0 && currentNumber <= 255)
        {
            partIP = strtok(NULL, "."); //cut the next part of the string
            if (partIP != NULL)
                numberOfDots++;
        }
        else return 0;
    }

    if (numberOfDots != 3) //if the number of dots are not 3, return false
        return 0;
    return 1;
}

/*
 * Check all the characters of a string are digit.
 */
int validateNumber(char *str)
{
    while (*str)
    {
        if(!isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

int validatePort(char *port)
{
    if(!validateNumber(port)) return 0;
    return 1;
}

int StringToInteger( char* s)
{
    return atoi(s);
}

void errorHandler(char* fileName, e_ErrorType e)
{
    switch (e) {
        case 0:
            printf("::INPUT ERROR:: File Name : %s \n",fileName);
            printf("Usage : %s <port> <ip>  or %s, <port>", fileName,fileName);
            break;
        case 1:
            printf("::PORT ERROR:: File Name : %s \n",fileName);
            printf("Please type in the correct syntax of a port");
            break;
        case 2:
            printf("::IP address ERROR:: File Name : %s \n",fileName);
            printf("Please type in the correct syntax of an IPv4 address");
            break;
        case 3:
            printf("::Socket Creation ERROR:: File Name : %s \n",fileName);
            perror("[-]Socket error");
            break;
        case 4:
            perror("[-]Bind error");
            break;
        default:
            perror("[-]Error occured");

    }
    exit(0);
}

char* getIpAddress(int argc, char** argv)
{
    if(argc <= 2 || argc == 5){
        return "127.0.0.1"; //localhost
    }
    if(argc == 3){
        if(validateIP(argv[2])){
            return argv[2];
        }
        else{
            e_ErrorType error = IP;
            errorHandler(argv[0], error);
        }
    }
    if(argc == 6){
        if(validateIP(argv[2])){
            return argv[2];
        }
        else{
            e_ErrorType error = IP;
            errorHandler(argv[0], error);
        }
    }
    return "127.0.0.1";
}

int getPort(int argc, char** argv)
{
    if(argc == 1) return 5000;
    if(validatePort(argv[1]))
    {
        return StringToInteger(argv[1]);
    }else{
        e_ErrorType error = Port;
        errorHandler(argv[0], error);
    }
    return 0;
}

void getInput(char input[])
{
    printf("type in the message : ");
    scanf("%s",input);
}


void cleanBuffer(char buffer[])
{
    memset(buffer, 0, bufferSize);
}
