#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

//need to include "-pthread" when compiling this file

#define NUM_OF_THREADS 10

int counter = 0;
sem_t sem;

//shared resource
void* increment_counter(void* arg)
{
    for(int i = 0; i < 1000; i++){
        sem_wait(&sem);
        counter++;
        sem_post(&sem);
    }
}

int main()
{
    //init mutex
    sem_init(&sem, 0, 1);
    pthread_t tid[NUM_OF_THREADS];

    //create threads
    for(int i = 0; i < NUM_OF_THREADS; i++){
        int error = pthread_create(&(tid[i]), NULL, increment_counter, NULL);
        if(error){
            printf("Thread can't be created : [%s]", strerror(error));
        }
    }

    //Join the threads to the main thread
    for(int i = 0; i < NUM_OF_THREADS; i++){
        pthread_join(tid[i], NULL);
    }

    printf("Counter final value: %d\n", counter);

    //destory mutex
    sem_destroy(&sem);

    return 0;
}