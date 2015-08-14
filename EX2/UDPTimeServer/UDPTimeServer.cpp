#include "UDPTimeServer.h"

using namespace std;

void main()
{
	SOCKET m_socket;

	// If can't initialize Winsock or can't create and bind the socket
	if (!init_winsock() || !(m_socket = create_socket()) || !bind_socket(m_socket))
	{
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		METHOD_TYPE method_type = get_method_type(recvBuff);

		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message." 
			 << endl << "Interperting as '" << get_method_string(method_type) << "' request.\n";

		// Determine request type:



		// Answer client's request.
		handle_request(method_type, sendBuff);

		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr *)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}

int init_winsock() 
{
	WSAData wsaData;

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";

		return 0;
	}

	return 1;
}

SOCKET create_socket()
{
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}

	return m_socket;
}

int bind_socket(SOCKET socket)
{
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(TIME_PORT);

	if (SOCKET_ERROR == bind(socket, (SOCKADDR *)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(socket);
		WSACleanup();
		return 0;
	}

	return 1;
}

void handle_request(METHOD_TYPE method_type, char* sendBuff)
{
	// Get the current time.
	struct tm* ts;
	time_t timer;
	time(&timer);

	switch (method_type)
	{
	case GetTime:
		strcpy(sendBuff, ctime(&timer));
		sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
		break;
	case GetTimeWithoutDate:
		ts = localtime(&timer);
		strftime(sendBuff, 255, "%H:%M:%S", ts);
		break;
	case GetTimeSinceEpoch:
		localtime(&timer);
		sprintf(sendBuff, "%d", (int)timer);
		break;
	case GetClientToServerDelayEstimation:
		sprintf(sendBuff, "%d", GetTickCount());
		break;
	case MeasureRTT:
		sprintf(sendBuff, "%d", GetTickCount());
		break;
	case GetTimeWithoutDateOrSeconds:
		ts = localtime(&timer);
		strftime(sendBuff, 255, "%H:%M", ts);
		break;
	case GetYear:
		ts = localtime(&timer);
		strftime(sendBuff, 255, "%Y", ts);
		break;
	case GetMonthAndDay:
		ts = localtime(&timer);
		strftime(sendBuff, 255, "%B, %d", ts);
		break;
	case GetSecondsSinceBeginingOfMonth:
	{
		ts = localtime(&timer);
		ts->tm_mday = 1;
		ts->tm_sec = 0;
		ts->tm_min = 0;
		ts->tm_hour = 0;

		time_t begin_of_month = mktime(ts);
		sprintf(sendBuff, "%d", (int)difftime(timer, begin_of_month));
		break;
	}
	case GetDayOfYear:
		ts = localtime(&timer);
		strftime(sendBuff, 255, "%j", ts);
		break;
	case GetDaylightSavings:
		ts = localtime(&timer);
		sprintf(sendBuff, "%d", ts->tm_isdst);
		break;
	default:
		strcpy(sendBuff, INVALID_REQUEST_MSG);
		break;
	}
}