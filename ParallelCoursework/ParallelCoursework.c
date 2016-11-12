// ParallelCoursework.c : Defines the entry point for the console application.
//

#include "stdafx.h"
const int ARRAY_SEED = 38423;

const double EPSILON = 0.0001;
const int ARRAY_DIMENSIONS = 100;
const int NUM_THREADS = 16;


double array[100][100];

int main()
{
	populateArray(ARRAY_SEED);
	
	

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

void *settleRows(void *start, void *end)
{
	int startRow = (int)start, endRow = (int)end;
	for (int i = startRow; i < endRow; i++)
	{
		for (int j = 0; j < ARRAY_DIMENSIONS; j++)
		{

		}
	}
}