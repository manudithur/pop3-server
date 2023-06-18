#include "./include/monitor_utils.h"

int sendCommand(int sock, char * command) {
    if (send(sock, command, strlen(command), 0) == -1) {
        perror("send");
        return -1;
    }

    return 0;
}

int createSocket(char * ip, char * port){
    struct addrinfo hints, *servinfo, *p;
    int rv, sock;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));

        return -1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");

            continue;
        }

        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("connect");

            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");

        return -1;
    }

    freeaddrinfo(servinfo);

    return sock;
}

int closeSocket(int sock) {
    return close(sock);
}

