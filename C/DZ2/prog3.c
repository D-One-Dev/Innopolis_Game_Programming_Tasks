#include <stdio.h>
#include <string.h>

/* #define MY_DEBUG */
#include "my_assert.h"

int main(void)
{
    const char *s = NULL;
    my_assert(s != NULL && strlen(s) > 0, "String shouldn't be empty");
    printf("prog3: this message shouldn't be triggered if MY_DEBUG is active\n");
    return 0;
}
