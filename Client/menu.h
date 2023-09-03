#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>

// Estructura para almacenar las rutas de los archivos seleccionados
typedef struct
{
    char **paths;
    size_t count;
    size_t capacity;
} FileSelection;

void initializeFileSelection(FileSelection *selection);
void addPathToFileSelection(FileSelection *selection, const char *path);
void freeFileSelection(FileSelection *selection);
void listFilesInDirectory(const char *directory);
int send_images(FileSelection selection, int sockfd);
int wait_response(int sockfd, char *response);
int menu(int sockfd);

#endif // SERVER_CLIENT_H