#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

//need to include "-pthread" when compiling this file

#define NUM_OF_THREADS 10

int counter = 0;
pthread_mutex_t mutex;

//shared resource
void* increment_counter(void* arg)
{
    for(int i = 0; i < 1000; i++){
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }
}

int main()
{
    //init mutex
    pthread_mutex_init(&mutex, NULL);
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
    pthread_mutex_destroy(&mutex);

    return 0;
}