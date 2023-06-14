#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_


#define COMMAND_COUNT  13
#define MAX_COMMAND_LENGTH 20
#define MAX_COMMAND_ARGS 5
#define MIN_ARGS 3

typedef struct completeCommand {
    char    version[MAX_COMMAND_LENGTH];
    char    token[MAX_COMMAND_LENGTH];
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
    {"-HISTORIC_CONEC", 0, {""}},
    {"-LIVE_CONEC", 0, {""}},
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
    {"-NEW_TOKEN", 2, {"oldToken", "newToken"}}
};

void commandDispatcher(int commandIndex, char argv[][MAX_COMMAND_LENGTH]);

completeCommand *parse(int argc, char *argv[]);

void help(char argv[][MAX_COMMAND_LENGTH]);

void invalidCommandResponse();

void printDivider();

void printCommand(completeCommand *command);

int getCommandIndex(char * commandName);

void newToken(char argv[][MAX_COMMAND_LENGTH]);

#endif