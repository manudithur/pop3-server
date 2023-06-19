// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    return ERROR_STATE;
}

void mailDeleter(const unsigned state,struct selector_key * key){
    client_data * data = ATTACHMENT(key);

    if (data->username != NULL) {
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
    }

    unregisterHandler(data->stm.current->state,key);
}

void unregisterHandler(const unsigned state, struct selector_key * key){
    client_data * data = ATTACHMENT(key);

    size_t writeLimit;
    ssize_t writeCount;
    uint8_t* writeBuffer;

    writeBuffer = buffer_read_ptr(&data->wbStruct, &writeLimit);
    writeCount = send(data->fd, writeBuffer, writeLimit, MSG_NOSIGNAL);
    stats_update(writeCount,0,0);

    if (writeCount <= 0) {
        printf("error en write\n");
        return;
    }

    buffer_read_adv(&data->wbStruct, writeCount);
    selector_set_interest_key(key, OP_NOOP);
    printf("NO DEBERIA INTENTAR LEER MAS\n");

    selector_unregister_fd(key->s, key->fd);
}

void freeAllPop3(const unsigned state, struct selector_key * key){
    //hace todos los frees
    client_data * data = ATTACHMENT(key);
    if (data->username != NULL){
        free(data->username);
    }
    free(data->emailDeleted);
    parser_destroy(data->parser);
    if (data->emailptr != NULL){
        selector_unregister_fd(key->s, data->emailptr->email_fd);
    }
    free(data->emailptr);
    free(data);

    close(key->fd);
    key->data = NULL;
}

unsigned errorHandler(struct selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"-ERR\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return lastValidState;
}

// TODO: check if return states are correctly managed
unsigned readHandler(struct selector_key * key) {
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
            return UPDATE_STATE;
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
                case TRANSACTION_STATE:
                    retState =  check_commands(key, command_list_transaction, TRANSACTION_COMMAND_AMOUNT);
                    break;
                case AUTH_STATE:
                    retState =  check_commands(key, command_list_auth, AUTH_COMMAND_AMOUNT);
                    break;
                case UPDATE_STATE:
                    retState = check_commands(key, command_list_update, UPDATE_COMMAND_AMOUNT);
                    break;
            }
            if(strcmp(data->command.command, "RETR") == 0 && retState != ERROR_STATE){
                printf("entre al chequeo\n");
                data->retrRunning = 1;
                if(buffer_can_read(&data->wbStruct)){
                    selector_set_interest_key(key, OP_WRITE);
                }else{
                    selector_set_interest_key(key, OP_NOOP); 
                    selector_set_interest(key->s, data->emailptr->email_fd, OP_READ);
                    
                }

            }else{
                selector_set_interest_key(key, OP_WRITE);
            }

            parser_reset(data->parser);
            data->command.commandLen = 0;
            data->command.arg1Len = 0;
            data->command.arg2Len = 0;
            data->command.command[0] = '\0';
            data->command.arg1[0] = '\0';
            data->command.arg2[0] = '\0';

            
           

            if(retState == ERROR_STATE){
                printf("error state\n");
                printf("cambie el retstate a %d\n", lastValidState);
                retState = lastValidState;
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

//void greetingHandler(const unsigned state, struct selector_key *key){
//	client_data * data = ATTACHMENT(key);
//    char buf[] = {"+OK POP3 server ready\r\n"};
//    for (int i = 0; buf[i] != '\0'; i++){
//        if (buffer_can_write(&data->wbStruct)){
//            buffer_write(&data->wbStruct,buf[i]);
//        }
//    }
//    return ;
//}

unsigned writeHandler(struct selector_key *key){
      client_data *data = ATTACHMENT(key);

    size_t writeLimit;
    ssize_t writeCount;
    uint8_t *writeBuffer;

    // Get the write buffer and its limit
    writeBuffer = buffer_read_ptr(&data->wbStruct, &writeLimit);

    // Write data from the buffer to the socket
    writeCount = send(data->fd, writeBuffer, writeLimit, MSG_NOSIGNAL);
    stats_update(writeCount, 0, 0);

    if (writeCount <= 0) {
        printf("Error in write\n");
        return ERROR_STATE;
    }

    buffer_read_adv(&data->wbStruct, writeCount);

    // Check if RETR command is running and not yet completed
    if (data->retrRunning && !data->emailptr->done) {
        // Adjust the selector interests for email read operation
        selector_set_interest_key(key, OP_NOOP);
        selector_set_interest(key->s, data->emailptr->email_fd, OP_READ);
        return data->stm.current->state;
    } else if (data->retrRunning) {
        // Adjust the selector interests for RETR completion
        selector_set_interest_key(key, OP_READ);
        data->retrRunning = 0;
        return data->stm.current->state;
    }

    // Set interest to read if there is more data in the read buffer
    selector_set_interest_key(key, OP_READ);

    if (buffer_can_read(&data->rbStruct)) {
        // More data to be read, invoke readHandler
        return readHandler(key);
    }

    // Return the current state
    return data->stm.current->state;
}



