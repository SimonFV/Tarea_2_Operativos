
#include "server.h"

int socket_desc, client_sock, c;
struct sockaddr_in server, client;
pthread_mutex_t locker;
struct client *clients_arr[4];
int current_client;
int matrix_length;

int PORT;
char DirColores[2048], DirHist[2048], DirLog[2048];

/**
 * @brief Reads the configuration file and gets its values
 *
 * @return int
 */
int read_config()
{
    FILE *file_ptr;
    char line[2048];

    file_ptr = fopen("/etc/server/config.conf", "r");

    if (NULL == file_ptr)
    {
        printf("Can't find the configuration file. Closing...\n");
        return -1;
    }

    while (fgets(line, 100, file_ptr))
    {
        char *key;
        char *line_ptr = line;

        key = strtok(line_ptr, ":");
        char *value = strtok(NULL, "\n");

        // Reads the port value
        if (strcmp(key, "Puerto") == 0)
        {
            PORT = atoi(value);
        }
        // Reads the directory path of the images separated by colour
        else if (strcmp(key, "DirColores") == 0)
        {
            strcpy(DirColores, value);
        }
        // Reads the directory path of the images equalized
        else if (strcmp(key, "DirHisto") == 0)
        {
            strcpy(DirHist, value);
        }
        // Reads the directory path of log file
        else if (strcmp(key, "DirLog") == 0)
        {
            strcpy(DirLog, value);
        }

        else
        {
            printf("Wrong argument in configuration file. Closing...\n");
            return -1;
        }
    }

    return 0;
}

/**
 * @brief Main method
 *
 * @return int
 */
int main()
{
    int result = read_config();
    if (result == -1)
        return 0;

    slog_config_t cfg;
    // Directorio para log
    struct stat st_ = {0};
    if (stat(DirLog, &st_) == -1)
        mkdir(DirLog, 0700);
    strcat(DirLog, "server_log");

    // log flags
    int nEnabledLevels = SLOG_INFO | SLOG_ERROR | SLOG_WARN | SLOG_FATAL;
    slog_init(DirLog, nEnabledLevels, 1);
    slog_config_get(&cfg);

    cfg.nToFile = 1;
    cfg.nToScreen = 1;
    slog_config_set(&cfg);

    // Initialize the server
    slog_info("Initializing server on PORT: %i", PORT);
    if (init_server(PORT) != 0)
    {
        slog_fatal("Initialization failed! Try running it again.");
    }

    run(DirColores, DirHist, DirLog);

    return 0;
}
