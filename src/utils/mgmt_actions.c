#include "mgmt_actions.h"


#define PATH_MAX_LENGTH 300

unsigned mgmt_user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK USER\r\n"};
    if (validateAdminUser(data->command.arg1) != VALID_CREDENTIALS || data->command.arg2[0] != '\0'){
        return ERROR_MGMT;
    }
    data->username = malloc(sizeof(char) * (strlen(data->command.arg1) + 1));
    strcpy(data->username, data->command.arg1);
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }

    return AUTH_MGMT;
}

unsigned mgmt_pass_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK PASS\r\n"};
    printf("password = %s\n", data->command.arg1);
    if (data->username == NULL || validateAdminCredentials(data->username, data->command.arg1) != VALID_CREDENTIALS || data->command.arg2[0] != '\0'){
        return ERROR_MGMT;
    }
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return ACTIVE_MGMT;
}

unsigned mgmt_noop_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK NOOP\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    stats_print();
    return data->stm.current->state;
}

//Deberia devolver el estado AUTH no?
unsigned mgmt_quit_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK GOODBYE\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }

    char dirPath[PATH_MAX_LENGTH];
    snprintf(dirPath, PATH_MAX_LENGTH, "src/mail/");
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "%s/", data->username);
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "cur/");

    DIR* directory = opendir(dirPath);
    struct dirent* file;
    int emailIndex = 0;

    while ((file = readdir(directory)) != NULL) {
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0 && data->emailDeleted[emailIndex++] == true) {
            char file_path[100];
            snprintf(file_path, sizeof(file_path), "%s/%s", dirPath, file->d_name);
            remove(file_path);
        }
    }

    closedir(directory);
    //finishConnection();
    return data->stm.current->state;
}

unsigned mgmt_capa_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK CAPA\nUSER\nPASS\nQUIT\nCAPA\nLIST\nRETR\nSTAT\nDELE\nNOOP\nRSET\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;    
}

unsigned mgmt_historic_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    // char buf[] = {"+OK HISTORIC_CONEC\nHISTORIC CONECTIONS = \r\n"};
    char buf[1000] = {'\0'};
    sprintf(buf, "+OK HISTORIC_CONEC\nHISTORIC CONECTIONS = %ld\r\n", getTotalConnections());
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;  
}

unsigned mgmt_live_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[1000] = {'\0'};
    sprintf(buf, "+OK LIVE_CONEC\nLIVE CONECTIONS = %ld\r\n", getConcurrentConnections());
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;  
}

unsigned mgmt_bytes_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[1000] = {'\0'};
    sprintf(buf, "+OK BYTES_TRANS\nBYTES SENT = %ld\nBYTES RECEIVED = %ld\r\n", getTotalBytesSent(), getTotalBytesReceived());
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;  
}

// unsigned mgmt_users_handler(selector_key *key);

// unsigned mgmt_status_handler(selector_key *key);

// unsigned mgmt_max_users_handler(selector_key *key);

// unsigned mgmt_max_connections_handler(selector_key *key);

// unsigned mgmt_timeout_handler(selector_key *key);

// unsigned mgmt_delete_user_handler(selector_key *key);

// unsigned mgmt_add_user_handler(selector_key *key);

// unsigned mgmt_reset_user_password_handler(selector_key *key);

// unsigned mgmt_change_password_handler(selector_key *key);

