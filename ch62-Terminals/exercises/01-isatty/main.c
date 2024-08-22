#include "isatty.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    if (_isatty(STDIN_FILENO))
        printf("File descriptor %d refers to a terminal\n", STDIN_FILENO);
    else
        printf("File descriptor %d does not refer to a termina\n", STDIN_FILENO);
    exit(EXIT_SUCCESS);

}
