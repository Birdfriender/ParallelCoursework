// ParallelCoursework.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

const unsigned int ARRAY_SEED = 38423;

const double EPSILON = 0.01;
const unsigned int ARRAY_DIMENSIONS = 100;
const unsigned int NUM_THREADS = 16;
pthread_mutex_t epsilon_mutex;
pthread_barrier_t barrier;

double array[100][100];
double resultArray[100][100];

double highestChange = 0.0;

struct Rows
{
	unsigned int startRow;
	unsigned int endRow;
};

void row_thread(struct Rows* rows)
{
	printf("Thread starting\n");
	int start = rows->startRow, end = rows->endRow;
	do
	{
		pthread_barrier_wait(&barrier);
		double change = 0.0;
		for (unsigned int i = start; i < end; i++)
		{
			for (unsigned int j = 1; j < ARRAY_DIMENSIONS -1; j++)
			{
				double result = (array[i - 1][j] +
					array[i][j + 1] +
					array[i + 1][j] +
					array[i][j - 1]) / 4.0;
				resultArray[i][j] = result;
				double diff = result - array[i][j];
				if (diff < 0.0)
					diff *= -1.0;
				if (diff > change)
					change = diff;
			}
		}
		pthread_mutex_lock(&epsilon_mutex);
		if (change > highestChange)
			highestChange = change;
		pthread_mutex_unlock(&epsilon_mutex);
		pthread_barrier_wait(&barrier);
	} while (highestChange > EPSILON);
}

void populateArrayRand(int aSeed)
{
	srand(aSeed);
	for (int i = 0; i < ARRAY_DIMENSIONS; i++)
	{
		for (int j = 0; j < ARRAY_DIMENSIONS; j++)
		{
			array[i][j] = rand();
		}
	}
	memcpy(resultArray, array, sizeof(double) * ARRAY_DIMENSIONS * ARRAY_DIMENSIONS);
	printf("Populated Array\n");
}

int main()
{
	printf("Starting\n");
	populateArrayRand(ARRAY_SEED);
	pthread_barrier_init(&barrier, NULL, NUM_THREADS);
	int rowsPerThread = ARRAY_DIMENSIONS / NUM_THREADS;
	time_t startTime;
	startTime = time(NULL);

	pthread_mutex_init(&epsilon_mutex, NULL);

	struct Rows rows[16];
	for (int i = 0; i < NUM_THREADS; i++)
	{
		rows[i].startRow = (i * rowsPerThread) + 1;
		rows[i].endRow = ((i + 1) * rowsPerThread) + 1;
	}
	rows[NUM_THREADS - 1].endRow += ARRAY_DIMENSIONS % NUM_THREADS - 2;
	printf("Firing off threads\n");
	pthread_t threads[15];
	for (int i = 0; i < NUM_THREADS - 1; i++)
	{
		pthread_create(&threads[i], NULL, (void*(*)(void*))row_thread, &rows[i]);
	}

	//Not sure if this is necessary but eh
	//I'll fix it later if I need to
	{
		do
		{
			memcpy(array, resultArray, sizeof(double) * ARRAY_DIMENSIONS * ARRAY_DIMENSIONS);
			pthread_barrier_wait(&barrier);
			highestChange = 0;
			double change = 0.0;
			for (unsigned int i = rows[NUM_THREADS - 1].startRow; i < rows[NUM_THREADS - 1].endRow; i++)
			{
				for (unsigned int j = 1; j < ARRAY_DIMENSIONS - 1; j++)
				{
					double result = (array[i - 1][j] +
						array[i][j + 1] +
						array[i + 1][j] +
						array[i][j - 1]) / 4.0;
					resultArray[i][j] = result;
					double diff = result - array[i][j];
					if (diff < 0.0)
						diff *= -1.0;
					if (diff > change)
						change = diff;
				}
			}
			pthread_mutex_lock(&epsilon_mutex);
			if (change > highestChange)
				highestChange = change;
			pthread_mutex_unlock(&epsilon_mutex);
			pthread_barrier_wait(&barrier);
		} while (highestChange > EPSILON);
	}
	for (int i = 0; i < NUM_THREADS - 1; i++)
	{
		pthread_join(threads[i], NULL);
	}
	time_t endTime;
	endTime = time(NULL);
	char string[100] = "";
	sprintf(string, "thh37-results.txt");/*
	bytes = sprintf(string + bytes, asctime(&startTime));
	sprintf(string + bytes, ".txt");*/
	FILE *f = fopen(string, "w");
	printf("fopened");
	for (int i = 0; i < ARRAY_DIMENSIONS; i++)
	{
		for (int j = 0; j < ARRAY_DIMENSIONS; j++)
		{
			fprintf(f, "%lf ", array[i][j]);
		}
		fprintf(f, "\n");
	}
	fprintf(f, "\nTime taken: ");
	fprintf(f, "%lf", difftime(endTime, startTime));
	fclose(f);
    return 0;
}



