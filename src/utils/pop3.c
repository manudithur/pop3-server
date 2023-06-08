#include "../include/pop3.h"
#define COMMAND_AMOUNT 10

typedef struct commands{
    char command_name[MAX_COMMAND_LEN];
    enum pop3_states valid_states[];
    void * action;
}commands;

//All pop3 commands
static const commands command_list[COMMAND_AMOUNT] = {
    {.command_name = "NOOP", .valid_states = {TRANSACTION_STATE}, .action = noop_handler },
    {.command_name = "PASS", .valid_states = {AUTH_STATE }, .action = pass_handler },                                       
    {.command_name = "USER", .valid_states = {AUTH_STATE }, .action = user_handler },                                         
    {.command_name = "QUIT", .valid_states = {AUTH_STATE, TRANSACTION_STATE, UPDATE_STATE}, .action = quit_handler},
    {.command_name = "STAT", .valid_states = {TRANSACTION_STATE}, .action = stat_handler},                                 
    {.command_name = "LIST", .valid_states = {TRANSACTION_STATE}, .action = list_handler},                                 
    {.command_name = "RETR", .valid_states = {TRANSACTION_STATE}, .action = retr_handler},                                 
    {.command_name = "DELE", .valid_states = {TRANSACTION_STATE}, .action = dele_handler},                                 
    {.command_name = "RSET", .valid_states = {TRANSACTION_STATE}, .action = rset_handler},                                 
    {.command_name = "CAPA", .valid_states = {AUTH_STATE, TRANSACTION_STATE}, .action = capa_handler}
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
            //Command ready to analyze
            for(int i = 0 ; i < COMMAND_AMOUNT; i ++){
                if(strcmp(data->command.command, command_list[i].command_name) == 0){
                    for(int j = 0; j < sizeof(command_list[i].valid_states); j++){
                        if(data->state == command_list[i].valid_states[j]){
                            command_list[i].action();
                        }
                    }
                }
            }
        }
    }
    


    //unsigned nextAction = parseHandler();

//  while (nextAction == KEEP_READING)
//    {
//        readCount = recv(key->fd, readBuffer, readLimit, 0);
//        if (readCount <= 0) {
//          return -1;
//        }
//        nextAction = parseHandler();
//    }

    //return nextState;

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



