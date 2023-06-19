// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/mgmt.h"
#include <stdio.h>

typedef struct commands{
    char command_name[MAX_COMMAND_LEN];
    unsigned (*action)(selector_key * key);
}commands;

static const commands command_list_auth[MGMT_AUTH_COMMAND_AMOUNT] = {
    {.command_name = "PASS",  .action = mgmt_pass_handler },                                       
    {.command_name = "USER",  .action = mgmt_user_handler },                                         
    {.command_name = "QUIT", .action = mgmt_quit_handler},                                
    {.command_name = "CAPA", .action = mgmt_capa_handler}
};

static const commands command_list_transaction[ACTIVE_MGMT_COMMAND_AMOUNT] = {
    {.command_name = "NOOP",  .action = mgmt_noop_handler },                                                                                
    {.command_name = "QUIT",  .action = mgmt_quit_handler},
    {.command_name = "HISTORIC_CONNEC",  .action = mgmt_historic_handler},
    {.command_name = "LIVE_CONNEC",  .action = mgmt_live_handler},
    {.command_name = "BYTES_TRANS",  .action = mgmt_bytes_handler},                                 
    {.command_name = "USERS",  .action = mgmt_users_handler},                                 
    {.command_name = "STATUS",  .action = mgmt_status_handler},                                 
    {.command_name = "MAX_USERS",  .action = mgmt_max_users_handler},
     {.command_name = "MAX_CONNEC",  .action = mgmt_max_connections_handler},
     {.command_name = "TIMEOUT",  .action = mgmt_timeout_handler},
    {.command_name = "DELETE_USER",  .action = mgmt_delete_user_handler}, 
    {.command_name = "ADD_USER",  .action = mgmt_add_user_handler},       
    {.command_name = "RESET_USER_PASSWORD",  .action = mgmt_reset_user_password_handler}, 
    {.command_name = "CHANGE_PASSWORD",  .action = mgmt_change_password_handler},                             
    {.command_name = "CAPA",  .action = mgmt_capa_handler}
};

static unsigned mgmt_check_commands(struct selector_key * key, const commands * command_list, int command_amount){
    client_data * data = ATTACHMENT(key);
    for(int i = 0 ; i < command_amount; i ++){
        if(strcmp(data->command.command, command_list[i].command_name) == 0){
            printf("ENTRE AL COMANDO %s\n", data->command.command);
            return command_list[i].action(key);
        }
    }
    return ERROR_MGMT;
}

void mgmt_errorHandler(const unsigned state,struct selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"-ERR\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    selector_set_interest_key(key, OP_WRITE);
//    return lastValidState;
}

void freeAllMgmt(const unsigned state, struct selector_key * key){
    //hace todos los frees
    printf("ENTRE AL FREE\n");
    client_data * data = ATTACHMENT(key);
    parser_destroy(data->parser);
    free(data->username);
    data->parser = NULL;

    free(data);


    close(key->fd);
    key->data = NULL;
}

// TODO: check if return states are correctly managed
unsigned mgmt_readHandler(struct selector_key * key) {
    printf("ENTRE AL READ DE MGMT\n");
    client_data * data = ATTACHMENT(key);

    size_t readLimit;
    ssize_t readCount;
    uint8_t * readBuffer;
    unsigned retState;

    if(!buffer_can_read(&data->rbStruct)){
        readBuffer = buffer_write_ptr(&data->rbStruct, &readLimit);
        readCount = recv(key->fd, readBuffer, readLimit, 0);
        stats_update(0,readCount,0);
        if (readCount == 0) {
            stats_remove_connection();
            selector_set_interest_key(key, OP_NOOP);
            return UPDATE_MGMT;
        }
        buffer_write_adv(&data->rbStruct, readCount);
    }

    while(buffer_can_read(&data->rbStruct)) {

        const struct parser_event *ret = parser_feed(data->parser, buffer_read(&data->rbStruct));
    

        if (ret->type == PARSE_COMMAND) {
            data->command.command[data->command.commandLen++] = ret->data[0];
            data->command.command[data->command.commandLen] = '\0';
        } else if (ret->type == PARSE_ARG1) {
            data->command.arg1[data->command.arg1Len++] = ret->data[0];
            data->command.arg1[data->command.arg1Len] = '\0';
        } else if (ret->type == PARSE_ARG2) {
            data->command.arg2[data->command.arg2Len++] = ret->data[0];
            data->command.arg2[data->command.arg2Len] = '\0';
        }
        else if(ret->type == ALMOST_DONE){
            //Que no haga nada
        }
        else{
          
            switch(data->stm.current->state){
                case ACTIVE_MGMT:
                    retState =  mgmt_check_commands(key, command_list_transaction, ACTIVE_MGMT_COMMAND_AMOUNT);
                    break;
                case AUTH_MGMT:
                    retState =  mgmt_check_commands(key, command_list_auth, MGMT_AUTH_COMMAND_AMOUNT);
                    break;
            }
            
            
            selector_set_interest_key(key, OP_WRITE);
            
            parser_reset(data->parser);
            data->command.commandLen = 0;
            data->command.arg1Len = 0;
            data->command.arg2Len = 0;
            data->command.command[0] = '\0';
            data->command.arg1[0] = '\0';
            data->command.arg2[0] = '\0';

            
            if(retState == ERROR_MGMT){
                printf("error state\n");
                retState = data->lastValidState;
            }
            else{
                data->lastValidState = retState;
            }
            printf("TERMINE EL READ\n");
            return retState;
        }
    }
 
    //selector_set_interest_key(key, OP_WRITE);

    return data->stm.current->state;
}



unsigned mgmt_writeHandler(struct selector_key *key){
    client_data * data = ATTACHMENT(key);

    size_t writeLimit;
    ssize_t writeCount;
    uint8_t* writeBuffer;

    writeBuffer = buffer_read_ptr(&data->wbStruct, &writeLimit);
    writeCount = send(data->fd, writeBuffer, writeLimit, MSG_NOSIGNAL);
    stats_update(writeCount,0,0);

    if (writeCount <= 0) {
        printf("error en write\n");
        selector_set_interest_key(key, OP_NOOP);
        return ERROR_MGMT;
    }

    buffer_read_adv(&data->wbStruct, writeCount);
    selector_set_interest_key(key, OP_READ);
//
    if(buffer_can_read(&data->rbStruct)){
        return mgmt_readHandler(key);
    }

    //if I can read more from buffer -> return UPDATE_STATE? no estoy seguro, tiene que seguir escribiendo
    return data->lastValidState;
}



