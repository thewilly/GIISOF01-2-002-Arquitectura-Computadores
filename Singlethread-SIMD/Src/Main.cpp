/*
 *  Created on: 6 oct. 2018
 *      Author: arias
 */

 // Example of use of intrinsic functions using aligned an unaligned arrays
 // This example doensn't include any code about image processing
 // The image processing code have to be added by the students and remove
 // the unnecessary code.
 
#include <CImg.h>
#include <immintrin.h> // Required to use intrinsic functions
#include <malloc.h>
#include <stdio.h>

using namespace cimg_library;

#define TAM 11 // Array size

int main()
{
	/********************************
	 * Here: variable initialization for handling images
	 */
	
	float a[TAM], b[TAM], c[TAM]; // Unaligned arrays of floats in memory
	
	// Create three float arrays aligned to 32 bytes memory boundaries
    float *a_aligned  = (float *)_mm_malloc(sizeof(float) * TAM, sizeof(__m256));
    float *b_aligned  = (float *)_mm_malloc(sizeof(float) * TAM, sizeof(__m256));
    float *c_aligned  = (float *)_mm_malloc(sizeof(float) * TAM, sizeof(__m256));

	__m256 va, vb, vc; // 32 bytes (256 bits) extended CPU registers

	// Initialize unaligned and aligned arrays
    for (int i = 0; i < TAM; i++)
    {
        a[i] = a_aligned[i] = i;      // a = a_aligned = 0, 1, 2, 3, …
        b[i] = b_aligned[i] = 2 * i;  // b = b_aligned = 0, 2, 4, 6, …
        c[i] = c_aligned[i] = -1;     // c = c_aligned = -1, -1, -1, …
    }	
	
	// Performs the addition of two aligned vectors, each vector containing 8 floats
    va = _mm256_load_ps(&a_aligned[0]); // va = a_aligned[0][1]…[7] = 0, 1, 2, 3,  4,  5,  6,  7
    vb = _mm256_load_ps(&b_aligned[0]); // vb = b_aligned[0][1]…[7] = 0, 2, 4, 6,  8, 10, 12, 14
    vc = _mm256_add_ps(va, vb);         // vc = va + vb =             0, 3, 6, 9, 12, 15, 18, 21
	
	 // Copy the result to the first locations into the c aligned array
    _mm256_store_ps(&c_aligned[0], vc);
	
	// Print the c_aligned array
    for (int i = 0; i < TAM; i++)
        printf("\nc_aligned[%d]: %f", i, c_aligned[i]);

    printf("\n\n-------------------------------\n");
	
	// Now, perform the same additions, but using unaligned arrays (performance decreases)
    va = _mm256_loadu_ps(&a[0]); // va = a[0][1]…[7] = 0, 1, 2, 3,  4,  5,  6,  7
    vb = _mm256_loadu_ps(&b[2]); // vb = b[0][1]…[7] = 0, 2, 4, 6,  8, 10, 12, 14
    vc = _mm256_add_ps(va, vb);  // vc = va + vb     = 0, 3, 6, 9, 12, 15, 18, 21

    // Copy the result to the first locations into the c unaligned array
    _mm256_storeu_ps(&c[0], vc);

	// Print the c unaligned array
    for (int i = 0; i < TAM; i++)
        printf("\nc[%d]: %f", i, c[i]);
    printf("\n\n-------------------------------\n");

	// Free memory allocated using _mm_malloc
	// It has to be freed with _mm_free
	_mm_free(a_aligned);
	_mm_free(b_aligned);
	_mm_free(c_aligned);
	
	return(0);
}


