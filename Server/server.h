#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
/*
   gcc server.c -lpthread -o server
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <ctype.h>
#include "client.h"

void handleMessage(char *msg, int sock);
int read_config();
void init_server(int PORT);
void run();
void *connection_handler(void *);
void send_to_all(char *message);
void send_to(char *message, int *client);
void set_client(int *sock);

#endif // SERVER_SERVER_H
