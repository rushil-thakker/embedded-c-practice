#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

//need to include "-pthread" when compiling this file

#define NUM_OF_THREADS 10
#define MAX_RESOURCES 3

sem_t sem;

//shared resource
void* use_resource(void* thread_id)
{
    int tid = *((int*)thread_id);
    printf("Thread %d attempting to use a resource....\n", tid);

    sem_wait(&sem);
    printf("Thread %d acquired a resource....\n", tid);
    sleep(2);
    printf("Thread %d released a resource....\n", tid);
    sem_post(&sem);
}

int main()
{
    //init mutex
    sem_init(&sem, 0, MAX_RESOURCES);
    pthread_t tid[NUM_OF_THREADS];
    int thread_ids[NUM_OF_THREADS];

    //create threads
    for(int i = 0; i < NUM_OF_THREADS; i++){
        thread_ids[i] = i + 1;
        int error = pthread_create(&tid[i], NULL, use_resource, (void*)(&thread_ids[i]));
        if(error){
            printf("Thread can't be created : [%s]", strerror(error));
        }
    }

    //Join the threads to the main thread
    for(int i = 0; i < NUM_OF_THREADS; i++){
        pthread_join(tid[i], NULL);
    }

    //destory mutex
    sem_destroy(&sem);

    return 0;
}