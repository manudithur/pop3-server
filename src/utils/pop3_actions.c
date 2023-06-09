// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pop3_actions.h"

#define STAT_BUF 256
#define LIST_BUF 400

unsigned user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = "+OK USER\r\n";
    if (validateUser(data->command.arg1) != VALID_CREDENTIALS || data->command.arg2[0] != '\0'){
        return ERROR_STATE;
    }
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    data->username=malloc(strlen((data->command.arg1))+1);
    strcpy(data->username,data->command.arg1);

     int emailCount = 0;
    DIR *dir;
    struct dirent *entry;

    char dirPath[PATH_MAX_LENGTH];
    snprintf(dirPath, PATH_MAX_LENGTH, "src/mail/");
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "%s/", data->username);
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "cur/");

    dir = opendir(dirPath);



    if (dir == NULL){
        printf("ERROR: Socket %d - %s mail directory does not exist\n", data->fd, data->username);
        return UPDATE_STATE;

    }
    else{
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if (entry->d_type == DT_REG) {  // Regular file
                emailCount++;
            }
        }

        closedir(dir);
    }

    data->emailDeleted = calloc(1, sizeof(bool) * (emailCount));  //inicializa todos los mails como no borrados
    data->emailCount = emailCount;

    return AUTH_STATE;
}

unsigned pass_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = "+OK PASS\r\n";
    if (data->username == NULL || validateUserCredentials(data->username, data->command.arg1) != VALID_CREDENTIALS || data->command.arg2[0] != '\0'){
        return ERROR_STATE;
    }
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
        
    }
    stats_log_user(data->username);
    printf("INFO: Socket %d - %s logged in\n", data->fd, data->username);
    return TRANSACTION_STATE;
}

unsigned stat_handler(selector_key *key){
    DIR* directory;
    client_data * data = ATTACHMENT(key);
    struct dirent* entry;
    struct stat fileStat;
    int count = 0;
    int index = 0;
    long long int totalSize = 0;
    char dirPath[PATH_MAX_LENGTH];
    snprintf(dirPath, PATH_MAX_LENGTH, "src/mail/");
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "%s/", data->username);
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "cur/");
    char resultBuffer[STAT_BUF];

    directory = opendir(dirPath);
    if (directory == NULL) {
        return ERROR_STATE;
    }
    size_t maxPathLength = PATH_MAX_LENGTH + sizeof(entry->d_name);
    while ((entry = readdir(directory)) != NULL) {
        char filePath[maxPathLength];
        snprintf(filePath, maxPathLength, "%s/%s", dirPath, entry->d_name);
        if (stat(filePath, &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            if (data->emailDeleted[index++] == false){
                count++;
                totalSize += fileStat.st_size;
            }
            }
        }
    }

    closedir(directory);

    snprintf(resultBuffer, sizeof(resultBuffer), "+OK %d %lld\r\n", count, totalSize);
    for(size_t i = 0; i < strlen(resultBuffer); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,resultBuffer[i]);
        }
    }
    return data->stm.current->state;
}

