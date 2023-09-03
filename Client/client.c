#include "menu.h"

int initClient(char *ip, int port)
{

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Error al crear el socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(port);          // Puerto del servidor
    server_addr.sin_addr.s_addr = inet_addr(ip); // Dirección IP del servidor

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error al conectarse al servidor");
        return 1;
    }

    menu(sockfd);

    close(sockfd);

    return 0;
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Uso: %s <direccion_ip> <puerto>\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    initClient((char *)ip, port);
}
