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
bool getSafeSeq();
	void* processCode(void* );
	
	int main(int argc, char** argv) {
		srand(time(NULL));
	

	        printf("How many Processes does one want to see for -->");
	        scanf("%d", &numberOfProcesses);
	

	        printf("what number of Resources required for checking --> ");
	        scanf("%d", &numberOfResources);
	

	        availableResources = (int *)malloc(numberOfResources * sizeof(*availableResources));
	        printf("Available Resource's count is --> ");
	        for(i=0; i<numberOfResources; i++)
	                scanf("%d", &availableResources[i]);
	

	        numberOfAllocatedResources = (int **)malloc(numberOfProcesses * sizeof(*numberOfAllocatedResources));
	        for(i=0; i<numberOfProcesses; i++)
	                numberOfAllocatedResources[i] = (int *)malloc(numberOfResources * sizeof(**numberOfAllocatedResources));
	

	        numberOfMaximumRequiredResources= (int **)malloc(numberOfProcesses * sizeof(*numberOfMaximumRequiredResources));
	        for(i=0; i<numberOfProcesses; i++)
	                numberOfMaximumRequiredResources[i] = (int *)malloc(numberOfResources * sizeof(**numberOfMaximumRequiredResources));
	

	        // numberOfAllocatedResources
	        printf("\n");
		for(i=0; i<numberOfProcesses; i++) {
	                printf("numberOfAllocatedResources to process %d is --> ", i+1);
	                for(j=0; j<numberOfResources; j++)
	                        scanf("%d", &numberOfAllocatedResources[i][j]);
	        }
	        
			// maximum required availableResources
	        printf("\n");
	        for(i=0; i<numberOfProcesses; i++) {
	                printf("maximumRequiredResources by process %d is --> ", i+1);
	                for(j=0; j<numberOfResources; j++)
	                        scanf("%d", &numberOfMaximumRequiredResources[i][j]);
	        }

		// calculate neededResources matrix
	        neededResources = (int **)malloc(numberOfProcesses * sizeof(*neededResources));
	        for(i=0; i<numberOfProcesses; i++)
	                neededResources[i] = (int *)malloc(numberOfResources * sizeof(**neededResources));
	

	        for(i=0; i<numberOfProcesses; i++)
	                for(j=0; j<numberOfResources; j++)
	                        neededResources[i][j] = numberOfMaximumRequiredResources[i][j] - numberOfAllocatedResources[i][j];
	

		// get safe sequence
		processSafeSequence = (int *)malloc(numberOfProcesses * sizeof(*processSafeSequence));
	        for(i=0; i<numberOfProcesses; i++) processSafeSequence[i] = -1;
	

	        if(!getSafeSeq()) {
					printf("This is an unsafe process or unsafe state, kindly check again Once. thnak you.");
	                exit(-1);
		        }
	

	        printf("\n Yeah!! Congratulations \n Safe Sequence is there for processes \n and that safe sequence is --> ");
	        for(i=0; i<numberOfProcesses; i++) {
	                printf("%-3d", processSafeSequence[i]+1);
	        }
	

	        printf("\nNow let's just see how process executes -->\n");
	        sleep(1);
		
		// run threads
		pthread_t processes[numberOfProcesses];
	        pthread_attr_t attr;
	        pthread_attr_init(&attr);
	

		int processNumber[numberOfProcesses];
		for(i=0; i<numberOfProcesses; i++) processNumber[i] = i;
	

	        for(i=0; i<numberOfProcesses; i++)
	                pthread_create(&processes[i], &attr, processCode, (void *)(&processNumber[i]));
	

	        for(i=0; i<numberOfProcesses; i++)
	                pthread_join(processes[i], NULL);
	

	        printf("\n..................CONGRATULATIONS.....................\n.....All Processes Have been Finished.....\n");	
		
		// free availableResources
	        free(availableResources);
		        for(i=0; i<numberOfProcesses; i++) {
	                free(numberOfAllocatedResources[i]);
	                free(numberOfMaximumRequiredResources[i]);
			free(neededResources[i]);
	        }
	        free(numberOfAllocatedResources);
	        free(numberOfMaximumRequiredResources);
		free(neededResources);
	        free(processSafeSequence);
	}
	

	

	bool getSafeSeq() {
		// get safe sequence
	        int tempRes[numberOfResources];
	        for(i=0; i<numberOfResources; i++) tempRes[i] = availableResources[i];
	

	        bool finished[numberOfProcesses];
	        for(i=0; i<numberOfProcesses; i++) finished[i] = false;
	        int nfinished=0;
	        while(nfinished < numberOfProcesses) {
	                bool safe = false;
	

	                for(i=0; i<numberOfProcesses; i++) {
	                        if(!finished[i]) {
	                                bool possible = true;
	

	                                for(j=0; j<numberOfResources; j++)
	                                        if(neededResources[i][j] > tempRes[j]) {
	                                                possible = false;
	                                                break;
	                                        }
	

	                                if(possible) {
	                                        for(j=0; j<numberOfResources; j++)
	                                                tempRes[j] += numberOfAllocatedResources[i][j];
	                                        processSafeSequence[nfinished] = i;
	                                        finished[i] = true;
	                                        ++nfinished;
	                                        safe = true;
	                                }
	                        }
	                }
	

	                if(!safe) {
	                        for(k=0; k<numberOfProcesses; k++) processSafeSequence[k] = -1;
	                        return false; // no safe sequence found
	                }
	        }
	        return true; // safe sequence found
	}
	

	// process code
	void* processCode(void *arg) {
	        int p = *((int *) arg);
	

		// lock availableResources
	        pthread_mutex_lock(&lockResources);
	

	        // condition check
	        while(p != processSafeSequence[processRanN])
			                pthread_cond_wait(&condition, &lockResources);
	

		// process
	        printf("\n#### Process Number %d  ####", p+1);
	        printf("\n\tNumber of Allocated Resources : ");
	        for(i=0; i<numberOfResources; i++)
	                printf("%3d", numberOfAllocatedResources[p][i]);
	

	        printf("\n\tNumber of Needed Resources   : ");
	        for(i=0; i<numberOfResources; i++)
	                printf("%3d", neededResources[p][i]);
	

	        printf("\n\tNumber of Available Resources : ");
	        for(i=0; i<numberOfResources; i++)
	                printf("%3d", availableResources[i]);
	

	        printf("\n"); sleep(1);
	

	        printf("\tResource have been allocated to the process!");
	        printf("\n"); 
			sleep(1);
	        printf("\tprocess is now running the code\n let's just wait........");
	        printf("\n"); 
			sleep(rand()%3 + 2); // process code
	        printf("\tprocess now completed the code");
	        printf("\n"); 
			sleep(1);
	        printf("\tProcess is now releasing all allocated resource\n let's just wait ........");
	        printf("\n"); 
			sleep(1);
	        printf("\tAll resources released by the process");
	

		for(i=0; i<numberOfResources; i++)
	                availableResources[i] += numberOfAllocatedResources[p][i];
	

	        printf("\n\tAvailable resources with us now : ");
	        for(i=0; i<numberOfResources; i++)
	                printf("%3d", availableResources[i]);
	        printf("\n\n");
	

	        sleep(1);
	

		// Broadcating the conditions -->
	        processRanN++;
	        pthread_cond_broadcast(&condition);
	        pthread_mutex_unlock(&lockResources);
		pthread_exit(NULL);
	}

			

			
