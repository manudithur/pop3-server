//
// Created by Gayba on 6/18/2023.
//

#ifndef POP3_SERVER_CLIENT_ACTIONS_H
#define POP3_SERVER_CLIENT_ACTIONS_H

#include "monitor_utils.h"
#include "admin_monitor.h"
#define MAX_RESPONSE_LENGTH 1000
#define MAX_PARAM_LENGTH 100

void help(char argv[][MAX_PARAM_LENGTH]);
void historicConnections(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char * password);
void liveConnections(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void users(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char * password);
void status(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void maxUsers(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void maxConnections(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void timeout(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void deleteUser(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void addUser(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void resetUserPassword(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);
void bytesTransferred(int sock, char argv[][MAX_PARAM_LENGTH],char * username,char *password);

#endif //POP3_SERVER_CLIENT_ACTIONS_H
