
#include "server.h"

int socket_desc, client_sock, c;
struct sockaddr_in server, client;
pthread_mutex_t locker;
struct client *clients_arr[4];
int current_client;
int matrix_length;

int PORT;
char DirColores[2048], DirHist[2048], DirLog[2048];

void *run_server(void *h);

int read_config()
{
    FILE *file_ptr;
    char line[2048];

    file_ptr = fopen("config.conf", "r");

    if (NULL == file_ptr)
    {
        // printf("file can't be opened \n");
        return -1;
    }

    while (fgets(line, 100, file_ptr))
    {
        char *key;
        char *line_ptr = line;

        key = strtok(line_ptr, ":");
        char *value = strtok(NULL, "\n");

        if (strcmp(key, "Puerto") == 0)
        {
            PORT = atoi(value);
        }

        else if (strcmp(key, "DirColores") == 0)
        {
            strcpy(DirColores, value);
        }

        else if (strcmp(key, "DirHisto") == 0)
        {
            strcpy(DirHist, value);
        }

        else if (strcmp(key, "DirLog") == 0)
        {
            strcpy(DirLog, value);
        }

        else
            return -1;
    }

    return 0;
}

int main()
{
    int result = read_config();
    if (result == -1)
        return 0;

    pthread_t thread_id;
    char b[100];

    // Se inicializa el server
    init_server(PORT);

    // Se crea un thread para manejar al servidor por aparte
    pthread_create(&thread_id, NULL, run_server, NULL);

    pthread_join(thread_id, NULL);

    return 0;
}

// Metodo para correr el servidor en un thread
void *run_server(void *h)
{
    run();
}
