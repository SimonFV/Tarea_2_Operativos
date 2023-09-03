#include "server.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

extern int socket_desc, client_sock, c;
extern struct sockaddr_in server, client;
extern pthread_mutex_t locker;
extern struct client *clients_arr[4];
extern int current_client;
extern int matrix_length;

#define MSG_SIZE 4096

struct args_struct
{
    void *client_sock;
    char *DirColores;
    char *DirHist;
    char *DirLog;
};

// Method that initializes the entire server
int init_server(int PORT)
{
    memset(clients_arr, 0, 4);
    current_client = 0;

    // Creation of the server socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    char iSetOption = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&iSetOption, sizeof(iSetOption));

    // Handles socket creation errors
    if (socket_desc == -1)
    {
        slog_error("Could not create the socket.");
        return -1;
    }
    slog_info("Socket created.");

    // Sets socket addresses
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind socket to desired port and address
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        slog_error("Socket binding failed!");
        return -1;
    }
    slog_info("Socket binding complete.");

    // Listen for possible connections
    listen(socket_desc, 3);

    c = sizeof(struct sockaddr_in);

    slog_info("Server is running: Waiting for connections...");
    return 0;
}

// Method to manage the server in a thread
void run(char *DirColores, char *DirHist, char *DirLog)
{
    pthread_t thread_id;

    // Accept connections
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        slog_info("Connection of client accepted.");

        struct args_struct *args = (struct args_struct *)malloc(sizeof(struct args_struct));
        args->client_sock = (void *)&client_sock;
        args->DirColores = DirColores;
        args->DirHist = DirHist;
        args->DirLog = DirLog;

        // Thread for each specific connection
        if (pthread_create(&thread_id, NULL, connection_handler, (void *)args) < 0)
        {
            slog_error("Error while creating the thread for the new connection.");
            return;
        }

        slog_info("Client connected. Controller assigned. Socket: %i", client_sock);
        current_client++;
    }

    if (client_sock < 0)
    {
        slog_error("Failed to established connection!");
        return;
    }

    pthread_join(thread_id, NULL);
}

// Handles each connection individually
void *connection_handler(void *args)
{
    int sock = *(int *)(((struct args_struct *)args)->client_sock);
    int read_size;
    char message[50], client_message[MSG_SIZE];

    int this_client = current_client;

    set_client(&sock);
    sprintf(message, "Connected!\n");
    write(sock, message, strlen(message));

    // It is constantly read to receive new messages
    while ((read_size = recv(sock, client_message, MSG_SIZE, 0)) > 0)
    {
        // Analyze message
        if (client_message != "")
        {
            handle_message(client_message, sock, this_client,
                           ((struct args_struct *)args)->DirColores,
                           ((struct args_struct *)args)->DirHist,
                           ((struct args_struct *)args)->DirLog);
        }

        bzero(client_message, MSG_SIZE);
    }

    if (read_size == 0)
    {
        slog_info("Client disconnected. Socket: %i", sock);
        // free(clients_arr[this_client]->image);
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        slog_error("Reading error, recv failed.");
    }
}

// Analyzes the client's incoming messages
void handle_message(char *msg, int sock, int client, char *DirColores, char *DirHist, char *DirLog)
{
    char buf[MSG_SIZE];
    strcpy(buf, msg);

    int i = 0;
    char *value = strtok(buf, ",");

    // Parse the incoming message
    if (strcmp(value, "start") == 0)
    {
        // gets the height and the width of the image and also its name
        int width = atoi(strtok(NULL, ","));
        int height = atoi(strtok(NULL, ","));
        char *file_name = strtok(NULL, ",");
        int len = sizeof(file_name) / sizeof(char);

        slog_info("Size of image %s received: Width: %i, Height: %i\n", file_name, width, height);

        clients_arr[client]->width = width;
        clients_arr[client]->height = height;
        clients_arr[client]->file_name = malloc(len * sizeof(char));
        strcpy(clients_arr[client]->file_name, file_name);

        int size = width * height * 3;
        clients_arr[client]->image = malloc(size * sizeof(int));
        clients_arr[client]->index = 0;

        send_to("Size received", &sock);
    }
    else if (strcmp(value, "pixels") == 0)
    {
        // saves the image pixels
        value = strtok(NULL, ",");
        while (value != NULL)
        {
            clients_arr[client]->image[clients_arr[client]->index++] = atoi(value);
            value = strtok(NULL, ",");
        }
        send_to("ok", &sock);
    }
    else if (strcmp(value, "end") == 0)
    {
        slog_info("Image received successfuly. Processing...");
        send_to("Image received", &sock);

        int size = clients_arr[client]->width * clients_arr[client]->height * 3;
        unsigned char *result = malloc((size) * sizeof(unsigned char));
        unsigned char *copy = malloc((size) * sizeof(unsigned char));
        // call the histogram equalization function

        slog_info("Equalizing: %s", clients_arr[client]->file_name);
        int category = equalize(clients_arr[client]->image, result, size);

        for (int i = 0; i < size; i++)
        {
            copy[i] = (unsigned char)clients_arr[client]->image[i];
        }

        // create required directories for the colors
        struct stat st_ = {0};
        char path_colors[2048];
        strcpy(path_colors, DirColores);
        if (stat(path_colors, &st_) == -1)
            mkdir(path_colors, 0700);

        // separates the image according to its category
        if (category == 0)
        {
            strcat(path_colors, "rojas/");
            slog_info("Image %s saved as red.", clients_arr[client]->file_name);
        }
        else if (category == 1)
        {
            strcat(path_colors, "verdes/");
            slog_info("Image %s saved as green.", clients_arr[client]->file_name);
        }
        else
        {
            strcat(path_colors, "azules/");
            slog_info("Image %s saved as blue.", clients_arr[client]->file_name);
        }

        // create required directories (red, green, blue)
        if (stat(path_colors, &st_) == -1)
            mkdir(path_colors, 0700);
        strcat(path_colors, clients_arr[client]->file_name);

        // save the categorized image
        stbi_write_jpg(path_colors,
                       clients_arr[client]->width,
                       clients_arr[client]->height,
                       3,
                       copy,
                       clients_arr[client]->width * 3);

        char path_hist[2048];
        strcpy(path_hist, DirHist);
        strcat(path_hist, clients_arr[client]->file_name);
        // Creates the directory for hist
        struct stat st = {0};
        if (stat(DirHist, &st) == -1)
            mkdir(DirHist, 0700);

        // save the equalized image
        stbi_write_jpg(path_hist,
                       clients_arr[client]->width,
                       clients_arr[client]->height,
                       3,
                       result,
                       clients_arr[client]->width * 3);

        slog_info("Image %s processed successfuly.", clients_arr[client]->file_name);
    }
}

// Method to send a message to all connected clients
void send_to_all(char *message)
{
    for (int i = 0; i < 4; ++i)
    {
        if (clients_arr[i] != 0)
            write(*clients_arr[i]->sock, message, strlen(message));
    }
}

void send_to(char *message, int *sock)
{
    write(*sock, message, strlen(message));
}

// The new client is added to the clients list.
void set_client(int *sock)
{
    pthread_mutex_lock(&locker);

    struct client *ptr_one;
    ptr_one = (struct client *)malloc(sizeof(struct client));
    ptr_one->sock = sock;

    clients_arr[current_client] = ptr_one;

    pthread_mutex_unlock(&locker);
}
