#ifndef USERS_H_
#define USERS_H_

#define IS_CONNECTED 1
#define IS_DISCONNECTED 0

#define MAX_FIELD_SIZE 30

#define VALID_CREDENTIALS 1
#define INVALID_CREDENTIALS 0

#define USER_ADDED 1

#define USER_ALREADY_EXISTS -1
#define MAX_USERS_REACHED -2

#define USER_NOT_FOUND -1
#define USER_FOUND 1

#define ADMIN_DELETE_ATTEMPT -2
#define USER_DELETED 1

#define TOKEN_UPDATED_SUCCESSFULLY 1
#define TOKEN_UPDATE_FAILED -1

#define STANDARD_USER 0
#define ADMIN_USER 1

#define PASSWORD_CHANGED_SUCCESSFULLY 1
#define PASSWORD_CHANGE_FAILED -1

#define INVALID_MAX_USERS -1
#define MAX_USERS_CHANGED_SUCCESSFULLY 1

typedef struct{
    char username[MAX_FIELD_SIZE];
    char password[MAX_FIELD_SIZE];
    int  isAdmin;
    int isConnected;
} TUser;

typedef struct{
    TUser * users;
    int count;
}TUsers;

// Inicializa la estructura de usuarios
void initUsers();

// Inserta usuarios default
void defaultUsers();

// Agrega un usuario a la estructura
int addUser(char * username, char * password);

// Elimina a un usuario
int deleteUser(char * username);

// Valida que el nombre de usuario exista y que sea administrador
int validateAdminUser(char * username);

// Valida que el nombre de usuario exista
int validateUser(char * username);

// Valida que el nombre de usuario y la contrasena sean correctos para un administrador
int validateAdminCredentials(char * username, char * password);

// Valida que el nombre de usuario y la contrasena sean correctos
int validateUserCredentials(char * username, char * password);

// Busca un usuario en la estructura
int getUserByUsername(char * username);

// Cambia la contrasena de un usuario
int changePassword(char * username, char * oldPassword, char * newPassword);

// Actualiza la cantidad maxima de usuarios
int setMaxUsers(int maxUsers);

// Retorna la cantidad maxima de usuarios
int getMaxUsers();

// Resetea la contrasena de un usuario (password = username)
int resetUserPassword(char * user);

// Retorna la estructura de usuarios
TUsers * getUsers();

// Libera toda la memoria utilizada por Users
void destroyUsers();

//Actualiza el isConnected del usuario
void disconnectUser(char * username);

#endif
