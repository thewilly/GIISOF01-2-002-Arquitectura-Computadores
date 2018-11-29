/*
 *  Created on: 6 oct. 2018
 *      Author:
 */

#include <CImg.h>
#include <immintrin.h>
#include <malloc.h>
#include <stdio.h>
#include <stdexcept>

using namespace cimg_library;

#define SIMD_BANDWITH 128
#define ALGORITHM_REPETITION_TIMES 80

int main() {

	// INPUT IMAGES
	CImg<float> srcImage1("bailarina.bmp"); // First image
	CImg<float> srcImage2("figura.bmp"); // Second image

	// First check that the images can be processed by our program.
	if (srcImage1.height() != srcImage2.height()
			|| srcImage1.width() != srcImage2.width()
			|| srcImage1.spectrum() != srcImage2.spectrum()) {
		throw std::domain_error("Both images must have the same size");
	}

	// Second check that the images size is a multiple of SIMD_BANDWITH / sizeOf(float)

	// Then show the initial images.
	srcImage1.display(); // Show the first image.
	srcImage2.display(); // show the second image.

	// IMAGES PROPERTIES
	const int IMAGES_WIDTH = srcImage1.width();
	const int IMAGES_HEIGHT = srcImage1.height();
	const int IMAGES_N_COMPONENTS = srcImage1.spectrum();
	const int IMAGES_SIZE = IMAGES_WIDTH * IMAGES_HEIGHT * IMAGES_N_COMPONENTS;

	// IMAGE POINTERS
	float *p_compImage1; // Pointer to the pixels of the input image 1
	float *p_compImage2; // Pointer to the pixel of input image 2
	float *p_dstImage; // Pointer to the new image pixels

	// SIMD PROPERTIES
	const __m128 V_SQRT2 = _mm_set1_ps(sqrtf(2.0f)); // Loading a __m128 vec. with sqrtf(2.0f);
	const int PACKAGE_DATA_SIZE = SIMD_BANDWITH / (sizeof(float)*CHAR_BIT);
	__m128 image1, image2;

	// BENCHMARK SETTINGS
	struct timespec tStart, tEnd;
	double dElapsedTimeS;


	// INITIALIZING POINTERS
	p_compImage1 = srcImage1.data(); // Pointers to the array of the source image 1
	p_compImage2 = srcImage2.data(); // Pointers to the array of the source image 2
	p_dstImage = (float *) malloc(IMAGES_SIZE * sizeof(float)); // Initializing the destination image pointer

	// If the allocation of memory fails...
	if (p_dstImage == NULL) {
		throw std::bad_alloc();
	}

	/*********************************************
	 * Algorithm start
	 *
	 * Measure initial time
	 */
	if (clock_gettime(CLOCK_REALTIME, &tStart) < 0) {
		printf("\n clock_gettime: %d.\n", errno);
		throw std::runtime_error("Error measuring initial time");
	}

	/************************************************
	 * Algorithm.
	 */

	for (int repetitions = 0; repetitions < ALGORITHM_REPETITION_TIMES; repetitions++) {
		for (int i = 0; i < IMAGES_SIZE; i += PACKAGE_DATA_SIZE) {

			image1 = _mm_loadu_ps(&p_compImage1[i]); // loading the first image.
			image2 = _mm_loadu_ps(&p_compImage2[i]); // loading the second image.

			_mm_storeu_ps( 					// Store
				&p_dstImage[i],				// At destination image
				_mm_div_ps(					// The division
					_mm_sqrt_ps(			// Of the square root
						_mm_add_ps(			// Of the addition
							_mm_mul_ps(		// Of the multiplication
								image1,		// Of the first image
								image1),	// By the first image
							_mm_mul_ps(		// And the multiplication
								image2,		// Of the second image
								image2))),	// By itself
					V_SQRT2));				// Finally the division is by the square root of 2.
		}
	}

	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
	 */

	// End time measurement
	if (clock_gettime(CLOCK_REALTIME, &tEnd) < 0) {
		printf("\n clock_gettime: %d.\n", errno);
		throw std::runtime_error("Error measuring initial time");
	}

	// Calculating the spent time
	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf("\n Tiempo ejecución: %fs", dElapsedTimeS);
	printf("\n Número de repeticiones: %i", ALGORITHM_REPETITION_TIMES);
	printf("\n Tiempo individual algoritmo: %fs", (dElapsedTimeS / (float) ALGORITHM_REPETITION_TIMES));

	// Create a new image object with the calculated pixels
	CImg<float> dstImage(p_dstImage, IMAGES_WIDTH, IMAGES_HEIGHT, 1, IMAGES_N_COMPONENTS);

	// Store the destination image in disk
	dstImage.save("bailarina3.bmp");

	// Display the destination image
	dstImage.display();

	// Exit
	return (0);
}
