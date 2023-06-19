// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "pop3_actions.h"

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

    const char * defaultMail1Path = "src/mail/defaults/default_mail1";
    const char * defaultMail2Path = "src/mail/defaults/default_mail2";
    const char * defaultMail3Path = "src/mail/defaults/default_mail3";

    char destinationFilePath[300];

    if (dir == NULL){
        char newDirPath[PATH_MAX_LENGTH] = {0};
        snprintf(newDirPath, PATH_MAX_LENGTH, "src/mail/%s",data->username);
        mkdir(newDirPath, 0777);
        mkdir(dirPath, 0777);

        const char * sourceFiles[DEFAULT_MAIL_COUNT] = {defaultMail1Path, defaultMail2Path, defaultMail3Path};
        const char * destinationFiles[DEFAULT_MAIL_COUNT] = {"default_mail1", "default_mail2", "default_mail3"};
        emailCount = 3;
        for (int i = 0; i < DEFAULT_MAIL_COUNT ; i++){
            FILE* sourceFile = fopen(sourceFiles[i], "rb");

            snprintf(destinationFilePath, sizeof(destinationFilePath), "%s%s", dirPath, destinationFiles[i]);
            FILE* destinationFile = fopen(destinationFilePath, "wb");

            int c;
            while ((c = fgetc(sourceFile)) != EOF) {
                fputc(c, destinationFile);
            }

            fclose(sourceFile);
            fclose(destinationFile);
        }



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
    char resultBuffer[256];

    directory = opendir(dirPath);
    if (directory == NULL) {
        return ERROR_STATE;
    }
    while ((entry = readdir(directory)) != NULL) {
        char filePath[PATH_MAX_LENGTH];
        snprintf(filePath, PATH_MAX_LENGTH, "%s/%s", dirPath, entry->d_name);
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
    for(int i = 0; i < strlen(resultBuffer); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,resultBuffer[i]);
        }
    }
    return data->stm.current->state;
}

unsigned list_handler(selector_key *key){
    DIR* directory;
    client_data * data = ATTACHMENT(key);
    struct dirent* entry;
    struct stat fileStat;
    long long int totalSize = 0;
    int count = 0;
    int index = 0;
    char dirPath[PATH_MAX_LENGTH];
    snprintf(dirPath, PATH_MAX_LENGTH, "src/mail/");
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "%s/", data->username);
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "cur/");

    //Decision de diseño: buffer de 5000 caracteres
    char resultBuffer[400];

    directory = opendir(dirPath);
    if (directory == NULL) {
        return ERROR_STATE;
    }

    snprintf(resultBuffer, sizeof(resultBuffer), "+OK LIST\n");
    while ((entry = readdir(directory)) != NULL) {
        char filePath[PATH_MAX_LENGTH];
        snprintf(filePath, PATH_MAX_LENGTH, "%s/%s", dirPath, entry->d_name);
        if (stat(filePath, &fileStat) == 0) {
            if (S_ISREG(fileStat.st_mode)) {
                if (data->emailDeleted[index++] == false){
                    count++;
                    snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer), "%d %lld\n", count, fileStat.st_size);
                }

            }
        }
    }
    snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer), "\r\n");
    closedir(directory);
//    snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer), "\r\n");

    for(int i = 0; i < strlen(resultBuffer); i++){
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
    buffer_write_adv(&email_data->bStruct, readCount);


    if (readCount <= 0){
        printf("llegue\n");
        buffer_write(email_data->pStruct, '\r');
        buffer_write(email_data->pStruct, '\n');
        buffer_write(email_data->pStruct, '.');
        buffer_write(email_data->pStruct, '\r');
        buffer_write(email_data->pStruct, '\n');
        email_data->done = 1;
        selector_set_interest(key->s, email_data->parent_fd, OP_WRITE);
        selector_unregister_fd(key->s, email_data->email_fd);
        return;
    }

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

unsigned retr_handler(selector_key *key){
    //TODO:primero deberia fijarme si estan bien los argumentos, si existe el mail etc.
    client_data * data = ATTACHMENT(key);
    struct stat fileStat;
    long long int totalSize = 0;
    char dirPath[PATH_MAX_LENGTH];
    snprintf(dirPath, PATH_MAX_LENGTH, "src/mail/");
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "%s/", data->username);
    snprintf(dirPath + strlen(dirPath), PATH_MAX_LENGTH, "cur/");
    if (data->command.arg2[0] != '\0' || isNumber(data->command.arg1) == false){
        return ERROR_STATE;
    }
    int targetFileIndex = atoi(data->command.arg1) -1;
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        return ERROR_STATE;
    }
     struct dirent *entry;
    int fileCount = 0;
    int index = 0;
    char filePath[PATH_MAX_LENGTH];
    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".." directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (data->emailDeleted[index++] == false){
            if (fileCount == targetFileIndex) {
                // Found the desired file

                snprintf(filePath, PATH_MAX_LENGTH, "%s%s", dirPath, entry->d_name);
                fileCount++;
                break;
            }

            fileCount++;
        }
    }
    stats_update(0,0,1);
    if (fileCount <= targetFileIndex) { 
        return ERROR_STATE;
    }

    closedir(dir);
    //printf("retr handler\n");
    size_t writeLimit;
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
    uint8_t * writeBuffer = buffer_write_ptr(&data->wbStruct, &writeLimit );
    snprintf(aux, sizeof(aux), "+OK %lld octets\r\n", totalSize);
    for(int i = 0; i < strlen(aux); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,aux[i]);
        }
    }

    data->emailptr = email_data;
    buffer_init(&email_data->bStruct, BUFFER_LEN, email_data->buffer);

    selector_register(key->s, email_data->email_fd, &mail_handler, OP_NOOP, email_data);

    //como llamo al read_mail_handler?? => calculo que se llama solo porque el fd va a estar llenisimo por el mail
    return data->stm.current->state;

}



unsigned dele_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK MESSAGE DELETED\r\n.\r\n"};
    int n = atoi(data->command.arg1) -1;
    if (data->command.arg2[0] != '\0' || isNumber(data->command.arg1) == false || n > data->emailCount - 1 || n < 0){
        return ERROR_STATE;
    }
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    DIR *dir;
    struct dirent *entry;
    int index = 0;
    for (int i = 0; i < data->emailCount; i++)
    {
        if (data->emailDeleted[i] == false){
            if (n == index){
                data->emailDeleted[i] = true;
                break;
            }
            index++;
        }
    }
    return TRANSACTION_STATE;
}

unsigned rset_handler(selector_key *key){
    //reset the SMTP connection to the initial state in which the sender
    // and recipient buffers are erased and the connection is ready to begin a new mail transaction.
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK RESET STATE\r\n.\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }

    for (int i = 0; i < data->emailCount; i++){
        data->emailDeleted[i] = false;
    }

    return TRANSACTION_STATE;
}

unsigned noop_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK NOOP\r\n.\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    stats_print();
    return data->stm.current->state;
}

//Deberia devolver el estado AUTH no?
unsigned quit_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK GOODBYE\r\n.\r\n"};
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
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

