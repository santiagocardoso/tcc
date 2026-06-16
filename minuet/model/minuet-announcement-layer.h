/*
 * minuet-announcement-layer.h
 *
 *  Created on: 5 de nov de 2018
 *      Author: everaldo
 */

#ifndef MINUET_ANNOUNCEMENT_LAYER_H_
#define MINUET_ANNOUNCEMENT_LAYER_H_

#include <math.h>
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "minuet-event.h"
#include "minuet-header.h"
#include "minuet-detection-layer.h"
#include "minuet-utils.h"
#include "minuet-monitoring-layer.h"
#include "minuet-clustering-manager.h"

namespace ns3 {

class CommunicationLayer;
class DetectionLayer;
class MonitoringLayer;
class ClusteringManager;

class AnnouncementLayerInterface : public Object {
public:
	AnnouncementLayerInterface();
	virtual ~AnnouncementLayerInterface();
	virtual void receiveControlMessage(Ptr<Packet> packet, Address addr) = 0;

	void AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer);

protected:
	Ptr<CommunicationLayer> m_communicationLayer;
};

class AnnouncementLayer : public AnnouncementLayerInterface {
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	AnnouncementLayer();
	~AnnouncementLayer();
	void StartLayer();
	void StopLayer();

	virtual void receiveControlMessage(Ptr<Packet> packet, Address addr);

	void TryAnnounceEvent(Ptr<EventDetected> event);

private:
	uint64_t m_sentCounter;
	bool m_isStarted;
	bool CheckAnnouncementZone(Ptr<EventDetected> eventDetected); // Check if event is in Announcement Zone
	bool CheckLifeTimeEvent(Ptr<EventDetected> eventDetected); // Check if life time of the event expired
	bool CheckForwardMessage(uint32_t senderId, uint64_t seq);
	void ForwardAnnounceMessage(uint32_t detectorNodeId, uint64_t seq, Ptr<EventDetected> event);
	void PrintInLog(string message);
	void AnnounceEvent(uint32_t eventId);

	Ptr<Node> m_node;
	Ptr<MobilityModel> m_mobilityModel;
	Ptr<MonitoringLayer> m_monitoringLayer;
	Ptr<ClusteringManager> m_clusteringManager;

	vector<Ptr<EventDetected>> m_eventsDataBase;

	vector<vector<uint64_t>> m_forwardMessages;

	EventId m_announcementFrequency;
};

} // namespace ns3

#endif /* MINUET_ANNOUNCEMENT_LAYER_H_ */
