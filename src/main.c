#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <wbtp.h>
#include <wbtp/net.h>
#include <wbtp/errors.h>

typedef struct
{
    WbtpRequestType type;
    const char *params;
    const char *payload;
    bool verbose;
    const char *url;
} CliFlags;

int usage(int argc, const char *argv[], const char *error)
{
    if (error != NULL)
        fprintf(stderr, "%s\n", error);

    fprintf(stderr, "Usage: %s [--type <get|put|request>] [--params <params>] [--payload <payload>] [--verbose] <url>\n", argv[0]);
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

        if (strcmp(arg, "--verbose") == 0)
        {
            flags->verbose = true;
            continue;
        }

        if (strcmp(arg, "--type") == 0)
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

        if (strcmp(arg, "--params") == 0)
        {
            if (i + 1 >= argc)
                return usage(argc, argv, "'--params' must be followed by a string!");

            flags->params = argv[++i];
            continue;
        }

        if (strcmp(arg, "--payload") == 0)
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

    WbtpSocket client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == WBTP_SOCKET_INVALID_VALUE)
    {
        fprintf(stderr, "Failed to create socket!\n");
        return 1;
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    if (inet_pton(AF_INET, hostname, &(address.sin_addr)) <= 0)
        return usage(argc, argv, "Invalid hostname!");

    if (connect(client, (struct sockaddr *)&address, sizeof(address)) == WBTP_SOCKET_ERROR_VALUE)
    {
        fprintf(stderr, "Connection refused! It's likely no server exists on that port.\n");
        close(client);
        return 1;
    }

    uint32_t request_serialized = wbtp_request_packet_serialize(request, client);
    if (!request_serialized)
    {
        fprintf(stderr, "%s\n", wbtp_get_error());
        close(client);
        return 1;
    }

    char payload[4096];
    WbtpResponse response = wbtp_response(WBTP_RESPONSE, 0, payload);
    if (!wbtp_response_packet_deserialize(&response, client))
    {
        fprintf(stderr, "%s\n", wbtp_get_error());
        close(client);
        return 1;
    }

    if (response.type == WBTP_RESPONSE)
    {
        if (request.type != WBTP_REQUEST)
        {
            printf("Server responded with an unexpected RESPONSE. To maintain spec compliance, we recommend servers only respond with RESPONSE to REQUEST requests!\n");
            return 1;
        }

        printf("Server responded!\n");
        close(client);
        return 0;
    }

    if (response.type == WBTP_FAILURE)
    {
        if (request.type == WBTP_REQUEST)
        {
            printf("Server responded with an unexpected FAILURE. To maintain spec compliance, we recommend servers only respond to REQUEST with RESPONSE! If a failure needs to be indicated, this can be done by not responding at all.\n");
            close(client);
            return 1;
        }

        char buf[4096];
        if (!wbtp_response_string(response, buf, 4096))
        {
            fprintf(stderr, "%s\n", wbtp_get_error());
            close(client);
            return 1;
        }

        printf("%s\n", buf);
        close(client);
        return 0;
    }

    printf("%.*s\n", response.payload_size, response.payload);
    close(client);
    free(hostname);
    return 0;
}
