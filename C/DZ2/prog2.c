#include <stdio.h>

/* #define MY_DEBUG */
#include "my_assert.h"

int main(void)
{
    int x = -5;
    my_assert(x > 0, "x must be positive");
    printf("prog2: this message shouldn't be triggered if MY_DEBUG is active\n");
    return 0;
}
