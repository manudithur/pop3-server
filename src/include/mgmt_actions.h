#ifndef MGMT_H
#define MGMT_H

#include "pop3_parser_impl.h"
#include "pop3.h"
#include "tcp_server_utils.h"
#include "selector.h"
#include "buffer.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "users.h"
#include "stats.h"  

unsigned mgmt_user_handler(selector_key *key);

unsigned mgmt_pass_handler(selector_key *key);

unsigned mgmt_noop_handler(selector_key *key);

unsigned mgmt_historic_handler(selector_key *key);

unsigned mgmt_live_handler(selector_key *key);

unsigned mgmt_bytes_handler(selector_key *key);

unsigned mgmt_users_handler(selector_key *key);

unsigned mgmt_status_handler(selector_key *key);

unsigned mgmt_max_users_handler(selector_key *key);

unsigned mgmt_max_connections_handler(selector_key *key);

unsigned mgmt_timeout_handler(selector_key *key);

unsigned mgmt_delete_user_handler(selector_key *key);

unsigned mgmt_add_user_handler(selector_key *key);

unsigned mgmt_reset_user_password_handler(selector_key *key);

unsigned mgmt_change_password_handler(selector_key *key);

//Deberia devolver el estado AUTH no?
unsigned mgmt_quit_handler(selector_key *key);

unsigned mgmt_capa_handler(selector_key *key);

void applyTimeout(int time);

#endif
