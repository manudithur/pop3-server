#include "tcp_server_utils.h"

#define MAXPENDING 5
#define BUFSIZE 256
#define MAX_ADDR_BUFFER 128

static char addrBuffer[MAX_ADDR_BUFFER];

static int setupSockAddr(char* addr, unsigned short port, void* res, socklen_t* socklenResult) {
  int ipv6 = strchr(addr, ':') != NULL;

  if (ipv6) {
    // Parse addr as IPv6
    struct sockaddr_in6 sock6;
    memset(&sock6, 0, sizeof(sock6));

    sock6.sin6_family = AF_INET6;
    sock6.sin6_addr = in6addr_any;
    sock6.sin6_port = htons(port);

    *((struct sockaddr_in6*)res) = sock6;
    *socklenResult = sizeof(struct sockaddr_in6);
    return 0;
  }

  // Parse addr as IPv4
  struct sockaddr_in sock4;
  memset(&sock4, 0, sizeof(sock4));
  sock4.sin_family = AF_INET;
  sock4.sin_addr.s_addr = INADDR_ANY;
  sock4.sin_port = htons(port);

  *((struct sockaddr_in*)res) = sock4;
  *socklenResult = sizeof(struct sockaddr_in);
  return 0;
}

/*
 ** Se encarga de resolver el número de puerto para service (puede ser un string con el numero o el nombre del servicio)
 ** y crear el socket pasivo, para que escuche en cualquier IP, ya sea v4 o v6
 */
int setupTCPServerSocket(const char *service) {
	
	int opt = 1;

        struct sockaddr_storage localAddr;
        memset(&localAddr, 0, sizeof(localAddr));
        socklen_t addrSize = sizeof(localAddr);
	int servSock = -1;

        //IPV6 y puerto hardcodeado
        if(setupSockAddr("::FFFF:127.0.0.1", 5000, &localAddr, &addrSize )){
          printf("problem 0\n");
          return -1;
        }

        // Create a TCP socket
        servSock = socket(localAddr.ss_family, SOCK_STREAM, IPPROTO_TCP);
        if (servSock < 0) {
          printf("problem 1\n");
          return -1;
        }


        if( setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 );

        // Bind to ALL the address and set socket to listen
        if (bind(servSock, (struct sockaddr *)&localAddr, addrSize) == 0) {

          if(listen(servSock, MAXPENDING) <0){
            printf("problem 3\n");
          }

        } else {
          printf("problem 2\n");
          close(servSock);
          servSock = -1;
        }

	//freeaddrinfo(servAddr);

	return servSock;
}

static void pop3_read(struct selector_key *key) {
	struct state_machine* stm = &ATTACHMENT(key)->stm;
    const enum pop3_states st = stm_handler_read(stm, key);
    //if (st == ERROR || st == DONE) {
        //TODO:cerrar conexion
    //}
}

static void pop3_write(struct selector_key *key) {
	struct state_machine* stm = &ATTACHMENT(key)->stm;
    const enum pop3_states st = stm_handler_write(stm, key);
    //if (st == ERROR || st == DONE) {
       //TODO:cerrar conexion
   // }
}

static void pop3_close(struct selector_key *key) {
struct state_machine* stm = &ATTACHMENT(key)->stm;
    stm_handler_close(stm, key);
    //TODO : cerrar la conexion al cliente
}

static void pop3_block(struct selector_key *key) {
	struct state_machine* stm = &ATTACHMENT(key)->stm;
    const enum pop3_states st = stm_handler_block(stm, key);
    //if (st == ERROR || st == DONE) {
        
    //}
}

static fd_handler pop3_handler = {
	.handle_read = pop3_read,
	.handle_write = pop3_write,
	.handle_close = pop3_close,
	.handle_block = pop3_block
};

static struct state_definition states[] = {
	{
		.state = AUTH_STATE,
		//TODO
		.on_arrival = NULL,
		.on_read_ready = readHandler,
        .on_write_ready = writeHandler
	},
	{
		.state = TRANSACTION_STATE,
		//TODO
		.on_arrival = NULL,
		.on_read_ready = readHandler,
        .on_write_ready = writeHandler
	},
	{
		.state = UPDATE_STATE,
		//TODO
		.on_arrival = NULL,
		.on_write_ready = writeHandler,
        .on_read_ready = readHandler
	},
    {
        .state = ERROR_STATE,
        .on_arrival = NULL,  //TODO: manejar error
        .on_read_ready = readHandler,
        .on_write_ready = errorHandler
    }
};

void handleNewConnection(struct selector_key * key){

	struct sockaddr_storage clntAddr; // Client address
	// Set length of client address structure (in-out parameter)
	socklen_t clntAddrLen = sizeof(clntAddr);

	int clntSock = accept(key->fd, (struct sockaddr *) &clntAddr, &clntAddrLen);
	if (clntSock < 0) {
	//log(ERROR, "accept() failed");
		return;
	}

	if (clntSock > 1023) {
        close(clntSock);
        return;
    }


	// clntSock is connected to a client!
	stats_add_connection();
	printSocketAddress((struct sockaddr *) &clntAddr, addrBuffer);
	log(INFO, "Handling client %s", addrBuffer);

	struct client_data * client = calloc(1, sizeof(struct client_data));
	if(client == NULL){
		close(clntSock);
		return;
	}

	buffer_init(&client->rbStruct, BUFFER_LEN, client->rb);
	buffer_init(&client->wbStruct, BUFFER_LEN, client->wb);
	client->fd = clntSock;
	client->stm.initial = AUTH_STATE;
	client->stm.max_state = ERROR_STATE;
    client->parser = parser_init(parser_no_classes(),&definition);
	
	client->stm.states = states;
	stm_init(&client->stm);

	int register_status = selector_register(key->s, clntSock, &pop3_handler, OP_READ, client);

	if(register_status != SELECTOR_SUCCESS){
		close(clntSock);
		free(client);
		return;
	}


}
