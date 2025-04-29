#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define ENABLE_MASK     (1 << 0)
#define DIRECTION_MASK  (1 << 1)
#define VALUE_MASK      (1 << 2)

typedef struct gpio_reg{
    unsigned int enable : 1;
    unsigned int direction : 1;
    unsigned int value : 1;
    unsigned int reserved : 29;
} gpio_reg_t;

void set_output_high_bitfield(volatile uint32_t *reg){
    gpio_reg_t *gpio = (gpio_reg_t*)reg;
    gpio->enable = 1;
    gpio->direction = 1;        //0 = input, 1 = output
    gpio->value = 1;            //0 = low, 1 = high
}

void set_output_low_bitfield(volatile uint32_t *reg){
    gpio_reg_t *gpio = (gpio_reg_t*)reg;
    gpio->enable = 1;
    gpio->direction = 1;
    gpio->value = 0;
}

void set_output_high_bitmask(volatile uint32_t *reg){
    *reg |= (ENABLE_MASK | DIRECTION_MASK | VALUE_MASK);
}

void set_output_low_bitmask(volatile uint32_t *reg){
    *reg |= (ENABLE_MASK | DIRECTION_MASK);
    *reg &= (~VALUE_MASK);
}

void toggle_register_enable(volatile uint32_t *reg){
    *reg ^= (ENABLE_MASK);
}

int main(void){
    uint32_t gpio_mem = 0;
    uint32_t gpio_mem1 = 0;

    gpio_reg_t *gpio0 = (gpio_reg_t*)&gpio_mem;
    gpio_reg_t *gpio1 = (gpio_reg_t*)&gpio_mem1;

    int blink_counter = 5;

    printf("BITFIELD LED BLINKING START\n");
    
    while(blink_counter > 0){
        set_output_high_bitfield(&gpio_mem);
        printf("Enable: %d, Dir: %d, val: %d\n", gpio0->enable, gpio0->direction, gpio0->value);
        set_output_low_bitfield(&gpio_mem);
        printf("Enable: %d, Dir: %d, val: %d\n", gpio0->enable, gpio0->direction, gpio0->value);
        blink_counter--;
    }

    printf("BITFIELD LED BLINKING END\n\n");
    printf("BITMASK LED BLINKING START\n");

    while(blink_counter < 5){
        set_output_high_bitmask(&gpio_mem1);
        printf("Enable: %d, Dir: %d, val: %d\n", gpio1->enable, gpio1->direction, gpio1->value);
        set_output_low_bitmask(&gpio_mem1);
        printf("Enable: %d, Dir: %d, val: %d\n", gpio1->enable, gpio1->direction, gpio1->value);
        blink_counter++;
    }

    printf("BITMASK LED BLINKING END\n\n");
    printf("DISABLING REGISTERS\n");

    toggle_register_enable(&gpio_mem);
    toggle_register_enable(&gpio_mem1);

    printf("Enable: %d, Dir: %d, val: %d\n", gpio0->enable, gpio0->direction, gpio0->value);
    printf("Enable: %d, Dir: %d, val: %d\n", gpio1->enable, gpio1->direction, gpio1->value);

    return 0;
}