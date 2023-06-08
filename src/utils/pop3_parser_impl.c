#include "parser.h"

enum command_state{
    PARSE_COMMAND,
    PARSE_ARG1,
    PARSE_ARG2,
    DONE
};


static void readCommand(struct parser_event *ret, const uint8_t c){
    ret->type    = KEEP_READING;
    command_data->command[command_data->command_len] = c;
}

static void readArg1(struct parser_event *ret, const uint8_t c){

}

static void readArg2(struct parser_event *ret, const uint8_t c){

}

static void deliver(struct parser_event *ret, const uint8_t c){

}

static void invalidParameters(struct parser_event *ret, const uint8_t c){

}



static const struct parser_state_transition ST_COMMAND [] =  {
    {.when = ' ',        .dest = PARSE_ARG1,        .act1 = /*TODO*/,},
    {.when = '\r',        .dest = DONE,        .act1 =  /*TODO*/,},
    {.when = ANY,        .dest = PARSE_COMMAND,        .act1 =  /*TODO*/,},
};

static const struct parser_state_transition ST_ARG1 [] =  {
    {.when = ' ',        .dest = PARSE_ARG2,        .act1 = /*TODO*/,},
    {.when = '\r',        .dest = DONE,        .act1 =  /*TODO*/,},
    {.when = ANY,        .dest = PARSE_ARG1,        .act1 =  /*TODO*/,},
};

static const struct parser_state_transition ST_ARG2 [] =  {
    {.when = '\r',        .dest = DONE,        .act1 =  /*TODO*/,},
    {.when = ANY,        .dest = PARSE_ARG2,        .act1 =  /*TODO*/,},
};

static const struct parser_state_transition ST_DONE [] =  {
    {.when = "\n",        .dest = DONE,        .act1 =  /*TODO devuelvo todo para ver si es comando*/,},
    {.when = ANY,        .dest = PARSE_COMMAND,        .act1 =  /*TODO ERROR EN EL COMANDO*/,},
};

static const struct parser_state_transition *states [] = {
    ST_COMMAND,
    ST_ARG1,
    ST_ARG2
};

#define N(x) (sizeof(x)/sizeof((x)[0]))

static const size_t states_n [] = {
    N(ST_COMMAND),
    N(ST_ARG1),
    N(ST_ARG2)
};

static struct parser_definition definition = {
    .states_count = N(states),
    .states       = states,
    .states_n     = states_n,
    .start_state  = PARSE_COMMAND
};
