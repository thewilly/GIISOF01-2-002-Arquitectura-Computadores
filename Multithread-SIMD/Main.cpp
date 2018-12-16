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
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <stdexcept>
#include <sys/sysinfo.h>

using namespace cimg_library;

#define ALGORITHM_REPETITION_TIMES 25
#define NUMBER_OF_CORES 4
#define THREADS_PER_CORE 2
#define SIMD_BANDWITH 128

const int NUMBER_OF_THREADS = NUMBER_OF_CORES * THREADS_PER_CORE;
const float SQRT2 = sqrt(2);

// SIMD PROPERTIES
const __m128 V_SQRT2 = _mm_set1_ps(sqrtf(2.0f)); // Loading a __m128 vec. with sqrtf(2.0f);
const int PIXELS_PER_DATA_PACKAGE = SIMD_BANDWITH / (sizeof(float)*CHAR_BIT);

/**
 * Information struct for the worker, it contains the pointers to the images,
 * the start position to apply the algorithm and the end position where it
 * will stop applying the algorithm.
 */
struct WorkerInfo {
	float *p_vimg1;
	float *p_vimg2;
	float *p_vimgres;
	int startPos;
	int endPos;
};

/**
 * A worker it is a thread that will apply the transformation function over the
 * specified information in the workerInfo.
 */
void *worker(void* workerInfo) {
	struct WorkerInfo *wi = (struct WorkerInfo*) workerInfo;
	__m128 dataPackageImage1;
	__m128 dataPackageImage2;
	__m128 squareDP1;
	__m128 squareDP2;
	__m128 additionOfSquaresDP12;
	__m128 squarerootDPS;
	__m128 division;

	if((wi->endPos - wi->startPos) %  PIXELS_PER_DATA_PACKAGE != 0) {
		printf("Not proper size of image for this program");
		exit(-1);
	}

	for(int i = wi->startPos; i < wi->endPos; i += PIXELS_PER_DATA_PACKAGE) {

		// Loading input images.
		dataPackageImage1 = _mm_loadu_ps(&wi->p_vimg1[i]);
		dataPackageImage2 = _mm_loadu_ps(&wi->p_vimg2[i]);

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
		_mm_storeu_ps(&wi->p_vimgres[i], division);
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

	// Check images sizes are multiple of the number of threads...
	if (IMAGES_SIZE % NUMBER_OF_THREADS != 0) {
		throw std::domain_error("Not proper size of image for this program");
	}

	// IMAGE POINTERS
	float *p_compImage1; // Pointer to the pixels of the input image 1
	float *p_compImage2; // Pointer to the pixel of input image 2
	float *p_dstImage; // Pointer to the new image pixels

	// THREADS SETTINGS
	const int PIXELS_PER_THREAD = IMAGES_SIZE/NUMBER_OF_THREADS;
	pthread_t th[NUMBER_OF_THREADS];

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

	// Measure initial time
	if (clock_gettime(CLOCK_REALTIME, &tStart) < 0) {
		printf("\n clock_gettime: %d.\n", errno);
		throw std::runtime_error("Error measuring initial time");
	}

	for (int repetitions = 0; repetitions < ALGORITHM_REPETITION_TIMES; repetitions++) {

		// ---- ALGORITHM STARTS ----

		for(int n_thread = 0; n_thread < NUMBER_OF_THREADS; n_thread++) {
			struct WorkerInfo *wi = (struct WorkerInfo*) malloc(sizeof(*wi));
			wi->startPos = n_thread * PIXELS_PER_THREAD;
			wi->endPos = wi->startPos + PIXELS_PER_THREAD;
			wi->p_vimg1 = p_compImage1;
			wi->p_vimg2 = p_compImage2;
			wi->p_vimgres = p_dstImage;

			pthread_create(&th[n_thread], NULL, worker, ( void* ) wi);
		}

		// Waiting for all the threads to end.
		for(int n_thread = 0; n_thread < NUMBER_OF_THREADS; n_thread++) {
			pthread_join(th[n_thread], NULL);
		}

		// ---- ALGORITHM ENDS ----
	}

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
