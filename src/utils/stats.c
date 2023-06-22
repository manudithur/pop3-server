// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stats.h"


static sv_stats * stats = NULL;


void stats_init(){
    if(stats != NULL) return;
    printf("INFO: Creating logs file\n");
    remove("logs");
    stats = malloc(sizeof(sv_stats));
    stats->totalBytesSent = 0;
    stats->totalBytesReceived = 0;
    stats->totalMailsSent = 0;
    stats->totalConnections = 0;
    stats->concurrentConnections = 0;
    return;

} 

//Volatil, en un futuro se pueden guardar en un archivo
void stats_update( size_t bytesSent, size_t bytesReceived, size_t mailsSent){
    if(stats == NULL) return;
    stats->totalBytesSent += bytesSent;
    stats->totalBytesReceived += bytesReceived;
    stats->totalMailsSent += mailsSent;

}
void stats_add_connection(){
    if(stats == NULL) return;
    stats->concurrentConnections++;
    stats->totalConnections++;
    printf("INFO: Concurrent Connections: %ld\n", stats->concurrentConnections);

}

void stats_destroy(){
    if(stats == NULL) return;
    free(stats);
}

void stats_remove_connection(){
    if(stats == NULL) return;
    stats->concurrentConnections--;
    printf("INFO: Concurrent Connections: %ld\n", stats->concurrentConnections);
}

void stats_log_connection(const char* ip){
    
    FILE* file = fopen("logs", "a");  // Open the file in append mode

    if (file == NULL) {
        return;
    }

    fprintf(file, "-\"%s\" has connected to the server\n", ip);
    fclose(file);
}

void stats_log_disconnection(const char* ip){
    
    FILE* file = fopen("logs", "a");  // Open the file in append mode

    if (file == NULL) {
        printf("ERROR: Error opening logs file\n");
        return;
    }

    fprintf(file, "-\"%s\" has disconnected from the server\n", ip);
    fclose(file);
}

void stats_log_user(const char* user){
    
    FILE* file = fopen("logs", "a");  // Open the file in append mode

    if (file == NULL) {
        return;
    }

    fprintf(file, "-%s has logged in\n", user);
    fclose(file);
}

void stats_log_user_logout(const char* user){
    
    FILE* file = fopen("logs", "a");  // Open the file in append mode

    if (file == NULL) {
        printf("ERROR: Error opening logs file\n");
        return;
    }

    fprintf(file, "-%s has logged out\n", user);
    fclose(file);
}

size_t getTotalConnections(){
    if(stats == NULL) return 0;
    return stats->totalConnections;
}

size_t getTotalBytesSent(){
    if(stats == NULL) return 0;
    return stats->totalBytesSent;
}

size_t getTotalBytesReceived(){
    if(stats == NULL) return 0;
    return stats->totalBytesReceived;
}

size_t getConcurrentConnections(){
    if(stats == NULL) return 0;
    return stats->concurrentConnections;
}

void stats_print(){
    if(stats == NULL) return;
    printf("Total connections: %ld\n", stats->totalConnections);
    printf("Total bytes sent: %ld\n", stats->totalBytesSent);
    printf("Total bytes received: %ld\n", stats->totalBytesReceived);
    printf("Total mails retrieved: %ld\n", stats->totalMailsSent);
    printf("Concurrent connections: %ld\n", stats->concurrentConnections);
}

