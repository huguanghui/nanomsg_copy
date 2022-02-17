#include <stdio.h>

#include "../src/nn.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    nn_errno();
    printf("Hello World!\n");
    return 0;
}
