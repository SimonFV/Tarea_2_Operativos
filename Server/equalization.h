#ifndef EQUALIZATION
#define EQUALIZATION

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "slog.h"

int equalize(int *pixels, unsigned char *result, int size);
void get_color(int *pixels, int *result, int size, int mode);
void get_frequency(int *pixels, int *result, int size);
void get_cdf(int *frequency, int *result);
void new_pixels(int *cdf, int *pixels, int *result, int size);
int categorizer(int *red, int *green, int *blue, int size);

#endif