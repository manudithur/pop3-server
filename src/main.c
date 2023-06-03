// #include <stdlib.h>
// #include "parser.h"
// #include "parser_utils.h"

// int main(int argc, char * argv[]){
//     //Defino un parser que valida "command1"
//     struct parser_definition command1_parser = parser_utils_strcmpi("command1");

//     //Inicializo el parser
//     struct parser * command1_parser_instance = parser_init(parser_no_classes(), &command1_parser);


//     char * command1 = "command1";

//     //Le feedeo la palabla "command1" al parser
//     const struct parser_event * event = parser_feed(command1_parser_instance, command1);

//     //Imprimo recursivamente los eventos que nos devuelve el parser
//     event_print(&event[0]);

//     parser_utils_strcmpi_destroy(&command1_parser);    
// }

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

#define max(n1,n2)     ((n1)>(n2) ? (n1) : (n2))

#define TRUE   1
#define FALSE  0
#define PORT 8888
#define MAX_SOCKETS 1000
#define BUFFSIZE 1024
#define PORT_UDP 8888
#define MAX_PENDING_CONNECTIONS   3    // un valor bajo, para realizar pruebas
#define POP3_PORT "5000"

struct User{
	char * wb;
	char * rb;
	int socket;
};


struct buffer {
	char * buffer;
	size_t len;     // longitud del buffer
	size_t from;    // desde donde falta escribir
};

/**
  Se encarga de escribir la respuesta faltante en forma no bloqueante
  */
void handleWrite(int socket, struct buffer * buffer, fd_set * writefds);

/**
  Limpia el buffer de escritura asociado a un socket
  */
void clear( struct buffer * buffer);

/**
  Lee el datagrama del socket, obtiene info asociado con getaddrInfo y envia la respuesta
  */
void handleAddrInfo(int socket);


