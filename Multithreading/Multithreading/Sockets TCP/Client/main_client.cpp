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
	// TODO-1: Winsock init
	WSADATA wsaData;
	std::string Error = "error";
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != NOERROR)
	{
		Error = "Client Error Initializing Winsock";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-2: Create socket (IPv4, stream, TCP)
	SOCKET mySocket_client = socket(AF_INET, SOCK_STREAM, 0);
	
	if (mySocket_client == INVALID_SOCKET)
	{
		Error = "Client Error creating the TCP Socket";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-3: Create an address object with the server address
	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(port);
	inet_pton(AF_INET, serverAddrStr, &remoteAddr.sin_addr);

	// TODO-4: Connect to server
	iResult = connect(mySocket_client, (const struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
	
	if (iResult == SOCKET_ERROR)
	{
		Error = "Client Error Connecting to server";
		printWSErrorAndExit(Error.c_str());
	}

	for (int i = 0; i < 5; ++i)
	{
		// TODO-5:		
		// - Control errors in both cases
		// - Send a 'ping' packet to the server
		std::string msg = "ping";
		std::string rec_msg;

		iResult = send(mySocket_client, msg.c_str(), msg.length(), 0);
		if (iResult == SOCKET_ERROR)
		{
			Error = "Client Error Sending Message";
			printWSErrorAndExit(Error.c_str());
		}

		// - Receive 'pong' packet from the server
		char* r_msg=new char[10];
		
		iResult = recv(mySocket_client, r_msg, 10, 0);
		if (iResult == SOCKET_ERROR)
		{
			Error = "Client Error Receiving Message";
			printWSErrorAndExit(Error.c_str());
		}
		else
		{
			// - Control graceful disconnection from the server (recv receiving 0 bytes)
			if (iResult == 0)
			{
				std::cout << "Connection finished" << std::endl;
				break;
			}

			r_msg[iResult] = '\0';
			std::cout << r_msg << std::endl;
			Sleep(500);
		}
		
	}

	// TODO-6: Close socket
	iResult = closesocket(mySocket_client);
	
	if (iResult == SOCKET_ERROR)
	{
		Error = "Client Error clossing the socket";
		printWSErrorAndExit(Error.c_str());
	}

	// TODO-7: Winsock shutdown
	iResult = WSACleanup();
	
	if (iResult != NOERROR)
	{
		Error = "Client Error clossing Winsock";
		printWSErrorAndExit(Error.c_str());
	}
}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
