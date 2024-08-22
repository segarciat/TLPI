#include <stdio.h>
#include <stdlib.h>
#include "getpass.h"

int
main(int argc, char *argv[])
{
    char secret[PASS_MAX];
    if (_getpass("Enter secret: ", secret, PASS_MAX) == NULL){
        fprintf(stderr, "Failed");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", secret);
    exit(EXIT_SUCCESS);
}
