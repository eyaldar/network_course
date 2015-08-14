#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
using namespace std;
// Don't forget to include "ws2_32.lib" in the library list.
#include <winsock2.h> 
#include <string.h>
#include "MessageTypes.h"

#define TIME_PORT	27015

#define MEASURE_COUNT 100

#define EXIT_REQUEST 12

// Functions
int init_winsock();
SOCKET create_socket();
sockaddr_in create_server(SOCKET socket);
int get_request_string(char* sendBuff);

int send_to_server(char* sendBuff, SOCKET socket, sockaddr_in server);
int recv_from_server(char* recvBuff, SOCKET socket, sockaddr_in server);