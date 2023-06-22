#include "./include/client_actions.h"

void readFromSocket(int socket){
    uint8_t c;
    //IMPRIME PERO SE QUEDA TRABADO
    while (read(socket, &c, 1) > 0) {
        putchar(c);
    }
}

void help() {
    putchar('\n');
    printf("Available commands:\n");
    printDivider();
    for (int i = 0; i < COMMAND_COUNT; i++) {
        printf("%s ", commands[i].name);

        for (int j = 0; j < commands[i].argc ; j++)
            printf("<%s> ", commands[i].argNames[j]);

        putchar('\n');
    }
}

void historicConnections(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password) {
    char command[MAX_COMMAND_LENGTH];
    printf("%s\n",argv[2]);
    sprintf(command, "USER %s\r\nPASS %s\r\nHISTORIC_CONNEC\r\nQUIT\r\n", username, password);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void liveConnections(int sock,char * username,char * password) {
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nLIVE_CONNEC\r\nQUIT\r\n", username, password);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void bytesTransferred(int sock,char * username,char * password) {
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nBYTES_TRANS\r\nQUIT\r\n", username, password);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void users(int sock,char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nUSERS\r\nQUIT\r\n", username, password);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void status(int sock,char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nSTATUS\r\nQUIT\r\n", username, password);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void maxUsers(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nMAX_USERS %d\r\nQUIT\r\n", username, password, atoi(argv[0]));
    sendCommand(sock, command);
    readFromSocket(sock);
}

void maxConnections(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nMAX_CONNEC %d\r\nQUIT\r\n", username, password, atoi(argv[0]));
    sendCommand(sock, command);
    readFromSocket(sock);
}

void timeout(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nTIMEOUT %d\r\nQUIT\r\n", username, password, atoi(argv[0]));
    sendCommand(sock, command);
    readFromSocket(sock);
}

void deleteUser(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nDELETE_USER %s\r\nQUIT\r\n", username, password, argv[0]);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void addUser(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nADD_USER %s %s\r\nQUIT\r\n", username, password, argv[0], argv[1]);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void resetUserPassword(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nRESET_USER_PASSWORD %s\r\nQUIT\r\n", username, password, argv[0]);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void changeUserPassword(int sock, char argv[][MAX_COMMAND_LENGTH],char * username,char * password){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "USER %s\r\nPASS %s\r\nCHANGE_PASSWORD %s %s\r\nQUIT\r\n", username, password, argv[0], argv[1]);
    sendCommand(sock, command);
    readFromSocket(sock);
}

void viewUserLogs(){
    FILE* file;
    int c;

    file = fopen("../../logs", "r");
    if (file == NULL) {
        return;
    }

    while ((c = fgetc(file)) != EOF) {
        putchar(c);
    }

    fclose(file);
}




