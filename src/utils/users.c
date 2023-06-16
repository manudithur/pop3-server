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

    addUser("admin", "admin");
    users[0].isAdmin = ADMIN_USER;
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

    strncpy(users[count].username, username, strlen(username)+1);
    strncpy(users[count].password, password, strlen(password)+1);
    users[count].isAdmin = STANDARD_USER;

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

int validateAdminUser(char * username){
    if(username[0] == '\0'){
        return INVALID_CREDENTIALS;
    }

    for(int i = 0 ; i < count; i ++){
        if(strcmp(users[i].username, username) == 0 && users[i].isAdmin == ADMIN_USER)
            return VALID_CREDENTIALS;
    }

    return INVALID_CREDENTIALS;
}

int validateAdminCredentials(char * username, char * password){
    if(username[0] == '\0'|| password[0] == '\0')
        return INVALID_CREDENTIALS;

    printf("VALIDATE ADMIN CREDENTIALS\n");
    printf("username: %s\n", username);
    printf("password: %s\n", password);

    for(int i = 0; i < count; i++){
        printf("Is admin: %s\n", users[i].isAdmin == ADMIN_USER ? "TRUE" : "FALSE");
        if(strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0 && users[i].isAdmin == ADMIN_USER)
            return VALID_CREDENTIALS;
    }

    return INVALID_CREDENTIALS;
}

int validateUser(char * username){
    if(username[0] == '\0'){
        return INVALID_CREDENTIALS;
    }

    for(int i = 0 ; i < count; i ++){
        if(strcmp(users[i].username, username) == 0 )
            return VALID_CREDENTIALS;
    }

    return INVALID_CREDENTIALS;
}

// Busca un usuario en la estructura
int validateUserCredentials(char * username, char * password){
    if(username[0] == '\0'|| password[0] == '\0')
        return INVALID_CREDENTIALS;

    for(int i = 0; i < count; i++)
        if(strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
            return VALID_CREDENTIALS;

    return INVALID_CREDENTIALS;
}

// Cambia la contrasena de un usuario
int changePassword(char * username, char * oldPassword, char * newPassword){
    if(username == NULL || oldPassword == NULL || newPassword == NULL)
        return PASSWORD_CHANGE_FAILED;

    for(int i = 0; i < count; i++)
        if(strcmp(users[i].username, username) == 0 && strcmp(users[i].password, oldPassword) == 0){
            strncpy(users[i].password, newPassword, strlen(newPassword)+1);
            return PASSWORD_CHANGED_SUCCESSFULLY;
        }

    return PASSWORD_CHANGE_FAILED;
}

// Libera toda la memoria utilizada por Users
void destroyUsers(){
    free(users);
}