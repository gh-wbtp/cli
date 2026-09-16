#include "client.h"
#include <string.h>
#include <stdio.h>
#include <wbtp/net.h>
#include <wbtp/errors.h>
#include "cli.h"

int client_request(int argc, const char *argv[], const WbtpRequest request, const char *hostname, uint16_t port)
{
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
    return 0;
}
