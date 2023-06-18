#include "mgmt_actions.h"


#define PATH_MAX_LENGTH 300

unsigned mgmt_user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK USER\n\r\n"};
    if (validateAdminUser(data->command.arg1) != VALID_CREDENTIALS || data->command.arg2[0] != '\0'){
        return ERROR_MGMT;
    }
    data->username = NULL;
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
    char buf[] = {"+OK PASS\n\r\n"};
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
    char buf[] = "+OK GOODBYE\r\n";
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return UPDATE_MGMT;
}


unsigned mgmt_capa_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char bud[] = {"+OK CAPA\n\nNOOP\nQUIT\nHISTORIC_CONEC\nLIVE_CONEC\nBYTES_TRANS\nUSERS\nSTATUS\nMAX_USERS <int>\nDELETE_USER <username>\nADD_USER <username> <password>\nRESET_USER_PASSWORD <username>\nCHANGE_PASSWORD <oldPassword> <newPassword>\r\n"};
    char buf[] = {"+OK CAPA\n\nUSER\nPASS\nQUIT\nCAPA\nLIST\nRETR\nSTAT\nDELE\nNOOP\nRSET\n\r\n"};
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
    sprintf(buf, "+OK HISTORIC_CONEC\n\nHISTORIC CONECTIONS = %ld\n\r\n", getTotalConnections());
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
    sprintf(buf, "+OK LIVE_CONEC\n\nLIVE CONECTIONS = %ld\n\r\n", getConcurrentConnections());
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
    sprintf(buf, "+OK BYTES_TRANS\n\nBYTES SENT = %ld\nBYTES RECEIVED = %ld\n\r\n", getTotalBytesSent(), getTotalBytesReceived());
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;  
}

unsigned mgmt_users_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    TUsers * users = getUsers();
    char buf[5000] = {'\0'};
    sprintf(buf, "+OK USERS\n\nUSER COUNT: %d\n\nUSER LIST:\n", users->count);
    for(int j = 0 ; j < users->count ; j++){
        sprintf(buf + strlen(buf), "%s\n", users->users[j].username);
    }

    sprintf(buf + strlen(buf), "\n\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;  
}

unsigned mgmt_status_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    TUsers * users = getUsers();
    char buf[5000] = {'\0'};
    sprintf(buf, "+OK STATUS\n\nBYTES SENT = %ld\nBYTES RECEIVED = %ld\n\n", getTotalBytesSent(), getTotalBytesReceived());
    sprintf(buf + strlen(buf), "LIVE CONECTIONS = %ld\n", getConcurrentConnections());
    sprintf(buf + strlen(buf), "HISTORIC CONECTIONS = %ld\n\n", getTotalConnections());
    sprintf(buf + strlen(buf), "TOTAL USERS = %ld\n\r\n", users->count);

    //Agregar la data de la configuracion.

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;  
}

unsigned mgmt_max_users_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);

    if(data->command.arg1[0] == '\0')
        return ERROR_MGMT;

    char buf[1000] = {'\0'};
    sprintf(buf, "+OK MAX_USERS\n\n");
    int maxUsers = setMaxUsers(atoi(data->command.arg1));

    if(maxUsers == MAX_USERS_CHANGED_SUCCESSFULLY)
        sprintf(buf + strlen(buf), "MAX USERS CHANGED SUCCESSFULLY\n");
    else if(maxUsers = INVALID_MAX_USERS){
        sprintf(buf + strlen(buf), "INVALID MAX USERS\n");
    }
    
    sprintf(buf + strlen(buf), "\n\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;  
}

 unsigned mgmt_max_connections_handler(selector_key *key){
     client_data * data = ATTACHMENT(key);
     if (data->command.arg2[0] != '\0' || isNumber(data->command.arg1) == false){
         return ERROR_MGMT;
     }
     int newMax = atoi(data->command.arg1);
     changeMaxConnections(newMax);

     char buf[1000] = {'\0'};
     sprintf(buf, "+OK MAX_CONNECTIONS CHANGED\n");
     for (int i = 0; buf[i] != '\0'; i++){
         if (buffer_can_write(&data->wbStruct)){
             buffer_write(&data->wbStruct,buf[i]);
         }
     }
     return data->stm.current->state;
}

