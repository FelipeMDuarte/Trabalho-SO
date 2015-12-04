#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "timer.h"

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int numThreads;
int sizeOfMatrix;
int **a, **b, **c;

struct thread_info {    		/* Used as argument to thread_start() */
	pthread_t thread_id;         /* ID returned by pthread_create() */
	int       thread_num;        /* Application-defined thread # */
	int 	  iterations_num;
};

static void *startThread(void *arg) {
	int i, j, line, start, stop;
	struct thread_info *tInfo = (struct thread_info *) arg;
	int threadNum = tInfo->thread_num;
	int iterationsNum = tInfo->iterations_num;
	
	//printf("Starting thread #%d\n", threadNum);

	start = (threadNum - 1) * iterationsNum + 1;
	stop = threadNum * iterationsNum;

	//printf("THREAD %d: start = %d, stop = %d\n", threadNum, start, stop);

	for(line = start; line <= stop; line++) {
		for(i = 0; i < sizeOfMatrix; i++) {
			for(j = 0; j < sizeOfMatrix; j++) {
				c[line-1][i] += a[line-1][j] * b[j][i];
			}
		}
	}

	pthread_exit(NULL);
}

void allocateMatrix(int*** matrix, int sizeOfMatrix) {
	int i = 0;

	*matrix = (int**)calloc(sizeOfMatrix, sizeof(int*));
	
	for(i = 0; i < sizeOfMatrix; i++) {
		(*matrix)[i] = (int*)calloc(sizeOfMatrix, sizeof(int));
	}
}

void printMatrix(int** matrix, int sizeOfMatrix) {
	int i, j;

	for(i = 0; i < sizeOfMatrix; i++) {
		for(j = 0; j < sizeOfMatrix; j++) {
			printf("%d\t", matrix[i][j]);
		}
		printf("\n");
	}
}

void initMatrix(int*** a, int*** b, int***c, int sizeOfMatrix) {
	int i, j;
	allocateMatrix(a, sizeOfMatrix);
	allocateMatrix(b, sizeOfMatrix);
	allocateMatrix(c, sizeOfMatrix);
	
	for(i = 0; i < sizeOfMatrix; i++) {
		for(j = 0; j < sizeOfMatrix; j++) {
			(*a)[i][j] = 1;
			(*b)[i][j] = 2;
		}
	}
}

int main(int argc, char *argv[]) {
	double  start, finish, elapsed;
	struct thread_info *tInfo;
	int i, j, k, option = 0; //option = 1 -> serial, option = 2 -> parallel
	int error = 0;

	if(argc < 3) {
		printf("\nPlease, run the application with correct parameters\n\n"
			   "\tEXAMPLE:\n\t[application-name] [matrix size] -s, for serial\n"
			   "\t[application-name] [matrix size] -p [number of threads] for parallel\n\n");

		return 0;
	}

	if(strcmp(argv[2], "-s") == 0) {
		option = 1;
	} else if(strcmp(argv[2], "-p") == 0) {
		option = 2;
		if(argc >= 4)
			numThreads = atoi(argv[3]);
		else
			error = 1;
	} else
		error = 1;

	if(error) {
		printf("\nPlease, run the application with correct parameters\n\n"
			   "\tEXAMPLE:\n\t[application-name] [matrix size] -s, for serial\n"
			   "\t[application-name] [matrix size] -p [number of threads] for parallel\n\n");

		return 0;
	}

	switch(option) {
		case 1:
			//Serial calculation of matrix multiplication
			GET_TIME(start);
			
			sizeOfMatrix = atoi(argv[1]);
			initMatrix(&a, &b, &c, sizeOfMatrix);
			int temp;

			for(k = 0; k < sizeOfMatrix; k++) {
				for(i = 0; i < sizeOfMatrix; i++) {
					for(j = 0; j < sizeOfMatrix; j++) {
						c[i][j] += a[i][k] * b[k][j];
					}
				}
			}

			GET_TIME(finish);
			elapsed = finish - start;
			printf("SERIAL\n");
			printf("The code took %e seconds\n", elapsed);

			//printMatrix(c, sizeOfMatrix);
			break;
			
		case 2:
			//Parallel calculation of matrix multiplication
			GET_TIME(start);
			
			sizeOfMatrix = atoi(argv[1]);
			//numThreads = atoi(argv[3]);
			
			initMatrix(&a, &b, &c, sizeOfMatrix);

			//Allocating memory for pthread_create() arguments
			tInfo = calloc(numThreads, sizeof(struct thread_info));
			if(tInfo == NULL)
				handle_error("calloc");

			if(sizeOfMatrix % numThreads) {
				printf("ERROR! The matrix size must be multiple of number of threads!\n");
				return 0;
			}

			int iterations = sizeOfMatrix / numThreads;

			//Creating threads
			for(i = 0; i < numThreads; i++) {
				tInfo[i].thread_num = i+1;
				tInfo[i].iterations_num = iterations;
				//printf("Creating thread #%d\n", i);
				if(pthread_create(&tInfo[i].thread_id, NULL, startThread, &tInfo[i]))
					handle_error("pthread_create");
			}
			
			//Joining
			for(i = 0; i < numThreads; i++) {
				if(pthread_join(tInfo[i].thread_id, NULL))
					handle_error("pthread_join");
			}

			GET_TIME(finish);
			elapsed = finish - start;
			printf("\nPARALLEL\n");
			printf("The code took %e seconds\n", elapsed);

			//printMatrix(c, sizeOfMatrix);

			break;
			
		default:
			printf("\nPlease, run the application with correct parameters\n\n"
				   "\tEXAMPLE:\n\t[application-name] [matrix size] -s, for serial\n"
				   "\t[application-name] [matrix size] -p [number of threads] for parallel\n\n");
	}

	return 0;	
}