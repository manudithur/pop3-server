#ifndef USERS_H_
#define USERS_H_

#define MAX_FIELD_SIZE 30
#define MAX_USERS 10

#define VALID_CREDENTIALS 1
#define INVALID_CREDENTIALS 0

#define USER_ADDED 1

#define USER_ALREADY_EXISTS -1
#define MAX_USERS_REACHED -2

#define USER_NOT_FOUND -1
#define USER_FOUND 1

typedef struct{
    char username[MAX_FIELD_SIZE];
    char password[MAX_FIELD_SIZE];
} TUser;

// Inicializa la estructura de usuarios
void initUsers();

// Agrega un usuario a la estructura
int addUser(char * username, char * password);

// Busca un usuario en la estructura
int validateUserCredentials(char * username, char * password);

// Busca un usuario en la estructura
int getUserByUsername(char * username);

// Libera toda la memoria utilizada por Users
void destroyUsers();

#endif