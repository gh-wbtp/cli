#include "cli.h"
#include <string.h>
#include <stdio.h>

int usage(int argc, const char *argv[], const char *error)
{
    if (error != NULL)
        fprintf(stderr, "%s\n", error);

    fprintf(stderr, "Usage: %s [--type|-t <get|put|request>] [--params|-p <params>] [--payload|-pl <payload>] [--verbose|-v] <url>\n", argv[0]);
    return 1;
}

int fill_flags(int argc, const char *argv[], CliFlags *flags)
{
    flags->type = WBTP_GET;
    flags->params = "";
    flags->payload = NULL;
    flags->verbose = false;
    flags->url = NULL;

    if (argc == 1)
        return usage(argc, argv, "Missing required fields! '[*]' means optional, '<*>' means required. '<*>' nested inside a '[*]' means required only when optional exists.");

    for (size_t i = 1; i < argc; i++)
    {
        char *arg = (char *)argv[i];

        if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-v") == 0)
        {
            flags->verbose = true;
            continue;
        }

        if (strcmp(arg, "--type") == 0 || strcmp(arg, "-t") == 0)
        {
            if (i + 1 >= argc)
                return usage(argc, argv, "'--type' must be followed by a valid request type!");

            const char *type = argv[++i];

            if (strcmp(type, "request") == 0)
            {
                flags->type = WBTP_REQUEST;
                continue;
            }

            if (strcmp(type, "get") == 0)
            {
                flags->type = WBTP_GET;
                continue;
            }

            if (strcmp(type, "put") == 0)
            {
                flags->type = WBTP_PUT;
                continue;
            }

            char buf[4096];
            snprintf(buf, 4096, "Unknown request type, '%s'!", arg);
            return usage(argc, argv, buf);
        }

        if (strcmp(arg, "--params") == 0 || strcmp(arg, "-p") == 0)
        {
            if (i + 1 >= argc)
                return usage(argc, argv, "'--params' must be followed by a string!");

            flags->params = argv[++i];
            continue;
        }

        if (strcmp(arg, "--payload") == 0 || strcmp(arg, "-pl") == 0)
        {
            if (i + 1 >= argc)
                return usage(argc, argv, "'--payload' must be followed by a string!");

            flags->payload = argv[++i];
            continue;
        }

        if (flags->url != NULL)
        {
            char buf[4096];
            snprintf(buf, 4096, "Unknown option, '%s'! If this is intended to be the URL, there must be a loose flag somewhere.\n\tCurrent URL: %s", arg, flags->url);
            return usage(argc, argv, buf);
        }

        flags->url = (const char *)arg;
    }

    if (flags->url == NULL)
        return usage(argc, argv, "URL not specified!");

    return 0;
}
