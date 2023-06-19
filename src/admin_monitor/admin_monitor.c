// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./include/admin_monitor.h"

void commandDispatcher(int commandIndex, char argv[][MAX_COMMAND_LENGTH], int sock, char * username, char * password) {
    switch (commandIndex) {
        case 0:
            help(argv);
            break;
        //make all the cases for the commands calling a function passing argv
         case 1:
            historicConnections(sock, argv, username, password);
            break;
         case 2:
            liveConnections(sock,argv, username, password);
            break;
         case 3:
            bytesTransferred(sock,argv, username, password);
            break;
         case 4:
            users(sock,argv, username, password);
            break;
         case 5:
            status(sock,argv, username, password);
            break;
         case 6:
            maxUsers(sock,argv, username, password);
            break;
         case 7:
            maxConnections(sock,argv, username, password);
            break;
         case 8:
            timeout(sock,argv, username, password);
            break;
         case 9:
            deleteUser(sock,argv, username, password);
            break;
         case 10:
            addUser(sock,argv, username, password);
            break;
         case 11:
            resetUserPassword(sock, argv, username, password);
            break;
        case 12:
            changeUserPassword(sock, argv, username, password);
            break;
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


void invalidCommandResponse() {
    printf("INVALID COMMAND\nSend -HELP to see the available commands\n");
}

void printDivider() {
    printf("--------------------------------------------------\n");
}

// void printCommand(completeCommand * command) {
//     printf("VERSION: %s\n", command->version);
//     printf("USERNAME: %s\n", command->username);
//     printf("PASSWORD: %s\n", command->password);
//     printf("COMMAND: %s\n", commands[command->commandIndex].name);
//     printf("ARGS:");
//     for (int i = 0; i < command->argc; i++)
//         printf(" <%s>: %s", commands[command->commandIndex].argNames[i], command->commandArgs[i]);
//     putchar('\n');
// }

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

    //Crear socket
    int socket =  createSocket("127.0.0.1", "6000");

    commandDispatcher(command->commandIndex, command->commandArgs, socket,command->username,command->password);
    
    //Cerrar socket
    closeSocket(socket);

    free(command);

    return 0;
}
