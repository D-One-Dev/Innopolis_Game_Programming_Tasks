#include <stdio.h>

long long fib(int n)
{
    if (n == 1 || n == 2) return 1;
    return fib(n - 1) + fib(n - 2);
}

int main(void) {
    for (int n = 1; n <= 10; n++)
    {
        printf("x(%d) = %lld\n", n, fib(n));
    }

    return 0;
}
