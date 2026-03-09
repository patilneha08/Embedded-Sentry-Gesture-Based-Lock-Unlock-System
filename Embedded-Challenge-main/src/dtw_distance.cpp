#include <cstring>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <string>

#include "dtw_distance.hpp"

float d[MAX_ARRAY_SIZE][MAX_ARRAY_SIZE] = { 0 };

/**
* Compute the p-norm distance between two 1D vectors.
*
* The p-norm is a generalization of various distance measures. Common examples include:
* - p=2 for Euclidean distance
* - p=1 for Manhattan distance
* 
* More information about p-norm can be found here:
* https://en.wikipedia.org/wiki/Norm_(mathematics)#p-norm
*
* @param a A 1D vector of size m, where m is the number of dimensions.
* @param b A 1D vector of size m (must match the size of vector a).
* @param a_b_dimLEN The number of dimensions in the vectors a and b.
* @param p The value of the norm (e.g., 1 for Manhattan, 2 for Euclidean).
* @return The p-norm distance between the vectors a and b.
*/
float p_norm(float a[3], float b[3], uint32_t a_b_dimLEN, float p) {
    float d = 0;
    // Calculate the p-norm distance between the vectors
    for (uint32_t i = 0; i < a_b_dimLEN; i++) {
        d += std::pow(std::abs(a[i] - b[i]), p);
    }
    return std::pow(d, 1.0 / p);
};

/**
* Compute the Dynamic Time Warping (DTW) distance between two 2D vectors.
*
* The DTW distance calculates the minimum distance between two time-series or sequences.
* It allows for alignment of sequences that may be out of phase, which makes it useful for tasks
* such as speech recognition or gesture recognition.
* 
* For more information about DTW, refer to:
* https://en.wikipedia.org/wiki/Dynamic_time_warping
*
* @param a A 2D array representing the first sequence, where each entry is [number_of_data_points][number_of_dimensions].
* @param a_dataPointLEN The number of data points in sequence a.
* @param a_dimLEN The number of dimensions for each data point in sequence a.
* @param b A 2D array representing the second sequence, formatted similarly to a.
* @param b_dataPointLEN The number of data points in sequence b.
* @param b_dimLEN The number of dimensions for each data point in sequence b.
* @param p The value of the p-norm to use when calculating distances.
* @return The DTW distance between sequences a and b.
*/
float dtw_distance_only(float a[MAX_ARRAY_SIZE][3], uint32_t a_dataPointLEN, uint32_t a_dimLEN,
    float b[MAX_ARRAY_SIZE][3], uint32_t b_dataPointLEN, uint32_t b_dimLEN, float p)

{
    uint32_t n = a_dataPointLEN;  // Number of data points in sequence a
    uint32_t o = b_dataPointLEN;  // Number of data points in sequence b
    uint8_t a_m = a_dimLEN;       // Number of dimensions in each data point for sequence a
    uint8_t b_m = b_dimLEN;       // Number of dimensions in each data point for sequence b
    
    // If the dimensions don't match, return an error value (-1)
    if (a_m != b_m)
    {
        return -1;  // Dimensions of sequences a and b must be the same
    }

    // Initialize the distance matrix (d) to store the DTW distance values
    memset(d, 0, sizeof d);
    
    // Set the starting point of the distance matrix as the p-norm distance between the first data points of both sequences
    d[0][0] = p_norm(a[0], b[0], a_dimLEN, p);  

    // Fill the first column of the distance matrix (for aligning sequence a with the first data point of sequence b)
    for (uint32_t i = 1; i < n; i++)
    {
        d[i][0] = d[i - 1][0] + p_norm(a[i], b[0], a_dimLEN, p);
    }

    // Fill the first row of the distance matrix (for aligning sequence b with the first data point of sequence a)
    for (uint32_t i = 1; i < o; i++)
    {
        d[0][i] = d[0][i - 1] + p_norm(a[0], b[i], a_dimLEN, p);
    }

    // Fill the rest of the distance matrix by considering all possible alignments
    for (uint32_t i = 1; i < n; i++)
    {
        for (uint32_t j = 1; j < o; j++) {
            // Calculate the DTW distance considering the p-norm and previous alignments
            d[i][j] = p_norm(a[i], b[j], a_dimLEN, p) + std::fmin(std::fmin(d[i - 1][j], d[i][j - 1]), d[i - 1][j - 1]);
        }
    }
    
    // Return the final DTW distance, which is the distance between the last points of both sequences
    return d[n - 1][o - 1];
};
