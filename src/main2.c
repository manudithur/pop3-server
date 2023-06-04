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
#include <sys/select.h>

#define TRUE   1
#define FALSE  0
#define MAX_SOCKETS 1000
#define BUFFSIZE 1024
#define POP3_PORT "5000"

static int killServer = FALSE;

static void sigterm_handler(){
    killServer = TRUE;
}

int main(int argc , char *argv[]){

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

	close(STDIN_FILENO);

    // Creamos el selector
    const char* err_msg = NULL;
    selector_status ss = SELECTOR_SUCCESS;
    fd_selector selector = NULL;
    const selector_init conf = {
        .signal = SIGALRM,
        .select_timeout = {
            .tv_sec = 10,
            .tv_nsec = 0,
        },
    };


    if (0 != selector_init(&conf)) {
        exit(1);
    }

    selector = selector_new(1024);
    if (selector == NULL) {
        selector_close();
        exit(1);
    }

	int serverSocket = setupTCPServerSocket(POP3_PORT);

    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);

    //registrar socket pasivo
    //pasarle los puntros a funcion para armar los handlers

    while(!killServer){

    }



}