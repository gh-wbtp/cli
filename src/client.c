#define _POSIX_C_SOURCE 200112L
#include "client.h"
#include <string.h>
#include <stdio.h>
#include <wbtp/net.h>
#include <wbtp/errors.h>

#ifdef _WIN32
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

int client_request(int argc, const char *argv[], const CliFlags flags, const WbtpRequest request, const char *hostname, uint16_t port)
{
    WbtpSocket client = WBTP_SOCKET_INVALID_VALUE;

    char port_string[6];
    snprintf(port_string, sizeof(port_string), "%u", port);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *addresses;
    if (getaddrinfo(hostname, port_string, &hints, &addresses) != 0)
        return usage(argc, argv, "Invalid hostname!");

    for (struct addrinfo *address = addresses; address; address = address->ai_next)
    {
        client = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
        if (client == WBTP_SOCKET_INVALID_VALUE)
            continue;

        if (connect(client, address->ai_addr, address->ai_addrlen) == 0)
            break;

        close(client);
        client = WBTP_SOCKET_INVALID_VALUE;
    }

    freeaddrinfo(addresses);
    if (client == WBTP_SOCKET_INVALID_VALUE)
    {
        fprintf(stderr, "Connection refused! It's likely no server exists on that port.\n");
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

    if (flags.verbose)
    {
        char buf[4096];
        if (!wbtp_response_string(response, buf, 4096))
        {
            fprintf(stderr, "%s\n", wbtp_get_error());
            close(client);
            return 1;
        }

        printf("%s\n", buf);
    }
    else if (flags.raw)
        fwrite(response.payload, 1, response.payload_size, stdout);
    else
        printf("%.*s\n", response.payload_size, response.payload);

    close(client);
    return 0;
}
