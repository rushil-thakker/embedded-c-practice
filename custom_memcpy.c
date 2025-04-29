#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Color macros
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//assumes no overlap between destination and source memory locations
//works for any & all memory alignments since default is to cast & copy 1 byte at a time
void* my_memcpy(void *dest, const void *src, size_t n)
{
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    //if both destination and source are aligned to 4-bytes, then copy word by word 
    if(((uintptr_t)d % 4 == 0) && ((uintptr_t)s % 4 == 0)){
        uint32_t* d_32 = (uint32_t*)d;
        const uint32_t* s_32 = (const uint32_t*)s;
        while(n >= 4){
            *d_32++ = *s_32++;
            n -= 4;
        }

        d = (uint8_t*)d_32;
        s = (uint8_t*)s_32;
    }

    //else copy byte by byte
    while(n--){
        *d++ = *s++; 
    }

    return dest;
}

//A safer version of memcpy that can handle if the destination and source
//  are writing to overlapping memory
void* my_memmove(void* dest, void* src, size_t n)
{
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;

    //dest is before src or after (src + n) - copy forwards (low to high)
    if(((uintptr_t)dest < (uintptr_t)src) || ((uintptr_t)dest >= ((uintptr_t)(src + n))))
    {
        for(size_t i = 0; i < n; i++){
            d[i] = s[i];
        }
    }
    
    //dest is after src - copy backwards (high to low)
    else{
        for(size_t i = n; i > 0; i--){
            d[i-1] = s[i-1];
        }
    }

    return dest;
}

void copy_to_buffer(uint32_t* buffer, const void* src, size_t num_bytes){
    const uint8_t* s = (const uint8_t*)src;
    uint8_t* b = (uint8_t*)buffer;

    //copy until the source is aligned to a word boundary
    while((uintptr_t)s % 4 != 0 && num_bytes--){
        *b++ = *s++;
    }

    //copy word by word
    while(num_bytes >=4){
        *(uint32_t*)b = *(const uint32_t*)s;
        b += 4;
        s += 4;
        num_bytes -= 4;
    }

    //copy the remaining bytes
    while(num_bytes--){
        *b++ = *s++;
    }
}

void misaligned_memory_access(uint8_t* buffer, size_t size)
{
    if((uintptr_t)buffer % sizeof(uint32_t) != 0){
        printf("Warning, pointer is not word aligned, adjusting to nearest alignement.\n");
        buffer = (uint8_t*)((uintptr_t)buffer + (sizeof(uint32_t) - ((uintptr_t)buffer % sizeof(uint32_t))));
    }

    uint32_t* data = (uint32_t*)buffer;

    for(size_t i = 0; i < (size/sizeof(uint32_t)); i++){
        printf("Data[%zu]: %u\n", i, data[i]);
    }
}

//Helper to print bytes
void print_bytes(const void *ptr, size_t n)
{
    const uint8_t* p = (const uint8_t *)ptr;
    for (size_t i = 0; i < n; i++){
        printf("%02X ", p[i]);
    }
    printf("\n");
}

// Helper to compare arrays
int compare_bytes(const void *a, const void *b, size_t n) {
    const uint8_t *p1 = (const uint8_t *)a;
    const uint8_t *p2 = (const uint8_t *)b;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i])
            return 0; // mismatch
    }
    return 1; // match
}


// Helper to run a memcpy test
void run_test(const uint8_t *src, size_t src_size, size_t copy_size, size_t dest_offset, const char *description) {
    uint8_t dest[128] = {0};
    uint8_t expected[128] = {0};

    memcpy(expected + dest_offset, src, copy_size);
    my_memcpy(dest + dest_offset, src, copy_size);

    printf("%s: ", description);
    if (compare_bytes(dest, expected, dest_offset + copy_size)) {
        printf(ANSI_COLOR_GREEN "PASS" ANSI_COLOR_RESET "\n");
    } else {
        printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "\n");
        printf("Expected: ");
        print_bytes(expected, dest_offset + copy_size);
        printf("Got     : ");
        print_bytes(dest, dest_offset + copy_size);
    }
}

// Helper to run a memmove test
void run_test_memmove(const uint8_t *src_data, size_t total_size, size_t dest_offset, size_t src_offset, size_t move_size, const char* description)
{
    uint8_t buffer[128] = {0};
    uint8_t expected[128] = {0};

    // Set up initial data
    memcpy(buffer, src_data, total_size);
    memcpy(expected, src_data, total_size);

    // Perform standard memmove for expected
    memmove(expected + dest_offset, expected + src_offset, move_size);

    // Perform custom memmove
    my_memmove(buffer + dest_offset, buffer + src_offset, move_size);

    printf("%s: ", description);
    if (compare_bytes(buffer, expected, total_size)) {
        printf(ANSI_COLOR_GREEN "PASS" ANSI_COLOR_RESET "\n");
    } else {
        printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "\n");
        printf("Expected: ");
        print_bytes(expected, total_size);
        printf("Got     : ");
        print_bytes(buffer, total_size);
    }
}

