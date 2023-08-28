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
#include "client.h"
#include "equalization.h"

void save_image(int w, int h, int channels_num);
void handleMessage(char *msg, int sock, int client);
int read_config();
void init_server(int PORT);
void run();
void *connection_handler(void *);
void send_to_all(char *message);
void send_to(char *message, int *sock);
void set_client(int *sock);

#endif // SERVER_SERVER_H
