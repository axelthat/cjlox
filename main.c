#include <stdio.h>
#include "lox.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Please input a file\n");
        return 1;
    }
    lox_run(argv[1]);
    return 0;
}