//helper to run copy_to_buffer test
void run_test_copy_to_buffer(const void *src_data, size_t copy_size, const char* description)
{
    uint32_t buffer[128] = {0};         // Destination
    uint8_t  expected[sizeof(buffer)] = {0};       // Expected destination

    // Use standard memcpy to create expected
    memcpy(expected, src_data, copy_size);

    // Use your custom function
    copy_to_buffer(buffer, src_data, copy_size);

    printf("%s: ", description);
    if (compare_bytes(buffer, expected, copy_size)) {
        printf(ANSI_COLOR_GREEN "PASS" ANSI_COLOR_RESET "\n");
    } else {
        printf(ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET "\n");
        printf("Expected: ");
        print_bytes(expected, copy_size);
        printf("Got     : ");
        print_bytes(buffer, copy_size);
    }
}

int main(void) {
    printf("\n--- Testing my_memcpy() ---\n");

    uint8_t src1[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    uint8_t src2[9] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22, 0x33};
    uint8_t src3[4] = {0xDE, 0xAD, 0xBE, 0xEF};

    uint8_t src4[16] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B,
        0x0C, 0x0D, 0x0E, 0x0F
    };

    uint8_t src5[20] = {
        0xAA, 0x55, 0xAA, 0x55,
        0xAA, 0x55, 0xAA, 0x55,
        0xAA, 0x55, 0xAA, 0x55,
        0xAA, 0x55, 0xAA, 0x55,
        0xAA, 0x55, 0xAA, 0x55
    };

    uint8_t src6[24] = {
        0xDE, 0xAD, 0xBE, 0xEF,
        0xBA, 0xAD, 0xF0, 0x0D,
        0xFE, 0xED, 0xFA, 0xCE,
        0xC0, 0xFF, 0xEE, 0x00,
        0x11, 0x22, 0x33, 0x44,
        0x55, 0x66, 0x77, 0x88
    };

    run_test(src1, sizeof(src1), 8, 0, "Test 1 - Full aligned copy");
    run_test(src1, sizeof(src1), 5, 0, "Test 2 - Partial aligned copy (5 bytes)");
    run_test(src2 + 1, sizeof(src2) - 1, 7, 1, "Test 3 - Misaligned copy (offset by 1)");
    run_test(src3, sizeof(src3), 0, 0, "Test 4 - Copy 0 bytes (should do nothing)");
    run_test(src3, sizeof(src3), 1, 0, "Test 5 - Copy only 1 byte");
    run_test(src3, sizeof(src3), 4, 0, "Test 6 - Copy exactly 4 bytes");
    run_test(src3 + 1, sizeof(src3) - 1, 2, 1, "Test 7 - Misaligned small copy (2 bytes)");

    uint8_t large_src[100];
    for (int i = 0; i < 100; i++) {
        large_src[i] = (uint8_t)(i + 1);
    }
    run_test(large_src, sizeof(large_src), 100, 0, "Test 8 - Large aligned copy (100 bytes)");

    uint8_t weird_src[5] = {0x10, 0x20, 0x30, 0x40, 0x50};
    run_test(weird_src, sizeof(weird_src), 3, 2, "Test 9 - Copy with weird offset (start mid-array)");

    printf("\n--- Testing my_memmove() ---\n");

    uint8_t move_src[20];
    for (int i = 0; i < 20; i++) {
        move_src[i] = (uint8_t)(0xA0 + i);
    }

    run_test_memmove(move_src, sizeof(move_src), 0, 5, 10, "Test 10 - Non-overlapping memmove");
    run_test_memmove(move_src, sizeof(move_src), 5, 0, 10, "Test 11 - Overlapping forward memmove (dest > src)");
    run_test_memmove(move_src, sizeof(move_src), 0, 5, 10, "Test 12 - Overlapping backward memmove (dest < src)");
    run_test_memmove(move_src, sizeof(move_src), 3, 3, 10, "Test 13 - Overlapping exact src==dest (should copy itself)");
    run_test_memmove(move_src, sizeof(move_src), 5, 5, 0, "Test 14 - Move zero bytes (nothing happens)");

    printf("\n--- Testing copy_to_buffer() ---\n");
    run_test_copy_to_buffer(src4, sizeof(src1), "Test 1 - Full aligned copy");
    run_test_copy_to_buffer(src4, 5, "Test 2 - Partial copy 5 bytes");
    run_test_copy_to_buffer(src5 + 1, 5, "Test 3 - Misaligned copy (start from src+1)");
    run_test_copy_to_buffer(src6, 0, "Test 4 - Zero bytes copy");
    run_test_copy_to_buffer(src6, 1, "Test 5 - Copy only 1 byte");
    run_test_copy_to_buffer(src6, 4, "Test 6 - Copy exactly 4 bytes");

    printf("\n--- Testing Memory Realignment ---\n");
    uint8_t buffer[10]; // buffer size 10 bytes
    for (int i = 0; i < 10; i++) {
        buffer[i] = i;
    }
    // Intentional misalignment: pointer starts at index 1 (instead of index 0)
    misaligned_memory_access(buffer + 1, 10);
    return 0;
}