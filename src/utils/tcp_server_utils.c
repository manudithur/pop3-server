#include "tcp_server_utils.h"


#define MAXPENDING 5
#define BUFSIZE 256
#define MAX_ADDR_BUFFER 128

static char addrBuffer[MAX_ADDR_BUFFER];
/*
 ** Se encarga de resolver el número de puerto para service (puede ser un string con el numero o el nombre del servicio)
 ** y crear el socket pasivo, para que escuche en cualquier IP, ya sea v4 o v6
 */
int setupTCPServerSocket(const char *service) {
	
	int opt = 1;
	struct addrinfo addrCriteria;                   
	memset(&addrCriteria, 0, sizeof(addrCriteria)); 
	addrCriteria.ai_family = AF_UNSPEC;             
	addrCriteria.ai_flags = AI_PASSIVE;             
	addrCriteria.ai_socktype = SOCK_STREAM;         
	addrCriteria.ai_protocol = IPPROTO_TCP;         

	struct addrinfo *servAddr; 			// List of server addresses
	int rtnVal = getaddrinfo("127.0.0.1", service, &addrCriteria, &servAddr);
	if (rtnVal != 0) {
		log(FATAL, "getaddrinfo() failed %s", gai_strerror(rtnVal));
		return -1;
	}

	int servSock = -1;

	// Intentamos ponernos a escuchar en alguno de los puertos asociados al servicio, sin especificar una IP en particular
	// Iteramos y hacemos el bind por alguna de ellas, la primera que funcione, ya sea la general para IPv4 (0.0.0.0) o IPv6 (::/0) .
	// Con esta implementación estaremos escuchando o bien en IPv4 o en IPv6, pero no en ambas
	for (struct addrinfo *addr = servAddr; addr != NULL && servSock == -1; addr = addr->ai_next) {

		errno = 0;

		// Create a TCP socket
		servSock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (servSock < 0) {
			log(DEBUG, "Cant't create socket on %s : %s ", printAddressPort(addr, addrBuffer), strerror(errno));  
			continue;
		}

		//TODO: IPv4 e IPv6 setteando algunas cosas en esta line
		// para que IPv6 acepte ambos (dual stack socket)
		// int on = 0;
		// setsockopt(socketIpV6, IPPROTO_IPV6, IPV6_V6ONLY, (const void *)&on, sizeof(on));
		if( setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){
			//log(ERROR, "set server socket options failed");
		}

		// Bind to ALL the address and set socket to listen
		if ((bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0) && (listen(servSock, MAXPENDING) == 0)) {
			// Print local address of socket
			struct sockaddr_storage localAddr;
			socklen_t addrSize = sizeof(localAddr);
			if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) >= 0) {
				printSocketAddress((struct sockaddr *) &localAddr, addrBuffer);
				log(INFO, "Binding to %s", addrBuffer);
			}
		} else {
			log(DEBUG, "Cant't bind %s", strerror(errno));  
			close(servSock);
			servSock = -1;
		}
	}

	freeaddrinfo(servAddr);

	return servSock;
}

static void pop3_read(struct selector_key *key) {
	struct state_machine* stm = &(((client_data*)(key)->data)->stm);
    const enum pop3_states st = stm_handler_read(stm, key);
    //if (st == ERROR || st == DONE) {
        //TODO:cerrar conexion
    //}
}

static void pop3_write(struct selector_key *key) {
	struct state_machine* stm = &(((client_data*)(key)->data)->stm);
    const enum pop3_states st = stm_handler_write(stm, key);
    //if (st == ERROR || st == DONE) {
       //TODO:cerrar conexion
   // }
}

static void pop3_close(struct selector_key *key) {
struct state_machine* stm = &(((client_data*)(key)->data)->stm);
    stm_handler_close(stm, key);
    //TODO : cerrar la conexion al cliente
}

static void pop3_block(struct selector_key *key) {
	struct state_machine* stm = &(((client_data*)(key)->data)->stm);
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
		.on_read_ready = handleTCPEchoClient
	},
	{
		.state = TRANSACTION_STATE,
		//TODO
		.on_arrival = NULL,
		.on_read_ready = NULL
	},
	{
		.state = UPDATE_STATE,
		//TODO
		.on_arrival = NULL,
		.on_read_ready = NULL
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

	// clntSock is connected to a client!
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
	client->stm.initial = AUTH_STATE; //TODO
	client->stm.max_state = UPDATE_STATE; //TODO
	//client->stm.states = states; //TODO
	stm_init(&client->stm);

	int register_status = selector_register(key->s, clntSock, &pop3_handler, OP_READ, client);

	if(register_status != SELECTOR_SUCCESS){
		close(clntSock);
		free(client);
		return;
	}


}

int acceptTCPConnection(int servSock) {
	struct sockaddr_storage clntAddr; // Client address
	// Set length of client address structure (in-out parameter)
	socklen_t clntAddrLen = sizeof(clntAddr);

	// Wait for a client to connect
	int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
	if (clntSock < 0) {
//		log(ERROR, "accept() failed");
		return -1;
	}

	// clntSock is connected to a client!
	printSocketAddress((struct sockaddr *) &clntAddr, addrBuffer);
	log(INFO, "Handling client %s", addrBuffer);

	return clntSock;
}

unsigned handleTCPEchoClient(struct selector_key * key) {
    client_data* data = (client_data*)(key)->data;
	//char buffer[BUFSIZE]; // Buffer for echo string
	// Receive message from client
	size_t readLimit;
	ssize_t amount_read;
	size_t writeLimit;
	uint8_t * readBuffer = buffer_write_ptr(&data->rbStruct, &readLimit);
	uint8_t * writeBuffer = buffer_read_ptr(&data->wbStruct, &writeLimit);


	amount_read = recv(key->fd, readBuffer, readLimit, 0);


	if (amount_read < 0) {
//		log(ERROR, "recv() failed");
		return -1;   // TODO definir codigos de error
	}

	// Send received string and receive again until end of stream
	while (amount_read > 0) { // 0 indicates end of stream
		// Echo message back to client
		ssize_t numBytesSent = send(data->fd, writeBuffer, writeLimit, 0);
		if (numBytesSent < 0) {
//			log(ERROR, "send() failed");
			return -1;   // TODO definir codigos de error
		}
		else if (numBytesSent != amount_read) {
//			log(ERROR, "send() sent unexpected number of bytes ");
			return -1;   // TODO definir codigos de error
		}

		// See if there is more data to receive
		amount_read = recv(key->fd, readBuffer, readLimit, 0);
		if (amount_read < 0) {
//			log(ERROR, "recv() failed");
			return -1;   // TODO definir codigos de error
		}
	}

	//close(clntSocket);
	return 0;
}


