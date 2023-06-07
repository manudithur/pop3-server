//
// Created by Gayba on 6/7/2023.
//

#include "../include/pop3.h"

unsigned readHandler(struct selector_key * key) {
    client_data * data = ATTACHMENT(key);

    size_t readLimit;
    ssize_t readCount;
    uint8_t* readBuffer;

    readBuffer = buffer_write_ptr(&data->rbStruct, &readLimit);
    readCount = recv(key->fd, readBuffer, readLimit, 0);

    if (readCount <= 0) {
        return -1;
    }

//    printf("%s", readBuffer);

    buffer_write_adv(&data->rbStruct, readCount);

    //Parse. If I need to still read -> return TRANSACTION_STATE? If not -> return UPDATE_STATE? No estoy seguro de los estados

    return TRANSACTION_STATE;
}


//unsigned writeHandler(struct selector_key *key){
//    client_data * data = ATTACHMENT(key);
//
//    size_t writeLimit;
//    ssize_t writeCount;
//    uint8_t* writeBuffer;
//
//    writeBuffer = buffer_read_ptr(&data->wbStruct, &writeLimit);
//    writeCount = send(data->fd, writeBuffer, writeLimit, MSG_NOSIGNAL);
//
//    if (writeCount <= 0) {
//        return -1;
//    }
//
//    buffer_read_adv(&data->wbStruct, writeCount);
//
//    //if I can read more from buffer -> return UPDATE_STATE? no estoy seguro, tiene que seguir escribiendo
//    return 0;
//}



