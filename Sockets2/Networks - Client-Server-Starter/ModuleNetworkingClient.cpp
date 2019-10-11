#include "ModuleNetworkingClient.h"


bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET) {
		reportError("socket");
	}
	// - Create the remote address object
	const int serverAddrLen = sizeof(serverAddress);
	serverAddress.sin_family = AF_INET; // IPv4
	inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr);
	serverAddress.sin_port = htons(serverPort); // Port
	// - Connect to the remote address
	int connectRes = connect(client_socket, (const sockaddr*)&serverAddress, serverAddrLen);
	if (connectRes == SOCKET_ERROR) {
		reportError("connect");
	}
	// - Add the created socket to the managed list of sockets using addSocket()
	addSocket(client_socket);

	// If everything was ok... change the state
	state = ClientState::Start;

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return state != ClientState::Stopped;
}

bool ModuleNetworkingClient::update()
{
	if (state == ClientState::Start)
	{
		// Input buffer
		const int inBufferLen = 1300;
		char inBuffer[inBufferLen];
		// TODO(jesus): Send the player name to the server
		int bytes = send(client_socket, playerName.c_str(), (int)playerName.size() + 1, 0);
		if (bytes > 0)
		{
			// Receive
			bytes = recv(client_socket, inBuffer, inBufferLen, 0);
			if (bytes == SOCKET_ERROR) {
				reportError("recv");
			}
			// Wait 1 second
			Sleep(1000);
		}
		else
		{
			int err = WSAGetLastError();
			reportError("send");
		}
	}

	return true;
}

bool ModuleNetworkingClient::gui()
{
	if (state != ClientState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Client Window");

		Texture *tex = App->modResources->client;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		ImGui::Text("%s connected to the server...", playerName.c_str());

		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, byte * data)
{
	state = ClientState::Stopped;
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	state = ClientState::Stopped;
}

