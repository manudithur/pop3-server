// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/users.h"
#include "../include/tcp_server_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static TUsers * usersStruct;
static int INITIALIZED = 0;

static int MAX_USERS = 500;

// Inicializa la estructura de usuarios
void initUsers(){
    if(INITIALIZED)
        return;
    
    INITIALIZED = 1;
    
    usersStruct = malloc(sizeof(TUsers));

    usersStruct->users = malloc(sizeof(TUser) * MAX_USERS);
    usersStruct->count = 0;

    addUser("admin", "admin");
    usersStruct->users[0].isAdmin = ADMIN_USER;
}

void defaultUsers(){
    addUser("user1", "user1");
    addUser("user2", "user2");
    addUser("user3", "user3");
    addUser("user4", "user4");
}

// Elimina a un usuario
int deleteUser(char * username){
    for(int i = 0; i < usersStruct->count; i++){
        if(strcmp(usersStruct->users[i].username, username) == 0){
            if(usersStruct->users[i].isAdmin == ADMIN_USER)
                return ADMIN_DELETE_ATTEMPT;

            for(int j = i; j < usersStruct->count - 1; j++){
                usersStruct->users[j] = usersStruct->users[j+1];
            }
            usersStruct->count--;
            return USER_DELETED;
        }
    }

    return USER_NOT_FOUND;
}

// Agrega un usuario a la estructura
int addUser(char * username, char * password){
    if(username == NULL || password == NULL)
        return INVALID_CREDENTIALS;

    //Cantidad maxima de usuarios alcanzada
    if(usersStruct->count == MAX_USERS)
        return MAX_USERS_REACHED;

    if(getUserByUsername(username) == USER_FOUND)
        return USER_ALREADY_EXISTS;

    strncpy(usersStruct->users[usersStruct->count].username, username, strlen(username)+1);
    strncpy(usersStruct->users[usersStruct->count].password, password, strlen(password)+1);
    usersStruct->users[usersStruct->count].isAdmin = STANDARD_USER;
    usersStruct->users[usersStruct->count].isConnected = IS_DISCONNECTED;

    usersStruct->count++;
    return USER_ADDED;
}

// Busca un usuario en la estructura
int getUserByUsername(char * username){
    if(username == NULL)
        return USER_NOT_FOUND;

    for(int i = 0; i < usersStruct->count; i++)
        if(strcmp(usersStruct->users[i].username, username) == 0)
            return USER_FOUND;

    return USER_NOT_FOUND;
}

int validateAdminUser(char * username){
    if(username[0] == '\0'){
        return INVALID_CREDENTIALS;
    }

    for(int i = 0 ; i < usersStruct->count; i ++){
        if(strcmp(usersStruct->users[i].username, username) == 0 && usersStruct->users[i].isAdmin == ADMIN_USER && usersStruct->users[i].isConnected == IS_DISCONNECTED)
            return VALID_CREDENTIALS;
    }

    return INVALID_CREDENTIALS;
}

int validateAdminCredentials(char * username, char * password){
    if(username[0] == '\0'|| password[0] == '\0')
        return INVALID_CREDENTIALS;


    for(int i = 0; i < usersStruct->count; i++){
        if(strcmp(usersStruct->users[i].username, username) == 0 && strcmp(usersStruct->users[i].password, password) == 0 && usersStruct->users[i].isAdmin == ADMIN_USER && usersStruct->users[i].isConnected == IS_DISCONNECTED){
            usersStruct->users[i].isConnected = IS_CONNECTED;
            return VALID_CREDENTIALS;
        }
    }

    return INVALID_CREDENTIALS;
}

int validateUser(char * username){
    if(username[0] == '\0'){
        return INVALID_CREDENTIALS;
    }

    for(int i = 0 ; i < usersStruct->count; i ++){
        if(strcmp(usersStruct->users[i].username, username) == 0 && usersStruct->users[i].isConnected == IS_DISCONNECTED)
            return VALID_CREDENTIALS;
    }

    return INVALID_CREDENTIALS;
}

void disconnectUser(char * username){
    for(int i = 0; i < usersStruct->count; i++){
        if(strcmp(usersStruct->users[i].username, username) == 0){
            usersStruct->users[i].isConnected = false;
            return;
        }
    }
}

// Busca un usuario en la estructura
int validateUserCredentials(char * username, char * password){
    if(username[0] == '\0'|| password[0] == '\0')
        return INVALID_CREDENTIALS;

    for(int i = 0; i < usersStruct->count; i++)
        if(strcmp(usersStruct->users[i].username, username) == 0 && strcmp(usersStruct->users[i].password, password) == 0 && usersStruct->users[i].isConnected == IS_DISCONNECTED){
            usersStruct->users[i].isConnected = IS_CONNECTED;
            return VALID_CREDENTIALS;
        }

    return INVALID_CREDENTIALS;
}

// Cambia la contrasena de un usuario
int changePassword(char * username, char * oldPassword, char * newPassword){
    if(username == NULL || oldPassword == NULL || newPassword == NULL)
        return PASSWORD_CHANGE_FAILED;

    for(int i = 0; i < usersStruct->count; i++)
        if(strcmp(usersStruct->users[i].username, username) == 0 && strcmp(usersStruct->users[i].password, oldPassword) == 0){
            strncpy(usersStruct->users[i].password, newPassword, strlen(newPassword)+1);
            return PASSWORD_CHANGED_SUCCESSFULLY;
        }

    return PASSWORD_CHANGE_FAILED;
}

// Cambia la cantidad maxima de usuarios
int setMaxUsers(int maxUsers){
    if(maxUsers < 0 || maxUsers < usersStruct->count || maxUsers > MAX_CONNECTIONS)
        return INVALID_MAX_USERS;

    MAX_USERS = maxUsers;
    return MAX_USERS_CHANGED_SUCCESSFULLY;
}

int getMaxUsers(){
    return MAX_USERS;
}

// Resetea la contrasena de un usuario (password = username)
int resetUserPassword(char * user){
    if(user == NULL)
        return PASSWORD_CHANGE_FAILED;

    for(int i = 0; i < usersStruct->count; i++)
        if(strcmp(usersStruct->users[i].username, user) == 0){
            strncpy(usersStruct->users[i].password, user, strlen(user)+1);
            return PASSWORD_CHANGED_SUCCESSFULLY;
        }

    return PASSWORD_CHANGE_FAILED;
}

// Retorna la estructura de usuarios
TUsers * getUsers(){
    return usersStruct;
}

// Libera toda la memoria utilizada por Users
void destroyUsers(){
    free(usersStruct->users);
    free(usersStruct);
}
