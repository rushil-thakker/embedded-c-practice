#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STACK_GUARD_PATTERN 0xAA

#define STACK_SIZE  8
#define START_GUARD 0xDEADBEFF          //start canary
#define END_GUARD   0xDEADFEED          //end canary

//most secure way is to use randomized canary values that get generated on startup

typedef struct{
    uint32_t stack[STACK_SIZE];
    size_t top;                     //pointer to the top of the stack
} Stack;

/* Basic Stack Guard Pattern
void stack_guard_init(uint8_t *stack, size_t size)
{
    for(size_t i = 0; i < size; i++){
        stack[i] = STACK_GUARD_PATTERN;
    }
}

// return 0 if ok, -1 if overwritten
int check_stack_guard(uint8_t *stack, size_t size)
{
    for(size_t = 0; i < size; i++){
        if(stack[i] != STACK_GUARD_PATTERN){
            return -1;
        }
    }
    
    return 0;
}*/

void init_stack(Stack* s)
{
    s->stack[0] = START_GUARD;
    s->stack[STACK_SIZE-1] = END_GUARD;
    s->top = 1;
}

//0 = success, -1 = failure
int push(Stack *s, uint32_t val)
{
    if((s->top >= STACK_SIZE-1) || (s->stack[s->top] == END_GUARD)){
        printf("STACK OVERFLOW ATTEMPTED!\n");
        return -1;
    }
    
    s->stack[s->top] = val;
    s->top++;
    return 0;
}

int pop(Stack *s)
{
    if((s->top <= 1) || (s->stack[s->top] == START_GUARD)){
        printf("STACK UNDERFLOW ATTEMPTED!\n");
        return -1;
    }

    int popped = s->stack[s->top];
    s->top--;
    return popped;
}

void check_stack_integrity(Stack *s){
    bool stack_safe = true;

    if(s->stack[0] != START_GUARD){
        printf("STACK UNDERFLOW DETECTED!\n");
        stack_safe = false;
    }

    if(s->stack[STACK_SIZE-1] != END_GUARD){
        printf("STACK OVERFLOW DETECTED!\n\n");
        stack_safe = false;
    }

    if(stack_safe)
        printf("STACK INTEGRITY MAINTAINED!\n\n");
}

int main(void) {
    Stack my_stack;
    init_stack(&my_stack);

    push(&my_stack, 0x12345678);
    push(&my_stack, 0x87654321);

    // Trying to overflow by pushing beyond the stack's limit
    for (size_t i = 0; i < STACK_SIZE+10; i++) {
        if (push(&my_stack, 0x28435829) == -1) {
            check_stack_integrity(&my_stack);
            break;
        }
    }

    pop(&my_stack);
    pop(&my_stack);

    // Trying to underflow by popping too many times
    for (size_t i = 0; i < STACK_SIZE+10; i++) {
        if (pop(&my_stack) == -1) {
            check_stack_integrity(&my_stack);
            break;
        }
    }

    return 0;
}