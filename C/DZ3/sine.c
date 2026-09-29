#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.14159265358979323846
#define FIRST_DURATION 3.0
#define TOTAL_DURATION 4.0

enum {
    F1, F2, F3, A1, A2, A3, FS, PARAMS_COUNT
};

static const char *const param_names[PARAMS_COUNT] = {
    "f1", "f2", "f3", "a1", "a2", "a3", "fs"
};

static void Fail(const char *fmt, ...)
{
    va_list ap;

    fputs("Error", stderr);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

static int ParamIndex(const char *name)
{
    int i;
    for (i = 0; i < PARAMS_COUNT; i++) {
        if (strcmp(name, param_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static int IsSpaceOrEnd(int c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f';
}

static void ReadParams(const char *path, double p[PARAMS_COUNT])
{
    FILE *f = fopen(path, "r");
    char line[256];
    int seen[PARAMS_COUNT];
    int i, pos;

    if (f == NULL) {
        Fail("Couldn't open file \"%s\".", path);
    }

    for (i = 0; i < PARAMS_COUNT; i++) {
        p[i] = 0.0;
        seen[i] = 0;
    }

    while (fgets(line, sizeof line, f) != NULL) {
        char name[16];
        char tail;
        double value;
        int idx;

        pos = 0;
        while (IsSpaceOrEnd((unsigned char)line[pos])) {
            pos++;
        }
        if (line[pos] == '\0') {
            continue;
        }

        if (sscanf(line + pos, " %15[^=] = %lf %c", name, &value, &tail) != 2) {
            fclose(f);
            Fail("Couldn't decode string in file \"%s\".", path);
        }

        idx = ParamIndex(name);
        if (idx < 0) {
            fclose(f);
            Fail("Unknown parameter \"%s\" in file \"%s\".", name, path);
        }
        p[idx] = value;
        seen[idx] = 1;
    }

    if (ferror(f)) {
        fclose(f);
        Fail("Error reading file \"%s\".", path);
    }
    fclose(f);

    for (i = 0; i < PARAMS_COUNT; i++) {
        if (!seen[i]) {
            Fail("File \"%s\" is missing parameter \"%s\".", path, param_names[i]);
        }
    }

    if (p[FS] <= 0.0) {
        Fail("Parameter \"fs\" in file \"%s\" must be grater than 0.", path);
    }
    if (p[F1] < 0.0 || p[F2] < 0.0 || p[F3] < 0.0) {
        Fail("Parameters \"f1\", \"f2\", \"f3\" in file \"%s\" must be non-negative.", path);
    }
}

static void FreqAndAmplitude(double t, const double p[PARAMS_COUNT],
                               double *f, double *a)
{
    if (t < FIRST_DURATION) {
        *f = p[F1] + (p[F2] - p[F1]) * (t / FIRST_DURATION);
        *a = p[A1] + (p[A2] - p[A1]) * (t / FIRST_DURATION);
    } else {
        double u = (t - FIRST_DURATION) / (TOTAL_DURATION - FIRST_DURATION);
        *f = p[F2] + (p[F3] - p[F2]) * u;
        *a = p[A2] + (p[A3] - p[A2]) * u;
    }
}

int main(int argc, char *argv[])
{
    double p[PARAMS_COUNT];
    double *signal;
    size_t count, i;
    double phase = 0.0;
    FILE *out;

    if (argc != 3) {
        fprintf(stderr, "Using: %s <входной_файл> <выходной_файл>\n",
                argc > 0 ? argv[0] : "chirp");
        return EXIT_FAILURE;
    }

    ReadParams(argv[1], p);

    count = (size_t)llround(p[FS] * TOTAL_DURATION);
    if (count == 0) {
        Fail("Parameters from file \"%s\" produce zero counts.", argv[1]);
    }

    signal = malloc(count * sizeof *signal);
    if (signal == NULL) {
        Fail("Cannot allocate memory for the array of %zu values.", count);
    }

    for (i = 0; i < count; i++) {
        double t = (double)i / p[FS];
        double f, a;
        FreqAndAmplitude(t, p, &f, &a);
        phase += 2.0 * PI * f / p[FS];
        signal[i] = a * sin(phase);
    }

    out = fopen(argv[2], "wb");
    if (out == NULL) {
        free(signal);
        Fail("Couldn't open file \"%s\" for write.", argv[2]);
    }

    if (fwrite(signal, sizeof *signal, count, out) != count) {
        fclose(out);
        free(signal);
        Fail("Error writing to file \"%s\".", argv[2]);
    }

    if (fclose(out) != 0) {
        free(signal);
        Fail("Error closing file \"%s\".", argv[2]);
    }
    free(signal);

    printf("Written %zu double values (%.2f с, fs = %.2f Hz) into file \"%s\".\n",
           count, TOTAL_DURATION, p[FS], argv[2]);

    return EXIT_SUCCESS;
}
