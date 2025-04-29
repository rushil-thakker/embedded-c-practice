#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

size_t my_strncpy(char *dest, const char *src, size_t max_len)
{
    int i = 0;

    while((i < max_len-1) && (src[i] != '\0')){
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';

    return i;
}

int main(void)
{
    int size = 10;
    char dest[size];
    char* src = ":)";

    int copied = my_strncpy(dest, src, size);
    printf("Copied string: %s\nLetters copied: %d\n", dest, copied);

}