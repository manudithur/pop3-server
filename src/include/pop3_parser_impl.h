#ifndef POP3_PARSER_UTILS_H_
#define POP3_PARSER_UTILS_H_

#include "parser.h"
#include "tcp_server_utils.h"
enum command_state{
    PARSE_COMMAND,
    PARSE_ARG1,
    PARSE_ARG2,
    ALMOST_DONE,
    DONE,
    INVALID
};
#endif