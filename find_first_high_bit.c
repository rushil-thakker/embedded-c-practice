#include <stdio.h>
#include <stdint.h>

#define COLOR_GREEN "\033[0;32m"
#define COLOR_RED   "\033[0;31m"
#define COLOR_RESET "\033[0m"

int find_first_set_bit(uint32_t num)
{
    int counter = 0;

    if(num == 0)
        return -1;

    while((num & 1) == 0){
        num = (num >> 1);
        counter++; 
    }

    return counter;
}

void run_test(uint32_t num, int expected)
{
    int result = find_first_set_bit(num);
    const char* status_color = (result == expected) ? COLOR_GREEN : COLOR_RED;
    const char* status_text  = (result == expected) ? "PASS" : "FAIL";

    printf("Input: 0x%08X | Expected: %3d | Got: %3d | %s%-4s%s\n",
           num, expected, result, status_color, status_text, COLOR_RESET);
}

int main(void)
{
    run_test(0x00000000, -1);  // All zeros
    run_test(0x00000001, 0);   // First bit set
    run_test(0x00000002, 1);   // Second bit set
    run_test(0x00000004, 2);   // Third bit set
    run_test(0x80000000, 31);  // Last bit (31st) set
    run_test(0x00000010, 4);   // 4th bit set
    run_test(0xFFFFFFFF, 0);   // All bits set
    run_test(0x0000F000, 12);  // 12th bit set
    run_test(0x7FFFFFFF, 0);   // Highest 31 bits set except 32nd
    run_test(0x01000000, 24);  // 24th bit set

    return 0;
}