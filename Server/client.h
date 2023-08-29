#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

struct client
{
    int *sock;
    int width, height;
    int *image;
    int index;
    char *file_name;
};

#endif // SERVER_CLIENT_H
