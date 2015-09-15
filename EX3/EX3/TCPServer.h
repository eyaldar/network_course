#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include "HTTPRequestParser.h"
#include <iostream>
// Don't forget to include "Ws2_32.lib" in the library list.
#include <winsock2.h>
#include <time.h>
#include <fstream>
#include <direct.h>

#define BUFFER_SIZE 1024

typedef enum socket_status {
	EMPTY = 0,
	LISTEN = 1,
	RECEIVE = 2,
	IDLE = 3,
	SEND = 4,
} socket_status;

typedef enum http_method {
	GET = 1,
	HEAD = 2,
	PUT = 3,
	DELETE_ = 4,
	TRACE = 5
} http_method;

struct SocketState {
	SOCKET id;
	socket_status recv;
	socket_status send;
	std::map<std::string, std::string> headers;
	http_method http_method;
	time_t last_activity;
	char data[BUFFER_SIZE];
	int len;
};

const int TIME_PORT = 27015;
const int MAX_SOCKETS = 60;
const int SEND_TIME = 1;
const int SEND_SECONDS = 2;

bool addSocket(SOCKET id, socket_status what);
void removeSocket(int index);
void acceptConnection(int index);
void receiveMessage(int index);
int sendMessage(int index);
void parseRequest(int index);

struct SocketState sockets[MAX_SOCKETS] = { 0 };
int socketsCount = 0;

// Files
//int write_socket_data_to_file(int socketindex, char * filename);
char* parseErrorMessage(int errorCode);