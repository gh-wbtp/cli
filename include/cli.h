#pragma once

#include <wbtp.h>

typedef struct
{
    WbtpRequestType type;
    const char *params;
    const char *payload;
    bool verbose;
    const char *url;
} CliFlags;

int usage(int argc, const char *argv[], const char *error);

int fill_flags(int argc, const char *argv[], CliFlags *flags);
