
#include "menu.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

void initializeFileSelection(FileSelection *selection)
{
    selection->paths = NULL;
    selection->count = 0;
    selection->capacity = 0;
}


void addPathToFileSelection(FileSelection *selection, const char *path)
{
    if (selection->count >= selection->capacity)
    {
        selection->capacity = (selection->capacity == 0) ? 1 : selection->capacity * 2;
        selection->paths = realloc(selection->paths, selection->capacity * sizeof(char *));
    }
    selection->paths[selection->count++] = strdup(path);
}


void freeFileSelection(FileSelection *selection)
{
    for (size_t i = 0; i < selection->count; i++)
    {
        free(selection->paths[i]);
    }
    free(selection->paths);
    selection->paths = NULL;
    selection->count = 0;
    selection->capacity = 0;
}


void listFilesInDirectory(const char *directory)
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);

    if (dir == NULL)
    {
        perror("Error al abrir el directorio");
        return;
    }

    printf("Archivos en %s:\n", directory);

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int wait_response(int sockfd, char *response)
{
    char received_message[50];
    while (1)
    {
        int bytes_received = recv(sockfd, received_message, 50, 0);
        if (bytes_received == -1)
        {
            perror("Error al recibir mensaje del servidor");
            return -1;
        }
        else if (bytes_received == 0)
        {
            // Connection closed by the server
            printf("Conexión cerrada por el servidor.\n");
            return -1;
        }
        else
        {
            strcpy(response, received_message);
            response[bytes_received] = '\0';
            break;
        }
    }

    return 0;
}

int sort(FileSelection *selection)
{
    FileSize *filesizes = malloc(selection->count * sizeof(FileSize));

    int width, height, channels;
    for (size_t i = 0; i < selection->count; i++)
    {
        unsigned char *image = stbi_load(selection->paths[i], &width, &height, &channels, 0);
        if (image == NULL)
        {
            printf("Error in loading the image\n");
            return -1;
        }
        filesizes[i].path = malloc(strlen(selection->paths[i]) * sizeof(char *));
        strcpy(filesizes[i].path, selection->paths[i]);
        filesizes[i].size = width * height;
    }

    bubble_sort(filesizes, selection->count);

    for (int i = 0; i < selection->count; i++)
    {
        free(selection->paths[i]);
        selection->paths[i] = malloc(strlen(filesizes[i].path) * sizeof(char));
        strcpy(selection->paths[i], filesizes[i].path);
    }

    for (int i = 0; i < selection->count; i++)
    {
        free(filesizes[i].path);
    }
    free(filesizes);

    return 0;
}

void swap(FileSize *a, FileSize *b)
{
    FileSize temp = *a;
    *a = *b;
    *b = temp;
}

void bubble_sort(FileSize *filesizes, int n)
{
    int swapped;
    for (int i = 0; i < n - 1; i++)
    {
        swapped = 0;
        for (int j = 0; j < n - i - 1; j++)
        {
            if ((filesizes[j + 1].size - filesizes[j].size) < 0)
            {
                swap(&filesizes[j], &filesizes[j + 1]);
                swapped = 1;
            }
        }
        if (swapped == 0)
        {
            break;
        }
    }
}

