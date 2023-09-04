#ifndef MENU_H
#define MENU_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>

// Structure for storing the paths to the selected files
typedef struct
{
    char **paths;
    size_t count;
    size_t capacity;
} FileSelection;

// Structure to sort images according to their size
typedef struct
{
    char *path;
    int size;
} FileSize;
/**
 * @brief Initializes the FileSelection structure
 * 
 * @param selection 
 */
void initializeFileSelection(FileSelection *selection);
/**
 * @brief Adds a path to the path vector in FileSelection
 * 
 * @param selection 
 * @param path 
 */
void addPathToFileSelection(FileSelection *selection, const char *path);
/**
 * @brief Frees the memory used by FileSelection
 * 
 * @param selection 
 */
void freeFileSelection(FileSelection *selection);
/**
 * @brief Function to list files in the current directory
 * 
 * @param directory 
 */
void listFilesInDirectory(const char *directory);
/**
 * @brief Function that sends images to the server for processing.
 * 
 * @param selection 
 * @param sockfd 
 * @return int 
 */
int send_images(FileSelection selection, int sockfd);
/**
 * @brief Function that waits for responses from the server
 * 
 * @param sockfd 
 * @param response 
 * @return int 
 */
int wait_response(int sockfd, char *response);
/**
 * @brief Function that handles the interaction with the user
 * 
 * @param sockfd 
 * @return int 
 */
int menu(int sockfd);
/**
 * @brief function that sorts the image according to its size
 * 
 * @param selection 
 * @return int 
 */
int sort(FileSelection *selection);
/**
 * @brief Function that performs a change if one image is larger than another.
 * 
 * @param a 
 * @param b 
 */
void swap(FileSize *a, FileSize *b);
/**
 * @brief Function with bubble sort algorithm for image sorting
 * 
 * @param filesizes 
 * @param n 
 */
void bubble_sort(FileSize *filesizes, int n);

#endif // SERVER_CLIENT_H