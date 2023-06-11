#include "pop3_actions.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PATH_MAX 300

unsigned user_handler(selector_key *key){

}

unsigned pass_handler(selector_key *key){

}

unsigned stat_handler(selector_key *key){
    DIR* directory;
    client_data * data = ATTACHMENT(key);
    struct dirent* entry;
    struct stat fileStat;
    int count = 0;
    long long int totalSize = 0;
    const char *path = "/TODO/enter/mail/dir/path/here";
    char resultBuffer[256];

    directory = opendir(path);
    if (directory == NULL) {
        snprintf(resultBuffer, sizeof(resultBuffer), "ERROR - unable to open mailbox");
        return ERROR_STATE;
    }
    while ((entry = readdir(directory)) != NULL) {
        if (S_ISREG(fileStat.st_mode)) { 
            count++;
        
            char filePath[PATH_MAX];
            snprintf(filePath, PATH_MAX, "%s/%s", path, entry->d_name);
            if (stat(filePath, &fileStat) == 0) {
                totalSize += fileStat.st_size;
            }
        }
    }

    closedir(directory);

    snprintf(resultBuffer, sizeof(resultBuffer), "+OK %d %lld", count, totalSize);
    for(int i = 0; i < strlen(resultBuffer); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,resultBuffer[i]);
        }
    }
    return data->stm.current->state;
}

unsigned list_handler(selector_key *key){

}

unsigned retr_handler(selector_key *key){

}

unsigned dele_handler(selector_key *key){

}

unsigned rset_handler(selector_key *key){
    //reset the SMTP connection to the initial state in which the sender
    // and recipient buffers are erased and the connection is ready to begin a new mail transaction.
}

unsigned noop_handler(selector_key *key){
    printf("Llegue");
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK NOOP\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

unsigned quit_handler(selector_key *key){

}

unsigned capa_handler(selector_key *key){
    
}

