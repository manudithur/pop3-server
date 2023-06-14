#ifndef POP3_ACTIONS_H_
#define POP3_ACTIONS_H_

#include "pop3_parser_impl.h"
#include "pop3.h"
#include "tcp_server_utils.h"
#include "selector.h"
#include "buffer.h"
#include <dirent.h>
#define BUFFER_LENGTH 10

unsigned user_handler(selector_key *key);
unsigned pass_handler(selector_key *key);
unsigned stat_handler(selector_key *key);
unsigned list_handler(selector_key *key);
unsigned retr_handler(selector_key *key);
unsigned dele_handler(selector_key *key);
unsigned rset_handler(selector_key *key);
unsigned noop_handler(selector_key *key);
unsigned quit_handler(selector_key *key);
unsigned capa_handler(selector_key *key);


void close_mail_handler(selector_key * key);

void write_mail_handler(selector_key * key);

void block_mail_handler(selector_key * key);

void read_mail_handler(selector_key * key);

#endif

