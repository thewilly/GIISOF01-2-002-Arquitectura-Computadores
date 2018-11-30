/*
 * Main.cpp
 *
 *  Created on: 09/10/2018
 *      Author: student
 */
#include <CImg.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <stdexcept>
#include <sys/sysinfo.h>

using namespace cimg_library;

#define ALGORITHM_REPETITION_TIMES 80
#define NUMBER_OF_CORES 4
#define THREADS_PER_CORE 2

const int NUMBER_OF_THREADS = NUMBER_OF_CORES * THREADS_PER_CORE;
const float SQRT2 = sqrt(2);

struct WorkerInfo {
	float *p_vimg1;
	float *p_vimg2;
	float *p_vimgres;
	int startPos;
	int endPos;
};

float fusionFunction(float pixel1, float pixel2) {
	return sqrtf(pow(pixel1, 2.0f) + pow(pixel2, 2.0f)) / SQRT2;
}

void *worker(void* taskInfo) {
	struct WorkerInfo *wi = (struct WorkerInfo*) taskInfo;

	for(int i = wi->startPos; i < wi->endPos; i++) {
		wi->p_vimgres[i] = fusionFunction(wi->p_vimg1[i], wi->p_vimg2[i]);
	}
	return wi;
}

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

	// BENCHMARK SETTINGS
	struct timespec tStart, tEnd;
	double dElapsedTimeS;

	pthread_t th[NUMBER_OF_THREADS];

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
		for(int n_thread = 0; n_thread < NUMBER_OF_THREADS; n_thread++) {
			struct WorkerInfo *wi = (struct WorkerInfo*)malloc(sizeof(*wi));
			wi->startPos = n_thread * (IMAGES_SIZE/NUMBER_OF_THREADS);
			wi->endPos = (n_thread+1) * (IMAGES_SIZE/NUMBER_OF_THREADS);
			wi->p_vimg1 = p_compImage1;
			wi->p_vimg2 = p_compImage2;
			wi->p_vimgres = p_dstImage;

			pthread_create(&th[n_thread], NULL, worker, ( void* )wi);
		}

		for(int n_thread = 0; n_thread < NUMBER_OF_THREADS; n_thread++) {
			pthread_join(th[n_thread], NULL);
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
	printf("\n Tiempo individual algoritmo: %fs",
			(dElapsedTimeS / (float) ALGORITHM_REPETITION_TIMES));
	printf("\n Se han empleado %d cores.", NUMBER_OF_CORES);
	printf("\n Se han empleado %d threads.", NUMBER_OF_THREADS);

	// Create a new image object with the calculated pixels
	CImg<float> dstImage(p_dstImage, IMAGES_WIDTH, IMAGES_HEIGHT, 1,
			IMAGES_N_COMPONENTS);

	// Store the destination image in disk
	dstImage.save("bailarina3.bmp");

	// Display the destination image
	dstImage.display();

	// Exit
	return (0);

}