unsigned list_handler(selector_key *key){
    DIR* directory;
    client_data * data = ATTACHMENT(key);

    int n = 0;
    if (data->command.arg1[0] != '\0')
        n = atoi(data->command.arg1);

    if (data->command.arg2[0] != '\0' || (data->command.arg1[0] != '\0' && (isNumber(data->command.arg1) == false || n > data->emailCount || n <= 0))){
        return ERROR_STATE;
    }

    struct dirent* entry;
    struct stat fileStat;
    int index = 0;
    char dirPath[PATH_MAX_LENGTH];
    snprintf(dirPath, PATH_MAX_LENGTH, "src/mail/");
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "%s/", data->username);
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "cur/");

    char resultBuffer[LIST_BUF];

    directory = opendir(dirPath);
    if (directory == NULL) {
        return ERROR_STATE;
    }

    size_t maxPathLength = PATH_MAX_LENGTH + sizeof(entry->d_name);


    if (n == 0){
        snprintf(resultBuffer, sizeof(resultBuffer), "+OK LIST\r\n");
        while ((entry = readdir(directory)) != NULL) {
            char filePath[maxPathLength];
            snprintf(filePath, maxPathLength, "%s/%s", dirPath, entry->d_name);
            if (stat(filePath, &fileStat) == 0) {
                if (S_ISREG(fileStat.st_mode)) {
                    if (data->emailDeleted[index++] == false){
                        snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer), "%d %ld\r\n", index, fileStat.st_size);
                    }
                }
            }
        }
        snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer), ".\r\n");
    }
    else{
        while ((entry = readdir(directory)) != NULL) {
            char filePath[maxPathLength];
            snprintf(filePath, maxPathLength, "%s/%s", dirPath, entry->d_name);
            if (stat(filePath, &fileStat) == 0) {
                if (S_ISREG(fileStat.st_mode)) {
                    if (index == n-1 && data->emailDeleted[index] == false){
                        snprintf(resultBuffer, sizeof(resultBuffer), "+OK %d %ld\r\n", n, fileStat.st_size);
                        break;
                    }
                    else if (index == n-1 && data->emailDeleted[index] == true){
                        return ERROR_STATE;
                    }
                    index++;
                }
            }
        }
    }
    closedir(directory);

    for(size_t i = 0; i < strlen(resultBuffer); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,resultBuffer[i]);
        }
    }
    return data->stm.current->state;
}

void close_mail_handler(selector_key * key){
}

void write_mail_handler(selector_key * key){

}

void block_mail_handler(selector_key * key){
}

fd_handler mail_handler ={
    .handle_read = read_mail_handler,
    .handle_close = close_mail_handler,
    .handle_write = NULL, //write_mail_handler, //Vamos a probar null primero y que use el otro
    .handle_block = block_mail_handler
};



void read_mail_handler(struct selector_key *key){
    
    email * email_data = ((email *)(key)->data);
    buffer * mail_buffer = &email_data->bStruct;
    
    size_t readLimit;
    ssize_t readCount;    
    uint8_t * readBuffer;
    readBuffer = buffer_write_ptr(mail_buffer, &readLimit);

    readCount = read(email_data->email_fd, readBuffer, readLimit);


    if (readCount <= 0){
        buffer_write(email_data->pStruct, '\r');
        buffer_write(email_data->pStruct, '\n');
        buffer_write(email_data->pStruct, '.');
        buffer_write(email_data->pStruct, '\r');
        buffer_write(email_data->pStruct, '\n');
        email_data->done = 1;
        selector_set_interest(key->s, email_data->parent_fd, OP_WRITE);
        selector_unregister_fd(key->s, email_data->email_fd);
        printf("INFO: Socket %d - finished reading email\n", email_data->email_fd);
        return;
    }
    buffer_write_adv(&email_data->bStruct, readCount);

    //deberia preguntar si el otro puede escribir tambien?
    while(buffer_can_read(mail_buffer) && buffer_can_write(email_data->pStruct)){
        readBuffer = buffer_read_ptr(mail_buffer, &readLimit);
        switch(email_data->stuffing){
            case 0:
                if (*readBuffer == '\r'){
                    email_data->stuffing = 1;
                }
                break;
            case 1:
                if (*readBuffer == '\n'){
                    email_data->stuffing = 2;
                }else{
                    email_data->stuffing = 0;
                }
                break;
            case 2:
                if (*readBuffer == '.'){
                    email_data->stuffing = 3;
                }else{
                    email_data->stuffing = 0;
                }
                break;
        }
        if (email_data->stuffing == 3){
            buffer_write(email_data->pStruct, '.');
            email_data->stuffing = 0;
        }else{
        buffer_write(email_data->pStruct, *readBuffer); //escribo lo que lei en el buffer del padre
        buffer_read_adv(mail_buffer, 1);
        }
    }

    //SOLO SI TERMINE DE ESCRIBIR EL MAIL, O SI NO HAY MAS LUGAR EN EL BUFFER DEL PADRE
    selector_set_interest_key(key, OP_NOOP);
    selector_set_interest(key->s, email_data->parent_fd, OP_WRITE);
}

bool isNumber(const char* str) {
    if (str == NULL || *str == '\0') {
        return false;
    }
    while (*str != '\0' && *str >= '0' && *str <= '9') {
        ++str;
    }
    while (*str != '\0') {
        if (*str != ' ') {
            return false;
        }
        ++str;
    }
    return true;
}

