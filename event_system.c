#include <stdio.h>
#include <limits.h>

#define MAX_EVENTS 5
#define PREVENT_REREGISTRATION 1

typedef void (EventCallback)(void);
EventCallback* event_table[MAX_EVENTS] = {NULL};
int priority_table[MAX_EVENTS] = { [0 ... MAX_EVENTS-1] = INT_MAX};

void register_event(int event_id, EventCallback* callback, int priority){
    if((event_id >= 0) && (event_id < MAX_EVENTS)){
        if(event_table[event_id] != NULL && PREVENT_REREGISTRATION == 1){
            printf("Event ID already taken, please choose another!\n");
            return;
        }
        else{
            printf("Registration Successful!\n");
            event_table[event_id] = callback;
            priority_table[event_id] = priority;
        }
    }
    else{
        printf("Registration Failed, Invalid event ID\n");
    }
}

void dispatch_event(int event_id){
    if(event_id < 0 || event_id >= MAX_EVENTS){
        printf("Dispatch Failed, Invalid event ID\n");
        return;
    }
    else if(event_table[event_id] != NULL){
        event_table[event_id]();
    }
    else{
        printf("No handler registered\n");
    }
}

void dispatch_highest_priority_event(){
    int min_index = INT_MAX;

    for(int i = 0; i < MAX_EVENTS; i++){
        if((event_table[i] != NULL) && (priority_table[i] < min_index)){
            min_index = i;
        }
    }

    if(min_index != INT_MAX){
        event_table[min_index]();
    }
    else{
        printf("No valid events to dispatch\n");
    }
}

void unregister_event(int event_id){
    if(event_id < 0 || event_id >= MAX_EVENTS){
        printf("Unregistration Failed, Invalid event ID\n");
        return;
    }
    else if(event_table[event_id] != NULL){
        event_table[event_id] = NULL;
        printf("Unregistration Successful\n");
    }
    else{
        printf("No handler registered, nothing to unregister\n");
    }
}

void clear_all_events()
{
    for(int i = 0; i < MAX_EVENTS; i++){
        event_table[i] = NULL;
        priority_table[i] = INT_MAX;
    }
    printf("\nAll events unregistered!\n");
}

void event_a()
{
    printf("Executing Event A\n");
}

void event_b()
{
    printf("Executing Event B\n");
}

void event_c()
{
    printf("Executing Event C\n");
}

int main()
{
    dispatch_highest_priority_event();

    printf("\nRegistering Events:\n");
    register_event(2, &event_b, 0);
    register_event(1, &event_c, 1);
    register_event(5, &event_a, 3);
    register_event(2, &event_a, 3);
    register_event(4, &event_a, 3);

    printf("\nDispatching Events\n");
    dispatch_event(8);
    dispatch_event(4);
    dispatch_event(2);
    dispatch_event(1);

    printf("\nDispatching the highest priority event\n");
    dispatch_highest_priority_event();

    printf("\nUnregistering Events\n");
    unregister_event(0);
    unregister_event(5);
    unregister_event(2);

    clear_all_events();
    unregister_event(1);
    unregister_event(4);

    printf("\nTest dispatching unregistered event\n");
    dispatch_event(2);
}