#include <stdio.h>

int main()
{
    long long a = 1;
    long long b = 1;

    for (int n = 1; n <= 10; n++)
    {
        if (n == 1) printf("x(%d) = %lld\n", n, a);
        else if (n == 2) printf("x(%d) = %lld\n", n, b);
        else
        {
            long long next = a + b;
            a = b;
            b = next;
            printf("x(%d) = %lld\n", n, b);
        }
    }

    return 0;
}
