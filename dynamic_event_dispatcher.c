#include <stdio.h>

void event_a();
void event_b();
void event_c();

typedef void (EventCallback)(void);
EventCallback* dispatcher_table[] = {event_a, event_b, event_c};

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

void dispatch_event(int event_id)
{
    if((event_id >= 0) && (event_id < (sizeof(dispatcher_table)/sizeof(dispatcher_table[0])))){
        dispatcher_table[event_id]();
    }
    else{
        printf("Invalid event ID\n");
    }
}

int main()
{
    dispatch_event(1);
    dispatch_event(-1);
    dispatch_event(2);
    dispatch_event(6);
    dispatch_event(0);
    return 0;
}