#ifndef PCTT_HEADER_H
#define PCTT_HEADER_H

#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nstime.h"
#include "ns3/header.h"

namespace ns3 {

class PCTTHeader: public Header {
public:
	PCTTHeader ();
	virtual ~PCTTHeader ();

	static TypeId GetTypeId ();

	//Para pegar a informação serialized
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);
};

class CHMHeader: public PCTTHeader {
public:
	CHMHeader ();
	virtual ~CHMHeader ();

	static TypeId GetTypeId ();

	struct VehicleInfo {
		int32_t nodeId;
		int32_t packetId;
		Vector position;
		Vector velocity;
		Time currentTime;
		Time resignTime;
		Time observationTime;
		bool resignValue;
		int32_t candidateClusterHeadId;
		bool reset;
		uint32_t hopCount;
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

class CMMHeader: public PCTTHeader {
public:
	CMMHeader ();
	virtual ~CMMHeader ();

	static TypeId GetTypeId ();

	struct VehicleInfo {
		int32_t nodeId;
		int32_t chid;
		int32_t packetId;
		bool targetDetectionValue;
		Time currentTime;
		Vector position;
		Vector velocity;
		Time observationTime;
		bool predictionDenial;
		int32_t hopCount;
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

class JackMHeader: public PCTTHeader {
public:
	JackMHeader ();
	virtual ~JackMHeader ();

	static TypeId GetTypeId ();

	struct VehicleInfo {
		int32_t nodeId;
		int32_t packetId;
		int32_t nonMemberId;
		uint32_t hopCount;
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