int send_images(FileSelection selection, int sockfd)
{
    char message[4096]; // Buffer para almacenar el mensaje
    char response[50];

    for (size_t i = 0; i < selection.count; i++)
    {
        int width, height, channels;
        unsigned char *image = stbi_load(selection.paths[i], &width, &height, &channels, 0);
        if (image == NULL)
        {
            printf("Error in loading the image\n");
            return -1;
        }

        char swidth[10];
        char sheight[10];
        sprintf(swidth, "%d", width);
        sprintf(sheight, "%d", height);

        strcpy(message, "start,");
        strcat(message, swidth);
        strcat(message, ",");
        strcat(message, sheight);
        strcat(message, ",");

        const char *filename;
        const char *slash = strrchr(selection.paths[i], '/');

        if (slash != NULL)
            filename = slash + 1;
        else
            filename = selection.paths[i];

        strcat(message, filename);

        if (send(sockfd, message, strlen(message), 0) == -1)
        {
            perror("Error al enviar el mensaje");
            return -1;
        }
        if (wait_response(sockfd, response) != 0)
            return -1;

        memset(message, '\0', sizeof(message));
        strcpy(message, "pixels,");
        for (int i = 0; i < width * height * channels; i += channels)
        {

            unsigned char pixels[4] = {image[i], image[i + 1], image[i + 2]};

            char red[5], green[5], blue[5];
            sprintf(red, "%u", pixels[0]);
            sprintf(green, "%u", pixels[1]);
            sprintf(blue, "%u", pixels[2]);

            strcat(message, red);
            strcat(message, ",");
            strcat(message, green);
            strcat(message, ",");
            strcat(message, blue);

            if (strlen(message) > 4060 || (i == (width * height * channels) - channels))
            {
                if (send(sockfd, message, strlen(message), 0) == -1)
                {
                    perror("Error al enviar el mensaje");
                    return -1;
                }
                if (wait_response(sockfd, response) != 0)
                    return -1;

                memset(message, '\0', sizeof(message));
                strcpy(message, "pixels,");
            }
            else
                strcat(message, ",");
        }

        if (send(sockfd, "end", strlen(message), 0) == -1)
        {
            perror("Error al enviar el mensaje");
            return -1;
        }
        if (wait_response(sockfd, response) != 0)
            return -1;
        printf("%s\n", response);

        printf("Enviado: %s\n", filename);
    }

    return 0;
}

int menu(int sockfd)
{
    char response[50];
    wait_response(sockfd, response);
    printf("%s", response);

    char currentDirectory[1024];
    getcwd(currentDirectory, sizeof(currentDirectory));

    FileSelection selection;
    initializeFileSelection(&selection);

    int choice = 0;
    while (1)
    {

        printf("Directorio actual: %s\n", currentDirectory);
        printf("Opciones:\n");
        printf("1. Listar archivos en el directorio actual\n");
        printf("2. Cambiar de directorio\n");
        printf("3. Seleccionar archivo(s)\n");
        printf("4. Mostrar archivos seleccionados\n");
        printf("5. Enviar\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            system("clear"); // Limpia la consola antes de mostrar los archivos seleccionados
            listFilesInDirectory(currentDirectory);
            break;
        case 2:
            printf("Ingrese la nueva ruta: ");
            char newDirectory[1024];
            scanf("%s", newDirectory);
            chdir(newDirectory);
            getcwd(currentDirectory, sizeof(currentDirectory));
            system("clear");
            break;
        case 3:
            printf("Ingrese el nombre del archivo a seleccionar (o '0' para finalizar): ");
            char fileName[1024];
            scanf("%s", fileName);
            if (strcmp(fileName, "0") == 0)
            {
                // Finalizar la selección
                for (size_t i = 0; i < selection.count; i++)
                {
                    printf("Archivo seleccionado: %s\n", selection.paths[i]);
                }
            }
            else
            {
                // Agregar la ruta del archivo seleccionado
                char filePath[2048];
                snprintf(filePath, sizeof(filePath), "%s/%s", currentDirectory, fileName);
                addPathToFileSelection(&selection, filePath);
            }
            system("clear");
            break;
        case 4:
            system("clear"); // Limpia la consola antes de mostrar los archivos seleccionados
            printf("Archivos seleccionados:\n");
            for (size_t i = 0; i < selection.count; i++)
            {
                printf("%s\n", selection.paths[i]);
            }
            break;
        case 5:
            system("clear");
            printf("Enviando archivos...\n");
            if (sort(&selection) != 0)
                return 0;

            if (send_images(selection, sockfd) != 0)
                return 0;
            break;

        case 6:
            freeFileSelection(&selection);
            exit(0);
        default:
            printf("Opción no válida\n");
            exit(0);
        }
    }

    return 0;
}
