/*
 * minuet-header.h
 *
 *  Created on: 18 de out de 2018
 *      Author: everaldo
 */

#ifndef MINUET_HEADER_H_
#define MINUET_HEADER_H_

#include <stdio.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"

using namespace std;

namespace ns3 {

class MINUETHeader : public Header {
public:
	MINUETHeader();
	~MINUETHeader();

	static TypeId GetTypeId(void);
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);

};

class AnnouncementEventHeader : public MINUETHeader {
public:
	AnnouncementEventHeader();
	~AnnouncementEventHeader();

	struct AnnouncementEventDetectedInfo {
		uint32_t eventId;
		bool fixed;
		double_t detectionTime; // Define Detection Time
		double_t occurrenceTime; // Define Occurrence Time
		double_t duration; // Time life of the event detected
		uint64_t lastUpdated;	// Last updated of the event

		Vector position;
		Vector velocity;
	};
	
	AnnouncementEventDetectedInfo GetEventDetectedInfo() const;
	void SetEventDetectedInfo(AnnouncementEventDetectedInfo eventDetectedInfo);

	uint64_t GetSeq() const;
	void SetSeq(uint64_t seq);

	uint32_t GetDetectorNodeId() const;
	void SetDetectorNodeId(uint32_t senderId);

	static TypeId GetTypeId(void);
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);

private:
	uint64_t m_seq;
	uint32_t m_detectorNodeId;
	AnnouncementEventDetectedInfo m_eventDetectedInfo;

};

class MonitoringEventHeader : public MINUETHeader {
public:
	MonitoringEventHeader();
	~MonitoringEventHeader();

	enum StatusNode {
		RETRANS = 0,  // RETRANSMITER
		GATEWAY_C = 1,
		GATEWAY_BS = 2,
		GATEWAY_C_GATEWAY_BS = 3,
		MONITOR = 4,
		GATEWAY_C_MONITOR = 5,
		GATEWAY_BS_MONITOR = 6,
		GATEWAY_C_GATEWAY_BS_MONITOR = 7
	};

	uint64_t GetSeq() const;
	void SetSeq(uint64_t seq);

	uint32_t GetMonitorId() const;
	void SetMonitorId(uint32_t monitorId);

	uint32_t GetRelayId() const;
	void SetRelayId(uint32_t clusterId);

	uint32_t GetNodeId() const;
	void SetNodeId(uint32_t nodeId);

	StatusNode GetStatusNode() const;
	void SetStatusNode(StatusNode statusNode);

	uint32_t GetEventId() const;
	void SetEventId(uint32_t eventId);

	uint64_t GetMonitoringTime() const;
	void SetMonitoringTime(uint64_t monitoringTime);

	uint32_t GetFrameId() const;
	void SetFrameId(uint32_t frameId);

	char32_t GetFrameType() const;
	void SetFrameTye(char32_t frameType);

	static TypeId GetTypeId(void);
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);

private:
	uint64_t m_seq;
	uint32_t m_monitorId;
	uint32_t m_relayId;
	uint32_t m_nodeId;
	StatusNode m_statusNode;
	uint32_t m_eventId;
	uint64_t m_monitoringTime;

	uint32_t m_frameId;
	char32_t m_frameType;
};

class AnnouncementBaseStationHeader : public MINUETHeader {
public:
	AnnouncementBaseStationHeader();
	~AnnouncementBaseStationHeader();

	struct AnnouncementBaseStationInfo {
		int payload;
	};

	AnnouncementBaseStationInfo GetAnnouncementBaseStationInfo() const;
	void SetAnnouncementBaseStationInfo(AnnouncementBaseStationInfo announcementBaseStationInfo);

	uint64_t GetSeq() const;
	void SetSeq(uint64_t seq);

	static TypeId GetTypeId(void);
	virtual TypeId GetInstanceTypeId(void) const;
	virtual void Print(std::ostream &os) const;
	virtual uint32_t GetSerializedSize(void) const;
	virtual void Serialize(Buffer::Iterator start) const;
	virtual uint32_t Deserialize(Buffer::Iterator start);

private:
	uint64_t m_seq;
	AnnouncementBaseStationInfo m_announcementBaseStationInfo;
};

} // namespace ns3

#endif /* MINUET_HEADER_H_ */
