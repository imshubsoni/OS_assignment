#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
	
	
	int i = 0;
	int j = 0;
	int k = 0;
	int *processSafeSequence;
	int numberOfResources;
	int **numberOfMaximumRequiredResources;
	int *availableResources;
	int **numberOfAllocatedResources;
	int **neededResources;
	int numberOfProcesses;
	int processRanN = 0;	

	pthread_mutex_t lockResources;
	pthread_cond_t condition;
