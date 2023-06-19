// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/users.h"
#include "./include/admin_monitor.h"
#include "./include/monitor_utils.h"

void commandDispatcher(int commandIndex, char argv[][MAX_COMMAND_LENGTH]) {
    printf("Command index: %d\n", commandIndex);
    switch (commandIndex) {
        case 0:
            help(argv);
            break;
        //make all the cases for the commands calling a function passing argv
        // case 1:
        //     historicConnections(argv);
        //     break;
        // case 2:
        //     liveConnections(argv);
        //     break;
        // case 3:
        //     bytesTransferred(argv);
        //     break;
        // case 4:
        //     users(argv);
        //     break;
        // case 5:
        //     status(argv);
        //     break;
        // case 6:
        //     maxUsers(argv);
        //     break;
        // case 7:
        //     maxConnections(argv);
        //     break;
        // case 8:
        //     timeout(argv);
        //     break;
        // case 9:
        //     deleteUser(argv);
        //     break;
        // case 10:
        //     addUser(argv);
        //     break;
        // case 11:
        //     resetUserPassword(argv);
        //     break;
        // case 12:
        //     newToken(argv);
        //     break;
        default:
            printf("NOT IMPLEMENTED YET\n");
            break;
    }
}

int getCommandIndex(char *commandName) {
    for (int i = 0; i < COMMAND_COUNT; i++)
        if (strcmp(commandName, commands[i].name) == 0)
            return i;
    return -1;
}

completeCommand *parse(int argc, char *argv[]) {

    completeCommand *command = malloc(sizeof(completeCommand));

    strncpy(command->version, argv[1], strlen(argv[1]));
    strncpy(command->username, argv[2], strlen(argv[2]));
    strncpy(command->password, argv[3], strlen(argv[3]));
    command->commandIndex = getCommandIndex(argv[4]);

    command->argc = argc - 1 - MIN_ARGS;

    if (command->argc != commands[command->commandIndex].argc)
        return NULL;

    for (int i = 0; i < command->argc; i++){   
        strncpy(command->commandArgs[i], argv[i + 5], strlen(argv[i + 5]));
    }

    return command;
}

void help(char argv[][MAX_COMMAND_LENGTH]) {
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

void invalidCommandResponse() {
    printf("INVALID COMMAND\nSend -HELP to see the available commands\n");
}

void printDivider() {
    printf("--------------------------------------------------\n");
}

void printCommand(completeCommand * command) {
    printf("VERSION: %s\n", command->version);
    printf("USERNAME: %s\n", command->username);
    printf("PASSWORD: %s\n", command->password);
    printf("COMMAND: %s\n", commands[command->commandIndex].name);
    printf("ARGS:");
    for (int i = 0; i < command->argc; i++)
        printf(" <%s>: %s", commands[command->commandIndex].argNames[i], command->commandArgs[i]);
    putchar('\n');
}

int main(int argc, char *argv[]) {
    int maxCommandArgs = 0;
    
    for (int i = 0; i < COMMAND_COUNT; i++)
        maxCommandArgs < commands[i].argc ? maxCommandArgs = commands[i].argc : 0;

    if (argc - 1 > MIN_ARGS + maxCommandArgs || argc - 1 < MIN_ARGS) {
        printf("Invalid number of arguments 1\n");
        invalidCommandResponse();
        return 0;
    }

    completeCommand *command = parse(argc, argv);

    if (command == NULL) {
        printf("Invalid number of arguments 2\n");
        invalidCommandResponse();
        return 0;
    }

    printCommand(command);

    putchar('\n');

    //Crear socket
    int socket =  createSocket("127.0.0.1", "6000");

    //Mandarle al socket USER
    char buffer[1024];
    sprintf(buffer, "USER %s\r\nPASS %s\r\n", command->username, command->password);

    if(sendCommand(socket, buffer) == -1){
        printf("Error sending command\n");
        return 0;
    }

    commandDispatcher(command->commandIndex, command->commandArgs);

    int qty;
    int readCarriageReturn = 0;
    uint8_t c;
    //IMPRIME PERO SE QUEDA TRABADO
    while ((qty = read(socket, &c, 1)) > 0) {
        putchar(c);
    }

    

    //Cerrar socket
    closeSocket(socket);

    free(command);

    return 0;
}
