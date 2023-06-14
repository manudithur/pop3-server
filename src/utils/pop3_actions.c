#include "pop3_actions.h"


#define PATH_MAX 300

unsigned user_handler(selector_key *key){
    client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK USER\r\n"};
    data->username=malloc(strlen((data->command.arg1))+1);
    strcpy(data->username,data->command.arg1);
    for (int i = 0; buf[i] != '\0'; i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,buf[i]);
        }
    }
    return AUTH_STATE;

}

unsigned pass_handler(selector_key *key){
      client_data * data = ATTACHMENT(key);
    char buf[] = {"+OK PASS\r\n"};
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
    long long int totalSize = 0;
    const char *path = "src/mail_test";
    char resultBuffer[256];

    directory = opendir(path);
    if (directory == NULL) {
        printf("ERROR - unable to open mailbox");
        return ERROR_STATE;
    }
    while ((entry = readdir(directory)) != NULL) {
        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", path, entry->d_name);
        if (stat(filePath, &fileStat) == 0) {
        if (S_ISREG(fileStat.st_mode)) {
            count++;
            totalSize += fileStat.st_size;
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
    const char *path = "src/mail_test";
    char resultBuffer[256];

    directory = opendir(path);
    if (directory == NULL) {
        printf("ERROR - unable to open mailbox\n");
        return ERROR_STATE;
    }
    snprintf(resultBuffer, sizeof(resultBuffer), "+OK LIST\n");
    while ((entry = readdir(directory)) != NULL) {
        char filePath[PATH_MAX];
        snprintf(filePath, PATH_MAX, "%s/%s", path, entry->d_name);
        if (stat(filePath, &fileStat) == 0) {
            if (S_ISREG(fileStat.st_mode)) {
                //printf("File %d size: %lld\n", count, fileStat.st_size);
                count++;
                 snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer),
                         "%d %lld\n", count, fileStat.st_size);
            }
        }
    }

    closedir(directory);
    snprintf(resultBuffer + strlen(resultBuffer), sizeof(resultBuffer), "\r\n");

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
    .handle_write = NULL, //Vamos a probar null primero y que use el otro
    .handle_block = block_mail_handler
};



void read_mail_handler(struct selector_key *key){
    
    email * email_data = ((email *)(key)->data);
    
    buffer * mail_buffer = &email_data->bStruct;
    //selector_set_interest(key->s, email_data->parent_fd, OP_READ);
    
    size_t readLimit;
    ssize_t readCount;    
    uint8_t * readBuffer;
    readBuffer = buffer_write_ptr(mail_buffer, &readLimit);

    readCount = read(email_data->email_fd, readBuffer, readLimit);
    buffer_write_adv(&email_data->bStruct, readCount);


    if (readCount <= 0){
    //    return -1;
        return;
    }
    printf("ANTES\n");
    
    //deberia preguntar si el otro puede escribir tambien?
    while(buffer_can_read(mail_buffer)){
        readBuffer=buffer_read_ptr(mail_buffer, &readLimit);
        buffer_write(email_data->pStruct, *readBuffer); //escribo lo que lei en el buffer del padre
        buffer_read_adv(mail_buffer, 1);
        printf("LEI: %c\n", *readBuffer);
    }
    buffer_write(email_data->pStruct, '\r');
    buffer_write(email_data->pStruct, '\n');
    printf("2\n");
    //SOLO SI TERMINE DE ESCRIBIR EL MAIL, O SI NO HAY MAS LUGAR EN EL BUFFER DEL PADRE
    //selector_set_interest_key(key, OP_NOOP);
    
    selector_set_interest(key->s, email_data->parent_fd, OP_WRITE);

   // return TRANSACTION_STATE;
}


unsigned retr_handler(selector_key *key){
    //TODO:primero deberia fijarme si estan bien los argumentos, si existe el mail etc.
    client_data * data = ATTACHMENT(key);
    struct stat fileStat;
    long long int totalSize = 0;
    char * filePath= "src/mail_test/mail1";
    //printf("retr handler\n");
    size_t writeLimit;
    email * email_data = malloc(sizeof(email));
    email_data->email_fd = open(filePath, O_RDONLY);
    email_data->parent_fd = data->fd;
    email_data->pStruct = &data->wbStruct;
    if (stat(filePath, &fileStat) == 0) {  
            if (S_ISREG(fileStat.st_mode)) {
                totalSize=fileStat.st_size;
            }
    }
    char aux[50] = {0};
    uint8_t * writeBuffer = buffer_write_ptr(&data->wbStruct, &writeLimit );
    snprintf(aux, sizeof(aux), "+OK %lld octets\n", totalSize);
    for(int i = 0; i < strlen(aux); i++){
        if (buffer_can_write(&data->wbStruct)){
            buffer_write(&data->wbStruct,aux[i]);
        }
    }
    buffer_write_adv(&data->wbStruct, strlen(aux));


    data->emailptr = email_data;
    buffer_init(&email_data->bStruct, BUFFER_LEN, email_data->buffer);
    printf("llegue1\n");
    
    selector_register(key->s, email_data->email_fd, &mail_handler, OP_READ, email_data);

    //como llamo al read_mail_handler?? => calculo que se llama solo porque el fd va a estar llenisimo por el mail
    return data->stm.current->state;

}



unsigned dele_handler(selector_key *key){

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
    data->username = NULL;  //Devuelvo al usuario al momento 0 de la aplicacion. No puso su usuario y esta en authorization state.

    DIR* directory;
    struct dirent* file;

    char * srcDir = "src/mail/trash"; //TODO: poner paths de mail y trash
    char * destDir = "src/mail";

    directory = opendir(srcDir);

    // Iterate over files in the source directory
    while ((file = readdir(directory)) != NULL) {
        // Ignore "." and ".." entries
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }
        char srcPath[256];
        char destPath[256];
        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, file->d_name);
        snprintf(destPath, sizeof(destPath), "%s/%s", destDir, file->d_name);

        // Move the file
        if (rename(srcPath, destPath) != 0) {
            printf("Failed to move file: %s\n", srcPath);
        }
    }

    closedir(directory);

    return AUTH_STATE;
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

    const char* directory_path = "src/mail/trash"; // TODO:Poner el path del trash

    DIR* directory = opendir(directory_path);
    struct dirent* file;

    while ((file = readdir(directory)) != NULL) {
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
            char file_path[100];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory_path, file->d_name);
            remove(file_path);
        }
    }

    closedir(directory);
    //finishConnection();
    return data->stm.current->state;
}

unsigned capa_handler(selector_key *key){
    
}

