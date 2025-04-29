#include <stdio.h>
#include <stdint.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define BIT_MASK(width)                 ((width) >= 32 ? 0xFFFFFFFFU : ((1U << (width)) - 1))
#define BIT_FIELD(value, start, width)  (((value) & BIT_MASK(width)) << (start))

uint32_t extract_field(uint32_t reg_value, uint8_t start_bit, uint8_t field_width)
{
    uint32_t mask = (1U << field_width) - 1;
    uint32_t extracted_byte = (reg_value >> start_bit) & mask;
    return extracted_byte;
}

uint32_t modify_field(uint32_t original_value, uint32_t new_field_value, uint8_t start_bit, uint8_t field_width)
{
    uint32_t mask = (((1U << field_width) - 1) << start_bit);
    uint32_t new_value = (original_value & ~mask);                  //clear the field 
    new_value |= ((new_field_value & ((1U << field_width) - 1)) << start_bit);  //insert new values
    return new_value;
}

void run_test(const char* name, uint32_t actual, uint32_t expected)
{
    if (actual == expected) {
        printf(ANSI_COLOR_GREEN "PASS" ANSI_COLOR_RESET ": %s (0x%X)\n", name, actual);
    } else {
        printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET ": %s (got 0x%X, expected 0x%X)\n", name, actual, expected);
    }
}

void test_BIT_MASK(void)
{
    printf("\nTesting BIT_MASK...\n");
    run_test("BIT_MASK(0)", BIT_MASK(0), 0x0);
    run_test("BIT_MASK(1)", BIT_MASK(1), 0x1);
    run_test("BIT_MASK(4)", BIT_MASK(4), 0xF);
    run_test("BIT_MASK(8)", BIT_MASK(8), 0xFF);
    run_test("BIT_MASK(16)", BIT_MASK(16), 0xFFFF);
}

void test_BIT_FIELD(void)
{
    printf("\nTesting BIT_FIELD macro...\n");
    run_test("BIT_FIELD(0xF, 0, 4)", BIT_FIELD(0xF, 0, 4), 0xF);
    run_test("BIT_FIELD(0xFF, 8, 8)", BIT_FIELD(0xFF, 8, 8), 0xFF00);
    run_test("BIT_FIELD(0x7, 5, 3)", BIT_FIELD(0x7, 5, 3), 0xE0);
    run_test("BIT_FIELD(0x1234, 16, 16)", BIT_FIELD(0x1234, 16, 16), 0x12340000);
    run_test("BIT_FIELD(0x1F, 11, 5)", BIT_FIELD(0x1F, 11, 5), 0xF800);
    run_test("BIT_FIELD(0x1, 0, 0)", BIT_FIELD(0x1, 0, 0), 0x0);
    run_test("BIT_FIELD(0xFFFFFFFF, 0, 32)", BIT_FIELD(0xFFFFFFFF, 0, 32), 0xFFFFFFFF);
    run_test("BIT_FIELD(0x1, 7, 1)", BIT_FIELD(0x1, 7, 1), 0x80);
}


void test_extract_field(void)
{
    printf("\nTesting extract_field...\n");
    run_test("extract 4 bits from 0x12345678 starting at 8",
             extract_field(0x12345678, 8, 4), 0x6);

    run_test("extract 8 bits from 0xFF00FF00 starting at 8",
             extract_field(0xFF00FF00, 8, 8), 0xFF);

    run_test("extract 3 bits from 0xAAAAAAAA starting at 1",
             extract_field(0xAAAAAAAA, 1, 3), 0x5);

    run_test("extract 5 bits from 0x0F0F0F0F starting at 4",
             extract_field(0x0F0F0F0F, 4, 5), 0x10);
}

void test_modify_field(void)
{
    printf("\nTesting modify_field...\n");
    run_test("modify 4 bits at bit 8 in 0x12345678 with 0xA",
             modify_field(0x12345678, 0xA, 8, 4), 0x12345A78);

    run_test("modify 8 bits at bit 16 in 0xFFFFFFFF with 0x12",
             modify_field(0xFFFFFFFF, 0x12, 16, 8), 0xFF12FFFF);

    run_test("modify 3 bits at bit 5 in 0x0 with 0x7",
             modify_field(0x0, 0x7, 5, 3), 0xE0);

    run_test("modify 5 bits at bit 0 in 0xABCD1234 with 0x1F",
             modify_field(0xABCD1234, 0x1F, 0, 5), 0xABCD123F);
}

int main(void)
{
    test_BIT_MASK();
    test_BIT_FIELD();
    test_extract_field();
    test_modify_field();
    return 0;
}