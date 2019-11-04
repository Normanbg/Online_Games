#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "Windows.h"
#include "WinSock2.h"
#include "Ws2tcpip.h"

#include <iostream>
#include <cstdlib>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_ADDRESS "127.0.0.1"

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

void client(const char *serverAddrStr, int port)
{
	std::string error = "";
	int counter = 0;

	// TODO-1: Winsock init

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NOERROR)
	{
		error="Client Error initializing Winsock:";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-2: Create socket (IPv4, datagrams, UDP)

	SOCKET mySocket_client = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (mySocket_client == INVALID_SOCKET)
	{
		error = "Client Error creating the socket:";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-3: Create an address object with the server address
	
	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;							//To indicate that we are using a the IPv4 adress family.
	remoteAddr.sin_port = htons(port);							//Transforms the port address to the network order.
	inet_pton(AF_INET, serverAddrStr, &remoteAddr.sin_addr);	//transforming the string into the appropriate library's IP.

	while (counter<5)
	{
		// TODO-4:
		// - Send a 'ping' packet to the server
		std::string msg = "ping";
		iResult = sendto(mySocket_client, msg.c_str(), msg.size(), NULL, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
		if (iResult == SOCKET_ERROR)
		{
			error = "Client Error Sending the message:";
			printWSErrorAndExit(error.c_str());
		}

		// - Control errors in both cases
		// - Receive 'pong' packet from the server
		char res_msg[10];
		int test=sizeof(remoteAddr);
		
		iResult = recvfrom(mySocket_client, (char*) res_msg, 10, NULL, (struct sockaddr*)&remoteAddr, &test);
		if (iResult == SOCKET_ERROR)
		{
			error = "Client Error receiving the message:";
			printWSErrorAndExit(error.c_str());
		}
		else
		{
			res_msg[iResult] = '\0';
			std::cout << res_msg << std::endl;
			counter++;
			Sleep(500);
		}
	}

	// TODO-5: Close socket
	
	iResult = closesocket(mySocket_client);

	if (iResult == SOCKET_ERROR)
	{
		std::string error = "Client Error closing the socket:";
		printWSErrorAndExit(error.c_str());
	}

	// TODO-6: Winsock shutdown

	iResult = WSACleanup();
	
	if (iResult != NOERROR)
	{
		std::string error = "Client Error shuting down the library:";
		printWSErrorAndExit(error.c_str());
	}
}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
