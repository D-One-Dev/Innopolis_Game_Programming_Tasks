#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

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

typedef struct
{
    int id;
    char name[32];
    uint64_t value;
    double score;
} Item;

int cmp_item(const void *a, const void *b)
{
    const Item *ia = (const Item *)a;
    const Item *ib = (const Item *)b;

    if (ia->value < ib->value) return -1;
    if (ia->value > ib->value) return  1;
    return 0;
}

int main()
{
    Item arr[10] =
    {
        {1, "first", 100ULL, 9.5},
        {2, "second", 50ULL, 7.1},
        {3, "third", 200ULL, 8.3},
        {4, "fourth", 10ULL, 6.0},
        {5, "fith", 150ULL, 9.0},
        {6, "sixth", 75ULL, 5.5},
        {7, "seventh", 300ULL, 4.2},
        {8, "eigth", 25ULL, 8.8},
        {9, "ninth", 125ULL, 7.7},
        {10, "tenth", 5ULL, 3.3}
    };

    printf("Before sorting:\n");
    for (int i = 0; i < 10; ++i) printf("id=%d name=%-8s value=%llu score=%.1f\n", arr[i].id, arr[i].name, (unsigned long long)arr[i].value, arr[i].score);
    printf("\n");

    my_sort(arr, 10, sizeof(Item), cmp_item);

    printf("After sorting by 'value':\n");
    for (int i = 0; i < 10; ++i) printf("id=%d name=%-8s value=%llu score=%.1f\n", arr[i].id, arr[i].name, (unsigned long long)arr[i].value, arr[i].score);
    printf("\n");

    return 0;
}
