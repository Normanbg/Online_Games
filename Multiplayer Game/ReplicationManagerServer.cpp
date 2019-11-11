#include "Networks.h"
#include "ReplicationManagerServer.h"

void ReplicationManagerServer::create(uint32 networkId)
{
	replicationCommands[networkId] = ReplicationAction::Create;
}

void ReplicationManagerServer::update(uint32 networkId)
{
	replicationCommands[networkId] = ReplicationAction::Update;
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	replicationCommands[networkId] = ReplicationAction::Destroy;
}

void ReplicationManagerServer::write(OutputMemoryStream & packet)
{
}
