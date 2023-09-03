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

/**
 * @brief It is in charge of managing the messages and
 *        performing actions with the images.
 * @param msg
 * @param sock
 * @param client
 * @param DirColores
 * @param DirHist
 * @param DirLog
 */
void handle_message(char *msg, int sock, int client, char *DirColores, char *DirHist, char *DirLog);

/**
 * @brief Reads the configuration file and gets its values
 * @return int
 */
int read_config();

/**
 * @brief This method is in charge of initializing the server.
 * @param PORT
 */
int init_server(int PORT);

/**
 * @brief Method in charge of running the server
 *
 * @param DirColores
 * @param DirHist
 * @param DirLog
 */
void run(char *DirColores, char *DirHist, char *DirLog);

/**
 * @brief Method for handling connections with clients
 * @param args
 * @return void*
 */
void *connection_handler(void *args);

/**
 * @brief Method in charge of sending a message to all
 * connected clients.
 * @param message
 */
void send_to_all(char *message);
/**
 * @brief Method in charge of sending a message
 * @param message
 * @param sock
 */
void send_to(char *message, int *sock);

/**
 * @brief Set the client object
 *
 * @param sock
 */
void set_client(int *sock);

#endif // SERVER_SERVER_H