unsigned retr_handler(selector_key *key) {
    //TODO:primero deberia fijarme si estan bien los argumentos, si existe el mail etc.
    client_data *data = ATTACHMENT(key);
    struct stat fileStat;
    long long int totalSize = 0;
    char dirPath[PATH_MAX_LENGTH];
    snprintf(dirPath, PATH_MAX_LENGTH, "src/mail/");
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "%s/", data->username);
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "cur/");
    if (data->command.arg2[0] != '\0' || isNumber(data->command.arg1) == false) {
        return ERROR_STATE;
    }
    int targetFileIndex = atoi(data->command.arg1) -1;
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        return ERROR_STATE;
    }
     struct dirent *entry;
    int index = 0;
    size_t maxPathLength = PATH_MAX_LENGTH + sizeof(entry->d_name);
    char filePath[maxPathLength];
    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (index == targetFileIndex) {
            if (data->emailDeleted[index] == false) {
                // Found the desired file
                snprintf(filePath, maxPathLength, "%s%s", dirPath, entry->d_name);
                index++;
                break;
            } else if (data->emailDeleted[index] == true) {
                return ERROR_STATE;
            }
        }
        index++;
        stats_update(0, 0, 1);
    }
    if (index <= targetFileIndex) {
        return ERROR_STATE;
    }

    closedir(dir);
    email * email_data = malloc(sizeof(email));
    email_data->email_fd = open(filePath, O_RDONLY);
    email_data->parent_fd = data->fd;
    email_data->done = 0;
    email_data->stuffing=0;
    email_data->pStruct = &data->wbStruct;
    if (stat(filePath, &fileStat) == 0) {  
            if (S_ISREG(fileStat.st_mode)) {
                totalSize=fileStat.st_size;
            }
    }
    char aux[50] = {0};
    snprintf(aux, sizeof(aux), "+OK %lld octets\r\n", totalSize);
    for(size_t i = 0; i < strlen(aux); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,aux[i]);
        }
    }

    free(data->emailptr);
    data->emailptr = email_data;
    buffer_init(&email_data->bStruct, BUFFER_LEN, email_data->buffer);

    selector_register(key->s, email_data->email_fd, &mail_handler, OP_NOOP, email_data);

    //como llamo al read_mail_handler?? => calculo que se llama solo porque el fd va a estar llenisimo por el mail
    return data->stm.current->state;

}



unsigned dele_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK MESSAGE DELETED\r\n"};
    int n = atoi(data->command.arg1) -1;
    if (data->command.arg2[0] != '\0' || isNumber(data->command.arg1) == false || n > data->emailCount - 1 || n < 0){
        return ERROR_STATE;
    }
    for (int i = 0; i < data->emailCount; i++)
    {
        if (n == i && data->emailDeleted[i] == false){
            data->emailDeleted[i] = true;
            printf("INFO: Socket %d - email %d deleted\n", data->fd, n+1);
            break;
        }
        else if (n == i && data->emailDeleted[i] == true){
            printf("INFO: Socket %d - email %d already deleted\n", data->fd, n+1);
            return ERROR_STATE;
        }
    }
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return TRANSACTION_STATE;
}

unsigned rset_handler(selector_key *key){
    //reset the SMTP connection to the initial state in which the sender
    // and recipient buffers are erased and the connection is ready to begin a new mail transaction.
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK RESET STATE\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }

    for (int i = 0; i < data->emailCount; i++){
        data->emailDeleted[i] = false;
        printf("INFO: Socket %d - deleted emails restored\n", data->fd);
    }

    return TRANSACTION_STATE;
}

unsigned noop_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK NOOP\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;
}

//Deberia devolver el estado AUTH no?
unsigned quit_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK GOODBYE\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    stats_remove_connection();
    return UPDATE_STATE;
}

unsigned capa_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK CAPA\r\nUSER\r\nPASS\r\nQUIT\r\nCAPA\r\nLIST\r\nRETR\r\nSTAT\r\nDELE\r\nNOOP\r\nRSET\r\n.\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return data->stm.current->state;    
}

