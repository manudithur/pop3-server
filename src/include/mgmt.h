#ifndef _MGMT_H_
#define _MGMT_H_

#include "tcp_server_utils.h"
#include <stdio.h>
#include <sys/socket.h>
#include "parser.h"
#include "pop3_parser_impl.h"
#include "mgmt_actions.h"
#include "stats.h"
#include "states.h"

#define ACTIVE_MGMT_COMMAND_AMOUNT 15
#define MGMT_AUTH_COMMAND_AMOUNT 4

unsigned mgmt_readHandler(struct selector_key * key);
unsigned mgmt_writeHandler(struct selector_key *key);
unsigned mgmt_errorHandler(struct selector_key *key);



#endif
