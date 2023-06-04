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

int main(int argc , char *argv[]){

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

	close(STDIN_FILENO);

    

	int serverSocket = setupTCPServerSocket(POP3_PORT);


}