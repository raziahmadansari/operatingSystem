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

int numberOfChairs = 0;
int CurrentIndex = 0;

//Function
void *Student_function(void *thread_id) {
	int randomTime;						//each thread will wait for a random time

	printf("[*] %ld is doing task...\n", (long)thread_id);
	randomTime = rand() % 11;
	sleep(randomTime);
		
	printf("%ld needs help...\n", (long)thread_id);
	pthread_mutex_lock(&ChairAccess);
	int count = numberOfChairs;
	pthread_mutex_unlock(&ChairAccess);

	if(count < 3) {
		if(count == 0)	sem_post(&TA_sleep);
		else	printf("%ld is sitting on chair (waiting for TA)...\n", (long)thread_id);
		
		//aquiring chair lock
		pthread_mutex_lock(&ChairAccess);
		int index = (CurrentIndex + numberOfChairs) % 3;
		numberOfChairs++;
		printf("Remaining Chairs are :: %d\n", 3 - numberOfChairs);
		pthread_mutex_unlock(&ChairAccess);	//unlock chair lock

		sem_wait(&ChairsSem[index]);
		printf("[*]	%ld is taking help (from TA)...\n", (long)thread_id);
		sem_wait(&Student_sem);
		printf("[*]	%ld left (TA)...\n", (long)thread_id);
	}
	else	printf("[**]No chair available...%ld will return later...", (long)thread_id);
}

void *TA_function() {
	
	sem_wait(&TA_sleep);	//sleep mode
	printf("\n\n[***]TA is activated...\n");

	while(1) {
		pthread_mutex_lock(&ChairAccess);	//chair lock
		if(numberOfChairs == 0) {
			pthread_mutex_unlock(&ChairAccess);	//break loop (if chair empty)
			break;
		}

		//next student
		sem_post(&ChairsSem[CurrentIndex]);
		numberOfChairs--;
		printf("		Student left...Remaining chairs %d\n", 3 - numberOfChairs);
		CurrentIndex = (CurrentIndex + 1) % 3;
		pthread_mutex_unlock(&ChairAccess);	//lock dismiss

		printf("		TA is busy...(helping a student)...\n");
		sleep(3);
		sem_post(&Student_sem);
		sleep(5);
	}
	
}


int main() {
	int students, id;
	
	//initialization of semaphore variables
	sem_init(&Student_sem, 0, 0);	//Students semaphore variable initialized
	sem_init(&TA_sleep, 0, 0);		//TA semapthore is initialized
	for(int i = 0; i < 3; i++)	sem_init(&ChairsSem[i], 0, 0);		//3 waiting chair semaphore are initialized
	pthread_mutex_init(&ChairAccess, NULL);		//TA helping chair is initialized

	printf("Enter number of students :: ");
	scanf("%d", &students);

	Students = (pthread_t*) malloc(sizeof(pthread_t) * students);	//memory allocation at runtime
	pthread_create(&TA, NULL, TA_function, NULL);	//creating TA thread
	for(int i = 0; i < students; i++)
		pthread_create(&Students[i], NULL, Student_function, (void*) (long)i);	//creating student threads

	pthread_join(TA, NULL);
	for(int i = 0; i < students; i++)	pthread_join(Students[i], NULL);

	free(Students);
}
