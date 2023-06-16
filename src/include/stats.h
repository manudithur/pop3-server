#ifndef _STATS_H_
#define _STATS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// 5.  implementar mecanismos que permitan recolectar métricas que
//        ayuden a monitorear la operación del sistema.

//        A.  cantidad de conexiones históricas

//        B.  cantidad de conexiones concurrentes

//        C.  cantidad de bytes transferidos

//        D.  cualquier otra métrica que considere oportuno para el
//            entendimiento del funcionamiento dinámico del sistema

//        Las métricas PUEDEN ser volátiles (si se reinicia el servidor las
//        estadísticas pueden perderse).

typedef struct sv_stats{
    size_t totalConnections;
    size_t concurrentConnections;
    size_t totalBytesSent;
    size_t totalBytesReceived;
    size_t totalMailsSent;
}sv_stats;

void stats_init(); //inicialmente va a ser volatil, en un futuro se pueden guardar en un archivo
void stats_update(size_t bytesSent, size_t bytesReceived, size_t mailsSent);
void stats_add_connection();
void stats_remove_connection();

size_t getTotalConnections();

size_t getTotalBytesSent();

size_t getTotalBytesReceived();

size_t getConcurrentConnections();

void stats_destroy();

void stats_print(); //temporal













#endif