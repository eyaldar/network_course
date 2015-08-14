#include "UDPTimeClient.h"

void main()
{
	SOCKET m_socket;

	// If can't initialize Winsock or can't create socker
	if (!init_winsock() || !(m_socket = create_socket()))
	{
		return;
	}

	sockaddr_in server = create_server(m_socket);

	// Send and receive data.
	
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	while(true) 
	{
		int request_type = get_request_string(sendBuff);
		if (request_type == EXIT_REQUEST)
		{
			break;
		}

		switch ((METHOD_TYPE)request_type)
		{
			case GetClientToServerDelayEstimation:
			{
				break;
			}
			case MeasureRTT:
			{
				break;
			}
			default: 
			{
				if (send_to_server(sendBuff, m_socket, server) == SOCKET_ERROR || 
					recv_from_server(recvBuff, m_socket, server) == SOCKET_ERROR)
				{
					return;
				}

				break;
			}
		}
	}

	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection" << endl;
	closesocket(m_socket);
	WSACleanup();
}

int init_winsock()
{
	WSAData wsaData;

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()" << endl;

		return 0;
	}

	return 1;
}

SOCKET create_socket()
{
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	return m_socket;
}

sockaddr_in create_server(SOCKET socket)
{
	// For a client to communicate on a network, it must connect to a server.
	// Need to assemble the required data for connection in sockaddr structure.
	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	return server;
}

int get_request_string(char* sendBuff)
{
	char* request = 0;
	int user_choice;

	while (request == 0)
	{
		cout << endl;
		cout << "1. Get Time"									<< endl;
		cout << "2. Get Time Without Date"						<< endl;
		cout << "3. Get Time Since Epoch"						<< endl;
		cout << "4. Get Client to Server Delay Estimation"		<< endl;
		cout << "5. Measure RTT"								<< endl;
		cout << "6. Get Time Without Date or Seconds"			<< endl;
		cout << "7. Get Year"									<< endl;
		cout << "8. Get Month and Day"							<< endl;
		cout << "9. Get Seconds Since Begining of Month"		<< endl;
		cout << "10. Get Day of Year"							<< endl;
		cout << "11. Get Daylight Savings"						<< endl;
		cout << "12. Exit"										<< endl;
																
		cout << "Please choose a method from the menu (1 - 11): ";
		cin >> user_choice;

		if (user_choice == EXIT_REQUEST)
		{
			return EXIT_REQUEST;
		}

		request = get_method_string((METHOD_TYPE)(user_choice - 1));
	}

	strcpy(sendBuff, request);

	return user_choice - 1;
}

int send_to_server(char* sendBuff, SOCKET socket, sockaddr_in server)
{
	int bytesSent = sendto(socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr *)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(socket);
		WSACleanup();
		return SOCKET_ERROR;
	}
	cout << endl << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message" << endl;

	return bytesSent;
}

int recv_from_server(char* recvBuff, SOCKET socket, sockaddr_in server)
{
	// Gets the server's answer using simple recieve (no need to hold the server's address).
	int bytesRecv = recv(socket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(socket);
		WSACleanup();
		return SOCKET_ERROR;
	}

	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message" << endl << endl;

	return bytesRecv;
}