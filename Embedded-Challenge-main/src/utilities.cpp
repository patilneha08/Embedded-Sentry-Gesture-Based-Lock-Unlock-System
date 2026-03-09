#include <cmath>

#include "utilities.hpp"
#include "macros.hpp"

/**
 * @brief Calculates the mean (average) of each dimension (x, y, z) from the input 2D array.
 * 
 * This function computes the mean of the x, y, and z values separately across all 
 * data points in the input array. It is useful for normalizing data by centering it around zero.
 * 
 * @param arr 2D array containing the data points with dimensions [MAX_ARRAY_SIZE][3].
 * @param mean_x Pointer to store the mean of the x-values.
 * @param mean_y Pointer to store the mean of the y-values.
 * @param mean_z Pointer to store the mean of the z-values.
 */
void calculate_mean(
    const float arr[MAX_ARRAY_SIZE][3],
    float *mean_x,
    float *mean_y,
    float *mean_z
) {
    float sum_x = 0.0;
    float sum_y = 0.0;
    float sum_z = 0.0;

    // Iterate through each data point and accumulate the values for each dimension
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        sum_x += arr[i][0];
        sum_y += arr[i][1];
        sum_z += arr[i][2];
    }

    // Calculate the mean for each dimension
    *mean_x = sum_x / MAX_ARRAY_SIZE;
    *mean_y = sum_y / MAX_ARRAY_SIZE;
    *mean_z = sum_z / MAX_ARRAY_SIZE;
}

/**
 * @brief Calculates the standard deviation for each dimension (x, y, z) from the input 2D array.
 * 
 * This function computes the standard deviation for each of the x, y, and z dimensions. 
 * The standard deviation gives a measure of the spread of the data points from the mean.
 * 
 * @param arr 2D array containing the data points with dimensions [MAX_ARRAY_SIZE][3].
 * @param std_x Pointer to store the standard deviation of the x-values.
 * @param std_y Pointer to store the standard deviation of the y-values.
 * @param std_z Pointer to store the standard deviation of the z-values.
 * @param mean_x The mean of the x-values.
 * @param mean_y The mean of the y-values.
 * @param mean_z The mean of the z-values.
 */
void calculate_std(
    const float arr[MAX_ARRAY_SIZE][3],
    float *std_x,
    float *std_y,
    float *std_z,
    float mean_x,
    float mean_y,
    float mean_z
) {
    float sum_x = 0.0;
    float sum_y = 0.0;
    float sum_z = 0.0;

    // Iterate through each data point and accumulate squared differences from the mean
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        sum_x += std::pow(mean_x - arr[i][0], 2);
        sum_y += std::pow(mean_y - arr[i][1], 2);
        sum_z += std::pow(mean_z - arr[i][2], 2);
    }

    // Calculate the standard deviation for each dimension
    *std_x = std::sqrt(sum_x / MAX_ARRAY_SIZE);
    *std_y = std::sqrt(sum_y / MAX_ARRAY_SIZE);
    *std_z = std::sqrt(sum_z / MAX_ARRAY_SIZE);
}

/**
 * @brief Standardizes (normalizes) the input array by applying the standard scaler.
 * 
 * This function standardizes the data by subtracting the mean and dividing by the 
 * standard deviation for each dimension (x, y, z). It ensures the data has a mean of 0 
 * and a standard deviation of 1, which can help with convergence when using machine learning 
 * algorithms or comparing gestures.
 * 
 * @param arr 2D array containing the data points with dimensions [MAX_ARRAY_SIZE][3].
 */
void standard_scaler(float arr[MAX_ARRAY_SIZE][3]) {
    float mean_x = 0;
    float mean_y = 0;
    float mean_z = 0;

    float std_x = 0;
    float std_y = 0;
    float std_z = 0;

    // Calculate the mean of each dimension
    calculate_mean(arr, &mean_x, &mean_y, &mean_z);

    // Calculate the standard deviation of each dimension
    calculate_std(arr, &std_x, &std_y, &std_z, mean_x, mean_y, mean_z);

    // Standardize the data by subtracting the mean and dividing by the standard deviation
    for (int i = 0; i < MAX_ARRAY_SIZE; i++) {
        arr[i][0] = (arr[i][0] - mean_x) / std_x;
        arr[i][1] = (arr[i][1] - mean_y) / std_y;
        arr[i][2] = (arr[i][2] - mean_z) / std_z;
    }
}
