#include "TCPServer.h"

void main() 
{
	time_t tCurrentTime;

    // Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
    // The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData; 
    	
	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2,2), &wsaData))
	{
        cout<<"Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.
	// Listen through the socket for incoming connections.

    // After initialization, a SOCKET object is ready to be instantiated.
	
	// Create a SOCKET object called listenSocket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							streaming sockets (SOCK_STREAM), 
	//							and the TCP/IP protocol (IPPROTO_TCP).
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The if statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == listenSocket)
	{
        cout<<"Time Server: Error at socket(): "<<WSAGetLastError()<<endl;
        WSACleanup();
        return;
	}

	// For a server to communicate on a network, it must bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

    // Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
    serverService.sin_family = AF_INET; 
	// IP address. The sin_addr is a union (s_addr is a unsigned long 
	// (4 bytes) data type).
	// inet_addr (Iternet address) is used to convert a string (char *) 
	// into unsigned long.
	// The IP address is INADDR_ANY to accept connections on all interfaces.
	serverService.sin_addr.s_addr = INADDR_ANY;
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order 
	// (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

    // The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
    if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR *) &serverService, sizeof(serverService))) 
	{
		cout<<"Time Server: Error at bind(): "<<WSAGetLastError()<<endl;
        closesocket(listenSocket);
		WSACleanup();
        return;
    }

    // Listen on the Socket for incoming connections.
	// This socket accepts only one connection (no pending connections 
	// from other clients). This sets the backlog parameter.
    if (SOCKET_ERROR == listen(listenSocket, 5))
	{
		cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
        closesocket(listenSocket);
		WSACleanup();
        return;
	}
	addSocket(listenSocket, LISTEN);

    // Accept connections and handles them one by one.
	while (true)
	{
		for (int i = 1; i < MAX_SOCKETS; i++)
		{
			tCurrentTime = time(0);
			if ((tCurrentTime - sockets[i].last_activity > 120) && (sockets[i].last_activity != 0))
			{
				removeSocket(i);
			}
		}

		// The select function determines the status of one or more sockets,
		// waiting if necessary, to perform asynchronous I/O. Use fd_sets for
		// sets of handles for reading, writing and exceptions. select gets "timeout" for waiting
		// and still performing other operations (Use NULL for blocking). Finally,
		// select returns the number of descriptors which are ready for use (use FD_ISSET
		// macro to check which descriptor in each set is ready to be used).
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
				FD_SET(sockets[i].id, &waitRecv);
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			if (sockets[i].send == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		//
		// Wait for interesting event.
		// Note: First argument is ignored. The fourth is for exceptions.
		// And as written above the last is a timeout, hence we are blocked if nothing happens.
		//
		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR)
		{
			cout <<"Time Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].recv)
				{
				case LISTEN:
					acceptConnection(i);
					break;

				case RECEIVE:
					receiveMessage(i);
					break;
				}
			}
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				switch (sockets[i].send)
				{
				case SEND:
					sendMessage(i);
					break;
				}
			}
		}
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
}

bool addSocket(SOCKET id, socket_status what)
{
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == EMPTY)
		{
			sockets[i].id = id;
			sockets[i].recv = what;
			sockets[i].send = IDLE;
			sockets[i].len = 0;
			socketsCount++;
			return (true);
		}
	}
	return (false);
}

void removeSocket(int index)
{
	sockets[index].recv = EMPTY;
	sockets[index].send = EMPTY;
	socketsCount--;
}

