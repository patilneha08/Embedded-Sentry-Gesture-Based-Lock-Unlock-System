#ifndef UTILS_H 
#define UTILS_H

// Include the macros file for defining constants like MAX_ARRAY_SIZE
#include "macros.hpp"

/**
 * @brief Standard scaler function to normalize the data.
 * 
 * This function scales the input 2D array by standardizing each of the 
 * features (columns) independently to have a mean of 0 and a standard 
 * deviation of 1. This is useful for preparing data before applying 
 * machine learning algorithms, where features with different scales 
 * can affect performance.
 * 
 * @param arr 2D array of float values [MAX_ARRAY_SIZE][3] to be normalized
 */
void standard_scaler(float arr[MAX_ARRAY_SIZE][3]);

#endif 
