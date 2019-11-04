#include "ModuleNetworkingClient.h"


bool  ModuleNetworkingClient::start(const char * serverAddressStr, int serverPort, const char *pplayerName)
{
	int ret = 0;
	playerName = pplayerName;

	// TODO(jesus): TCP connection stuff
	// - Create the socket
	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	if (socketClient == INVALID_SOCKET)
	{
		reportError("Client Error Creating the Socket");
	}

	// - Create the remote address object
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	inet_pton(AF_INET, serverAddressStr, &serverAddress.sin_addr);

	// - Connect to the remote address
	ret = connect(socketClient, (const sockaddr*)&serverAddress, sizeof(serverAddress));
	if (ret == SOCKET_ERROR)
	{
		reportError("Client Error Conecting to the Server");
	}
	else
	{
		// - Add the created socket to the managed list of sockets using addSocket()
		addSocket(socketClient);

		// If everything was ok... change the state
		state = ClientState::Start;
	}

	

	return true;
}

bool ModuleNetworkingClient::isRunning() const
{
	return state != ClientState::Stopped;
}

bool ModuleNetworkingClient::update()
{
	OutputMemoryStream packet;
	if (state == ClientState::Start)
	{
		packet << ClientMessage::Hello;
		packet << playerName;

		if (sendPacket(packet, socketClient))
		{
			state = ClientState::Logging;
		}
		else
		{
			disconnect();
			state = ClientState::Stopped;
		}
	}

	if (state == ClientState::Logging)
	{
		if (message.length() > 0)
		{
			packet << ClientMessage::Send;
			packet << message;
		}

		if (!sendPacket(packet, socketClient))
		{
			disconnect();
			state = ClientState::Stopped;
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
		if (ImGui::Button("Disconect"))
		{
			onSocketDisconnected(socketClient);
			shutdown(socketClient, 2);
		}
		ImGui::BeginChild("Chat:", ImVec2(350, 450), true);
		for (int i = 0; i < messages.size(); ++i)
		{
			ImGui::Text("%s", messages[i].c_str());
		}
		ImGui::EndChild();

		char buff[1024] = "\0";

		if (ImGui::InputText("Chat", buff, 1024, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			std::string message(buff);
			DLOG("%s", message.c_str());
		}


		ImGui::End();
	}

	return true;
}

void ModuleNetworkingClient::onSocketReceivedData(SOCKET socket, const InputMemoryStream &packet)
{
	ServerMessage serverMessage;
	packet >> serverMessage;

	switch (serverMessage) {
	case ServerMessage::Welcome: {
		std::string msg = std::string();
		packet >> msg;
		bool colorRed = true;
		packet >> colorRed;
		LOG(msg.c_str());
		break;
	}
	case ServerMessage::UserNameExists: {
		std::string msg = std::string();
		packet >> msg;
		ELOG(msg.c_str());
		onSocketDisconnected(socket);
		state = ClientState::Stopped;
		break;
	}
	/*case ServerMessage::UserLeft: {
		std::string msg = std::string();
		packet >> msg;
		LOG(msg.c_str());
		break;
	}
	case ServerMessage::UserJoin: {
		std::string msg = std::string();
		packet >> msg;
		LOG(msg.c_str());
		break;
	}*/
	}
}

void ModuleNetworkingClient::onSocketDisconnected(SOCKET socket)
{
	state = ClientState::Stopped;
}

