#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

//LOCK Variable
pthread_mutex_t ChairAccess;
sem_t ChairsSem[3];
sem_t Student_sem;
sem_t TA_sleep;

//student thread
pthread_t *Students;

//TA thread
pthread_t TA;

int ChairsCount = 0;
int CurrentIndex = 0;

//Function
void *Student_function(void *threadID) {
	int pTime;
	while(1) {
		printf("%ld is doing task...\n", (long)threadID);
		pTime = rand() % 20;
		sleep(pTime);
		
		printf("%ld needs help...\n", (long)threadID);
		pthread_mutex_lock(&ChairAccess);
		int count = ChairsCount;
		pthread_mutex_unlock(&ChairAccess);

		if(count < 3) {
			if(count == 0)	sem_post(&TA_sleep);
			else	printf("%ld is sitting on chair (waiting for TA)...\n", (long)threadID);
			
			//aquiring chair lock
			pthread_mutex_lock(&ChairAccess);
			int index = (CurrentIndex + ChairsCount) % 3;
			ChairsCount++;
			printf("Remaining Chairs are :: %d\n", 3 - ChairsCount);
			pthread_mutex_unlock(&ChairAccess);	//unlock chair lock

			sem_wait(&ChairsSem[index]);
			printf("		%ld is taking halp (from TA)...\n", (long)threadID);
			sem_wait(&Student_sem);
			printf("%ld left (TA)...", (long)threadID);
		}
		else	printf("No chair available...%ld will return later...", (long)threadID);
	}
}

void *TA_function() {
	while(1) {
		sem_wait(&TA_sleep);	//sleep mode
		printf("\n\nTA is activated...\n");

		while(1) {
			pthread_mutex_lock(&ChairAccess);	//chair lock
			if(ChairsCount == 0) {
				pthread_mutex_unlock(&ChairAccess);	//break loop (if chair empty)
				break;
			}

			//next student
			sem_post(&ChairsSem[CurrentIndex]);
			ChairsCount--;
			printf("Student left...Remaining chairs %d\n", 3 - ChairsCount);
			CurrentIndex = (CurrentIndex + 1) % 3;
			pthread_mutex_unlock(&ChairAccess);	//lock dismiss

			printf("		TA is busy...(helping a student)...\n");
			sleep(4);
			sem_post(&Student_sem);
			sleep(10);
		}
	}
}


int main() {
	int students, id;
	
	sem_init(&Student_sem, 0, 0);
	sem_init(&TA_sleep, 0, 0);
	for(int i = 0; i < 3; i++)	sem_init(&ChairsSem[i], 0, 0);
	pthread_mutex_init(&ChairAccess, NULL);

	printf("Enter number of students :: ");
	scanf("%d", &students);

	Students = (pthread_t*) malloc(sizeof(pthread_t) * students);	//memory allocation at runtime
	pthread_create(&TA, NULL, TA_function, NULL);
	for(int i = 0; i < students; i++)
		pthread_create(&Students[i], NULL, Student_function, (void*) (long)i);

	pthread_join(TA, NULL);
	for(int i = 0; i < students; i++)	pthread_join(Students[i], NULL);

	free(Students);
}
