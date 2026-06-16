/*
 * minuet-monitoring-layer.h
 *
 *  Created on: 13 de nov de 2018
 *      Author: everaldo
 */

#ifndef MINUET_MONITORING_LAYER_H_
#define MINUET_MONITORING_LAYER_H_

#include <math.h>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "minuet-header.h"
#include "minuet-communication-layer.h"
#include "minuet-clustering-manager.h"

#define PI 3.14159265

using namespace std;

namespace ns3 {

class CommunicationLayer;
class ClusteringManager;
class DetectionLayer;

class MonitoringLayerInterface : public Object {
public:
	MonitoringLayerInterface();
	virtual ~MonitoringLayerInterface();
	virtual void ReceiveDataMessage(Ptr<Packet> packet, Address addr) = 0;
	virtual void ReceiveBaseStationAnnouncementMessage(Ptr<Packet> packet, Address addr) = 0;

	void AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer);

protected:
	Ptr<CommunicationLayer> m_communicationLayer;
};

class MonitoringLayer : public MonitoringLayerInterface {
public:

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	MonitoringLayer();
	~MonitoringLayer();
	void StartLayer();
	void StopLayer();

	void StartMonitoring(Ptr<EventDetected> event);

	bool IsMonitoring() const;

private:
	void StopMonitoring();
	void TryMonitoring(Ptr<EventDetected> event);
	bool IsInVisionField(Ptr<EventDetected> event);
	void Monitoring(Ptr<EventDetected> eventDetected, uint32_t packetPayloadSize);
	void SetupTraceVideoStream();

	virtual void ReceiveDataMessage(Ptr<Packet> packet, Address addr);
	virtual void ReceiveBaseStationAnnouncementMessage(Ptr<Packet> packet, Address addr);

	void AddStatus(MonitoringEventHeader::StatusNode statusNode);
	void RemoveStatus(MonitoringEventHeader::StatusNode statusNode);
	void PrintInLog(string message);
	void PrintInDump(Time time, uint64_t pkt_id, uint32_t pkt_size, uint32_t event_id, uint32_t node_id);
	void FinalizeDump(uint64_t pkt_id, uint32_t event_id);
	void PrintTotalFramesSent(uint32_t totalFrames, uint32_t nodeId, uint32_t eventId);

	void ForwardMonitoringMessage(uint64_t seq, uint32_t monitorId, uint32_t eventId, uint64_t monitoringTime, uint32_t frameId, char32_t frameType, uint32_t payloadSize);
	bool CheckForwardMessage(uint32_t relayId, uint32_t nodeId, uint64_t seq);
	bool CheckMonitoringZone(uint64_t monitoringTime);

	struct m_videoInfoStruct_t {
		char32_t   frameType;
		uint32_t frameSize;
		uint16_t numOfUdpPackets;
		Time     packetInterval;
	};

	map<uint32_t, m_videoInfoStruct_t*> m_videoInfoMap;
	map<uint32_t, m_videoInfoStruct_t*>::iterator m_videoInfoMapIt;
	uint32_t    m_numOfFrames;

	uint64_t m_sentMonitoringMensCounter;
	uint64_t m_forwardMonitoringMensCounter;
	bool m_isStarted;
	bool m_isMonitoring;
	MonitoringEventHeader::StatusNode m_statusNode;

	Ptr<Node> m_node;
	Ptr<MobilityModel> m_mobilityModel;
	Ptr<ClusteringManager> m_clusteringManager;

	EventId m_monitoringEvent;
	EventId m_checkingBaseStationProximityEvent;

	vector<Ptr<Packet>> m_packetVector;

	vector<vector<uint64_t>> m_forwardMessages;
};

} // namespace ns3

#endif /* MINUET_MONITORING_LAYER_H_ */
