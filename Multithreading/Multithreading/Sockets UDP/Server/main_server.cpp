#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888

#define PAUSE_AND_EXIT() system("pause"); exit(-1)

void printWSErrorAndExit(const char *msg)
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	fprintf(stderr, "%s: %S\n", msg, s);
	LocalFree(s);
	PAUSE_AND_EXIT();
}

void server(int port)
{
	std::string error="";
	int counter = 0;

	// TODO-1: Winsock init

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NOERROR)
	{
		error = "Server Error initializing Winsock:";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-2: Create socket (IPv4, datagrams, UDP)

	SOCKET mySocket_server = socket(AF_INET, SOCK_DGRAM, 0);

	if (mySocket_server == INVALID_SOCKET)
	{
		error = "Server Error creating the socket:";
		printWSErrorAndExit(error.c_str());
	}

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = htons(port);
	bindAddr.sin_addr.S_un.S_addr = INADDR_ANY; 

	// TODO-3: Force address reuse

	int enable = 1;
	iResult = setsockopt(mySocket_server, SOL_SOCKET, SO_REUSEADDR, (const char *)& enable, sizeof(int));
	
	if (iResult == SOCKET_ERROR) 
	{ 
		error = "Server Error it's not possible to bind the given socket";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-4: Bind to a local address

	iResult = bind(mySocket_server, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));

	if (iResult == SOCKET_ERROR)
	{
		error = "Server Error binding the socket to the IP";
		printWSErrorAndExit(error.c_str());
	}

	while (counter<5)
	{
		// TODO-5:
		// - Control errors in both cases
		// - Receive 'ping' packet from a remote host
		
		char res_msg[10];
		int test = sizeof(sockaddr_in);

		iResult = recvfrom(mySocket_server, (char*)res_msg, 10, NULL, (struct sockaddr*)&bindAddr, &test);
		if (iResult == SOCKET_ERROR)
		{
			error = "Server Error receiving the message";
			printWSErrorAndExit(error.c_str());
		}
		else
		{
			res_msg[iResult] = '\0';			//we cap the message if it's shorter than the memory allocation
			std::cout << res_msg << std::endl;	//printing the message
			Sleep(500);
		}

		// - Answer with a 'pong' packet

		std::string msg = "pong";
		iResult = sendto(mySocket_server, msg.c_str(), msg.size(), NULL, (const struct sockaddr*)&bindAddr, sizeof(bindAddr));
		if (iResult == SOCKET_ERROR)
		{
			error = "Server Error Sending the message";
			printWSErrorAndExit(error.c_str());
		}
		else
		{
			counter++;
		}

	}

	// TODO-6: Close socket

	iResult = closesocket(mySocket_server);

	if (iResult == SOCKET_ERROR)
	{
		std::string error = "Server Error closing the socket";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-7: Winsock shutdown

	iResult = WSACleanup();

	if (iResult != NOERROR)
	{
		std::string error = "Server Error shutingdown the library";
		printWSErrorAndExit(error.c_str());
	}
}

int main(int argc, char **argv)
{
	server(SERVER_PORT);

	PAUSE_AND_EXIT();
}
