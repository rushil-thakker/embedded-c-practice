#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BUFF_SIZE 256

/*
    The data is added/written to the tail and removed/read from the head
    Can write n-1 elements into an n sized buffer to differentiate between full & empty
    So when head = tail, then the buffer is empty & when head = tail-1, the buffer is full
*/

typedef struct{
    uint8_t* buffer;
    int head;
    int tail;
    int max_len;
} circular_buffer;


void init_circular_buffer(circular_buffer* cb, uint8_t* buffer, int size){
    cb->buffer = buffer;
    cb->head = 0;
    cb->tail = 0;
    cb->max_len = size;
}

bool isEmpty(circular_buffer* cb){
    return (cb->head == cb->tail);
}

bool isFull(circular_buffer* cb){
    return (cb->head == ((cb->tail+1) % cb->max_len));
}

int retrieve_front(circular_buffer* cb){
    if(isEmpty(cb)){
        printf("BUFFER IS EMPTY. CANNOT RETRIEVE VALUE\n");
        return -1;
    }

    return cb->buffer[cb->head];
}

int retrieve_rear(circular_buffer* cb){
    if(isEmpty(cb)){
        printf("BUFFER IS EMPTY. CANNOT RETRIEVE VALUE\n");
        return -1;
    }
    int rear_index = ((cb->tail - 1 + cb->max_len) % cb->max_len);
    return cb->buffer[rear_index];
}

bool enqueue_overwrite_when_full(circular_buffer* cb, int value){
    if (isFull(cb)){
        printf("BUFFER IS FULL. OVERWRITING OLDEST VALUE\n");
        cb->head = (cb->head+1) % cb->max_len;
    }
    cb->buffer[cb->tail] = value;
    cb->tail = ((cb->tail+1) % cb->max_len);
    return true;
}

bool enqueue_ignore_when_full(circular_buffer* cb, int value){
    if(isFull(cb)){
        printf("BUFFER IS FULL. IGNORING NEW VALUES\n");
        return false;
    }

    cb->buffer[cb->tail] = value;
    cb->tail = ((cb->tail+1) % cb->max_len);
    return true;
}

bool dequeue(circular_buffer* cb){
    if(isEmpty(cb)){
        printf("BUFFER IS EMPTY. CANNOT DEQUEUE VALUE\n");
        return false;
    }

    cb->head = ((cb->head+1) % cb->max_len);
    return true;
}

int main(void){
    circular_buffer cb;
    uint8_t buffer[BUFF_SIZE];

    /*
    uint8_t *buffer = (uint8_t*)calloc(BUFF_SIZE, sizeof(uint8_t));
    if(buffer == NULL){
        printf("Buffer memory allocation falied!\n");
        return EXIT_FAILURE;
    }
        free(buffer);
        buffer = NULL;  
    */

    init_circular_buffer(&cb, buffer, BUFF_SIZE);

    // Test: Enqueue data into the buffer
    printf("Enqueueing values...\n");
    for (int i = 0; i < BUFF_SIZE+10; i++) {
        if (enqueue_overwrite_when_full(&cb, i+10)) {
            printf("Enqueued: %d\n", i+10);
        }
    }

    // Test: Try to enqueue when buffer is full
    printf("Trying to enqueue when buffer is full...\n");
    if (!enqueue_overwrite_when_full(&cb, BUFF_SIZE)) {
        printf("Buffer full, failed to enqueue: %d\n", BUFF_SIZE);
    }

    if (!enqueue_overwrite_when_full(&cb, BUFF_SIZE*2)) {
        printf("Buffer full, failed to enqueue: %d\n", BUFF_SIZE*2);
    }

    if (!enqueue_ignore_when_full(&cb, BUFF_SIZE/2)) {
        printf("Buffer full, failed to enqueue: %d\n", BUFF_SIZE/2);
    }

    // Test: Retrieve front and rear values
    printf("Front value: %d\n", retrieve_front(&cb));
    printf("Rear value: %d\n", retrieve_rear(&cb));

    // Test: Dequeue one element
    if (dequeue(&cb)) {
        printf("Dequeued an element\n");
    } else {
        printf("Buffer empty, nothing to dequeue\n");
    }

    if (!enqueue_overwrite_when_full(&cb, BUFF_SIZE*2)) {
        printf("Buffer full, failed to enqueue: %d\n", BUFF_SIZE*2);
    }

    // Test: Retrieve front and rear values again after dequeue
    printf("Front value after dequeue: %d\n", retrieve_front(&cb));
    printf("Rear value after dequeue: %d\n", retrieve_rear(&cb));

    // Test: Dequeue all elements and try to retrieve from an empty buffer
    while (!isEmpty(&cb)) {
        dequeue(&cb);
    }

    printf("Buffer emptied. Trying to retrieve from empty buffer...\n");
    printf("Front value: %d\n", retrieve_front(&cb));
    printf("Rear value: %d\n", retrieve_rear(&cb));

    // Test: Enqueue when buffer is empty
    if (enqueue_ignore_when_full(&cb, 100)) {
        printf("Enqueued value when buffer was empty\n");
    }
}