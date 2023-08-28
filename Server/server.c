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

// Metodo que inicializa todo el servidor
void init_server(int PORT)
{
    memset(clients_arr, 0, 4);
    current_client = 0;

    // Creacion del socket del servidor
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    int iSetOption = 1;
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
void run()
{
    pthread_t thread_id;

    // Aceptar conexiones
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Conexion aceptada");

        // Thread para cada conexion especifica
        if (pthread_create(&thread_id, NULL, connection_handler, (void *)&client_sock) < 0)
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
void *connection_handler(void *socket_desc)
{
    int sock = *(int *)socket_desc;
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
            handleMessage(client_message, sock, this_client);
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
void handleMessage(char *msg, int sock, int client)
{
    char buf[MSG_SIZE];
    strcpy(buf, msg);

    int i = 0;
    char *value = strtok(buf, ",");

    if (strcmp(value, "start") == 0)
    {
        int width = atoi(strtok(NULL, ","));
        int height = atoi(strtok(NULL, ","));

        printf("%i\n", width);
        printf("%i\n", height);

        clients_arr[client]->width = width;
        clients_arr[client]->height = height;

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
        send_to("Pixels received", &sock);
    }
    else if (strcmp(value, "end") == 0)
    {
        send_to("Image received", &sock);

        int size = clients_arr[client]->width * clients_arr[client]->height * 3;
        unsigned char *result = malloc((size) * sizeof(unsigned char));
        // equalize(clients_arr[client]->image, result, size);

        for (int i = 0; i < size; i++)
        {
            result[i] = (unsigned char)clients_arr[client]->image[i];
        }

        stbi_write_jpg("jpg_test_.jpg",
                       clients_arr[client]->width,
                       clients_arr[client]->height,
                       3,
                       result,
                       clients_arr[client]->width * 3);

        // free(clients_arr[client]->image);
    }
}

void save_image(int w, int h, int channels_num)
{
    unsigned char data[w * h * channels_num];

    int index = 0;
    for (int j = h - 1; j >= 0; --j)
    {
        for (int i = 0; i < w; ++i)
        {
            data[index++] = (unsigned char)(255.0 * i / w);
            data[index++] = (unsigned char)(255.0 * j / h);
            data[index++] = (unsigned char)(255.0 * 0.2);
        }
    }

    stbi_write_jpg("asd.jpg", w, h, channels_num, data, w * channels_num);
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
