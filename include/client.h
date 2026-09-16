#pragma once

#include <wbtp.h>

int client_request(int argc, const char *argv[], const WbtpRequest request, const char *hostname, uint16_t port);
