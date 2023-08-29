#ifndef EQUALIZATION
#define EQUALIZATION

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "slog.h"

/**
 * @brief Method for equalizing the image by means of the 
 *         equalization histogram
 * 
 * @param pixels 
 * @param result 
 * @param size 
 * @return int 
 */
int equalize(int *pixels, unsigned char *result, int size);

/**
 * @brief Gets the array of a specific color
 * @param pixels 
 * @param result 
 * @param size 
 * @param mode 
 */
void get_color(int *pixels, int *result, int size, int mode);

/**
 * @brief Get the frequency of a pixel value
 * 
 * @param pixels 
 * @param result 
 * @param size 
 */
void get_frequency(int *pixels, int *result, int size);

/**
 * @brief Get the CDF 
 * @param frequency 
 * @param result 
 */
void get_cdf(int *frequency, int *result);

/**
 * @brief Set the new array with the equalized pixels
 * 
 * @param cdf 
 * @param pixels 
 * @param result 
 * @param size 
 */
void new_pixels(int *cdf, int *pixels, int *result, int size);

/**
 * @brief Verifies which color is predominant in an image
 * @param red 
 * @param green 
 * @param blue 
 * @param size 
 * @return int 
 */
int categorizer(int *red, int *green, int *blue, int size);

#endif