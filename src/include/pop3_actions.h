#ifndef POP3_ACTIONS_H_
#define POP3_ACTIONS_H_

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

#define BUFFER_LENGTH 10
#define BUFFER 5000
#define DEFAULT_MAIL_COUNT 3
#define PATH_MAX_LENGTH 300

typedef struct email{
    int email_fd;
    int parent_fd;
    struct buffer bStruct;
    uint8_t buffer[BUFFER];
    struct buffer * pStruct; //acceso al buffer del padre
    int stuffing;
    int done;
}email;

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

bool isNumber(const char* str);
void close_mail_handler(selector_key * key);

void write_mail_handler(selector_key * key);

void block_mail_handler(selector_key * key);

void read_mail_handler(selector_key * key);

#endif

