#include "pop3_actions.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PATH_MAX 300

unsigned user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK USER\r\n"};
    data->username=malloc(strlen((data->command.arg1))+1);
    strcpy(data->username,data->command.arg1);
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return AUTH_STATE;

}

unsigned pass_handler(selector_key *key){
      client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK PASS\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return TRANSACTION_STATE;


}

unsigned stat_handler(selector_key *key){
    DIR* directory;
    client_data * data = ATTACHMENT(key);
    struct dirent* entry;
    struct stat fileStat;
    int count = 0;
    long long int totalSize = 0;
    const char *path = "src/mail_test";
    char resultBuffer[256];

    directory = opendir(path);
    if (directory == NULL) {
        printf("ERROR - unable to open mailbox");
        return ERROR_STATE;
    }
    while ((entry = readdir(directory)) != NULL) {
        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", path, entry->d_name);
        if (stat(filePath, &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            count++;
            totalSize += fileStat.st_size;
            }
        }
    }

    closedir(directory);

    snprintf(resultBuffer, sizeof(resultBuffer), "+OK %d %lld\r\n", count, totalSize);
    for(int i = 0; i < strlen(resultBuffer); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,resultBuffer[i]);
        }
    }
    return data->stm.current->state;
}

unsigned list_handler(selector_key *key){
    DIR* directory;
    client_data * data = ATTACHMENT(key);
    struct dirent* entry;
    struct stat fileStat;
    long long int totalSize = 0;
    int count = 0;
    const char *path = "src/mail_test";
    char resultBuffer[256];

    directory = opendir(path);
    if (directory == NULL) {
        printf("ERROR - unable to open mailbox\n");
        return ERROR_STATE;
    }
    snprintf(resultBuffer, sizeof(resultBuffer), "+OK LIST\n");
    while ((entry = readdir(directory)) != NULL) {
        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", path, entry->d_name);
        if (stat(filePath, &fileStat) == 0) {
            if (S_ISREG(fileStat.st_mode)) {
                printf("File %d size: %lld\n", count, fileStat.st_size);
                count++;
                 snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer),
                         "%d %lld\n", count, fileStat.st_size);
            }
        }
    }

    closedir(directory);
    snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer), "\r\n");

    for(int i = 0; i < strlen(resultBuffer); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,resultBuffer[i]);
        }
    }
    return data->stm.current->state;
}

unsigned retr_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    
    

}



unsigned dele_handler(selector_key *key){

}

unsigned rset_handler(selector_key *key){
    //reset the SMTP connection to the initial state in which the sender
    // and recipient buffers are erased and the connection is ready to begin a new mail transaction.
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK RESET STATE\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    data->username = NULL;  //Devuelvo al usuario al momento 0 de la aplicacion. No puso su usuario y esta en authorization state.

    DIR* directory;
    struct dirent* file;

    char * srcDir = "src/mail/trash"; //TODO: poner paths de mail y trash
    char * destDir = "src/mail";

    directory = opendir(srcDir);

    // Iterate over files in the source directory
    while ((file = readdir(directory)) != NULL) {
        // Ignore "." and ".." entries
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }
        char srcPath[256];
        char destPath[256];
        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, file->d_name);
        snprintf(destPath, sizeof(destPath), "%s/%s", destDir, file->d_name);

        // Move the file
        if (rename(srcPath, destPath) != 0) {
            printf("Failed to move file: %s\n", srcPath);
        }
    }

    closedir(directory);

    return AUTH_STATE;
}

unsigned noop_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK NOOP\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

//Deberia devolver el estado AUTH no?
unsigned quit_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK GOODBYE\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }

    const char* directory_path = "src/mail/trash"; // TODO:Poner el path del trash

    DIR* directory = opendir(directory_path);
    struct dirent* file;

    while ((file = readdir(directory)) != NULL) {
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
            char file_path[100];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, file->d_name);
            remove(file_path);
        }
    }

    closedir(directory);
    //finishConnection();
    return data->stm.current->state;
}

unsigned capa_handler(selector_key *key){
    
}

