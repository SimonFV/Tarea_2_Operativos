#include "equalization.h"

void equalize(int *pixels, unsigned char *result, int size)
{
    int *red_pixels = malloc((size / 3) * sizeof(int));
    int *green_pixels = malloc((size / 3) * sizeof(int));
    int *blue_pixels = malloc((size / 3) * sizeof(int));

    get_color(pixels, red_pixels, size, 0);
    get_color(pixels, green_pixels, size, 1);
    get_color(pixels, blue_pixels, size, 2);

    int red_frequency[256] = {0};
    int green_frequency[256] = {0};
    int blue_frequency[256] = {0};

    get_frequency(red_pixels, red_frequency, size / 3);
    get_frequency(green_pixels, green_frequency, size / 3);
    get_frequency(blue_pixels, blue_frequency, size / 3);

    int red_cdf[256] = {0};
    int green_cdf[256] = {0};
    int blue_cdf[256] = {0};

    get_cdf(red_frequency, red_cdf);
    get_cdf(green_frequency, green_cdf);
    get_cdf(blue_frequency, blue_cdf);

    int *new_red_pixels = malloc((size / 3) * sizeof(int));
    int *new_green_pixels = malloc((size / 3) * sizeof(int));
    int *new_blue_pixels = malloc((size / 3) * sizeof(int));

    new_pixels(red_cdf, red_pixels, new_red_pixels, size / 3);
    new_pixels(green_cdf, green_pixels, new_green_pixels, size / 3);
    new_pixels(blue_cdf, blue_pixels, new_blue_pixels, size / 3);

    /*
    printf("pixels\n");
    for (int i = 0; i < size / 3; i++)
        printf("%i ", red_pixels[i]);

    printf("\n\n frec\n");
    for (int i = 0; i < 256; i++)
        printf("%i ", red_frequency[i]);

    printf("\n\n final\n");
    for (int j = 0; j < size / 3; j++)
    {
        printf("%i ", new_red_pixels[j]);
    }

    printf("\n\n cdf\n");
    for (int j = 0; j < 256; j++)
    {
        printf("%i ", red_cdf[j]);
    }*/
    int index = 0;
    for (int i = 0; i < size / 3; i++)
    {
        result[index++] = (unsigned char)new_red_pixels[i];
        result[index++] = (unsigned char)new_green_pixels[i];
        result[index++] = (unsigned char)new_blue_pixels[i];
    }

    /*
    free(red_pixels);
    free(green_pixels);
    free(blue_pixels);
    free(new_red_pixels);
    free(new_green_pixels);
    free(new_blue_pixels);*/
}

void get_color(int *pixels, int *result, int size, int mode)
{
    for (int i = mode; i < size; i += 3)
        result[i] = pixels[i];
}

void get_frequency(int *pixels, int *result, int size)
{
    for (int i = 0; i < size; i++)
        result[pixels[i]] += 1;
}

void get_cdf(int *frequency, int *result)
{
    result[0] = frequency[0];
    for (int i = 1; i < 256; i++)
        result[i] = frequency[i] + result[i - 1];
}

void new_pixels(int *cdf, int *pixels, int *result, int size)
{
    /*
    float div = 0.0f;
    for (int i = 0; i < size; i++)
    {
        div = cdf[pixels[i]] / (float)cdf[255];
        result[i] = (int)roundf((div)*255.0);
    }*/

    // Normalizar la CDF al rango [0, 255]
    float cdf_normalized[256];
    for (int i = 0; i < 256; i++)
    {
        cdf_normalized[i] = (float)cdf[i] * 255.0 / cdf[255];
    }

    // Usar la CDF para ecualizar los valores de los píxeles
    for (int i = 0; i < size; i++)
    {
        result[i] = (int)cdf_normalized[pixels[i]];
    }
}
