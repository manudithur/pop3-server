// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <limits.h>    /* LONG_MIN et al */
#include <string.h>    /* memset */
#include <errno.h>
#include <getopt.h>
#include "../include/users.h"
#include "../include/args.h"

// static unsigned short
// port(const char *s) {
//      char *end     = 0;
//      const long sl = strtol(s, &end, 10);

//      if (end == s|| '\0' != *end
//         || ((LONG_MIN == sl || LONG_MAX == sl) && ERANGE == errno)
//         || sl < 0 || sl > USHRT_MAX) {
//          fprintf(stderr, "port should in in the range of 1-65536: %s\n", s);
//          exit(1);
//          return 1;
//      }
//      return (unsigned short)sl;
// }

// static user
// user(char *s, struct users *user) {
//     char *p = strchr(s, ':');
//     if(p == NULL) {
//         fprintf(stderr, "password not found\n");
//         exit(1);
//     } else {
//         *p = 0;
//         p++;
//         user->name = s;
//         user->pass = p;
//     }
// }

// static void
// version(void) {
//     fprintf(stderr, "POP3 version 1.4.2\n"
//                     "ITBA Protocolos de Comunicación 2023/1 -- Grupo 8\n"
//                     "AQUI VA LA LICENCIA\n");
// }

// static void
// usage(const char *progname) {
//     fprintf(stderr,
//         "Usage: %s [OPTION]...\n"
//         "\n"
//         "   -h               Imprime la ayuda y termina.\n"
//         "   -l <SOCKS addr>  Dirección donde servirá el proxy SOCKS.\n"
//         "   -L <conf  addr>  Dirección donde servirá el servicio de management.\n"
//         "   -p <SOCKS port>  Puerto entrante conexiones SOCKS.\n"
//         "   -P <conf port>   Puerto entrante conexiones configuracion\n"
//         "   -u <name>:<pass> Usuario y contraseña de usuario que puede usar el proxy. Hasta 10.\n"
//         "   -v               Imprime información sobre la versión versión y termina.\n"
//         "\n"
//         "   --doh-ip    <ip>    \n"
//         "   --doh-port  <port>  XXX\n"
//         "   --doh-host  <host>  XXX\n"
//         "   --doh-path  <host>  XXX\n"
//         "   --doh-query <host>  XXX\n"

//         "\n",
//         progname);
//     exit(1);
// }

void 
invalidUserFormat(){
    fprintf(stderr, "Invalid format.\nip port -u user:pass \n");
    exit(1);
}

void
parseAndAddUsers(const int argc, char ** argv){

    if(argc < 2 || strcmp(argv[0], "-u") != 0){
        defaultUsers();
        return;
    }


    for(int i = 1 ; i < argc ; i++){
        char *p = strchr(argv[i], ':');
        *p = 0;
        p++;

        if(p == NULL) {
            fprintf(stderr, "Password not found for user number: %d\n", i);
            exit(1);
        } else {
            if(addUser(argv[i], p) != USER_ADDED) {
                fprintf(stderr, "Problem adding user: %s\n", argv[i]);
                exit(1);
            }
        }
    }   
}

// void 
// parse_args(const int argc, char **argv, struct socks5args *args) {
//     // memset(args, 0, sizeof(*args)); // sobre todo para setear en null los punteros de users

//     // args->socks_addr = "0.0.0.0";
//     // args->socks_port = 1080;

//     // args->mng_addr   = "127.0.0.1";
//     // args->mng_port   = 8080;

//     // args->disectors_enabled = true;

//     // args->doh.host = "localhost";
//     // args->doh.ip   = "127.0.0.1";
//     // args->doh.port = 8053;
//     // args->doh.path = "/getnsrecord";
//     // args->doh.query = "?dns=";

//     int c;
//     int nusers = 0;

//     while (true) {
//         int option_index = 0;
//         static struct option long_options[] = {
//             { "doh-ip",    required_argument, 0, 0xD001 },
//             { "doh-port",  required_argument, 0, 0xD002 },
//             { "doh-host",  required_argument, 0, 0xD003 },
//             { "doh-path",  required_argument, 0, 0xD004 },
//             { "doh-query", required_argument, 0, 0xD005 },
//             { 0,           0,                 0, 0 }
//         };

//         c = getopt_long(argc, argv, "hl:L:Np:P:u:v", long_options, &option_index);
//         if (c == -1)
//             break;

//         switch (c) {
//             case 'h':
//                 usage(argv[0]);
//                 break;
//             case 'l':
//                 args->socks_addr = optarg;
//                 break;
//             case 'L':
//                 args->mng_addr = optarg;
//                 break;
//             case 'N':
//                 args->disectors_enabled = false;
//                 break;
//             case 'p':
//                 args->socks_port = port(optarg);
//                 break;
//             case 'P':
//                 args->mng_port   = port(optarg);
//                 break;
//             case 'u':
//                 if(nusers >= MAX_USERS) {
//                     fprintf(stderr, "maximun number of command line users reached: %d.\n", MAX_USERS);
//                     exit(1);
//                 } else {
//                     user(optarg, args->users + nusers);
//                     nusers++;
//                 }
//                 break;
//             case 'v':
//                 version();
//                 exit(0);
//                 break;
//             case 0xD001:
//                 args->doh.ip = optarg;
//                 break;
//             case 0xD002:
//                 args->doh.port = port(optarg);
//                 break;
//             case 0xD003:
//                 args->doh.host = optarg;
//                 break;
//             case 0xD004:
//                 args->doh.path = optarg;
//                 break;
//             case 0xD005:
//                 args->doh.query = optarg;
//                 break;
//             default:
//                 fprintf(stderr, "unknown argument %d.\n", c);
//                 exit(1);
//         }

//     }
//     if (optind < argc) {
//         fprintf(stderr, "argument not accepted: ");
//         while (optind < argc) {
//             fprintf(stderr, "%s ", argv[optind++]);
//         }
//         fprintf(stderr, "\n");
//         exit(1);
//     }
// }
