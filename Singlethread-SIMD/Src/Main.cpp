/*
 *  Created on: 6 oct. 2018
 *      Author: arias
 */

#include <CImg.h>
#include <immintrin.h> // Required to use intrinsic functions
#include <malloc.h>
#include <stdio.h>
#include <stdexcept>

using namespace cimg_library;

#define SIMD_BANDWITH 8

int main() {
	// IMAGES SETTINGS
		CImg<float> srcImage1("bailarina.bmp"); // First image
		CImg<float> srcImage2("figura.bmp"); // Second image

		// FIRST IMAGE POINTERS
		float *pcompImage1; // Pointer to the pixels of the image 1

		// SECOND IMAGE POINTERS
		float *pcompImage2; // Pointer to the pixel of image 2

		// NEW IMAGE POINTERS AND ATTRIBUTES
		float *pdstImage; // Pointer to the new image pixels
		int width, height; // Width and height of the image
		int nComp; // Number of image components

		// BENCHMARK SETTINGS
		struct timespec tStart, tEnd;
		double dElapsedTimeS;
		const int ALGORITHM_REPETITION_TIMES = 40;

		// SMID SETTINGS
		int size = srcImage1._height*srcImage1._width*srcImage1._spectrum;
		//__m256 a, b, a2, b2, ab2, raizab2, res8;

		__m256 vsqrt2;

		float sqrt2 = sqrtf(2.0f);
		vsqrt2[0] = sqrt2;
		vsqrt2[1] = sqrt2;
		vsqrt2[2] = sqrt2;
		vsqrt2[3] = sqrt2;
		vsqrt2[4] = sqrt2;
		vsqrt2[5] = sqrt2;
		vsqrt2[6] = sqrt2;
		vsqrt2[7] = sqrt2;


		srcImage1.display(); // Show the first image.
		srcImage2.display(); // show the second image.

		if (srcImage1.height() != srcImage2.height() || srcImage1.width() != srcImage2.width() || srcImage1.spectrum() != srcImage2.spectrum()) {
			throw std::domain_error("Both images must have the same size");
		}


		width = srcImage1.width(); // Getting information from the source image
		height = srcImage1.height();
		nComp = srcImage1.spectrum(); // source image number of components
		// Common values for spectrum (number of image components):
		//  B&W images = 1
		//	Normal color images = 3 (RGB)
		//  Special color images = 4 (RGB and alpha/transparency channel)


		// Allocate memory space for the pixels of the destination (processed) image
		pdstImage = (float *) malloc(width * height * nComp * sizeof(float));
		if (pdstImage == NULL) {
			throw std::bad_alloc();
		}


		// Pointers to the array of the source image 1
		pcompImage1 = srcImage1.data();

		// Pointers to the array of the source image 2
		pcompImage2 = srcImage2.data();

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
			for (int i = 0; i < size; i += SIMD_BANDWITH) {
				/*
					P-SEUDOCODE VERSION (NOT TO INCLUDE IN FINAL RELEASE)

				a 		= 	_mm256_loadu_ps(&pcompImage1[i]);
				b 		= 	_mm256_loadu_ps(&pcompImage2[i]);
				a2 		= 	_mm256_mul_ps(a, a);
				b2 		= 	_mm256_mul_ps(b, b);
				ab2 	= 	_mm256_add_ps(a2, b2);
				raizab2 = 	_mm256_sqrt_ps(ab2);
				res8 	= 	_mm256_div_ps(raizab2, vsqrt2);

				 _mm256_storeu_ps(&pdstImage[i], res8);

				 */

				 _mm256_storeu_ps(&pdstImage[i],
						 _mm256_div_ps(
								 _mm256_sqrt_ps(
										 _mm256_add_ps(
												 _mm256_mul_ps(
														 _mm256_loadu_ps(&pcompImage1[i]),
														 _mm256_loadu_ps(&pcompImage1[i])
												 ),
												 _mm256_mul_ps(
														 _mm256_loadu_ps(&pcompImage2[i]),
														 _mm256_loadu_ps(&pcompImage2[i])
												)
										 )
								 ),
								 vsqrt2
						 )
				 );
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
		dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
		dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
		printf("\n Tiempo ejecución: %fs", dElapsedTimeS);
		printf("\n Número de repeticiones: %i", ALGORITHM_REPETITION_TIMES);
		printf("\n Tiempo individual algoritmo: %fs", (dElapsedTimeS / (float) ALGORITHM_REPETITION_TIMES));

		// Create a new image object with the calculated pixels
		// In case of normal color image use nComp=3,
		// In case of B&W image use nComp=1.
		CImg<float> dstImage(pdstImage, width, height, 1, nComp);

		// Store the destination image in disk
		dstImage.save("bailarina3.bmp");

		// Display the destination image
		dstImage.display(); // If needed, show the result image
		return (0);
}
