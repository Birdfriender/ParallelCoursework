// ParallelCoursework.c : Defines the entry point for the console application.
//

#include "stdafx.h"
const int ARRAY_SEED = 38423;

const double EPSILON = 0.0001;
const int ARRAY_DIMENSIONS = 100;
const int NUM_THREADS = 16;
pthread_mutex_t row_mutex;
pthread_mutex_t array_mutex;
pthread_mutex_t epsilon_mutex;
pthread_barrier_t barrier;

double array[100][100];
double resultArray[100][100];

double testEpsilon = 0;

struct Rows
{
	int startRow;
	int endRow;
};

void *row_thread(struct Rows* rows)
{
	int start = rows->startRow, end = rows->endRow;
	double change = 0;
	do
	{
		for (int i = start; i < end; i++)
		{
			for (int j = 0; j < ARRAY_DIMENSIONS; j++)
			{
				double result = (array[i - 1][j] +
					array[i][j + 1] +
					array[i + 1][j] +
					array[i][j - 1]) / 2;
				resultArray[i][j] = result;
				double diff = result - array[i][j];
				if (diff < 0)
					diff *= -1;
				if (diff > change)
					change = diff;
			}
		}
		pthread_mutex_lock(&epsilon_mutex);
		if (change > testEpsilon)
			testEpsilon = change;
		pthread_mutex_unlock(&epsilon_mutex);
		pthread_barrier_wait(&barrier);
	} while (testEpsilon < EPSILON);
}

int main()
{
	populateArray(ARRAY_SEED);
	pthread_barrier_init(&barrier, NULL, NUM_THREADS);
	int rowsPerThread = ARRAY_DIMENSIONS / NUM_THREADS;
	int modRows = ARRAY_DIMENSIONS % NUM_THREADS;
	pthread_t thr1, thr2, thr3, thr4, thr5, thr6, thr7, thr8, thr9, thr10, thr11, thr12, thr13, thr14, thr15, thr16;
	struct Rows row1; row1.startRow = 0; row1.endRow;
	
	//pthread_create(&thr1, NULL, (void*(*)(void*))row_thread, );

    return 0;
}

void populateArray(int seed)
{
	srand(seed);
	for (int i = 0; i < ARRAY_DIMENSIONS; i++)
	{
		for (int j = 0; j < ARRAY_DIMENSIONS; j++)
		{
			array[i][j] = rand();
		}
	}
}

