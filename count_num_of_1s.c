#include <stdio.h>
#include <stdint.h>

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED   "\033[0;31m"
#define COLOR_RESET "\033[0m"

int count_num_of_1s(uint32_t num)
{
    int num_of_bits = 8*sizeof(num);
    int counter = 0;

    while(num){
        counter += num & 1;
        num = num >> 1;
    }

    return counter;
}

void run_test(uint32_t num, int expected)
{
    int result = count_num_of_1s(num);
    const char* status_color = (result == expected) ? COLOR_GREEN : COLOR_RED;
    const char* status_text  = (result == expected) ? "PASS" : "FAIL";

    printf("Input: 0x%08X | Expected: %3d | Got: %3d | %s%-4s%s\n",
           num, expected, result, status_color, status_text, COLOR_RESET);
}

int main(void)
{
    run_test(0x00000000, 0);  // no bits set
    run_test(0x00000001, 1);  // one bit set
    run_test(0x00000003, 2);  // two bits set
    run_test(0xFFFFFFFF, 32); // all 32 bits set
    run_test(0xAAAAAAAA, 16); // 0b101010... pattern -> 16 bits set
    run_test(0x55555555, 16); // 0b010101... pattern -> 16 bits set
    run_test(0x80000000, 1);  // only the highest bit set
    run_test(0x7FFFFFFF, 31); // all bits except highest set

    return 0;
}