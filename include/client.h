#pragma once

#include <wbtp.h>
#include "cli.h"

int client_request(int argc, const char *argv[], const CliFlags flags, const WbtpRequest request, const char *hostname, uint16_t port);
