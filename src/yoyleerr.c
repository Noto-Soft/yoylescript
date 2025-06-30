#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

__attribute__((noreturn))
void fatal(const char *msg) {
    fprintf(stderr, "\x1b[1myoyle: \x1b[31mfatal error: \x1b[0m%s\n", msg);
    exit(EXIT_FAILURE);
}

__attribute__((noreturn))
void fatalf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (needed < 0) {
        fprintf(stderr, "\x1b[1myoyle: \x1b[31mfatal error: \x1b[0m(formatting failed)\n");
        exit(EXIT_FAILURE);
    }

    char *msg = (char *)malloc(needed + 1);
    if (!msg) {
        fprintf(stderr, "\x1b[1myoyle: \x1b[31mfatal error: \x1b[0m(out of memory)\n");
        exit(EXIT_FAILURE);
    }

    vsnprintf(msg, needed + 1, fmt, args);
    va_end(args);

    fprintf(stderr, "\x1b[1myoyle: \x1b[31mfatal error: \x1b[0m%s\n", msg);
    free(msg);
    exit(EXIT_FAILURE);
}