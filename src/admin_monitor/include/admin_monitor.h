#ifndef _ADMIN_CONTROLLER_H_
#define _ADMIN_CONTROLLER_H_

#include "client_actions.h"
#include "monitor_utils.h"

#define COMMAND_COUNT  13
#define MAX_COMMAND_LENGTH 100
#define MAX_COMMAND_ARGS 5
#define MIN_ARGS 4

typedef struct completeCommand {
    char    version[MAX_COMMAND_LENGTH];
    char    username[MAX_COMMAND_LENGTH];
    char    password[MAX_COMMAND_LENGTH];
    int     commandIndex;
    int     argc;
    char    commandArgs[MAX_COMMAND_ARGS][MAX_COMMAND_LENGTH];
} completeCommand;

typedef struct command {
    char    name[MAX_COMMAND_LENGTH];
    int     argc;
    char    argNames[MAX_COMMAND_ARGS][MAX_COMMAND_LENGTH];
} command;

static command commands[] = {
    {"-HELP", 0, {""}},

    //Estadisticas
    {"-HISTORIC_CONNEC", 0, {""}},
    {"-LIVE_CONNEC", 0, {""}},
    {"-BYTES_TRANS", 0, {""}},
    {"-USERS", 0, {""}},
    {"-STATUS", 0, {""}},

    //Configuracion
    {"-MAX_USERS", 1, {"maxUsers"}},
    {"-MAX_CONNECTIONS", 1, {"maxConnections"}},
    {"-TIMEOUT", 1, {"seconds"}},

    //Manejo de usuarios
    {"-DELETE_USER", 1, {"username"}},
    {"-ADD_USER", 2, {"username", "password"}},
    {"-RESET_USER_PASSWORD", 1, {"username"}},

    //Manejo de administrador
    {"-CHANGE_PASSWORD", 2, {"oldPassword", "newPassword"}}
};

void commandDispatcher(int commandIndex, char argv[][MAX_COMMAND_LENGTH], int sock, char *username, char *password);

completeCommand *parse(int argc, char *argv[]);

void invalidCommandResponse();

void printDivider();

void printCommand(completeCommand *command);

int getCommandIndex(char * commandName);

void newToken(char argv[][MAX_COMMAND_LENGTH]);

#endif