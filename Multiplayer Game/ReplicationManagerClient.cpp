#include "Networks.h"
#include "ReplicationManagerClient.h"

void ReplicationManagerClient::read(const InputMemoryStream & packet)
{
	uint32 networkId;
	packet >> networkId;
	ReplicationAction repAction;
	packet >> repAction;
	switch (repAction)
	{
	case ReplicationAction::Create: {
		CreatePacket cPacket;
		packet >> cPacket.position;
		packet >> cPacket.angle;
		packet >> cPacket.size;
		packet >> cPacket.texName;
		cPacket.networkId = networkId;
		packetsToCreate.push_back(cPacket);
		break;
	}
	case ReplicationAction::Update: {
		UpdatePacket uPacket;
		packet >> uPacket.position;
		packet >> uPacket.angle;
		uPacket.networkId = networkId;
		packetsToUpdate.push_back(uPacket);
		break;
	}
	case ReplicationAction::Destroy: {
		DestroyPacket dPacket;
		dPacket.networkId = networkId;
		packetsToDestroy.push_back(dPacket);
		break;
	}
	}

	for (auto &it : packetsToDestroy) {
		GameObject* g = App->modLinkingContext->getNetworkGameObject(it.networkId);
		if (g != nullptr) {
			App->modLinkingContext->unregisterNetworkGameObject(g);
			Destroy(g);
		}
		else {
			ELOG("network ID object not found %i", it.networkId);
		}
	}
	packetsToDestroy.clear();

	for (auto &it : packetsToCreate) {
		GameObject* g = Instantiate();
		App->modLinkingContext->registerNetworkGameObjectWithNetworkId(g, it.networkId);
		g->position = it.position;
		g->angle = it.angle;
		g->size = it.size;
		g->texture = App->modTextures->loadTexture(it.texName.c_str());
	}
	packetsToCreate.clear();

	for (auto &it : packetsToUpdate) {
		GameObject* g = App->modLinkingContext->getNetworkGameObject(it.networkId);
		if (g != nullptr) {
			g->position = it.position;
			g->angle = it.angle;
		}
		else {
			ELOG("network ID object not found %i", it.networkId);
		}
	}
	packetsToUpdate.clear();
}
