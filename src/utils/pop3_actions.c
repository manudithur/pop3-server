#include "pop3_actions.h"
#include <stdio.h>

unsigned user_handler(selector_key *key){

}

unsigned pass_handler(selector_key *key){

}

unsigned stat_handler(selector_key *key){

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
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK RESET STATE\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    data->username = NULL;  //Devuelvo al usuario al momento 0 de la aplicacion. No puso su usuario y esta en authorization state.
    //TODO: desborrar los mensajes borrados con dele (estan en un trash)
    return AUTH_STATE;
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
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK GOODBYE\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }

    const char* directory_path = "/"; // TODO:Poner el path del trash

    DIR* directory = opendir(directory_path);
    struct dirent* file;

    while ((file = readdir(directory)) != NULL) {
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
            char file_path[100];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, file->d_name);
//            if (remove(file_path) == 0) {
//                printf("Deleted file: %s\n", file_path);
//            } else {
//                printf("Failed to delete file: %s\n", file_path);
//            }
        }
    }

    closedir(directory);
    //finishConnection();
    return data->stm.current->state;
}

unsigned capa_handler(selector_key *key){
    
}

