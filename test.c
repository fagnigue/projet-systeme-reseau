#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    FILE *file = fopen(argv[2], "r");
    if (file == NULL)
    {
        printf("Oups ! fichier inexistant\n");
        exit(1);
    }
    fclose(file);

    return 0;
}
