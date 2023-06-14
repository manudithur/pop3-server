#include "stats.h"


static sv_stats * stats = NULL;


void stats_init(){
    if(stats != NULL) return;
    stats = malloc(sizeof(sv_stats));
    stats->totalBytesSent = 0;
    stats->totalBytesReceived = 0;
    stats->totalMailsSent = 0;
    stats->totalConnections = 0;
    stats->concurrentConnections = 0;
    return;

} //inicialmente va a ser volatil, en un futuro se pueden guardar en un archivo
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

}

void stats_destroy(){
    if(stats == NULL) return;
    free(stats);
}

void stats_remove_connection(){
    if(stats == NULL) return;
    stats->concurrentConnections--;
}

void stats_print(){
    if(stats == NULL) return;
    printf("Total connections: %ld\n", stats->totalConnections);
    printf("Total bytes sent: %ld\n", stats->totalBytesSent);
    printf("Total bytes received: %ld\n", stats->totalBytesReceived);
    printf("Total mails sent: %ld\n", stats->totalMailsSent);
    printf("Concurrent connections: %ld\n", stats->concurrentConnections);
}