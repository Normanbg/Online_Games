#pragma once

class ReplicationManagerClient
{
public:

	struct CreatePacket {
		uint32 networkId;
		vec2 position;
		float angle;
		vec2 size;
		std::string texName;
	};
	struct UpdatePacket {
		uint32 networkId;
		vec2 position;
		float angle;
	};
	struct DestroyPacket {
		uint32 networkId;
	};

	void read(const InputMemoryStream &packet);

	std::vector<CreatePacket> packetsToCreate;
	std::vector<UpdatePacket> packetsToUpdate;
	std::vector<DestroyPacket> packetsToDestroy;
};

