#include "../include/users.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static TUser * users;
static int count;
static int INITIALIZED = 0;

// Inicializa la estructura de usuarios
void initUsers(){
    if(INITIALIZED)
        return;
    
    INITIALIZED = 1;

    users = malloc(sizeof(TUser) * MAX_USERS);
    count = 0;
}

// Agrega un usuario a la estructura
int addUser(char * username, char * password){
    if(username == NULL || password == NULL)
        return INVALID_CREDENTIALS;

    //Cantidad maxima de usuarios alcanzada
    if(count == MAX_USERS)
        return MAX_USERS_REACHED;

    if(getUserByUsername(username) == USER_FOUND)
        return USER_ALREADY_EXISTS;

    strncpy(users[count].username, username, MAX_FIELD_SIZE);
    strncpy(users[count].password, password, MAX_FIELD_SIZE);

    count++;
    return USER_ADDED;
}

// Busca un usuario en la estructura
int getUserByUsername(char * username){
    if(username == NULL)
        return USER_NOT_FOUND;

    for(int i = 0; i < count; i++)
        if(strcmp(users[i].username, username) == 0)
            return USER_FOUND;

    return USER_NOT_FOUND;
}

// Busca un usuario en la estructura
int validateUserCredentials(char * username, char * password){
    if(username == NULL || password == NULL)
        return INVALID_CREDENTIALS;

    for(int i = 0; i < count; i++)
        if(strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
            return VALID_CREDENTIALS;

    return INVALID_CREDENTIALS;
}

// Libera toda la memoria utilizada por Users
void destroyUsers(){
    free(users);
}