void acceptConnection(int index)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;		// Address of sending partner
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr *)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{ 
		 cout << "Time Server: Error at accept(): " << WSAGetLastError() << endl; 		 
		 return;
	}
	cout << "Time Server: Client "<<inet_ntoa(from.sin_addr)<<":"<<ntohs(from.sin_port)<<" is connected." << endl;

	//
	// Set the socket to be in non-blocking mode.
	//
	unsigned long flag=1;
	if (ioctlsocket(msgSocket, FIONBIO, &flag) != 0)
	{
		cout<<"Time Server: Error at ioctlsocket(): "<<WSAGetLastError()<<endl;
	}

	if (addSocket(msgSocket, RECEIVE) == false)
	{
		cout<<"\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

void receiveMessage(int index)
{
	SOCKET msgSocket = sockets[index].id;

	int len = sockets[index].len;
	int bytesRecv = recv(msgSocket, &sockets[index].data[len], sizeof(sockets[index].data) - len, 0);
	
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);			
		removeSocket(index);
		return;
	}
	if (bytesRecv == 0)
	{
		closesocket(msgSocket);			
		removeSocket(index);
		return;
	}
	else
	{
		sockets[index].data[len + bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Web Server: Recieved: " << bytesRecv << " bytes of \"" << &sockets[index].data[len] << "\" message.\n";
		
		sockets[index].len += bytesRecv;

		if (sockets[index].len > 0)
		{
			if (strncmp(sockets[index].data, "HEAD", 4) == 0)
			{
				sockets[index].send = SEND;
				sockets[index].http_method = HEAD;
				strcpy(sockets[index].data, &sockets[index].data[6]);
				sockets[index].len = strlen(sockets[index].data);
				sockets[index].data[sockets[index].len] = '\0';
				return;
			}
			else if (strncmp(sockets[index].data, "GET", 3) == 0)
			{
				sockets[index].send  = SEND;
				memcpy(sockets[index].data, &sockets[index].data[5], sockets[index].len - 5);
				sockets[index].len -= 5;

				sockets[index].http_method = GET;
				return;
			}
		}
	}

}

int sendMessage(int index)
{
	int bytesSent = 0;
	string sendBuff;
	string fSize_str;
	string message;
	string strBuff;

	int buffLen = 0;
	int fSize_int = 0;
	char* tmp = 0;
	char ctmp[20];

	char tmpbuff[BUFFER_SIZE];
	char readBuff[512], filename[128];
	time_t rawtime;
	ifstream file;
	time(&rawtime);
	SOCKET msgSocket = sockets[index].id;
	sockets[index].last_activity = time(0);

	switch (sockets[index].http_method)
	{
		case HEAD:
			tmp = strtok(sockets[index].data, " ");
			file.open(sockets[index].data);
			if (!file.is_open())
			{
				sockets[index].send = IDLE;
				cout << "HTTP Server: Error at SEND(): " << WSAGetLastError() << endl;
				return false;
			}
			else
			{
				message = "HTTP/1.1 200 OK\r\nDate: ";
				message += ctime(&rawtime);
				message += "\r\nContent-type: text/html";
				message += "\r\n\r\n";
				buffLen = message.size();
				sendBuff = message;
				file.close();
			}

			break;

		case GET:
			tmp = 0;

			tmp = strtok(sockets[index].data, " ");
			file.open(sockets[index].data);
			if (file.is_open() == false)
			{
				cout << "HTTP Server: Error at S_SEND(): " << WSAGetLastError() << endl;
				sockets[index].send = IDLE;
				return false;
			}
			memset(tmpbuff, 0, BUFFER_SIZE);
			message = strBuff = tmpbuff;
			while (file.getline(readBuff, 512))
			{
				strBuff += readBuff;
				fSize_int += strlen(readBuff);
			}

			fSize_str = _itoa(fSize_int, ctmp, 10);
			message = "HTTP/1.1 200 OK \r\nContent-type: text/html\r\nDate: ";
			message += ctime(&rawtime);
			message += "Content-length: ";
			message += fSize_str;
			message += "\r\n\r\n";

			message += strBuff;

			buffLen = message.size();
			sendBuff = message;
			file.close();
			break;
	}

	//if (sockets[index].sendSubType == SEND_TIME)
	//{
	//	// Answer client's request by the current time string.
	//	
	//	// Get the current time.
	//	time_t timer;
	//	time(&timer);
	//	// Parse the current time to printable string.
	//	strcpy(sendBuff, ctime(&timer));
	//	sendBuff[strlen(sendBuff)-1] = 0; //to remove the new-line from the created string
	//}
	//else if(sockets[index].sendSubType == SEND_SECONDS)
	//{
	//	// Answer client's request by the current time in seconds.
	//	
	//	// Get the current time.
	//	time_t timer;
	//	time(&timer);
	//	// Convert the number to string.
	//	_itoa((int)timer, sendBuff, 10);		
	//}

	bytesSent = send(msgSocket, sendBuff.c_str(), buffLen, 0);
	memset(sockets[index].data, 0, BUFFER_SIZE);
	sockets[index].len = 0;
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "HTTP Server: Error at S_SEND(): " << WSAGetLastError() << endl;
		sockets[index].send = IDLE;
		return false;
	}

	cout << "HTTP Server: Sent: " << bytesSent << "\\" << buffLen << " bytes of\n \"" << sendBuff.c_str() << "\"\n message.\n";
	sockets[index].send = IDLE;

	return true;
}

