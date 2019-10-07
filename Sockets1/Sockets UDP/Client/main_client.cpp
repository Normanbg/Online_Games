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
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		// Log and handle error
		return;
	}

	// TODO-2: Create socket (IPv4, datagrams, UDP

	SOCKET sockymocky = socket(AF_INET, SOCK_DGRAM, 0);

	// TODO-3: Force address reuse

	struct sockaddr_in remoteAddr;
	remoteAddr.sin_family = AF_INET; // IPv4
	remoteAddr.sin_port = htons(port); // Port
	const char * remoteAddrStr = "127.0.0.1"; // Not so remote… :-P
	inet_pton(AF_INET, remoteAddrStr, &remoteAddr.sin_addr);

	//REMOTE SOCKETADDR & BINDADDR?????

	struct sockaddr_in bindAddr;
	int res = bind(s, (const struct sockaddr *)&bindAddr, sizeof(bindAddr));



	while (true)
	{
		// TODO-4:
		// - Send a 'ping' packet to the server
		// - Receive 'pong' packet from the server
		// - Control errors in both cases
		sendto(sockymocky, remoteAddrStr, int len, 0, remoteAddr, (sizeof(sockaddr_in)));
	}

	// TODO-5: Close socket
	closesocket(sockymocky);

	// TODO-6: Winsock shutdown
}

int main(int argc, char **argv)
{
	client(SERVER_ADDRESS, SERVER_PORT);

	PAUSE_AND_EXIT();
}
