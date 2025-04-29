#include <stdio.h> 
#include <stdlib.h>
#include <time.h>

typedef void (CallbackFunc)(void);
typedef int (Operation)(int);

void event_a()
{
    printf("EVENT A EXECUTING\n");
}

void event_b()
{
    printf("EVENT B EXECUTING\n");
}

//the event trigger would be a hardware sensor. emulating it in this case,
//but basically if the sensor does one thing then trigger event a, otherwise 
//trigger event_b
CallbackFunc* process_event()
{
    int event = rand() % 2;

    if(event){
        return event_b;
    }
    else {
        return event_a;
    }
}

int square(int x)
{
    return (x*x);
}

int negate(int x)
{
    return (-1*x);
}

//steps through the array and calls the callback function on each element
//the function to use is determined when the user calls this function and 
//passes a pointer to the function that they want to be the operation
void apply_operation(int* arr, int n, Operation callback)
{
    for(int i = 0; i < n; i++)
    {
        arr[i] = callback(arr[i]);
    }
}

//helper function to print the array
void print_array(int* arr, int n)
{
    for(int i = 0; i < n; i++)
    {
        printf("%d   ", arr[i]);
    }
    printf("\n");
}

int main(void)
{
    //event based trigger example
    srand(time(NULL));
    CallbackFunc* selected_event = process_event();
    selected_event();

    //overloaded operations example
    //pass in pointer to whatever function you want to use
    int arr[5] = {1, 2, 3, 4, 5};
    int arr_size = sizeof(arr) / sizeof(arr[0]);
    apply_operation(arr, arr_size, &square);
    apply_operation(arr, arr_size, &negate);
    print_array(arr, arr_size);
    return 0;
}