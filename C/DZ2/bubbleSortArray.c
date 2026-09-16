#include <stddef.h>
#include <stdio.h>

void my_sort(void *base, size_t number, size_t width, int (*compare)(const void *, const void *))
{
    if (base == NULL || number < 2 || width == 0 || compare == NULL) return;

    char *arr = (char *)base;
    int swapped;

    do
    {
        swapped = 0;
        for (size_t i = 0; i < number - 1; ++i)
        {
            char *a = arr + i * width;
            char *b = arr + (i + 1) * width;

            if (compare(a, b) > 0)
            {
                for (size_t j = 0; j < width; ++j)
                {
                    char tmp = a[j];
                    a[j] = b[j];
                    b[j] = tmp;
                }
                swapped = 1;
            }
        }
    }
    while (swapped);
}

int cmp_int(const void *a, const void *b)
{
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

int main()
{
    int arr[10] = {42, 17, 8, 99, 3, 56, 21, 7, 12, 0};

    printf("Before sorting:\n");
    for(int i = 0; i < 10; ++i) printf("%d ", arr[i]);
    printf("\n");

    my_sort(arr, 10, sizeof(int), cmp_int);

    printf("After sorting:\n");
    for(int i = 0; i < 10; ++i) printf("%d ", arr[i]);
    printf("\n");

    return 0;
}
