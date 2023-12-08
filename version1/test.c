#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *test_affiche() {
    char test[];

    printf("=> %ld\n", strlen(test));

    test = "yuiop";

    printf("=> %ld\n", strlen(test));

    return test;
}


int main(int argc, char const *argv[])
{
    char *test;
    printf("=> %s\n", test);

    test = test_affiche();
    //strcpy(test,test_affiche());

    printf("=> %s\n", test);

    return 0;
}
