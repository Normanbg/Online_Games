#include "ModuleNetworkingServer.h"




//////////////////////////////////////////////////////////////////////
// ModuleNetworkingServer public methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::start(int port)
{
	// TODO(jesus): TCP listen socket stuff
	// - Create the listenSocket
	listenSocket = socket(AF_INET, SOCK_STREAM, 0); // IPv4, TCP socket
	if (listenSocket == INVALID_SOCKET) {
		reportError("Server error: Error creating listening socket, INVALID_SOCkET");
	}
	// - Set address reuse
	int enable = 1;
	int iResult = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&enable, sizeof(int));
	if (iResult != NO_ERROR) {//error case	
		reportError("Server error: Error reusing socket's address");
	}
	sockaddr_in addressBound;
	addressBound.sin_family = AF_INET; //IPv4
	addressBound.sin_port = htons(port);
	addressBound.sin_addr.S_un.S_addr = INADDR_ANY;

	// - Bind the socket to a local interface
	iResult = bind(listenSocket, (const sockaddr*)&addressBound, sizeof(addressBound));//bind socket with address
	if (iResult != NO_ERROR) {//error case	
		reportError("Server error: Error binding socket's address");
	}

	// - Enter in listen mode
	iResult = listen(listenSocket, MAX_SOCKETS); ///EXLUSIVE OF TCP SOCKETS (listen, accept, connect)
	if (iResult != NO_ERROR) {//error case	
		reportError("Server error: Error listening socket");
	}

	// - Add the listenSocket to the managed list of sockets using addSocket()
	addSocket(listenSocket);

	state = ServerState::Listening;

	return true;
}

bool ModuleNetworkingServer::isRunning() const
{
	return state != ServerState::Stopped;
}



//////////////////////////////////////////////////////////////////////
// Module virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::update()
{
	return true;
}

bool ModuleNetworkingServer::gui()
{
	if (state != ServerState::Stopped)
	{
		// NOTE(jesus): You can put ImGui code here for debugging purposes
		ImGui::Begin("Server Window");

		Texture *tex = App->modResources->server;
		ImVec2 texSize(400.0f, 400.0f * tex->height / tex->width);
		ImGui::Image(tex->shaderResource, texSize);

		if (ImGui::Button("Close Server"))
		{
			disconnect();
			state = ServerState::Stopped;
		}

		ImGui::Text("List of connected sockets:");

		for (auto &connectedSocket : connectedSockets)
		{
			ImGui::Separator();
			ImGui::Text("Socket ID: %d", connectedSocket.socket);
			ImGui::Text("Address: %d.%d.%d.%d:%d",
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b1,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b2,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b3,
				connectedSocket.address.sin_addr.S_un.S_un_b.s_b4,
				ntohs(connectedSocket.address.sin_port));
			ImGui::Text("Player name: %s", connectedSocket.playerName.c_str());
		}

		ImGui::End();
	}

	return true;
}



//////////////////////////////////////////////////////////////////////
// ModuleNetworking virtual methods
//////////////////////////////////////////////////////////////////////

bool ModuleNetworkingServer::isListenSocket(SOCKET socket) const
{
	return socket == listenSocket;
}

void ModuleNetworkingServer::onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress)
{
	// Add a new connected socket to the list
	ConnectedSocket connectedSocket;
	connectedSocket.socket = socket;
	connectedSocket.address = socketAddress;
	connectedSockets.push_back(connectedSocket);

}

void ModuleNetworkingServer::onSocketReceivedData(SOCKET socket, const InputMemoryStream &packet)
{
	ClientMessage clientMessage;
	packet >> clientMessage;
	if (clientMessage == ClientMessage::Hello) {

		std::string playerName;
		packet >> playerName;
		// Set the player name of the corresponding connected socket proxy
		for (auto &connectedSocket : connectedSockets)
		{
			if (connectedSocket.socket == socket)
			{
				connectedSocket.playerName = playerName; 
				bool nameMatch = false;
				nameMatch = checkName(connectedSocket);
				
				if (nameMatch) {
					OutputMemoryStream warningNamePacket;//---------------USERNAME EXISTS MESSAGE
					warningNamePacket << ServerMessage::UserNameExists;
					std::string msg = std::string("Error Logging in: Name already in usage!");
					warningNamePacket << msg;
					if (!sendPacket(warningNamePacket, connectedSocket.socket))
					{
						reportError("sending warning name duplication message");
					}	//-----------------				
				}
				else {//if name already exists.
					OutputMemoryStream welcomePacket; //---------------WELCOME MESSAGE
					welcomePacket << ServerMessage::Welcome;
					std::string msg = std::string("Welcome to the chat");
					welcomePacket << msg; 
					int color = 3;//yellow color
					welcomePacket << color;
					bool colorRed = false;
					welcomePacket << colorRed;
					if (!sendPacket(welcomePacket, connectedSocket.socket))
					{
						reportError("sending welcome message");
					}
					//-----------------
					OutputMemoryStream userJoinPacket; //---------------USER JOIN MESSAGE
					userJoinPacket << ServerMessage::UserJoin;
					std::string msg2 = std::string(connectedSocket.playerName + " joined the chat.");
					userJoinPacket << msg2; 
					int color2 = 2;//blue color
					userJoinPacket << color2;
					for (auto &connectedSocket2 : connectedSockets) {
						if (connectedSocket2.socket != connectedSocket.socket) {
							if (!sendPacket(userJoinPacket, connectedSocket2.socket))
							{
								reportError("sending left chat message");
							}
						}
					}
					//------------------
				}
			}
		}		
	}
	if (clientMessage == ClientMessage::SendChatMsg) {
		std::string sender = std::string();
		char txt[MAX_CHAR_INPUT_CHAT];
		memset(txt, 0, IM_ARRAYSIZE(txt));
		packet >> sender;
		packet >> txt; 

		OutputMemoryStream chatTxtPacket;
		chatTxtPacket << ServerMessage::ReceiveChatMsg;
		
		chatTxtPacket << sender;
		chatTxtPacket << txt;
		int  color =0;//white color
		chatTxtPacket << color;


		for (auto &connectedSocket : connectedSockets) {
			
				if (!sendPacket(chatTxtPacket, connectedSocket.socket))
				{
					reportError("sending chat message");
				}
			
		}
	}
}


bool ModuleNetworkingServer::checkName(ConnectedSocket socket)
{
	for (int i = 0; i < connectedSockets.size(); ++i)
	{
		if (socket.playerName == connectedSockets[i].playerName)
		{
			return false;
		}
	}
	return true;
}


void ModuleNetworkingServer::onSocketDisconnected(SOCKET socket)
{
	// Remove the connected socket from the list
	for (auto it = connectedSockets.begin(); it != connectedSockets.end(); ++it)
	{
		auto &connectedSocket = *it;
		if (connectedSocket.socket == socket)
		{			
			OutputMemoryStream userLeftPacket; //-----------------USER LEFT MESSAGE
			userLeftPacket << ServerMessage::UserLeft;
			std::string msg = std::string((*it).playerName + " left the chat.");
			userLeftPacket << msg;
			connectedSockets.erase(it);
			int color2 = 2;//blue color
			userLeftPacket << color2;
			for (auto &connectedSocket2 : connectedSockets) {
				if (!sendPacket(userLeftPacket, connectedSocket2.socket))
				{
					reportError("sending left chat message");
				}
			}
			break;//-----------------
		}
	}
}

