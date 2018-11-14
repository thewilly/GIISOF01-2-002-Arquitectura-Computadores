/*
 * Main.cpp
 *
 * Authors: Guillermo Facundo Colunga,
 *      	Álvaro Baños Gómez,
 *      	Alejandro....,
 *      	4º integrante.
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <stdexcept>

using namespace cimg_library;


int main() {

	// IMAGES SETTINGS
	CImg<float> srcImage1("bailarina.bmp"); // First image
	CImg<float> srcImage2("figura.bmp"); // Second image

	// FIRST IMAGE POINTERS
	float *pRcomp_1, *pGcomp_1, *pBcomp_1; // Pointers to the R, G and B components

	// SECOND IMAGE POINTERS
	float *pRcomp_2, *pGcomp_2, *pBcomp_2; // Pointers to the R, G and B components

	// NEW IMAGE POINTERS AND ATTRIBUTES
	float *pRnew, *pGnew, *pBnew;
	float *pdstImage; // Pointer to the new image pixels
	int width, height, nPixels; // Width and height of the image
	int nComp; // Number of image components

	// BENCHMARK SETTINGS
	struct timespec tStart, tEnd;
	double dElapsedTimeS;
	const int ALGORITHM_REPETITION_TIMES = 3;

	// OTHER CONSTANTS
	const float SQRT2 = sqrt(2);


	/***************************************************
	 *
	 * Variables initialization.
	 * Preparation of the necessary elements for the algorithm
	 * Out of the benchmark time
	 *
	 */

	srcImage1.display(); // Show the first image.
	srcImage2.display(); // show the second image.

	if(srcImage1.height() != srcImage2.height() || srcImage1.width() != srcImage2.width() || srcImage1.spectrum() != srcImage2.spectrum()) {
		throw std::domain_error("Both images must have the same size");
	}

	width = srcImage1.width(); // Getting information from the source image
	height  = srcImage1.height();
	nComp = srcImage1.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)
	nPixels = height*width;

	// PROCESSING THE POINTERS FOR INPUT IMAGES

	// Pointers to the RGB arrays of the source image 1
	pRcomp_1 = srcImage1.data(); // pRcomp_1 points to the R component
	pGcomp_1 = pRcomp_1 + nPixels; // pGcomp points to the G component
	pBcomp_1 = pGcomp_1 + nPixels; // pBcomp points to B component

	// Pointers to the RGB arrays of the source image 2
	pRcomp_2 = srcImage2.data(); // pRcomp_2 points to the R component
	pGcomp_2 = pRcomp_2 + nPixels; // pGcomp_2 points to the G component
	pBcomp_2 = pGcomp_2 + nPixels; // pBcomp_2 points to B component

	// CREATING THE OUTPUT IMAGE AND ITS POINTERS

	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (float *) malloc (width * height * nComp * sizeof(float));
	if (pdstImage == NULL) {
	    throw std::bad_alloc();
	}

	// Pointers to the RGB arrays of the destination image
	pRnew = pdstImage;
	pGnew = pRnew + nPixels;
	pBnew = pGnew + nPixels;

	/*********************************************
	 * Algorithm start
	 *
	 * Measure initial time
	 */
	if (clock_gettime(CLOCK_REALTIME, &tStart) == -1) {
		printf("\n clock_gettime: %d.\n", errno);
		throw std::runtime_error("Error measuring initial time");
	}


	/************************************************
	 * Algorithm.
	 */
	for(int repetitions = 0; repetitions < ALGORITHM_REPETITION_TIMES; repetitions++) {

	  for (int i = 0; i < nPixels; i++) {
        *pRnew = sqrtf(pow(*pRcomp_1, 2.0f) + pow(*pRcomp_2, 2.0f)) / SQRT2;
        *pGnew = sqrtf(pow(*pGcomp_1, 2.0f) + pow(*pGcomp_2, 2.0f)) / SQRT2;
        *pBnew = sqrtf(pow(*pBcomp_1, 2.0f) + pow(*pBcomp_2, 2.0f)) / SQRT2;

        pRnew++; pRcomp_1++; pRcomp_2++;
        pGnew++; pGcomp_1++; pGcomp_2++;
        pBnew++; pBcomp_1++; pBcomp_2++;
      }

	  // RESTORING THE POINTERS FOR NEXT REPETITION.

	  if(repetitions != (ALGORITHM_REPETITION_TIMES-1)) {

        // Pointers to the RGB arrays of the source image 1
        pRcomp_1 = srcImage1.data(); // pRcomp_1 points to the R component
        pGcomp_1 = pRcomp_1 + nPixels; // pGcomp points to the G component
        pBcomp_1 = pGcomp_1 + nPixels; // pBcomp points to B component

        // Pointers to the RGB arrays of the source image 2
        pRcomp_2 = srcImage2.data(); // pRcomp_2 points to the R component
        pGcomp_2 = pRcomp_2 + nPixels; // pGcomp_2 points to the G component
        pBcomp_2 = pGcomp_2 + nPixels; // pBcomp_2 points to B component

        // Pointers to the RGB arrays of the destination image
        pRnew = pdstImage;
        pGnew = pRnew + nPixels;
        pBnew = pGnew + nPixels;
	  }
	}

	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
	 */
	// End time measurement
	if (clock_gettime(CLOCK_REALTIME, &tEnd) == -1) {
		printf("\n clock_gettime: %d.\n", errno);
		throw std::runtime_error("Error measuring initial time");
	}

	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf("\n Tiempo ejecución: %fs", dElapsedTimeS);
	printf("\n Número de repeticiones: %i", ALGORITHM_REPETITION_TIMES);
	printf("\n Tiempo individual algoritmo: %fs", (dElapsedTimeS/(float)ALGORITHM_REPETITION_TIMES));

		
	// Create a new image object with the calculated pixels
	// In case of normal color image use nComp=3,
	// In case of B&W image use nComp=1.
	CImg<float> dstImage(pdstImage, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("bailarina3.bmp");

	// Display the destination image
	dstImage.display(); // If needed, show the result image
	return(0);
}
