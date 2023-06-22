// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "mgmt_actions.h"


#define PATH_MAX_LENGTH 300

unsigned mgmt_user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK USER\r\n"};
    if (validateAdminUser(data->command.arg1) != VALID_CREDENTIALS || data->command.arg2[0] != '\0'){
        return ERROR_MGMT;
    }
    data->username=malloc(strlen((data->command.arg1))+1);
    strcpy(data->username,data->command.arg1);
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
    if (data->username == NULL || validateAdminCredentials(data->username, data->command.arg1) != VALID_CREDENTIALS || data->command.arg2[0] != '\0'){
        printf("WARN: Admin wrong password.\n");
        return ERROR_MGMT;
    }
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    printf("INFO: Admin connected.\n");
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
    printf("INFO: Admin requested NOOP.\n");
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
    printf("INFO: Admin disconnected.\n");
    stats_remove_connection();
    return UPDATE_MGMT;
}


unsigned mgmt_capa_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK CAPA\nNOOP\nQUIT\nHISTORIC_CONNEC\nLIVE_CONEC\nBYTES_TRANS\nUSERS\nSTATUS\nMAX_USERS <int>\nDELETE_USER <username>\nADD_USER <username> <password>\nRESET_USER_PASSWORD <username>\nCHANGE_PASSWORD <oldPassword> <newPassword>\r\n"};
    //char buf[] = {"+OK CAPA\nUSER\nPASS\nQUIT\nCAPA\nLIST\nRETR\nSTAT\nDELE\nNOOP\nRSET\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    printf("INFO: Admin requested capabilities.\n");
    return data->stm.current->state;    
}

unsigned mgmt_historic_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    // char buf[] = {"+OK HISTORIC_CONNEC\nHISTORIC CONECTIONS = \r\n"};
    char buf[1000] = {'\0'};
    sprintf(buf, "+OK HISTORIC_CONNEC\nHISTORIC CONECTIONS = %ld\r\n", getTotalConnections());
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    printf("INFO: Admin requested historic connections.\n");
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
    printf("INFO: Admin requested live connections.\n");
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
    printf("INFO: Admin requested bytes sent and received.\n");
    return data->stm.current->state;  
}

unsigned mgmt_users_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    TUsers * users = getUsers();
    char buf[5000] = {'\0'};
    sprintf(buf, "+OK USERS\nUSER COUNT: %d\nUSER LIST:\n", users->count);
    for(int j = 0 ; j < users->count ; j++){
        sprintf(buf + strlen(buf), "%s\n", users->users[j].username);
    }

    sprintf(buf + strlen(buf), "\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    printf("INFO: Admin requested user list.\n");
    return data->stm.current->state;  
}

unsigned mgmt_status_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    TUsers * users = getUsers();
    char buf[5000] = {'\0'};
    sprintf(buf, "+OK STATUS\nBYTES SENT = %ld\nBYTES RECEIVED = %ld\n", getTotalBytesSent(), getTotalBytesReceived());
    sprintf(buf + strlen(buf), "LIVE CONECTIONS = %ld\n", getConcurrentConnections());
    sprintf(buf + strlen(buf), "HISTORIC CONECTIONS = %ld\n", getTotalConnections());
    sprintf(buf + strlen(buf), "TOTAL USERS = %d\n", users->count);

    sprintf(buf + strlen(buf), "MAX USERS = %d\n", getMaxUsers());
    sprintf(buf + strlen(buf), "MAX CONNECTIONS = %d\r\n", getMaxConnections());
    //Agregar la data de la configuracion.

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    printf("INFO: Admin requested server status\n");
    return data->stm.current->state;  
}

