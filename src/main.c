#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <wbtp/net.h>
#include "cli.h"
#include "client.h"

int main(int argc, const char *argv[])
{
    CliFlags flags = {};
    int flags_filled = fill_flags(argc, argv, &flags);
    if (flags_filled != 0)
        return flags_filled;

    if (flags.payload != NULL && flags.type != WBTP_PUT)
        return usage(argc, argv, "A payload can only be passed to PUT requests!");

    size_t url_length = strlen(flags.url);
    char *hostname = malloc(url_length + 1);
    uint16_t port = WBTP_PORT_TCP;
    char path[WBTP_PATH_MAX];

    size_t hostname_length = 0;
    size_t path_length = 0;
    size_t i = 0;
    while (i < url_length && flags.url[i] != ':' && flags.url[i] != '/')
        hostname[hostname_length++] = flags.url[i++];

    if (hostname_length == 0)
    {
        free(hostname);
        return usage(argc, argv, "Hostname not specified!");
    }

    hostname[hostname_length] = '\0';

    if (flags.url[i] == ':')
    {
        i++;
        if (i >= url_length)
        {
            free(hostname);
            return usage(argc, argv, "Port not specified! Remove the colon (to use the default) or add one.");
        }

        uint32_t temp = 0;
        while (i < url_length && flags.url[i] != '/')
        {
            char c = flags.url[i++];
            if (c < '0' || c > '9')
            {
                free(hostname);
                return usage(argc, argv, "Invalid port! Must contain only digits.");
            }

            temp = (temp * 10) + (c - '0');
            if (temp > UINT16_MAX)
            {
                char buf[4096];
                snprintf(buf, sizeof(buf), "Port too big! Must be between 0 and %u (inclusive).", UINT16_MAX);
                free(hostname);
                return usage(argc, argv, buf);
            }
        }

        port = (uint16_t)temp;
    }

    if (i < url_length)
    {
        if (flags.url[i] != '/')
        {
            free(hostname);
            return usage(argc, argv, "Invalid URL!");
        }

        path_length = url_length - i;
        if (path_length >= sizeof(path))
        {
            free(hostname);
            return usage(argc, argv, "URL path too long!");
        }

        memcpy(path, flags.url + i, path_length);
    }
    else
    {
        path[0] = '/';
        path_length = 1;
    }

    path[path_length] = '\0';

    WbtpRequest request = wbtp_request(flags.type, path, flags.payload == NULL ? 0 : strlen(flags.payload), (char *)flags.payload);
    memcpy(request.params, flags.params, strlen(flags.params));

    int success = client_request(argc, argv, flags, request, hostname, port);
    free(hostname);
    return success;
}
