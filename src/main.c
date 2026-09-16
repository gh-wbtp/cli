#include <stdio.h>

int usage(int argc, const char *argv[], const char *error)
{
    if (error != NULL)
        fprintf(stderr, "%s\n", error);

    fprintf(stderr, "Usage: %s [--type <get|put|request>] [--verbose] <url>\n", argv[0]);
    return 1;
}

int main(int argc, const char *argv[])
{
    if (argc == 1)
        return usage(argc, argv, "Missing required fields! '[*]' means optional, '<*>' means required. '<*>' nested inside a '[*]' means required only when optional exists.");

    return 0;
}