void applyTimeout(int time){
    sleep(time);
}

 unsigned mgmt_timeout_handler(selector_key *key){
     client_data * data = ATTACHMENT(key);
     if (data->command.arg2[0] != '\0' || isNumber(data->command.arg1) == false){
         return ERROR_MGMT;
     }
     int timeoutTime = atoi(data->command.arg1);
     applyTimeout(timeoutTime);

     char buf[1000] = {'\0'};
     sprintf(buf, "+OK TIMEOUT COMPLETED\n");
     for (int i = 0; buf[i] != '\0'; i++){
         if (buffer_can_write(&data->wbStruct)){
             buffer_write(&data->wbStruct,buf[i]);
         }
     }

     return data->stm.current->state;
}

unsigned mgmt_delete_user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    
    char buf[1000] = {'\0'};
    sprintf(buf, "+OK DELETE_USER\n\n");
    int deleteRes = deleteUser(data->command.arg1);
    printf("deleteRes: %d\n", deleteRes);
    if(deleteRes == USER_DELETED)
        sprintf(buf + strlen(buf), "USER DELETED SUCCESSFULLY\n");
    else if(deleteRes == USER_NOT_FOUND)
        sprintf(buf + strlen(buf), "USER NOT FOUND\n");
    else if (deleteRes == ADMIN_DELETE_ATTEMPT)
        sprintf(buf + strlen(buf), "ADMIN CANNOT BE DELETED\n");

    
    sprintf(buf + strlen(buf), "\n\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

unsigned mgmt_add_user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    
    char buf[1000] = {'\0'};

    sprintf(buf, "+OK ADD_USER\n\n");
    int addRes = addUser(data->command.arg1, data->command.arg2);

    if(addRes == USER_ADDED)
        sprintf(buf + strlen(buf), "USER ADDED SUCCESSFULLY\n");
    else if(addRes == USER_ALREADY_EXISTS)
        sprintf(buf + strlen(buf), "USER ALREADY EXISTS\n");
    else if (addRes == MAX_USERS_REACHED)
        sprintf(buf + strlen(buf), "MAX USERS REACHED\n");
    else if(addRes == INVALID_CREDENTIALS)
        sprintf(buf + strlen(buf), "INVALID CREDENTIALS\n");
    
    sprintf(buf + strlen(buf), "\n\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

unsigned mgmt_reset_user_password_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    
    char buf[1000] = {'\0'};

    sprintf(buf, "+OK RESET_USER_PASSWORD\n\n");
    int resetRes = resetUserPassword(data->command.arg1);

    if(resetRes == PASSWORD_CHANGED_SUCCESSFULLY)
        sprintf(buf + strlen(buf), "PASSWORD RESET SUCCESSFULLY\n");
    else if(resetRes == PASSWORD_CHANGE_FAILED)
        sprintf(buf + strlen(buf), "USER NOT FOUND\n");
    
    sprintf(buf + strlen(buf), "\n\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

unsigned mgmt_change_password_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    
    char buf[1000] = {'\0'};

    sprintf(buf, "+OK CHANGE_PASSWORD\n\n");
    int changeRes = changePassword(data->username, data->command.arg1, data->command.arg2);

    if(changeRes == PASSWORD_CHANGED_SUCCESSFULLY)
        sprintf(buf + strlen(buf), "PASSWORD CHANGED SUCCESSFULLY\n");
    else if(changeRes == PASSWORD_CHANGE_FAILED)
        sprintf(buf + strlen(buf), "PASSWORD CHANGE FAILED\n");
    
    sprintf(buf + strlen(buf), "\n\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

