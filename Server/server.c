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

// Metodo que inicializa todo el servidor
void init_server(int PORT)
{
    memset(clients_arr, 0, 4);
    current_client = 0;

    // Creacion del socket del servidor
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    char iSetOption = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&iSetOption, sizeof(iSetOption));

    // Maneja errores de creacion del socket
    if (socket_desc == -1)
    {
        printf("No se pudo crear el socket");
    }
    puts("Socket creado");

    // Establece direcciones del socket
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind del socket al puerto y direccion deseados
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Enlace fallido. Error");
        return;
    }
    puts("Enlace hecho");

    // Escuchar posibles conexiones
    listen(socket_desc, 3);

    c = sizeof(struct sockaddr_in);

    puts("Esperando conexiones entrantes...");
}

// Metodo para manejar al servidor en un thread
void run(char *DirColores, char *DirHist, char *DirLog)
{
    pthread_t thread_id;

    // Aceptar conexiones
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Conexion aceptada");

        struct args_struct *args = (struct args_struct *)malloc(sizeof(struct args_struct));
        args->client_sock = (void *)&client_sock;
        args->DirColores = DirColores;
        args->DirHist = DirHist;
        args->DirLog = DirLog;

        // Thread para cada conexion especifica
        if (pthread_create(&thread_id, NULL, connection_handler, (void *)args) < 0)
        {
            perror("No se pudo crear el thread ");
            return;
        }

        puts("Controlador asignado");
        current_client++;
    }

    if (client_sock < 0)
    {
        perror("Aceptacion fallida");
        return;
    }

    pthread_join(thread_id, NULL);
}

// Maneja cada conexion individualmente
void *connection_handler(void *args)
{
    int sock = *(int *)(((struct args_struct *)args)->client_sock);
    int read_size;
    char message[50], client_message[MSG_SIZE];

    int this_client = current_client;

    set_client(&sock);
    sprintf(message, "Connected!\n");
    write(sock, message, strlen(message));

    // Se lee constantemente para recibir nuevos mensajes
    while ((read_size = recv(sock, client_message, MSG_SIZE, 0)) > 0)
    {
        // Analizar mensaje
        if (client_message != "")
        {
            handleMessage(client_message, sock, this_client,
                          ((struct args_struct *)args)->DirColores,
                          ((struct args_struct *)args)->DirHist,
                          ((struct args_struct *)args)->DirLog);
        }

        bzero(client_message, MSG_SIZE);
    }

    if (read_size == 0)
    {
        puts("Cliente desconectado");
        // free(clients_arr[this_client]->image);
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("recv failed");
    }
}

// Analiza los mensajes entrantes del cliente
void handleMessage(char *msg, int sock, int client, char *DirColores, char *DirHist, char *DirLog)
{
    char buf[MSG_SIZE];
    strcpy(buf, msg);

    int i = 0;
    char *value = strtok(buf, ",");

    if (strcmp(value, "start") == 0)
    {
        int width = atoi(strtok(NULL, ","));
        int height = atoi(strtok(NULL, ","));
        char *file_name = strtok(NULL, ",");
        int len = sizeof(file_name) / sizeof(char);

        printf("Size received: Width: %i, Height:%i\n", width, height);

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
        send_to("Image received", &sock);

        int size = clients_arr[client]->width * clients_arr[client]->height * 3;
        unsigned char *result = malloc((size) * sizeof(unsigned char));
        unsigned char *copy = malloc((size) * sizeof(unsigned char));

        int category = equalize(clients_arr[client]->image, result, size);

        for (int i = 0; i < size; i++)
        {
            copy[i] = (unsigned char)clients_arr[client]->image[i];
        }

        struct stat st_ = {0};
        char path_colors[2048];
        strcpy(path_colors, DirColores);
        if (stat(path_colors, &st_) == -1)
            mkdir(path_colors, 0700);

        if (category == 0)
            strcat(path_colors, "rojas/");
        else if (category == 1)
            strcat(path_colors, "verdes/");
        else
            strcat(path_colors, "azules/");

        if (stat(path_colors, &st_) == -1)
            mkdir(path_colors, 0700);
        strcat(path_colors, clients_arr[client]->file_name);

        stbi_write_jpg(path_colors,
                       clients_arr[client]->width,
                       clients_arr[client]->height,
                       3,
                       copy,
                       clients_arr[client]->width * 3);

        char path_hist[2048];
        strcpy(path_hist, DirHist);
        strcat(path_hist, clients_arr[client]->file_name);
        // Crear la carpeta para hist
        struct stat st = {0};
        if (stat(DirHist, &st) == -1)
            mkdir(DirHist, 0700);

        stbi_write_jpg(path_hist,
                       clients_arr[client]->width,
                       clients_arr[client]->height,
                       3,
                       result,
                       clients_arr[client]->width * 3);
    }
}

// Metodo para enviar un mensaje a todos los clientes conectados
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

// Se incluye el nuevo cliente a la lista de clientes
void set_client(int *sock)
{
    pthread_mutex_lock(&locker);

    struct client *ptr_one;
    ptr_one = (struct client *)malloc(sizeof(struct client));
    ptr_one->sock = sock;

    clients_arr[current_client] = ptr_one;

    pthread_mutex_unlock(&locker);
}
