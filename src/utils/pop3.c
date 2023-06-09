#include "../include/pop3.h"
#define COMMAND_AMOUNT 10

typedef struct commands{
    char command_name[MAX_COMMAND_LEN];

    void * action;
}commands;

static void check_commands(struct selector_key * key, commands * command_list, int command_amount){
    client_data * data = ATTACHMENT(key);
    for(int i = 0 ; i < command_amount; i ++){
        if(strcmp(data->command.command, command_list[i].command_name) == 0){
            command_list[i].action(key);
            break;
        }
    }
}

//All pop3 commands
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

unsigned readHandler(struct selector_key * key) {
    client_data * data = ATTACHMENT(key);

    size_t readLimit;
    ssize_t readCount;
    uint8_t* readBuffer;

    readBuffer = buffer_write_ptr(&data->rbStruct, &readLimit);
    readCount = recv(key->fd, readBuffer, readLimit, 0);

    if (readCount <= 0) {
        return -1;
    }

    buffer_write_adv(&data->rbStruct, readCount);

    while(buffer_can_read(&data->rbStruct)){

        parser_event * ret = parser_feed(&data->parser, buffer_read(&data->rbStruct));

        if(ret->type == PARSE_COMMAND)
            data->command.command[data->command.commandLen++] = ret->c;
        else if(ret->type == PARSE_ARG1)
            data->command.arg1[data->command.arg1Len++] = ret->c;
        else if(ret-> type == PARSE_ARG2)
            data->command.arg2[data->command.arg2Len++] = ret->c;
        else{
            switch(data->stm.current.state){
                case TRANSACTION_STATE:
                    check_commands(key, command_list_transaction, TRANSACTION_COMMAND_AMOUNT);
                    break;
                case AUTH_STATE:
                    check_commands(key, command_list_auth, AUTH_COMMAND_AMOUNT);
                    break;
                case UPDATE_STATE:
                    check_commands(key, command_list_update, UPDATE_COMMAND_AMOUNT);
                    break;
            }
    }

    return TRANSACTION_STATE;
}






//unsigned writeHandler(struct selector_key *key){
//    client_data * data = ATTACHMENT(key);
//
//    size_t writeLimit;
//    ssize_t writeCount;
//    uint8_t* writeBuffer;
//
//    writeBuffer = buffer_read_ptr(&data->wbStruct, &writeLimit);
//    writeCount = send(data->fd, writeBuffer, writeLimit, MSG_NOSIGNAL);
//
//    if (writeCount <= 0) {
//        return -1;
//    }
//
//    buffer_read_adv(&data->wbStruct, writeCount);
//
//    //if I can read more from buffer -> return UPDATE_STATE? no estoy seguro, tiene que seguir escribiendo
//    return 0;
//}



