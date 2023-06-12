#include "../include/pop3.h"
#include <stdio.h>

typedef struct commands{
    char command_name[MAX_COMMAND_LEN];
    unsigned (*action)(selector_key * key);
}commands;

static unsigned lastValidState = AUTH_STATE;

static const commands command_list_auth[AUTH_COMMAND_AMOUNT] = {
    {.command_name = "PASS",  .action = pass_handler },                                       
    {.command_name = "USER",  .action = user_handler },                                         
    {.command_name = "QUIT", .action = quit_handler},                                
    {.command_name = "CAPA", .action = capa_handler}
};

static const commands command_list_transaction[TRANSACTION_COMMAND_AMOUNT] = {
    {.command_name = "NOOP",  .action = noop_handler },                                                                                
    {.command_name = "QUIT",  .action = quit_handler},
    {.command_name = "STAT",  .action = stat_handler},                                 
    {.command_name = "LIST",  .action = list_handler},                                 
    {.command_name = "RETR",  .action = retr_handler},                                 
    {.command_name = "DELE",  .action = dele_handler},                                 
    {.command_name = "RSET",  .action = rset_handler},                                 
    {.command_name = "CAPA",  .action = capa_handler}
};

static const commands command_list_update[UPDATE_COMMAND_AMOUNT] = {                                        
    {.command_name = "QUIT", .action = quit_handler},
};

static unsigned check_commands(struct selector_key * key, const commands * command_list, int command_amount){
    client_data * data = ATTACHMENT(key);
    for(int i = 0 ; i < command_amount; i ++){
        if(strcmp(data->command.command, command_list[i].command_name) == 0){
            return command_list[i].action(key);
        }
    }
    return data->stm.current->state;
}


// TODO: check if return states are correctly managed
unsigned readHandler(struct selector_key * key) {
    client_data * data = ATTACHMENT(key);

    size_t readLimit;
    ssize_t readCount;
    uint8_t * readBuffer;
    unsigned retState;

    readBuffer = buffer_write_ptr(&data->rbStruct, &readLimit);
    readCount = recv(key->fd, readBuffer, readLimit, 0);

    if (readCount <= 0) {
        return -1;
    }

    buffer_write_adv(&data->rbStruct, readCount);

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
                case TRANSACTION_STATE:
                    retState =  check_commands(key, command_list_transaction, TRANSACTION_COMMAND_AMOUNT);
                    break;
                case AUTH_STATE:
                    retState =  check_commands(key, command_list_transaction, AUTH_COMMAND_AMOUNT);
                    break;
                case UPDATE_STATE:
                    retState = check_commands(key, command_list_update, UPDATE_COMMAND_AMOUNT);
                    break;
            }


            parser_reset(data->parser);
            data->command.commandLen = 0;
            data->command.arg1Len = 0;
            data->command.arg2Len = 0;
            data->command.command[0] = '\0';
            data->command.arg1[0] = '\0';
            data->command.arg2[0] = '\0';

            selector_set_interest_key(key, OP_WRITE);

            if(retState == ERROR_STATE){
                printf("error state\n");
            }
            else{
                lastValidState = retState;
            }
            return retState;
        }
    }
 
    //selector_set_interest_key(key, OP_WRITE);
    return data->stm.current->state;
}



unsigned writeHandler(struct selector_key *key){
    client_data * data = ATTACHMENT(key);

    size_t writeLimit;
    ssize_t writeCount;
    uint8_t* writeBuffer;

    writeBuffer = buffer_read_ptr(&data->wbStruct, &writeLimit);
    writeCount = send(data->fd, writeBuffer, writeLimit, MSG_NOSIGNAL);

    if (writeCount <= 0) {
        return ERROR_STATE;
    }

    buffer_read_adv(&data->wbStruct, writeCount);
    selector_set_interest_key(key, OP_READ);
//

    //if I can read more from buffer -> return UPDATE_STATE? no estoy seguro, tiene que seguir escribiendo
    return data->stm.current->state;
}



