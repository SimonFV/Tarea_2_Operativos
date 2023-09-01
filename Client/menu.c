#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h> 

// Estructura para almacenar las rutas de los archivos seleccionados
typedef struct {
    char **paths;
    size_t count;
    size_t capacity;
} FileSelection;

// Inicializa la estructura FileSelection
void initializeFileSelection(FileSelection *selection) {
    selection->paths = NULL;
    selection->count = 0;
    selection->capacity = 0;
}

// Agrega una ruta al vector de rutas en FileSelection
void addPathToFileSelection(FileSelection *selection, const char *path) {
    if (selection->count >= selection->capacity) {
        selection->capacity = (selection->capacity == 0) ? 1 : selection->capacity * 2;
        selection->paths = realloc(selection->paths, selection->capacity * sizeof(char *));
    }
    selection->paths[selection->count++] = strdup(path);
}

// Libera la memoria utilizada por FileSelection
void freeFileSelection(FileSelection *selection) {
    for (size_t i = 0; i < selection->count; i++) {
        free(selection->paths[i]);
    }
    free(selection->paths);
    selection->paths = NULL;
    selection->count = 0;
    selection->capacity = 0;
}

// Función para listar archivos en el directorio actual
void listFilesInDirectory(const char *directory) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);

    if (dir == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    printf("Archivos en %s:\n", directory);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int main() {
    char currentDirectory[1024];
    getcwd(currentDirectory, sizeof(currentDirectory));

    FileSelection selection;
    initializeFileSelection(&selection);

    while (1) {

        printf("Directorio actual: %s\n", currentDirectory);
        printf("Opciones:\n");
        printf("1. Listar archivos en el directorio actual\n");
        printf("2. Cambiar de directorio\n");
        printf("3. Seleccionar archivo(s)\n");
        printf("4. Mostrar archivos seleccionados\n");
        printf("5. Salir\n");
        printf("Seleccione una opción: ");

        int choice;
        scanf("%d", &choice);

        switch (choice) {
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
                if (strcmp(fileName, "0") == 0) {
                    // Finalizar la selección
                    for (size_t i = 0; i < selection.count; i++) {
                        printf("Archivo seleccionado: %s\n", selection.paths[i]);
                    }
                } else {
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
                for (size_t i = 0; i < selection.count; i++) {
                    printf("%s\n", selection.paths[i]);
                }
                break;
            case 5:
                freeFileSelection(&selection);
                exit(0);
            default:
                printf("Opción no válida\n");
        }
    }

    return 0;
}
