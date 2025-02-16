#include <stdio.h>
#include <setjmp.h>

static jmp_buf env;

static void prepareForDangerousJump()
{
#define EXPECTED_VALUE 50
    int x = EXPECTED_VALUE;
    printf("jumpSetter: setting jump and returning immediateley\n");
    setjmp(env);
    printf("Value should be %d. Its actual value is %d.\n", EXPECTED_VALUE, x);
#undef EXPECTED_VALUE
}

static void doDangerousJump()
{
    printf("doDangerousJump: about to make very dangerous jump\n");
    longjmp(env, 1);
    printf("doDangerousJump: is this ever reached?\n");
}

int
main(int argc, char *argv[])
{
    char *endingMessage = "Back in main after dangerous jump";
    printf("In main, about to set dangerous jump\n");
    prepareForDangerousJump();
    doDangerousJump();
    puts(endingMessage);
}
