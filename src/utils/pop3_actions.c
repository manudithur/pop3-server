#include "pop3_actions.h"

unsigned user_handler(selector_key *key){

}

unsigned pass_handler(selector_key *key){

}

unsigned stat_handler(selector_key *key){

}

unsigned list_handler(selector_key *key){

}

unsigned retr_handler(selector_key *key){

}

unsigned dele_handler(selector_key *key){

}

unsigned rset_handler(selector_key *key){
    //reset the SMTP connection to the initial state in which the sender
    // and recipient buffers are erased and the connection is ready to begin a new mail transaction.
}

unsigned noop_handler(selector_key *key){
    //Does nothing, always has a succesfull outcome, can't affect state.
}

unsigned quit_handler(selector_key *key){

}

unsigned capa_handler(selector_key *key){
    
}

