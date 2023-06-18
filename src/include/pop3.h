#ifndef POP3_SERVER_POP3_H
#define POP3_SERVER_POP3_H

#include "tcp_server_utils.h"
#include <stdio.h>
#include <sys/socket.h>
#include "parser.h"
#include "pop3_parser_impl.h"
#include "pop3_actions.h"
#include "stats.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define COMMAND_AMOUNT 10
#define TRANSACTION_COMMAND_AMOUNT 8
#define AUTH_COMMAND_AMOUNT 4
#define UPDATE_COMMAND_AMOUNT 1

unsigned readHandler(struct selector_key * key);
unsigned writeHandler(struct selector_key *key);
unsigned errorHandler(struct selector_key *key);
void freeAllPop3(const unsigned state, struct selector_key * key);
void unregisterHandler(struct selector_key * key);
void mailDeleter(const unsigned state,struct selector_key * key);



#endif //POP3_SERVER_POP3_H
