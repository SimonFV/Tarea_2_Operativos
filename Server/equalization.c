#include "equalization.h"

int equalize(int *pixels, unsigned char *result, int size)
{
    int category = 0;

    // The pixels of each color are extracted.
    int *red_pixels = malloc((size / 3) * sizeof(int));
    int *green_pixels = malloc((size / 3) * sizeof(int));
    int *blue_pixels = malloc((size / 3) * sizeof(int));

    get_color(pixels, red_pixels, size / 3, 0);
    get_color(pixels, green_pixels, size / 3, 1);
    get_color(pixels, blue_pixels, size / 3, 2);

    // The values of each color are summed to classify the image.
    category = categorizer(red_pixels, green_pixels, blue_pixels, size / 3);

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

    int index = 0;
    for (int i = 0; i < size / 3; i++)
    {
        result[index++] = (unsigned char)new_red_pixels[i];
        result[index++] = (unsigned char)new_green_pixels[i];
        result[index++] = (unsigned char)new_blue_pixels[i];
    }

    return category;
}

void get_color(int *pixels, int *result, int size, int mode)
{
    for (int i = 0; i < size; i++)
    {
        if (mode == 0)
            result[i] = pixels[mode + i * 3];
        else if (mode == 1)
            result[i] = pixels[mode + i * 3];
        else
            result[i] = pixels[mode + i * 3];
    }
}

int categorizer(int *red, int *green, int *blue, int size)
{
    int total_red = 0;
    int total_green = 0;
    int total_blue = 0;
    for (int i = 0; i < size; i++)
    {
        if (red[i] >= blue[i] && red[i] >= green[i])
            total_red += 1;
        else if (green[i] >= blue[i] && green[i] >= red[i])
            total_green += 1;
        else
            total_blue += 1;
    }

    if (total_red >= total_blue && total_red >= total_green)
        return 0;
    else if (total_green >= total_blue && total_green >= total_red)
        return 1;
    return 2;
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
    // Normalize the CDF to the range [0, 255].
    float cdf_normalized[256];
    for (int i = 0; i < 256; i++)
    {
        cdf_normalized[i] = (float)cdf[i] * 255.0 / cdf[255];
    }

    // Using the CDF to equalize pixel values
    for (int i = 0; i < size; i++)
    {
        result[i] = (int)cdf_normalized[pixels[i]];
    }
}
