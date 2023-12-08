#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char *str;

    printf("1: strlen: %ld\tsizeof: %ld\n", strlen(str), sizeof(str));

    str = calloc(10, sizeof(char));

    printf("2: strlen: %ld\tsizeof: %ld\n", strlen(str), sizeof(str));

    strcpy(str, "Bonjour");

    printf("3: strlen: %ld\tsizeof: %ld\n", strlen(str), sizeof(str));

    return 0;
}
