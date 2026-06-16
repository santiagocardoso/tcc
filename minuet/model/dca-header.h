#ifndef DCA_HEADER_H
#define DCA_HEADER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/header.h"


namespace ns3 {

class DCAHeader: public Header {
public:
	DCAHeader();
	virtual ~DCAHeader();

	static TypeId GetTypeId();

	//Para pegar a informação serialized
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);
};

class BeaconHeader2: public DCAHeader {

public:

	BeaconHeader2();
	virtual ~BeaconHeader2();

	static TypeId GetTypeId();

	struct VehicleInfo {
			uint32_t id;
			uint32_t score;
			bool isClusterHead;
			uint32_t clusterHeadId;
			Vector position;
			Vector direction;
		};

	uint64_t GetSeq() const;
	void SetSeq(uint64_t seq);

	void SetVehicleInfo(VehicleInfo);
	VehicleInfo GetVehicleInfo();

	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);

private:
	uint64_t m_seq;
	VehicleInfo m_vehicleInfo;
};
}

#endif
