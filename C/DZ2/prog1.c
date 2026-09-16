#include <stdio.h>

/* #define MY_DEBUG */
#include "my_assert.h"

int main()
{
    int x = 10;
    my_assert(x > 0, "x must be positive");
    printf("prog1: everything's good, assert didn't trigger");
    return 0;
}
