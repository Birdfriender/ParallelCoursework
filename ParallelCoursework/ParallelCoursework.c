// ParallelCoursework.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

const unsigned int ARRAY_SEED = 332484;

const double EPSILON = 1.0;
unsigned long ARRAY_DIMENSIONS;
unsigned long NUM_THREADS;
unsigned long loopIt; //this is to generate a unique filename not actually part of the functionality
pthread_mutex_t epsilon_mutex;
pthread_barrier_t barrier;

double **array;
double **resultArray;

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
		pthread_barrier_wait(&barrier);
		pthread_mutex_lock(&epsilon_mutex);
		if (change > highestChange)
			highestChange = change;
		pthread_mutex_unlock(&epsilon_mutex);
		pthread_barrier_wait(&barrier);
	} while (highestChange > EPSILON);
}

void populateArrayRand(int aSeed)
{
	printf("Populating array");
	srand(aSeed);
	for (int i = 0; i < ARRAY_DIMENSIONS; i++)
	{
		for (int j = 0; j < ARRAY_DIMENSIONS; j++)
		{
			array[i][j] = rand() % 100;
			resultArray[i][j] = array[i][j];
		}
	}
	printf("Populated Array\n");
}

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		printf("Invalid Arguments");
		return 1;
	}
	ARRAY_DIMENSIONS = strtoul(argv[1], NULL, 10);
	NUM_THREADS = strtoul(argv[2], NULL, 10);
	loopIt = strtoul(argv[3], NULL, 10);
	array = malloc(sizeof(double*) * ARRAY_DIMENSIONS);
	if (array == NULL)
	{
		printf("Malloc Failed");
		return 1;
	}

	resultArray = malloc(sizeof(double*) * ARRAY_DIMENSIONS);
	if (resultArray == NULL)
	{
		printf("Malloc Failed");
		return 1;
	}

	for (int i = 0; i < ARRAY_DIMENSIONS; i++)
	{
		array[i] = malloc(sizeof(double) * ARRAY_DIMENSIONS);
		if (array[i] == NULL)
		{
			printf("Malloc Failed");
			return 1;
		}
		resultArray[i] = malloc(sizeof(double) * ARRAY_DIMENSIONS);
		if (resultArray[i] == NULL)
		{
			printf("Malloc Failed");
			return 1;
		}
	}

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
			double **temp = resultArray;
			resultArray = array;
			array = temp;
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
			pthread_barrier_wait(&barrier);
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
	struct tm * timeinfo;
	timeinfo = localtime(&startTime);
	double timeTaken = difftime(endTime, startTime);

	char string[100] = "";
	/*sprintf(string, "%ld-thh37-results-%d-%d-%d--%d-%d-%d.txt", NUM_THREADS, 
		timeinfo->tm_year + 1900,
		timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour,
		timeinfo->tm_min, timeinfo->tm_sec);*/
	sprintf(string, "ParallelResults-%ld-%ld.txt", NUM_THREADS, loopIt);

	FILE *f = fopen(string, "w");
	fprintf(f, "Number of threads %ld\n", NUM_THREADS);
	fprintf(f, "Size of array: %ld\n", ARRAY_DIMENSIONS);
	fprintf(f, "\nTime taken: ");
	fprintf(f, "%lf\n", timeTaken);
	for (int i = 0; i < ARRAY_DIMENSIONS; i++)
	{
		for (int j = 0; j < ARRAY_DIMENSIONS; j++)
		{
			fprintf(f, "%lf ", resultArray[i][j]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
    return 0;
}