int main(int argc , char *argv[]){

	close(STDIN_FILENO);

	// TODO adaptar setupTCPServerSocket para que cree socket para IPv4 e IPv6 y ademas soporte opciones (y asi no repetir codigo)
	int serverSocket = setupTCPServerSocket(POP3_PORT);
	
	int addrlen;
	int new_socket;
	int client_socket[MAX_SOCKETS];
	int max_clients = MAX_SOCKETS;
	int activity, i , sd;
	long valread;
	int max_sd;
	struct sockaddr_in address;

	struct sockaddr_storage clntAddr; // Client address
	socklen_t clntAddrLen = sizeof(clntAddr);

	char buffer[BUFFSIZE + 1];


	fd_set readfds;
	fd_set writefds;

	// Agregamos un buffer de escritura asociado a cada socket, para no bloquear por escritura
	struct buffer bufferWrite[MAX_SOCKETS];
	memset(bufferWrite, 0, sizeof bufferWrite);

	
	//initialise all client_socket[] to 0 so not checked
	memset(client_socket, 0, sizeof(client_socket));

	int maxSocket = serverSocket;

	while(TRUE) {

		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(serverSocket, &readfds);

		maxSocket = serverSocket;
		max_sd = serverSocket;

		// add child sockets to set
		for ( i = 0 ; i < max_clients ; i++) {
			// socket descriptor
			sd = client_socket[i];

			// if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);

			// highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}

		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		activity = select( max_sd+1 , &readfds , &writefds , NULL , NULL);
//		log(DEBUG, "select has something...");

		if ((activity < 0) && (errno!=EINTR)) {
			log(ERROR, "select error, errno=%d",errno);
			continue;
		}
        log(INFO, "PASE POR ACA X",errno);


        //si el server socket tiene algo es porque alguien quiere conectarse
		if(FD_ISSET(serverSocket, &readfds)){

//			log(DEBUG, "select has something on serverSocket...");

			if ((new_socket = acceptTCPConnection(serverSocket)) < 0){
				log(ERROR, "Accept error on master socket %d", serverSocket);
			}else{
				for (i = 0; i < max_clients; i++){
					if( client_socket[i] == 0 ){
						client_socket[i] = new_socket;
						log(DEBUG, "Adding to list of sockets as %d\n" , i);
						break;
					}
				}
			}

		}else{ //else its some IO operation on some other socket :)
            for(i =0; i < max_clients; i++) {
                sd = client_socket[i];

                if (FD_ISSET(sd, &writefds)) {
                    handleWrite(sd, bufferWrite + i, &writefds);
                }
            }


            for (i = 0; i < max_clients; i++) {
                sd = client_socket[i];

                if (FD_ISSET( sd , &readfds)) {
                    //Check if it was for closing , and also read the incoming message
                    if ((valread = read( sd , buffer, BUFFSIZE)) <= 0){
                        //Somebody disconnected , get his details and print
                        getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                        log(INFO, "Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                        //Close the socket and mark as 0 in list for reuse
                        close( sd );
                        client_socket[i] = 0;

                        FD_CLR(sd, &writefds);
                        // Limpiamos el buffer asociado, para que no lo "herede" otra sesión
                        clear(bufferWrite + i);
                    } else {
                        log(DEBUG, "Received %zu bytes from socket %d\n", valread, sd);
                        // activamos el socket para escritura y almacenamos en el buffer de salida
                        FD_SET(sd, &writefds);

                        // Tal vez ya habia datos en el buffer
                        // TODO: validar realloc != NULL
                        bufferWrite[i].buffer = realloc(bufferWrite[i].buffer, bufferWrite[i].len + valread);
                        memcpy(bufferWrite[i].buffer + bufferWrite[i].len, buffer, valread);
                        bufferWrite[i].len += valread;
                    }
                }
            }
        }


	}

	return 0;
}




void clear( struct buffer * buffer) {
	free(buffer->buffer);
	buffer->buffer = NULL;
	buffer->from = buffer->len = 0;
}

// Hay algo para escribir?
// Si está listo para escribir, escribimos. El problema es que a pesar de tener buffer para poder
// escribir, tal vez no sea suficiente. Por ejemplo podría tener 100 bytes libres en el buffer de
// salida, pero le pido que mande 1000 bytes.Por lo que tenemos que hacer un send no bloqueante,
// verificando la cantidad de bytes que pudo consumir TCP.
void handleWrite(int socket, struct buffer * buffer, fd_set * writefds) {
	size_t bytesToSend = buffer->len - buffer->from;
	if (bytesToSend > 0) {  // Puede estar listo para enviar, pero no tenemos nada para enviar
		log(INFO, "Trying to send %zu bytes to socket %d\n", bytesToSend, socket);
		size_t bytesSent = send(socket, buffer->buffer + buffer->from,bytesToSend,  MSG_DONTWAIT); 
		log(INFO, "Sent %zu bytes\n", bytesSent);

		if ( bytesSent < 0) {
			// Esto no deberia pasar ya que el socket estaba listo para escritura
			// TODO: manejar el error
			log(FATAL, "Error sending to socket %d", socket);
		} else {
			size_t bytesLeft = bytesSent - bytesToSend;

			// Si se pudieron mandar todos los bytes limpiamos el buffer y sacamos el fd para el select
			if ( bytesLeft == 0) {
				clear(buffer);
				FD_CLR(socket, writefds);
			} else {
				buffer->from += bytesSent;
			}
		}
	}
}

void handleAddrInfo(int socket) {
	// En el datagrama viene el nombre a resolver
	// Se le devuelve la informacion asociada

	char buffer[BUFFSIZE];
	unsigned int len, n;

	struct sockaddr_in clntAddr;

	// Es bloqueante, deberian invocar a esta funcion solo si hay algo disponible en el socket    
	n = recvfrom(socket, buffer, BUFFSIZE, 0, ( struct sockaddr *) &clntAddr, &len);
	if ( buffer[n-1] == '\n') // Por si lo estan probando con netcat, en modo interactivo
		n--;
	buffer[n] = '\0';
	log(DEBUG, "UDP received:%s", buffer );
	// TODO: parsear lo recibido para obtener nombre, puerto, etc. Asumimos viene solo el nombre

	// Especificamos solo SOCK_STREAM para que no duplique las respuestas
	struct addrinfo addrCriteria;                   // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC;             // Any address family
	addrCriteria.ai_socktype = SOCK_STREAM;         // Only stream sockets
	addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol


	// Armamos el datagrama con las direcciones de respuesta, separadas por \r\n
	// TODO: hacer una concatenacion segura
	// TODO: modificar la funcion printAddressInfo usada en sockets bloqueantes para que sirva
	//       tanto si se quiere obtener solo la direccion o la direccion mas el puerto
	char bufferOut[BUFFSIZE];
	bufferOut[0] = '\0';

	struct addrinfo *addrList;
	int rtnVal = getaddrinfo(buffer, NULL, &addrCriteria, &addrList);
	if (rtnVal != 0) {
		log(ERROR, "getaddrinfo() failed: %d: %s", rtnVal, gai_strerror(rtnVal));
		strcat(strcpy(bufferOut,"Can't resolve "), buffer);

	} else {
		for (struct addrinfo *addr = addrList; addr != NULL; addr = addr->ai_next) {
			struct sockaddr *address = addr->ai_addr;
			char addrBuffer[INET6_ADDRSTRLEN];

			void *numericAddress = NULL;
			switch (address->sa_family) {
				case AF_INET:
					numericAddress = &((struct sockaddr_in *) address)->sin_addr;
					break;
				case AF_INET6:
					numericAddress = &((struct sockaddr_in6 *) address)->sin6_addr;
					break;
			}
			if ( numericAddress == NULL) {
				strcat(bufferOut, "[Unknown Type]");
			} else {
				// Convert binary to printable address
				if (inet_ntop(address->sa_family, numericAddress, addrBuffer, sizeof(addrBuffer)) == NULL)
					strcat(bufferOut, "[invalid address]");
				else {
					strcat(bufferOut, addrBuffer);
				}
			}
			strcat(bufferOut, "\r\n");
		}
		freeaddrinfo(addrList);
	}

	// Enviamos respuesta (el sendto no bloquea)
	sendto(socket, bufferOut, strlen(bufferOut), 0, (const struct sockaddr *) &clntAddr, len);

	log(DEBUG, "UDP sent:%s", bufferOut );

}
