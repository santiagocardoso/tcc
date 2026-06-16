#ifndef SOCIAL_HEADER_H
#define SOCIAL_HEADER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/header.h"


namespace ns3 {

class SOCIALHeader: public Header {
public:
	SOCIALHeader();
	virtual ~SOCIALHeader();

	static TypeId GetTypeId();

	//Para pegar a informação serialized
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);
};

class BeaconHeader: public SOCIALHeader {

public:

	BeaconHeader();
	virtual ~BeaconHeader();

	static TypeId GetTypeId();

	struct VehicleInfo {
			uint16_t id;
			bool isRelay;
			uint16_t relayId;
			Vector position;
			Vector direction;
			bool isPerfilSocial;
			bool isGateway;

			/////////STR/////////
			uint32_t score;
			uint32_t eigenScore;
			float closenessScore;

			uint16_t atividadeSocial;

			//uint32_t amizade;
		};

	uint64_t GetSeq() const;
	void SetSeq(uint64_t seq);

	/*std::vector<uint32_t> GetPathList() const;
	void SetPathList(std::vector<uint32_t> list);*/
	std::vector<std::vector<uint16_t>> GetNeighborsList() const;
	void SetNeighborsList(std::vector<std::vector<uint16_t>> list);

	void SetVehicleInfo(VehicleInfo);
	VehicleInfo GetVehicleInfo();

	uint16_t GetLenNeighborsList() const;
	void SetLenNeighborsList(uint16_t lenNeighborList);

	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);

private:
	uint64_t m_seq;
	VehicleInfo m_vehicleInfo;
	std::vector<std::vector<uint16_t>> neighborsList;
	uint16_t lenNeighborsList=0;
};
}

#endif
