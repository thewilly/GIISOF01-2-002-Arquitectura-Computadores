/*
 * Main.cpp
 *
 * Authors: Álvaro Baños Gómez - UO245852,
 * 			Guillermo Facundo Colunga - UO236856,
 *      	Alejandro León Pereira - UO258774,
 *      	Iñaki Salgado Uralde - UO237133.
 */

#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <stdexcept>

using namespace cimg_library;

#define ALGORITHM_REPETITION_TIMES 25

int main() {

	// IMAGES SETTINGS
	CImg<float> srcImage1("bailarina.bmp");
	CImg<float> srcImage2("figura.bmp");

	// INPUT IMAGE POINTERS
	float *p_inputImage1;
	float *p_inputImage2;

	// NEW IMAGE POINTERS AND ATTRIBUTES
	float *p_outputImage;
	int width, height, imagesSize;
	int nComp;

	// BENCHMARK SETTINGS
	struct timespec tStart, tEnd;
	double dElapsedTimeS = 0.0;

	// OTHER CONSTANTS
	const float SQRT2 = sqrt(2);

	// First, check that the input images are suitable for our program.
	if (srcImage1.height() != srcImage2.height()
			|| srcImage1.width() != srcImage2.width()
			|| srcImage1.spectrum() != srcImage2.spectrum()) {

		throw std::domain_error("Both images must have the same size.");
	}

	// PRE-ALGORITHM COMPUTATIONS

	srcImage1.display(); // Show the first image.
	srcImage2.display(); // show the second image.

	// Setting the parameters of the output image.
	width = srcImage1.width();
	height = srcImage1.height();
	nComp = srcImage1.spectrum();
	imagesSize = width * height * nComp;

	// PROCESSING THE POINTERS FOR THE INPUT IMAGES

	// Pointers to the RGB arrays of the source image 1
	p_inputImage1 = srcImage1.data(); // pRcomp_1 points to the R component

	// Pointers to the RGB arrays of the source image 2
	p_inputImage2 = srcImage2.data(); // pRcomp_2 points to the R component

	// CREATING THE OUTPUT IMAGE AND ITS POINTERS

	// Allocate memory space for the pixels of the destination (processed) image 
	p_outputImage = (float *) malloc(width * height * nComp * sizeof(float));
	if (p_outputImage == NULL) {
		throw std::bad_alloc();
	}

	// Initial time measure
	if (clock_gettime(CLOCK_REALTIME, &tStart) == -1) {
		printf("\n clock_gettime: %d.\n", errno);
		throw std::runtime_error("Error measuring initial time");
	}

	// Loop to repeat the algorithm ALGORITHM_REPETITION_TIMES times.
	for (int repetitions = 0; repetitions < ALGORITHM_REPETITION_TIMES; repetitions++) {

		// ---- ALGORITHM STARTS HERE ----
		for (int i = 0; i < imagesSize; i++) {
			p_outputImage[i] = sqrtf(pow(p_inputImage1[i], 2.0f) + pow(p_inputImage2[i], 2.0f)) / SQRT2;
		}
		// ---- ALGORITHM ENDS HERE ----
	}

	// End time measurement
	if (clock_gettime(CLOCK_REALTIME, &tEnd) == -1) {
		printf("\n clock_gettime: %d.\n", errno);
		throw std::runtime_error("Error measuring initial time");
	}

	dElapsedTimeS += (tEnd.tv_sec - tStart.tv_sec);
	dElapsedTimeS += (tEnd.tv_nsec - tStart.tv_nsec) / 1e+9;

	// Printing the execution time data.
	printf("\n Tiempo ejecución: %fs", dElapsedTimeS);
	printf("\n Número de repeticiones: %i", ALGORITHM_REPETITION_TIMES);
	printf("\n Tiempo individual algoritmo: %fs \n",
			(dElapsedTimeS / (float) ALGORITHM_REPETITION_TIMES));

	// Create a new image object with the calculated pixels
	// In case of normal color image use nComp=3,
	// In case of B&W image use nComp=1.
	CImg<float> dstImage(p_outputImage, width, height, 1, nComp);

	// Store the destination image in disk
	dstImage.save("imagen_fusionada.bmp");

	// Display the destination image
	dstImage.display();

	// Exit the application.
	return (0);
}
