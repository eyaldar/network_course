#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>

// Don't forget to include "Ws2_32.lib" in the library list.
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include "MessageTypes.h"

#define TIME_PORT	27015

#define INVALID_REQUEST_MSG "The requested method doesn't exist."

// Functions
int init_winsock();
SOCKET create_socket();
int bind_socket(SOCKET socket);
void handle_request(METHOD_TYPE method_type, char* sendBuff);