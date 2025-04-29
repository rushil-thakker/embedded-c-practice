#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_ALLOWABLE_FAULTS 3
#define SYSTEM_RECOVERED_CALLS 7
#define MAX_RUNS_LOOP 3

//function definitions
void led_toggle_isr();
void led_force_off_isr();
void led_recovery_isr();
void* led_timer_thread(void*);
void* fault_timer_thread(void*);

//flag to be modified by the ISR, simulates an LED
volatile sig_atomic_t led_flag = 0;

//counts the total number of faults
volatile sig_atomic_t fault_count = 0;

//counts the total number of system recovery function calls
volatile sig_atomic_t recovery_count = 0;

//this code loops between normal operation, faulting, and system recovery
//this counter is just here so the code doesn't run forever
volatile sig_atomic_t loop_counter = 0;

//ISR callback type definition
typedef void (*isr_callback_t)(void);

//ISR function pointers
isr_callback_t led_callback = NULL;
isr_callback_t fault_callback = NULL;

//Mutex to protect the led_flag
pthread_mutex_t led_mutex;

//isr to toggle LED
//normal operation
void led_toggle_isr()
{
    pthread_mutex_lock(&led_mutex);
    led_flag ^= 1; //toggle between 0 & 1
    pthread_mutex_unlock(&led_mutex);
}

//How the system will recover after too many faults
//forces the led to 1 for a set number of cycles
//once the system is recovered, dynamically change the function pointers. in this case
//reenable the fault handler & change the LED function pointer back to the normal LED operation callback function
void led_recovery_isr()
{
    pthread_mutex_lock(&led_mutex);
    led_flag = 1;
    recovery_count++;
    pthread_mutex_unlock(&led_mutex);


    if(recovery_count >= SYSTEM_RECOVERED_CALLS){
        printf("SYSTEM RECOVERED. SWTICHING BACK TO NORMAL OPERATION\n");
        pthread_mutex_lock(&led_mutex);
        fault_callback = led_force_off_isr;
        led_callback = led_toggle_isr;
        recovery_count = 0;
        loop_counter++;
        pthread_mutex_unlock(&led_mutex);
    }
}

//There was a fault, so force the LED off (reset) & count total faults
//after so many faults, switch the system to recovery mode, by dyanmically changing the function pointers
//to point to a different ISR, in this case change the LED function pointer to callback the recovery function & disable faults
void led_force_off_isr()
{    
    pthread_mutex_lock(&led_mutex);
    led_flag = 0;
    fault_count++;
    pthread_mutex_unlock(&led_mutex);
    printf("FAULT DETECTED ON SYSTEM, TURNING LED OFF, TOTAL FAULTS: %d\n", fault_count);

    if(fault_count >= MAX_ALLOWABLE_FAULTS){
        printf("SYSTEM SWITCHING TO RECOVERY MODE\n");
        pthread_mutex_lock(&led_mutex);
        fault_callback = NULL;
        led_callback = led_recovery_isr;
        fault_count = 0;
        pthread_mutex_unlock(&led_mutex);
    }
}

//thread to handle simulating led callback trigger
//occurs every 300 ms
void* led_timer_thread(void* arg)
{
    while(1){
        usleep(300000);  //wait for 300 ms
        if(led_callback){
            led_callback();
        }
    }
}

//thread to handle simulating hardware fault
//occurs every 1000 ms
void* fault_timer_thread(void* arg)
{
    while(1){
        usleep(1000000);     //1000 ms
        if(fault_callback){
            fault_callback();
        }
    }
}

int main()
{
    pthread_t tid1, tid2;
    int led_state_copy = 0; 

    //set the default behavior
    led_callback = led_toggle_isr;
    fault_callback = led_force_off_isr;

    pthread_mutex_init(&led_mutex, NULL);

    if(pthread_create(&tid1, NULL, led_timer_thread, NULL) != 0){
        perror("Failed to create thread\n");
        return -1;
    }

    if(pthread_create(&tid2, NULL, fault_timer_thread, NULL) != 0){
        perror("Failed to create thread\n");
        return -1;
    }

    while(loop_counter < 3){
        pthread_mutex_lock(&led_mutex);
        led_state_copy = led_flag;
        pthread_mutex_unlock(&led_mutex);

        printf("LED state %s\n", led_state_copy ? "ON" : "OFF");
        usleep(200000);
    }
    
    pthread_mutex_destroy(&led_mutex);
    return 0;
}