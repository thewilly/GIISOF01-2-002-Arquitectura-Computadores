/*
 * Main.cpp
 *
 * Authors: Álvaro Baños Gómez - UO245852,
 * 			Guillermo Facundo Colunga - UO236856,
 *      	Alejandro León Pereira - UO258774,
 *      	Iñaki Salgado Uralde - UO237133.
 */

#include <CImg.h>
#include <immintrin.h>
#include <malloc.h>
#include <stdio.h>
#include <stdexcept>

using namespace cimg_library;

#define SIMD_BANDWITH 128
#define ALGORITHM_REPETITION_TIMES 25

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
	const int PIXELS_PER_DATA_PACKAGE = SIMD_BANDWITH / (sizeof(float)*CHAR_BIT);
	__m128 dataPackageImage1;
	__m128 dataPackageImage2;
	__m128 squareDP1;
	__m128 squareDP2;
	__m128 additionOfSquaresDP12;
	__m128 squarerootDPS;
	__m128 division;

	// Second check that the images size is a multiple of SIMD_BANDWITH / sizeOf(float)
	if ( IMAGES_SIZE % PIXELS_PER_DATA_PACKAGE != 0) {
		throw std::domain_error("Those images sizes are not a multiple of the number of threads to use");
	}

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


	for (int repetitions = 0; repetitions < ALGORITHM_REPETITION_TIMES; repetitions++) {

		// ---- ALGORITHM STARTS ----

		for (int i = 0; i < IMAGES_SIZE; i += PIXELS_PER_DATA_PACKAGE) {

			// Loading input images.
			dataPackageImage1 = _mm_loadu_ps(&p_compImage1[i]);
			dataPackageImage2 = _mm_loadu_ps(&p_compImage2[i]);

			// Computing the squares.
			squareDP1 = _mm_mul_ps(dataPackageImage1, dataPackageImage1);
			squareDP2 = _mm_mul_ps(dataPackageImage2, dataPackageImage2);

			// Addition of the squares.
			additionOfSquaresDP12 = _mm_add_ps(squareDP1, squareDP2);

			// Computing the square root of the addition of the squares.
			squarerootDPS = _mm_sqrt_ps(additionOfSquaresDP12);

			// Computing the division of the squarerootDPS over the square root of 2.
			division = _mm_div_ps(squarerootDPS, V_SQRT2);

			// Storing the result on to the destination image information vector.
			_mm_storeu_ps(&p_dstImage[i], division);

		}

		// ---- ALGORITHM ENDS ----
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
