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
#include "parser.h"
#include "mgmt.h"
#include "pop3.h"
#include "pop3_parser_impl.h"
#include "pop3_actions.h"
#include "stats.h"

#define BUFFER_LEN 5000
#define MAX_COMMAND_LEN 40
#define MAX_ARG_LEN 100
#define ATTACHMENT(key) ((client_data*)(key)->data)

extern const struct parser_definition definition;


typedef struct command_data{
    char command[MAX_COMMAND_LEN];
    char arg1[MAX_ARG_LEN];
    char arg2[MAX_ARG_LEN];
    uint8_t commandLen, arg1Len, arg2Len;
}command_data;

typedef struct client_data{
    char * username;
    bool * emailDeleted;
    int emailCount;
    struct buffer rbStruct;
    struct buffer wbStruct;
    uint8_t rb[BUFFER_LEN];
    uint8_t wb[BUFFER_LEN];
    struct state_machine stm;
    int fd;
    struct parser * parser;
    struct command_data command;
    struct email * emailptr;
}client_data;

typedef struct mgmt_data{
    char * username;
    bool * emailDeleted;
    int emailCount;
    struct buffer rbStruct;
    struct buffer wbStruct;
    uint8_t rb[BUFFER_LEN];
    uint8_t wb[BUFFER_LEN];
    struct state_machine stm;
    int fd;
    struct parser * parser;
    struct command_data command;
    struct email * emailptr;
}mgmt_data;


static int setupSockAddr(char* addr, unsigned short port, void* res, socklen_t* socklenResult);

// Create, bind, and listen a new TCP server socket
int setupTCPServerSocket(char * ip, const int service);

// Accept a new TCP connection on a server socket
void handleNewConnection(struct selector_key * key);

void handleAdminConnection(struct selector_key * key);

void changeMaxConnections(int newMax);

void maxConnectionsReached(int clntSock);

bool isIp(char * ip);

bool isPort(char * port);

#endif 
