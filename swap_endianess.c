#include <stdio.h>
#include <stdint.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define DEFINE_SWAP_FUNCTION(type)                                      \
type swap_endianess_##type(type num){                                   \
    type swapped_num = 0;                                               \
    size_t size = sizeof(type);                                         \
    for(size_t i = 0; i < size; i++){                                   \
        swapped_num |= ((num >> (8 * i)) & 0xFF) << (8 * (size-1-i));   \
    } return swapped_num;                                               \
}

DEFINE_SWAP_FUNCTION(uint16_t)
DEFINE_SWAP_FUNCTION(uint32_t)
DEFINE_SWAP_FUNCTION(uint64_t)

uint32_t swap_endianess32(uint32_t num)
{
    //simple bit shifting & masking
    uint32_t swapped_num = (((num >> 24) & 0x000000FF) |
                            ((num >> 8)  & 0x0000FF00) |
                            ((num << 8)  & 0x00FF0000) |
                            ((num << 24) & 0xFF000000));

    //doing it in a loop
    uint32_t loop_swap_num = 0;
    for(size_t i = 0; i < sizeof(uint32_t); i++){
        loop_swap_num |= ((num >> (8*i) & 0xFF) << (8 * (sizeof(uint32_t) - 1 - i)));
    }
    return loop_swap_num;
}

void run_test(uint32_t num, uint32_t expected)
{
    uint32_t result = swap_endianess32(num);
    if (result == expected) {
        printf(ANSI_COLOR_GREEN "PASS" ANSI_COLOR_RESET ": swap_endianess(0x%08X) = 0x%08X\n", num, result);
    } else {
        printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET ": swap_endianess(0x%08X) = 0x%08X (expected 0x%08X)\n", num, result, expected);
    }
}

int main(void)
{
    run_test(0x12345678, 0x78563412);
    run_test(0xAABBCCDD, 0xDDCCBBAA);
    run_test(0x0, 0x0);                // Edge case: all zeros
    run_test(0xFFFFFFFF, 0xFFFFFFFF);  // Edge case: all ones
    run_test(0x01020304, 0x04030201);
    run_test(0x11223344, 0x44332211);
    run_test(0x000000FF, 0xFF000000);  // Only the last byte is nonzero
    run_test(0xFF000000, 0x000000FF);  // Only the first byte is nonzero

    uint16_t a = 0x1234;
    uint32_t b = 0x12345678;
    uint64_t c = 0x1122334455667788ULL;

    printf("Original uint16: 0x%04X\n", a);
    printf("Swapped  uint16: 0x%04X\n\n", swap_endianess_uint16_t(a));

    printf("Original uint32: 0x%08X\n", b);
    printf("Swapped  uint32: 0x%08X\n\n", swap_endianess_uint32_t(b));

    printf("Original uint64: 0x%016lX\n", c);
    printf("Swapped  uint64: 0x%016lX\n", swap_endianess_uint64_t(c));
    return 0;
}