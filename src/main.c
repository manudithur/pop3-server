#include <stdlib.h>
#include "parser.h"
#include "parser_utils.h"

int main(int argc, char * argv[]){
    //Defino un parser que valida "command1"
    struct parser_definition command1_parser = parser_utils_strcmpi("command1");

    //Inicializo el parser
    struct parser * command1_parser_instance = parser_init(parser_no_classes(), &command1_parser);


    char * command1 = "command1";

    //Le feedeo la palabla "command1" al parser
    const struct parser_event * event = parser_feed(command1_parser_instance, command1);

    //Imprimo recursivamente los eventos que nos devuelve el parser
    event_print(&event[0]);

    parser_utils_strcmpi_destroy(&command1_parser);    
}