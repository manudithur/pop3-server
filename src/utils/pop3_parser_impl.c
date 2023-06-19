// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pop3_parser_impl.h"
#include <stdio.h>


static void readCommand(struct parser_event *ret, const uint8_t c){
    ret->type    = PARSE_COMMAND;
    ret->n       = 1;
    ret->data[0] = c;
}

static void readArg1(struct parser_event *ret, const uint8_t c){
    ret->type    = PARSE_ARG1;
    ret->n       = 1;
    ret->data[0] = c;
}

static void readArg2(struct parser_event *ret, const uint8_t c){
    ret->type    = PARSE_ARG2;
    ret->n       = 1;
    ret->data[0] = c;
}

static void deliver(struct parser_event *ret, const uint8_t c){
    ret->type = DONE;
}

static void invalidParameters(struct parser_event *ret, const uint8_t c){
    ret->type = INVALID;
    ret->n = 1;
    ret->data[0] = c;
}

static void almostDone(struct parser_event *ret, const uint8_t c){
  ret->type = ALMOST_DONE;
}



static const struct parser_state_transition ST_COMMAND [] =  {
    {.when = ' ',        .dest = PARSE_ARG1,        .act1 = almostDone},
    {.when = '\r',        .dest = ALMOST_DONE,        .act1 =  almostDone},
    {.when = ANY,        .dest = PARSE_COMMAND,        .act1 =  readCommand},
};

static const struct parser_state_transition ST_ARG1 [] =  {
    {.when = ' ',        .dest = PARSE_ARG2,        .act1 = almostDone},
    {.when = '\r',        .dest = ALMOST_DONE,        .act1 =  almostDone},
    {.when = ANY,        .dest = PARSE_ARG1,        .act1 =  readArg1},
};

static const struct parser_state_transition ST_ARG2 [] =  {
    {.when = '\r',        .dest = ALMOST_DONE,        .act1 =  almostDone},
    {.when = ANY,        .dest = PARSE_ARG2,        .act1 =  readArg2},
};

static const struct parser_state_transition ST_DONE [] =  {
    {.when = '\n',        .dest = DONE,        .act1 =  deliver},
    {.when = ANY,        .dest = PARSE_COMMAND,        .act1 =  invalidParameters},
};

static const struct parser_state_transition *states [] = {
    ST_COMMAND,
    ST_ARG1,
    ST_ARG2,
    ST_DONE
};

#define N(x) (sizeof(x)/sizeof((x)[0]))

static const size_t states_n [] = {
    N(ST_COMMAND),
    N(ST_ARG1),
    N(ST_ARG2),
    N(ST_DONE)
};

const struct parser_definition definition = {
    .states_count = N(states),
    .states       = states,
    .states_n     = states_n,
    .start_state  = PARSE_COMMAND
};