unsigned mgmt_max_users_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);

    if(data->command.arg1[0] == '\0')
        return ERROR_MGMT;

    char buf[1000] = {'\0'};
    int maxUsers = setMaxUsers(atoi(data->command.arg1));

    if(maxUsers == MAX_USERS_CHANGED_SUCCESSFULLY){
        sprintf(buf, "+OK MAX_USERS\n");
        sprintf(buf + strlen(buf), "MAX USERS CHANGED SUCCESSFULLY");
        printf("INFO: Admin changed max users\n");
    }
    else if(maxUsers == INVALID_MAX_USERS){
        sprintf(buf + strlen(buf), "-ERR INVALID MAX USERS");
    }
    
    sprintf(buf + strlen(buf), "\r\n");

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
     sprintf(buf, "+OK MAX_CONNECTIONS CHANGED\r\n");
     for (int i = 0; buf[i] != '\0'; i++){
         if (buffer_can_write(&data->wbStruct)){
             buffer_write(&data->wbStruct,buf[i]);
         }
     }
    printf("INFO: Admin changed max connections\n");
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
     sprintf(buf, "+OK TIMEOUT COMPLETED\r\n");
     for (int i = 0; buf[i] != '\0'; i++){
         if (buffer_can_write(&data->wbStruct)){
             buffer_write(&data->wbStruct,buf[i]);
         }
     }
    printf("INFO: Admin applied timeout\n");
     return data->stm.current->state;
}

unsigned mgmt_delete_user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    
    char buf[1000] = {'\0'};
    
    int deleteRes = deleteUser(data->command.arg1);
    if(deleteRes == USER_DELETED){
        sprintf(buf, "+OK DELETE_USER\n");
        sprintf(buf + strlen(buf), "USER DELETED SUCCESSFULLY");
        printf("INFO: Admin deleted user\n");
    }
    else if(deleteRes == USER_NOT_FOUND)
        sprintf(buf + strlen(buf), "-ERR USER NOT FOUND");
    else if (deleteRes == ADMIN_DELETE_ATTEMPT)
        sprintf(buf + strlen(buf), "-ERR ADMIN CANNOT BE DELETED");

    
    sprintf(buf + strlen(buf), "\r\n");

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

    int addRes = addUser(data->command.arg1, data->command.arg2);

    if(addRes == USER_ADDED){
        sprintf(buf, "+OK ADD_USER\n");
        sprintf(buf + strlen(buf), "USER ADDED SUCCESSFULLY");
        printf("INFO: Admin added user\n");
    }
    else if(addRes == USER_ALREADY_EXISTS)
        sprintf(buf + strlen(buf), "-ERR USER ALREADY EXISTS");
    else if (addRes == MAX_USERS_REACHED)
        sprintf(buf + strlen(buf), "-ERR MAX USERS REACHED");
    else if(addRes == INVALID_CREDENTIALS)
        sprintf(buf + strlen(buf), "-ERR INVALID CREDENTIALS");
    
    sprintf(buf + strlen(buf), "\r\n");

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

    
    int resetRes = resetUserPassword(data->command.arg1);

    if(resetRes == PASSWORD_CHANGED_SUCCESSFULLY){
        sprintf(buf, "+OK RESET_USER_PASSWORD\n");
        sprintf(buf + strlen(buf), "PASSWORD RESET SUCCESSFULLY");
        printf("INFO: Admin reset user password\n");
    }
    else if(resetRes == PASSWORD_CHANGE_FAILED)
        sprintf(buf + strlen(buf), "-ERR USER NOT FOUND");
    
    sprintf(buf + strlen(buf), "\r\n");

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

    int changeRes = changePassword(data->username, data->command.arg1, data->command.arg2);

    if(changeRes == PASSWORD_CHANGED_SUCCESSFULLY){
        sprintf(buf, "+OK CHANGE_PASSWORD\n");
        sprintf(buf + strlen(buf), "PASSWORD CHANGED SUCCESSFULLY");
        printf("INFO: Admin changed password\n");
    }
    else if(changeRes == PASSWORD_CHANGE_FAILED)
        sprintf(buf + strlen(buf), "PASSWORD CHANGE FAILED");
    
    sprintf(buf + strlen(buf), "\r\n");

    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

