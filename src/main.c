// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>   
#include <arpa/inet.h>    
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> 
#include "logger.h"
#include "tcp_server_utils.h"
#include "selector.h"
#include "users.h"
#include "args.h"
#include "stats.h"

#define TRUE   1
#define FALSE  0
#define MAX_SOCKETS 1000
#define BUFFSIZE 1024
#define POP3_PORT 5000
#define MGMT_PORT 6000

static int killServer = FALSE;

static void sigterm_handler(int signum){
    killServer = TRUE;
}

int main(int argc , char *argv[]){

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    close(STDIN_FILENO);

    // Creamos el selector
    selector_status ss = SELECTOR_SUCCESS;
    fd_selector selector = NULL;

    struct selector_init conf = {
        .signal = SIGALRM,
        .select_timeout = {
            .tv_sec = 10, 
            .tv_nsec = 0 
            }
    };


    initUsers();
    char * ip = (argv+1)[0];
    char * port = (argv+2)[0];
    if (strcmp(ip, "localhost") == 0)
        ip = "::FFFF:127.0.0.1";
    if (!isIp(ip) || !isPort(port)) {
        fprintf(stderr, "Invalid IP or invalid port\nFormat: ip port -u user:pass\n");
        exit(1);
    }
    int portNumber = atoi(port);
    parseAndAddUsers(argc-3, argv+3);
    stats_init();

    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);


    if (0 != selector_inits(&conf)) {
        exit(1);
    }

    selector = selector_new(MAX_SOCKETS);
    if (selector == NULL) {
        selector_close();
        exit(1);
    }

    //POP3 Server socket
    int serverSocket = setupTCPServerSocket(ip, portNumber);

    fd_handler socket_handler = {
        .handle_read = handleNewConnection,
        .handle_write = NULL,
        .handle_block = NULL,
        .handle_close = NULL
    };

    ss = selector_register(selector, serverSocket, &socket_handler, OP_READ, NULL);
    if(ss != SELECTOR_SUCCESS){
        //handle error
        return 1;
    }


    //Management socket
    int mgmtSocket = setupTCPServerSocket(ip,MGMT_PORT);
    fd_handler mgmt_handler = {
            .handle_read = handleAdminConnection,
            .handle_write = NULL,
            .handle_block = NULL,
            .handle_close = NULL
    };

    ss = selector_register(selector, mgmtSocket, &mgmt_handler, OP_READ, NULL);
    if(ss != SELECTOR_SUCCESS){
        //handle error
        return 1;
    }


    while(!killServer){
        ss = selector_select(selector);
        if(ss != SELECTOR_SUCCESS){
            break;
        }
    }

 
    remove("../logs");
    selector_destroy(selector);
    selector_close();
    close(serverSocket);

    return 0;
}

