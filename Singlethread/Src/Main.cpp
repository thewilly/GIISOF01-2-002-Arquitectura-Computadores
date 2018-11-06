/*
 * Main.cpp
 *
 *  Created on: 13 sept. 2018
 *      Author: arias
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
using namespace cimg_library;

/**********************************
 * TODO
 * 	- Change the data type returned by CImg.srcImage to adjust the
 * 	requirements of your workgroup
 * 	- Change the data type of the components pointers to adjust the
 * 	requirements of your workgroup
 */

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
	int width, height; // Width and height of the image
	int nComp; // Number of image components

	// BENCHMARK SETTINGS
	struct timespec tStart, tEnd;
	double dElapsedTimeS;


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
		printf("\n Both images must have same dimensions.");
		//exit(-2);
	}

	width = srcImage1.width(); // Getting information from the source image
	height  = srcImage1.height();
	nComp = srcImage1.spectrum(); // source image number of components
				// Common values for spectrum (number of image components):
				//  B&W images = 1
				//	Normal color images = 3 (RGB)
				//  Special color images = 4 (RGB and alpha/transparency channel)


	// Allocate memory space for the pixels of the destination (processed) image 
	pdstImage = (float *) malloc (width * height * nComp * sizeof(float));
	if (pdstImage == NULL) {
		printf("\nMemory allocating error\n");
		exit(-2);
	}

	// Pointers to the RGB arrays of the source image 1
	pRcomp_1 = srcImage1.data(); // pRcomp_1 points to the R component
	pGcomp_1 = pRcomp_1 + height * width; // pGcomp points to the G component
	pBcomp_1 = pGcomp_1 + height * width; // pBcomp points to B component

	// Pointers to the RGB arrays of the source image 2
	pRcomp_2 = srcImage2.data(); // pRcomp_2 points to the R component
	pGcomp_2 = pRcomp_2 + height * width; // pGcomp_2 points to the G component
	pBcomp_2 = pGcomp_2 + height * width; // pBcomp_2 points to B component

	// Pointers to the RGB arrays of the destination image
	pRnew = pdstImage;
	pGnew = pRnew + height * width;
	pBnew = pGnew + height * width;

	/*********************************************
	 * Algorithm start
	 *
	 * Measure initial time
	 *
	 *	COMPLETE
	 *
	 */

	// Start time measurement
	clock_gettime(CLOCK_REALTIME, &tStart);


	/************************************************
	 * Algorithm.
	 * In this example, the algorithm is a components exchange
	 *
	 * TO BE REPLACED BY YOUR ALGORITHM
	 */

	for (int i = 0; i < height ; i++) {
		for (int j=0; j < width; j++)  {
			pRnew[i * width + j] = sqrtf( pow(pRcomp_1[i * width + j], 2.0f) + pow(pRcomp_2[i * width + j], 2.0f) ) / sqrtf(2.0f);
			pGnew[i * width + j] = sqrtf( pow(pGcomp_1[i * width + j], 2.0f) + pow(pGcomp_2[i * width + j], 2.0f) ) / sqrtf(2.0f);
			pBnew[i * width + j] = sqrtf( pow(pBcomp_1[i * width + j], 2.0f) + pow(pBcomp_2[i * width + j], 2.0f) ) / sqrtf(2.0f);
		}
	}

	/***********************************************
	 * End of the algorithm
	 *
	 * Measure the final time and calculate the time spent
	 *
	 * COMPLETE
	 *
	 */
	// Start time measurement
	clock_gettime(CLOCK_REALTIME, &tEnd);

	dElapsedTimeS = (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;
	printf("\n Tiempo ejecución: %fs", dElapsedTimeS);

		
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
