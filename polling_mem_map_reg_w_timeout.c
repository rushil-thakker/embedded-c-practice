#include <stdio.h>
#include <stdint.h>

#define READY_MASK (1 << 3)

//simulating a hardware register
//typically hardware register looks something like this with an actual memory address, but will seg fault on PC
//#define STATUS_REG ((volatile uint32_t*)0x40000010)
volatile uint32_t* status_register = NULL;

int wait_for_ready(uint32_t timeout_cycles)
{
    while(timeout_cycles > 0){
        if(*status_register & READY_MASK)
            return 0;
        timeout_cycles--;
    }

    return -1;
}

int main(void)
{
    uint32_t sim_status = 0;
    status_register = &sim_status;
    int result = wait_for_ready(1000);

    if(result == 0){
        printf("READY bit set! Operation successful.\n");
    } else {
        printf("Timeout occurred. READY bit not set within timeout.\n");
    }

    sim_status = READY_MASK;  // Simulate setting the READY bit, what the hardware should do

    result = wait_for_ready(1000);
    if (result == 0) {
        printf("READY bit set! Operation successful.\n");
    } else {
        printf("Timeout occurred. READY bit not set within timeout.\n");
    }

    return 0;
}