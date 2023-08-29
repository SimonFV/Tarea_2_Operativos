#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
/*
   gcc server.c -lpthread -o server
*/

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "client.h"
#include "equalization.h"

void handleMessage(char *msg, int sock, int client, char *DirColores, char *DirHist, char *DirLog);
int read_config();
void init_server(int PORT);
void run(char *DirColores, char *DirHist, char *DirLog);
void *connection_handler(void *args);
void send_to_all(char *message);
void send_to(char *message, int *sock);
void set_client(int *sock);

#endif // SERVER_SERVER_H
