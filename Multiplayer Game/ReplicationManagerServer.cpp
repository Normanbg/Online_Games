#include "Networks.h"
#include "ReplicationManagerServer.h"

void ReplicationManagerServer::create(uint32 networkId)
{
	replicationCommands[networkId] = ReplicationAction::Create;
}

void ReplicationManagerServer::update(uint32 networkId)
{
	if (replicationCommands[networkId] == ReplicationAction::None)
		replicationCommands[networkId] = ReplicationAction::Update;
}

void ReplicationManagerServer::destroy(uint32 networkId)
{
	replicationCommands[networkId] = ReplicationAction::Destroy;
}

void ReplicationManagerServer::write(OutputMemoryStream & packet)
{
	for (auto& it : replicationCommands)
	{
		if (it.second != ReplicationAction::None)
		{
			GameObject* gameObject = App->modLinkingContext->getNetworkGameObject(it.first);
			packet << it.first;
			packet << it.second;
			if (it.second == ReplicationAction::Create)
			{
				packet << gameObject->position;
				packet << gameObject->angle;
				packet << gameObject->size;
				packet << gameObject->texture->filename;
			}
			else if (it.second == ReplicationAction::Update) 
			{
				packet << gameObject->position;
				packet << gameObject->angle;
			}

		}
	}
	replicationCommands.clear();
}
