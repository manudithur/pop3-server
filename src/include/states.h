#ifndef _STATES_H
#define _STATES_H

enum pop3_states{
    AUTH_STATE =0,
    TRANSACTION_STATE,
    UPDATE_STATE,
    ERROR_STATE
};

enum mgmt_states{
    AUTH_MGMT =0,
    ACTIVE_MGMT,
    UPDATE_MGMT,
    ERROR_MGMT
};



#endif
