//
// Created by Gayba on 6/7/2023.
//

#ifndef POP3_SERVER_POP3_H
#define POP3_SERVER_POP3_H

#include "tcp_server_utils.h"
#include <stdio.h>
#include <sys/socket.h>
#include "../include/parser.h"
#include "parser.h"

#define TRANSACTION_COMMAND_AMOUNT 8
#define AUTH_COMMAND_AMOUNT 4
#define UPDATE_COMMAND_AMOUNT 1

unsigned readHandler(struct selector_key * key);

//unsigned writeHandler(struct selector_key *key);

#endif //POP3_SERVER_POP3_H
