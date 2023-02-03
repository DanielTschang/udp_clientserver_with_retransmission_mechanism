#ifndef UDP_SERVER_UTILS_H
#define UDP_SERVER_UTILS_H
typedef enum e_ErrorType e_ErrorType;
enum e_ErrorType {Input, Port, IP ,Socket, Bind, Send, Receive, SetSocketOpt};
int validateNumber(char *);
int validateIP(char *);
int validatePort(char *);
int StringToInteger( char* );
void errorHandler(char*, e_ErrorType);
char* getIpAddress(int, char**);
int getPort(int, char**);
void getInput(char input[]);
void cleanBuffer(char []);

#endif //UDP_SERVER_UTILS_H
