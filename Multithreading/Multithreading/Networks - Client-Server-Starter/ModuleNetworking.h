#pragma once

#define MAX_CHAR_INPUT_CHAT 64

class ModuleNetworking : public Module
{
private:

	//////////////////////////////////////////////////////////////////////
	// Module virtual methods
	//////////////////////////////////////////////////////////////////////

	bool init() override;

	bool preUpdate() override;

	bool cleanUp() override;
;

	//////////////////////////////////////////////////////////////////////
	// Socket event callbacks
	//////////////////////////////////////////////////////////////////////

	virtual bool isListenSocket(SOCKET socket) const { return false; }

	virtual void onSocketConnected(SOCKET socket, const sockaddr_in &socketAddress) { }

	virtual void onSocketReceivedData(SOCKET s,const InputMemoryStream &packet) = 0;

	virtual void onSocketDisconnected(SOCKET s) = 0;


	

protected:

	std::vector<SOCKET> sockets;
	std::vector<SOCKET> disconnectedSockets;

	void addSocket(SOCKET socket);

	void disconnect();

	bool sendPacket(const OutputMemoryStream &packet, SOCKET socket);
	static void reportError(const char *message);
};

