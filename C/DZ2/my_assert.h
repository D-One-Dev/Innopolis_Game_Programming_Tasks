#ifndef MY_ASSERT_H
#define MY_ASSERT_H

#include <stdio.h>
#include <stdlib.h>

#ifdef MY_DEBUG

#define my_assert(expr, msg) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, \
                    "Assertion failed: %s\n" \
                    "  Function: %s\n" \
                    "  File:     %s\n" \
                    "  Line:     %d\n" \
                    "  Message:  %s\n", \
                    #expr, __func__, __FILE__, __LINE__, (msg)); \
            abort(); \
        } \
    } while (0)

#else

#define my_assert(expr, msg)  ((void)0)

#endif

#endif /* MY_ASSERT_H */
