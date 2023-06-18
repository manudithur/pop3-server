#ifndef _MONITOR_UTILS_H_
#define _MONITOR_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define MAX_SEND_LENGTH 512

int sendCommand(int sock, char * command);

int createSocket(char * ip, char * port);

int closeSocket(int sock);

#endif