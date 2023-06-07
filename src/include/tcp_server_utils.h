#ifndef TCPSERVERUTIL_H_
#define TCPSERVERUTIL_H_

#include <stdio.h>
#include <sys/socket.h>
#include "selector.h"
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "logger.h"
#include "util.h"
#include "buffer.h"
#include "stm.h"
#include "states.h"

#define BUFFER_LEN 5000
#define ATTACHMENT(key) &(((client_data*)(key)->data))

typedef struct client_data{
    struct buffer rbStruct;
    struct buffer wbStruct;
    uint8_t rb[BUFFER_LEN];
    uint8_t wb[BUFFER_LEN];
    struct state_machine stm;
    int fd;
}client_data;


// Create, bind, and listen a new TCP server socket
int setupTCPServerSocket(const char *service);

void handleNewConnection(struct selector_key * key);

// Accept a new TCP connection on a server socket
int acceptTCPConnection(int servSock);

// Handle new TCP client
int handleTCPEchoClient(int clntSocket);

void closeConnection(struct selector_key * key);


#endif 
